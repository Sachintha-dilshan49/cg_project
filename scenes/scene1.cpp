// ============================================================
//  scenes/scene1.cpp  -  MEMBER 1 OWNS THIS FILE
//  Topic : Drawing Primitives + OpenGL Setup
//  Concepts implemented (all drawn manually / procedurally):
//    1. Points        - glVertex2f at several positions, varying glPointSize
//    2. Lines         - Bresenham's Line Algorithm (integer-only), animated
//    3. Circles       - Midpoint Circle Algorithm, 8-octant symmetry
//    4. Polygons      - triangle, square, then a hollow HOUSE outline
//
//  The house is left on screen at the end so Member 2 can fill it.
// ============================================================

#include "../common.h"
#include <vector>
#include <cstdio>    // snprintf
#include <cstdlib>   // abs(int)
#include <utility>   // std::pair, std::make_pair

// ── Sub-scene timeline (milliseconds) ────────────────────────
// Phases run one after another; the final phase (house) stays.
static const float T_POINTS = 1500.0f;   // points start appearing
static const float T_LINE   = 12000.0f;  // Bresenham line phase
static const float T_CIRCLE = 26000.0f;  // Midpoint circle phase
static const float T_POLY   = 39000.0f;  // polygons -> house phase

// ── Magnified "pixel grid" helpers ───────────────────────────
// To make the algorithms visible we draw onto a zoomed grid where
// one logical pixel = CELL screen pixels. (gx,gy) is the grid's
// bottom-left corner in screen coords.

// Draw the faint background grid (cols x rows cells of size cell).
static void drawPixelGrid(float gx, float gy, int cols, int rows, float cell) {
    setColor(COL_DARK_GREY);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 0; i <= cols; i++) {
        glVertex2f(gx + i * cell, gy);
        glVertex2f(gx + i * cell, gy + rows * cell);
    }
    for (int j = 0; j <= rows; j++) {
        glVertex2f(gx, gy + j * cell);
        glVertex2f(gx + cols * cell, gy + j * cell);
    }
    glEnd();
}

// Light up one logical pixel (i,j) on the grid as a filled square.
static void litCell(float gx, float gy, int i, int j, float cell, Color c) {
    setColor(c);
    glBegin(GL_QUADS);
    glVertex2f(gx + i * cell + 1,        gy + j * cell + 1);
    glVertex2f(gx + (i + 1) * cell - 1,  gy + j * cell + 1);
    glVertex2f(gx + (i + 1) * cell - 1,  gy + (j + 1) * cell - 1);
    glVertex2f(gx + i * cell + 1,        gy + (j + 1) * cell - 1);
    glEnd();
}

// ── Bresenham's Line Algorithm ───────────────────────────────
// Classic integer-only line rasterizer. Works for any slope/octant.
// Fills 'out' with the sequence of pixels from (x0,y0) to (x1,y1).
static void bresenham(int x0, int y0, int x1, int y1,
                      std::vector<std::pair<int,int>>& out) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;       // step direction in x
    int sy = (y0 < y1) ? 1 : -1;       // step direction in y
    int err = dx - dy;                 // decision variable
    while (true) {
        out.push_back(std::make_pair(x0, y0));
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }   // step in x
        if (e2 <  dx) { err += dx; y0 += sy; }   // step in y
    }
}

// ── Midpoint Circle Algorithm ────────────────────────────────
// Computes ONE octant using an integer decision parameter, then
// mirrors each point into all 8 octants. 'out' holds octant points
// as (x,y) offsets from the centre; mirroring is done at draw time.
static void midpointCircle(int radius, std::vector<std::pair<int,int>>& out) {
    int x = 0, y = radius;
    int p = 1 - radius;                // initial decision parameter
    out.push_back(std::make_pair(x, y));
    while (x < y) {
        x++;
        if (p < 0) {                   // midpoint inside circle
            p += 2 * x + 1;
        } else {                       // midpoint outside -> step y in
            y--;
            p += 2 * (x - y) + 1;
        }
        out.push_back(std::make_pair(x, y));
    }
}

// Plot the 8 symmetric points of (x,y) around centre (i,j).
static void plot8(float gx, float gy, int ci, int cj,
                  int x, int y, float cell, Color c) {
    litCell(gx, gy, ci + x, cj + y, cell, c);
    litCell(gx, gy, ci - x, cj + y, cell, c);
    litCell(gx, gy, ci + x, cj - y, cell, c);
    litCell(gx, gy, ci - x, cj - y, cell, c);
    litCell(gx, gy, ci + y, cj + x, cell, c);
    litCell(gx, gy, ci - y, cj + x, cell, c);
    litCell(gx, gy, ci + y, cj - x, cell, c);
    litCell(gx, gy, ci - y, cj - x, cell, c);
}

// ── Hollow house outline (square body + triangle roof) ───────
// Drawn with GL_LINE_LOOP for the silhouette plus the eave line.
// Centre (cx,cy) is the middle of the wall; s = half-width.
static void drawHouseOutline(float cx, float cy, float s, Color c, float lw) {
    float wallH = 2.0f * s;            // wall height
    float roofH = 1.3f * s;            // roof height above the wall
    setColor(c);
    glLineWidth(lw);
    // Silhouette: bottom-left -> bottom-right -> wall-top-right
    //             -> apex -> wall-top-left -> (loop closes)
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - s, cy);
        glVertex2f(cx + s, cy);
        glVertex2f(cx + s, cy + wallH);
        glVertex2f(cx,     cy + wallH + roofH);
        glVertex2f(cx - s, cy + wallH);
    glEnd();
    // Eave: the line between the wall top and the roof
    glBegin(GL_LINES);
        glVertex2f(cx - s, cy + wallH);
        glVertex2f(cx + s, cy + wallH);
    glEnd();
}

// ── scene1 ─────────────────────────────────────────────────
//  Layout zones (so text and the magnified grid never overlap):
//    y ~565 : heading
//    y ~543 : one-line OpenGL/setup note
//    y ~515 : current phase title (yellow)
//    y  100-470 : demo / magnified grid area (x: 300-700)
//    y  ~70 : explanation line for the current phase
void scene1(float t) {

    // ── Header (kept clear of the demo area) ─────────────────
    drawHeading(20, WIN_HEIGHT - 35, "Drawing Primitives + OpenGL Setup");
    drawText(20, WIN_HEIGHT - 57,
             "GLUT opens the window; the main loop clears, draws and swaps ~60x/sec.",
             COL_LIGHT_GREY, false);

    // ── Prof. Pixel (waves briefly, then "talks") ────────────
    float wave = (t > 1500.0f) ? 0.0f : 1.0f;
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(95, 135, 0.85f, wave > 0.5f, talk);

    // ===========================================================
    //  PHASE 1 - POINTS
    // ===========================================================
    if (t < T_LINE) {
        drawText(30, WIN_HEIGHT - 95,
                 "1. POINTS  -  a pixel at an (x, y) location; glPointSize sets its size",
                 COL_YELLOW, true);

        float baseY = 300.0f, startX = 320.0f;
        int   nPts  = 6;
        int   shown = (int)((t - T_POINTS) / 1300.0f) + 1;   // ~one per 1.3s
        if (shown > nPts) shown = nPts;
        if (shown < 0)    shown = 0;

        for (int i = 0; i < shown; i++) {
            float sz = 5.0f + i * 5.0f;              // growing point size
            glPointSize(sz);
            setColor(COL_SCENE1);
            glBegin(GL_POINTS);
            glVertex2f(startX + i * 70.0f, baseY);
            glEnd();
            char buf[16];
            snprintf(buf, sizeof(buf), "%.0f px", sz);
            drawText(startX + i * 70.0f - 14, baseY - 35, buf, COL_LIGHT_GREY, false);
        }
        glPointSize(1.0f);

        drawText(180, 235,
                 "Everything starts with a single point.",
                 COL_WHITE, true);
    }

    // ===========================================================
    //  PHASE 2 - BRESENHAM LINE
    // ===========================================================
    else if (t < T_CIRCLE) {
        float lt = t - T_LINE;
        drawText(30, WIN_HEIGHT - 95,
                 "2. LINES  -  Bresenham's Line Algorithm (integer math)",
                 COL_YELLOW, true);

        // Magnified grid (kept below the phase title, above the caption).
        float gx = 300, gy = 110, cell = 24;
        int cols = 16, rows = 13;                     // 384 x 312, top = 422
        drawPixelGrid(gx, gy, cols, rows, cell);

        std::vector<std::pair<int,int>> pts;
        int x0 = 1, y0 = 2, x1 = 14, y1 = 11;
        bresenham(x0, y0, x1, y1, pts);

        int shown = (int)(lt / 750.0f) + 1;          // ~one pixel / 0.75s
        if (shown > (int)pts.size()) shown = pts.size();

        // Faint "ideal" mathematical line for comparison.
        setColor(COL_DARK_GREY);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(gx + (x0 + 0.5f) * cell, gy + (y0 + 0.5f) * cell);
        glVertex2f(gx + (x1 + 0.5f) * cell, gy + (y1 + 0.5f) * cell);
        glEnd();

        for (int i = 0; i < shown; i++)
            litCell(gx, gy, pts[i].first, pts[i].second, cell, COL_SCENE1);

        // Endpoints highlighted.
        litCell(gx, gy, x0, y0, cell, COL_YELLOW);
        if (shown >= (int)pts.size())
            litCell(gx, gy, x1, y1, cell, COL_YELLOW);

        char buf[40];
        snprintf(buf, sizeof(buf), "pixel %d of %d", shown, (int)pts.size());
        drawText(gx + cols * cell + 12, gy + rows * cell - 20, buf, COL_YELLOW, false);

        drawText(180, 70,
                 "Picks the nearest pixel with integer math - very fast.",
                 COL_WHITE, true);
    }

    // ===========================================================
    //  PHASE 3 - MIDPOINT CIRCLE
    // ===========================================================
    else if (t < T_POLY) {
        float ct = t - T_CIRCLE;
        drawText(30, WIN_HEIGHT - 95,
                 "3. CIRCLES  -  Midpoint Algorithm: compute 1/8, mirror 8 ways",
                 COL_YELLOW, true);

        float gx = 330, gy = 110, cell = 20;
        int   R  = 8;
        int   cols = 2 * R + 1, rows = 2 * R + 1;     // 340 x 340, top = 450
        int   ci = R, cj = R;
        drawPixelGrid(gx, gy, cols, rows, cell);

        std::vector<std::pair<int,int>> oct;
        midpointCircle(R, oct);

        int shown = (int)(ct / 1000.0f) + 1;          // ~one octant point / 1.0s
        if (shown > (int)oct.size()) shown = oct.size();

        litCell(gx, gy, ci, cj, cell, COL_DARK_GREY); // centre marker

        for (int i = 0; i < shown; i++) {
            int x = oct[i].first, y = oct[i].second;
            plot8(gx, gy, ci, cj, x, y, cell, COL_SCENE1);    // 8 mirrors
            litCell(gx, gy, ci + x, cj + y, cell, COL_YELLOW); // computed point
        }

        drawText(gx + cols * cell + 12, gy + rows * cell - 20,
                 "1 computed", COL_YELLOW, false);
        drawText(gx + cols * cell + 12, gy + rows * cell - 38,
                 "8 mirrored", COL_SCENE1, false);

        drawText(180, 70,
                 "One pixel computed, eight mirrored around the centre.",
                 COL_WHITE, true);
    }

    // ===========================================================
    //  PHASE 4 - POLYGONS -> HOUSE  (stays on screen)
    // ===========================================================
    else {
        float pt = t - T_POLY;
        drawText(30, WIN_HEIGHT - 95,
                 "4. POLYGONS  -  connect vertices in order (GL_LINE_LOOP)",
                 COL_YELLOW, true);

        if (pt < 4000.0f) {                           // triangle
            setColor(COL_CYAN);
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(390, 180); glVertex2f(560, 180); glVertex2f(475, 340);
            glEnd();
            drawText(180, 130, "Triangle - 3 vertices joined in a loop.", COL_WHITE, true);
        }
        else if (pt < 8000.0f) {                      // square
            setColor(COL_CYAN);
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(395, 180); glVertex2f(555, 180);
                glVertex2f(555, 340); glVertex2f(395, 340);
            glEnd();
            drawText(180, 130, "Square - 4 vertices joined in a loop.", COL_WHITE, true);
        }
        else {                                        // house outline (stays)
            drawHouseOutline(475, 180, 85, COL_SCENE1, 3.0f);
            drawText(180, 130,
                     "A house = square body + triangle roof.", COL_WHITE, true);
            drawText(180, 105,
                     "Looks a little empty, doesn't it?", COL_YELLOW, true);
        }
    }

    // ── Transition hint ──────────────────────────────────────
    if (t > T_POLY + 9000.0f) {
        drawText(WIN_WIDTH/2 - 80, 45,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
