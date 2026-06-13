// ============================================================
//  scenes/scene4.cpp  -  MEMBER 4 OWNS THIS FILE
//  Topic : Viewing and Clipping
//  Concepts implemented:
//    1. Window & Viewport      - world space vs screen space
//    2. Window-to-Viewport map - proportional mapping formula
//    3. Cohen-Sutherland       - region codes: accept / reject / clip
//    4. Liang-Barsky           - parametric clip (t_enter / t_leave)
//    5. ADVANCED: clip a whole house+tree scene to the window
//  Dashed boundary rectangles use glLineStipple.
// ============================================================

#include "../common.h"
#include <cstdio>    // snprintf
#include <string>

// ── Sub-scene timeline (milliseconds) ────────────────────────
static const float T_WV    = 1000.0f;
static const float T_MAP   = 9000.0f;
static const float T_CS    = 19000.0f;
static const float T_LB    = 33000.0f;
static const float T_SCENE = 45000.0f;   // advanced scene-clip (stays)

// The clip window used by phases 3-5 (kept constant so it is familiar).
static const float WXMIN = 380, WYMIN = 185, WXMAX = 640, WYMAX = 420;

// ── small drawing helpers ────────────────────────────────────
static void dashedRect(float x0, float y0, float x1, float y1, Color c, float lw) {
    setColor(c);
    glLineWidth(lw);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00FF);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x0, y0); glVertex2f(x1, y0);
        glVertex2f(x1, y1); glVertex2f(x0, y1);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}
static void solidLine(float x0, float y0, float x1, float y1, Color c, float lw) {
    setColor(c);
    glLineWidth(lw);
    glBegin(GL_LINES); glVertex2f(x0, y0); glVertex2f(x1, y1); glEnd();
}
static void dashedLine(float x0, float y0, float x1, float y1, Color c, float lw) {
    setColor(c);
    glLineWidth(lw);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00FF);
    glBegin(GL_LINES); glVertex2f(x0, y0); glVertex2f(x1, y1); glEnd();
    glDisable(GL_LINE_STIPPLE);
}
static void dot(float x, float y, Color c, float sz) {
    setColor(c);
    glPointSize(sz);
    glBegin(GL_POINTS); glVertex2f(x, y); glEnd();
    glPointSize(1.0f);
}
static void miniHouse(float x, float y, float s, Color c) {
    setColor(c);
    glBegin(GL_QUADS);
        glVertex2f(x - s, y); glVertex2f(x + s, y);
        glVertex2f(x + s, y + s); glVertex2f(x - s, y + s);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(x - s, y + s); glVertex2f(x + s, y + s); glVertex2f(x, y + 1.8f * s);
    glEnd();
}

// ── Cohen-Sutherland region codes ────────────────────────────
static const int CS_INSIDE = 0, CS_LEFT = 1, CS_RIGHT = 2, CS_BOTTOM = 4, CS_TOP = 8;

static int csCode(float x, float y, float xmin, float ymin, float xmax, float ymax) {
    int code = CS_INSIDE;
    if (x < xmin) code |= CS_LEFT;  else if (x > xmax) code |= CS_RIGHT;
    if (y < ymin) code |= CS_BOTTOM; else if (y > ymax) code |= CS_TOP;
    return code;
}
// Format a region code as a 4-bit string in TOP-BOTTOM-RIGHT-LEFT order.
static std::string codeStr(int c) {
    std::string s;
    s += (c & CS_TOP)    ? '1' : '0';
    s += (c & CS_BOTTOM) ? '1' : '0';
    s += (c & CS_RIGHT)  ? '1' : '0';
    s += (c & CS_LEFT)   ? '1' : '0';
    return s;
}

// Clip line to rect; returns true if any part is visible and writes
// the (possibly shortened) endpoints back into the references.
static bool cohenSutherland(float& x0, float& y0, float& x1, float& y1,
                            float xmin, float ymin, float xmax, float ymax) {
    int c0 = csCode(x0, y0, xmin, ymin, xmax, ymax);
    int c1 = csCode(x1, y1, xmin, ymin, xmax, ymax);
    while (true) {
        if (!(c0 | c1)) return true;          // both inside -> accept
        if (c0 & c1)    return false;         // share an outside zone -> reject
        int co = c0 ? c0 : c1;
        float x = 0, y = 0;
        if (co & CS_TOP)        { x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0); y = ymax; }
        else if (co & CS_BOTTOM){ x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0); y = ymin; }
        else if (co & CS_RIGHT) { y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0); x = xmax; }
        else                    { y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0); x = xmin; }
        if (co == c0) { x0 = x; y0 = y; c0 = csCode(x0, y0, xmin, ymin, xmax, ymax); }
        else          { x1 = x; y1 = y; c1 = csCode(x1, y1, xmin, ymin, xmax, ymax); }
    }
}

// ── Liang-Barsky parametric clip ─────────────────────────────
static bool liangBarsky(float x0, float y0, float x1, float y1,
                        float xmin, float ymin, float xmax, float ymax,
                        float& u1, float& u2) {
    float dx = x1 - x0, dy = y1 - y0;
    float p[4] = { -dx, dx, -dy, dy };
    float q[4] = { x0 - xmin, xmax - x0, y0 - ymin, ymax - y0 };
    u1 = 0.0f; u2 = 1.0f;
    for (int i = 0; i < 4; i++) {
        if (p[i] == 0.0f) { if (q[i] < 0.0f) return false; }
        else {
            float r = q[i] / p[i];
            if (p[i] < 0.0f) { if (r > u1) u1 = r; }
            else             { if (r < u2) u2 = r; }
        }
    }
    return u1 <= u2;
}

// Draw the clip window plus the 9 region-code zones with labels.
static void drawRegionGrid() {
    dashedRect(WXMIN, WYMIN, WXMAX, WYMAX, COL_SCENE4, 2.5f);
    // Extended boundary lines.
    setColor(COL_DARK_GREY);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(WXMIN, 120); glVertex2f(WXMIN, 470);
        glVertex2f(WXMAX, 120); glVertex2f(WXMAX, 470);
        glVertex2f(300, WYMIN); glVertex2f(720, WYMIN);
        glVertex2f(300, WYMAX); glVertex2f(720, WYMAX);
    glEnd();
    // 4-bit codes (TOP-BOTTOM-RIGHT-LEFT) in each of the 9 zones.
    float lx = 330, mx = 500, rx = 665;
    float ty = 445, my = 300, by = 150;
    drawText(lx, ty, "1001", COL_LIGHT_GREY); drawText(mx, ty, "1000", COL_LIGHT_GREY); drawText(rx, ty, "1010", COL_LIGHT_GREY);
    drawText(lx, my, "0001", COL_LIGHT_GREY); drawText(mx, my, "0000", COL_YELLOW);      drawText(rx, my, "0010", COL_LIGHT_GREY);
    drawText(lx, by, "0101", COL_LIGHT_GREY); drawText(mx, by, "0100", COL_LIGHT_GREY); drawText(rx, by, "0110", COL_LIGHT_GREY);
    drawLabel(WXMIN - 12, WYMIN - 16, "xmin");
    drawLabel(WXMAX - 12, WYMIN - 16, "xmax");
}

// ── scene4 ─────────────────────────────────────────────────
void scene4(float t) {

    drawHeading(20, WIN_HEIGHT - 35, "Viewing and Clipping");

    float wave = (t > 1200.0f) ? 0.0f : 1.0f;
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(110, 110, 0.9f, wave > 0.5f, talk);

    // ===========================================================
    //  1. WINDOW & VIEWPORT   +   2. WINDOW-TO-VIEWPORT MAPPING
    // ===========================================================
    if (t < T_CS) {
        float wx0 = 250, wy0 = 210, wx1 = 430, wy1 = 390;
        dashedRect(wx0, wy0, wx1, wy1, COL_SCENE1, 2.5f);
        drawLabel(wx0, wy1 + 10, "WINDOW (world space)");
        miniHouse(300, 240, 14, COL_SCENE2);
        miniHouse(360, 260, 12, COL_ORANGE);
        miniHouse(395, 230, 10, COL_PINK);

        float vx0 = 520, vy0 = 230, vx1 = 700, vy1 = 370;
        dashedRect(vx0, vy0, vx1, vy1, COL_ORANGE, 2.5f);
        drawLabel(vx0, vy1 + 10, "VIEWPORT (screen space)");

        solidLine(wx1 + 6, (wy0 + wy1) / 2, vx0 - 6, (vy0 + vy1) / 2, COL_WHITE, 1.5f);
        drawText(458, (wy0 + wy1) / 2 + 8, "maps to", COL_WHITE, false);

        if (t < T_MAP) {
            drawLabel(250, WIN_HEIGHT - 85,
                      "1. WINDOW vs VIEWPORT  -  what we see (world) vs where it appears (screen)");
            drawText(250, 150,
                     "The window selects a region of the world; the viewport is the screen area it fills.",
                     COL_WHITE, false);
        } else {
            drawLabel(250, WIN_HEIGHT - 85,
                      "2. WINDOW-TO-VIEWPORT MAPPING  -  every point maps across by simple proportion");

            float p = 0.5f * (1 + sin((t - T_MAP) * 0.0011f));
            float q = 0.5f * (1 + cos((t - T_MAP) * 0.0009f));
            float pwx = wx0 + p * (wx1 - wx0);
            float pwy = wy0 + q * (wy1 - wy0);
            float pvx = vx0 + (pwx - wx0) * (vx1 - vx0) / (wx1 - wx0);
            float pvy = vy0 + (pwy - wy0) * (vy1 - vy0) / (wy1 - wy0);
            dashedLine(pwx, pwy, pvx, pvy, COL_DARK_GREY, 1.0f);
            dot(pwx, pwy, COL_YELLOW, 11.0f);
            dot(pvx, pvy, COL_YELLOW, 11.0f);

            drawText(250, 150,
                     "sx = vx0 + (wx - wx0) * (vx1 - vx0) / (wx1 - wx0)", COL_WHITE, false);
            drawText(250, 130,
                     "sy = vy0 + (wy - wy0) * (vy1 - vy0) / (wy1 - wy0)", COL_WHITE, false);
        }
    }

    // ===========================================================
    //  3. COHEN-SUTHERLAND  -  one clear example at a time
    // ===========================================================
    else if (t < T_LB) {
        drawLabel(250, WIN_HEIGHT - 85,
                  "3. COHEN-SUTHERLAND  -  give each endpoint a 4-bit region code");
        drawRegionGrid();

        // Cycle through the three cases so the screen is never cluttered.
        float lt = t - T_CS;
        int mode = ((int)(lt / 4000.0f)) % 3;   // 0=accept 1=reject 2=clip

        float ax, ay, bx, by;
        const char* verdict;
        Color vcol;
        if (mode == 0) {                         // ACCEPT: both inside
            ax = 450; ay = 250; bx = 590; by = 380;
            verdict = "ACCEPT  (both codes 0000)"; vcol = COL_SCENE2;
        } else if (mode == 1) {                  // REJECT: both above (share TOP bit)
            ax = 360; ay = 455; bx = 560; by = 460;
            verdict = "REJECT  (codes AND != 0)"; vcol = COL_SCENE4;
        } else {                                 // CLIP: crosses the boundary
            ax = 300; ay = 230; bx = 715; by = 400;
            verdict = "CLIP  (trim to the boundary)"; vcol = COL_YELLOW;
        }

        int ca = csCode(ax, ay, WXMIN, WYMIN, WXMAX, WYMAX);
        int cb = csCode(bx, by, WXMIN, WYMIN, WXMAX, WYMAX);

        // Original line (dashed grey) and its endpoint codes.
        dashedLine(ax, ay, bx, by, COL_LIGHT_GREY, 1.5f);
        dot(ax, ay, ca ? COL_SCENE4 : COL_SCENE2, 11.0f);
        dot(bx, by, cb ? COL_SCENE4 : COL_SCENE2, 11.0f);
        drawText(ax - 30, ay + 12, codeStr(ca), ca ? COL_SCENE4 : COL_SCENE2);
        drawText(bx + 8,  by + 8,  codeStr(cb), cb ? COL_SCENE4 : COL_SCENE2);

        // Resulting visible part (if any).
        float cx0 = ax, cy0 = ay, cx1 = bx, cy1 = by;
        if (cohenSutherland(cx0, cy0, cx1, cy1, WXMIN, WYMIN, WXMAX, WYMAX))
            solidLine(cx0, cy0, cx1, cy1, COL_YELLOW, 4.0f);

        drawText(250, 150, verdict, vcol, true);
        drawText(250, 125,
                 "Code bits: TOP BOTTOM RIGHT LEFT.  0000 means inside.",
                 COL_WHITE, false);
        drawText(250, 105,
                 "Both 0 -> accept.   Bitwise AND != 0 -> reject.   Otherwise clip.",
                 COL_LIGHT_GREY, false);
    }

    // ===========================================================
    //  4. LIANG-BARSKY
    // ===========================================================
    else if (t < T_SCENE) {
        drawLabel(250, WIN_HEIGHT - 85,
                  "4. LIANG-BARSKY  -  parametric clip: find t_enter and t_leave");
        dashedRect(WXMIN, WYMIN, WXMAX, WYMAX, COL_SCENE4, 2.5f);

        float lt = t - T_LB;
        float x0 = 300, y0 = 250 + 40 * sin(lt * 0.0009f);
        float x1 = 715, y1 = 360 + 40 * cos(lt * 0.0009f);

        dashedLine(x0, y0, x1, y1, COL_LIGHT_GREY, 1.5f);
        drawText(x0 - 6, y0 + 10, "t = 0", COL_LIGHT_GREY);
        drawText(x1 - 6, y1 + 10, "t = 1", COL_LIGHT_GREY);

        float u1, u2;
        if (liangBarsky(x0, y0, x1, y1, WXMIN, WYMIN, WXMAX, WYMAX, u1, u2)) {
            float ex0 = x0 + u1 * (x1 - x0), ey0 = y0 + u1 * (y1 - y0);
            float ex1 = x0 + u2 * (x1 - x0), ey1 = y0 + u2 * (y1 - y0);
            solidLine(ex0, ey0, ex1, ey1, COL_YELLOW, 4.0f);
            dot(ex0, ey0, COL_SCENE2, 12.0f);
            dot(ex1, ey1, COL_SCENE4, 12.0f);
            drawText(ex0 - 34, ey0 - 18, "t_enter", COL_SCENE2);
            drawText(ex1 + 8, ey1 + 8,  "t_leave", COL_SCENE4);

            char buf[64];
            snprintf(buf, sizeof(buf), "t_enter = %.2f      t_leave = %.2f", u1, u2);
            drawText(250, 150, buf, COL_WHITE, true);
        }
        drawText(250, 125,
                 "Walk the line as t goes 0 -> 1; keep only the t range inside the window.",
                 COL_WHITE, false);
        drawText(250, 105,
                 "Fewer calculations than Cohen-Sutherland, same result.",
                 COL_LIGHT_GREY, false);
    }

    // ===========================================================
    //  5. ADVANCED  -  clip a WHOLE house+tree scene to the window
    // ===========================================================
    else {
        drawLabel(250, WIN_HEIGHT - 85,
                  "CLIPPING A WHOLE SCENE  -  only what is inside the window survives");
        dashedRect(WXMIN, WYMIN, WXMAX, WYMAX, COL_SCENE4, 2.5f);

        // Each row is one line segment {x0,y0,x1,y1}. Parts of the house
        // roof and the whole tree fall outside the window on purpose.
        static const float segs[][4] = {
            // house body
            {430,225, 610,225}, {610,225, 610,360}, {610,360, 430,360}, {430,360, 430,225},
            // roof (apex pokes above the window top)
            {430,360, 520,475}, {610,360, 520,475},
            // door
            {500,225, 500,300}, {540,225, 540,300}, {500,300, 540,300},
            // tree trunk (left of the window)
            {352,225, 352,330}, {372,225, 372,330},
            // tree foliage (straddles the left edge)
            {318,310, 398,310}, {318,310, 358,398}, {398,310, 358,398},
        };
        int n = (int)(sizeof(segs) / sizeof(segs[0]));

        for (int i = 0; i < n; i++) {
            float x0 = segs[i][0], y0 = segs[i][1], x1 = segs[i][2], y1 = segs[i][3];
            // faint original (what we would have drawn)
            dashedLine(x0, y0, x1, y1, COL_DARK_GREY, 1.0f);
            // clipped, visible part
            float cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;
            if (cohenSutherland(cx0, cy0, cx1, cy1, WXMIN, WYMIN, WXMAX, WYMAX))
                solidLine(cx0, cy0, cx1, cy1, COL_SCENE2, 3.0f);
        }

        drawText(250, 150,
                 "Every edge is clipped with Cohen-Sutherland (green = kept, grey = removed).",
                 COL_WHITE, false);
        drawText(250, 128, "The real world has depth, though...", COL_YELLOW, false);
    }

    if (t > T_SCENE + 4000.0f) {
        drawText(WIN_WIDTH/2 - 80, 45,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
