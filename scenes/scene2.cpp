// ============================================================
//  scenes/scene2.cpp  -  MEMBER 2 OWNS THIS FILE
//  Topic : Filling Algorithms
//  Concepts implemented (cell-grid simulation, no glReadPixels):
//    1. Boundary Fill - spread from a seed, stop at a boundary colour
//    2. Flood Fill    - replace every pixel matching the seed's colour
//    3. Scan-Line Fill- sweep a horizontal line, fill edge spans
//
//  Ends with a fully coloured house (continues Member 1's house).
// ============================================================

#include "../common.h"
#include <vector>
#include <utility>   // std::pair, std::make_pair, std::swap

// ── Sub-scene timeline (milliseconds) ────────────────────────
static const float T_BFILL = 1500.0f;
static const float T_FLOOD = 18000.0f;
static const float T_SCAN  = 27000.0f;   // flood phase ends soon after it fills
static const float T_DONE  = 41000.0f;   // coloured house stays after this

// ── Grid helpers (one logical pixel = 'cell' screen pixels) ──
static void gridLines(float gx, float gy, int cols, int rows, float cell) {
    setColor(COL_DARK_GREY);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 0; i <= cols; i++) {
        glVertex2f(gx + i * cell, gy); glVertex2f(gx + i * cell, gy + rows * cell);
    }
    for (int j = 0; j <= rows; j++) {
        glVertex2f(gx, gy + j * cell); glVertex2f(gx + cols * cell, gy + j * cell);
    }
    glEnd();
}
static void fillCell(float gx, float gy, int i, int j, float cell, Color c) {
    setColor(c);
    glBegin(GL_QUADS);
    glVertex2f(gx + i * cell + 1,       gy + j * cell + 1);
    glVertex2f(gx + (i + 1) * cell - 1, gy + j * cell + 1);
    glVertex2f(gx + (i + 1) * cell - 1, gy + (j + 1) * cell - 1);
    glVertex2f(gx + i * cell + 1,       gy + (j + 1) * cell - 1);
    glEnd();
}

// 4-connected breadth-first fill. Returns, for each cell, the BFS
// step at which it was filled (or -1 if never). 'fillable[k]' tells
// whether cell k may be entered. This is the heart of BOTH boundary
// fill and flood fill - only the 'fillable' rule differs:
//   * boundary fill : enter any cell that is NOT the boundary colour
//   * flood fill     : enter any cell that MATCHES the seed's colour
static std::vector<int> bfsFill(int cols, int rows,
                                const std::vector<bool>& fillable,
                                int si, int sj) {
    std::vector<int> order(cols * rows, -1);
    if (si < 0 || sj < 0 || si >= cols || sj >= rows) return order;
    if (!fillable[sj * cols + si]) return order;

    std::vector<std::pair<int,int>> q;
    q.push_back(std::make_pair(si, sj));
    order[sj * cols + si] = 0;
    size_t head = 0;
    int step = 0;
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    while (head < q.size()) {
        int x = q[head].first, y = q[head].second;
        step = order[y * cols + x];
        head++;
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= cols || ny >= rows) continue;
            int idx = ny * cols + nx;
            if (order[idx] != -1) continue;          // already visited
            if (!fillable[idx]) continue;            // blocked
            order[idx] = step + 1;
            q.push_back(std::make_pair(nx, ny));
        }
    }
    return order;
}

// ── Final coloured house (matches Member 1's house placement) ─
static void drawColoredHouse(float cx, float cy, float s) {
    float wallH = 2.0f * s, roofH = 1.3f * s;
    // Wall (filled body)
    setColor(COL_SCENE2);
    glBegin(GL_QUADS);
        glVertex2f(cx - s, cy); glVertex2f(cx + s, cy);
        glVertex2f(cx + s, cy + wallH); glVertex2f(cx - s, cy + wallH);
    glEnd();
    // Roof (filled triangle)
    setColor(COL_SCENE4);
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - s, cy + wallH);
        glVertex2f(cx + s, cy + wallH);
        glVertex2f(cx,     cy + wallH + roofH);
    glEnd();
    // Door
    setColor(COL_SCENE3);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.25f * s, cy);
        glVertex2f(cx + 0.25f * s, cy);
        glVertex2f(cx + 0.25f * s, cy + 0.9f * s);
        glVertex2f(cx - 0.25f * s, cy + 0.9f * s);
    glEnd();
    // Window
    setColor(COL_CYAN);
    glBegin(GL_QUADS);
        glVertex2f(cx + 0.35f * s, cy + 1.1f * s);
        glVertex2f(cx + 0.75f * s, cy + 1.1f * s);
        glVertex2f(cx + 0.75f * s, cy + 1.6f * s);
        glVertex2f(cx + 0.35f * s, cy + 1.6f * s);
    glEnd();
    // Outline on top
    setColor(COL_WHITE);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - s, cy); glVertex2f(cx + s, cy);
        glVertex2f(cx + s, cy + wallH);
        glVertex2f(cx, cy + wallH + roofH);
        glVertex2f(cx - s, cy + wallH);
    glEnd();
}

// ── Scan-line polygon fill ───────────────────────────────────
// For each horizontal scan row, find the x where the row crosses
// each polygon edge, sort the crossings, and fill between pairs.
// We reveal rows from the top down to the moving sweep line.
static void scanFillPolygon(const float* vx, const float* vy, int n,
                            float yTop, float yBot, float sweepY, Color c) {
    setColor(c);
    for (float y = yTop; y >= yBot && y >= sweepY; y -= 3.0f) {
        // Collect intersection x's of this scan row with all edges.
        std::vector<float> xs;
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            float y0 = vy[i], y1 = vy[j], x0 = vx[i], x1 = vx[j];
            // Edge crosses this row? (half-open to avoid double counts)
            if ((y0 <= y && y1 > y) || (y1 <= y && y0 > y)) {
                float xcross = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
                xs.push_back(xcross);
            }
        }
        // Sort crossings, fill between consecutive pairs.
        for (size_t a = 0; a + 1 < xs.size(); a++)
            for (size_t b = a + 1; b < xs.size(); b++)
                if (xs[b] < xs[a]) std::swap(xs[a], xs[b]);
        glBegin(GL_LINES);
        for (size_t a = 0; a + 1 < xs.size(); a += 2) {
            glVertex2f(xs[a], y);
            glVertex2f(xs[a + 1], y);
        }
        glEnd();
    }
}

// ── scene2 ─────────────────────────────────────────────────
void scene2(float t) {

    drawHeading(20, WIN_HEIGHT - 35, "Filling Algorithms");

    float wave = (t > 1200.0f) ? 0.0f : 1.0f;
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(110, 110, 0.9f, wave > 0.5f, talk);

    // ===========================================================
    //  PHASE 1 - BOUNDARY FILL
    // ===========================================================
    if (t < T_FLOOD) {
        float lt = t - T_BFILL;
        drawLabel(300, WIN_HEIGHT - 100,
                  "1. BOUNDARY FILL  -  spread from a seed, stop at the boundary");

        int cols = 22, rows = 16; float cell = 18, gx = 300, gy = 150;
        gridLines(gx, gy, cols, rows, cell);

        // Build a rectangular boundary (the "wall").
        std::vector<bool> isBoundary(cols * rows, false);
        for (int i = 2; i <= 19; i++) { isBoundary[2 * cols + i] = true; isBoundary[13 * cols + i] = true; }
        for (int j = 2; j <= 13; j++) { isBoundary[j * cols + 2] = true; isBoundary[j * cols + 19] = true; }

        // Draw the boundary cells.
        for (int j = 0; j < rows; j++)
            for (int i = 0; i < cols; i++)
                if (isBoundary[j * cols + i]) fillCell(gx, gy, i, j, cell, COL_SCENE1);

        // Fillable = anything that is NOT a boundary cell.
        std::vector<bool> fillable(cols * rows);
        for (int k = 0; k < cols * rows; k++) fillable[k] = !isBoundary[k];

        int si = 10, sj = 7;            // seed inside the wall
        std::vector<int> order = bfsFill(cols, rows, fillable, si, sj);

        int shown = (int)(lt / 95.0f);  // reveal speed (slow, easy to follow)
        for (int j = 0; j < rows; j++)
            for (int i = 0; i < cols; i++) {
                int o = order[j * cols + i];
                if (o >= 0 && o <= shown) fillCell(gx, gy, i, j, cell, COL_SCENE2);
            }
        // Seed marker.
        fillCell(gx, gy, si, sj, cell, COL_YELLOW);

        drawText(300, gy - 26,
                 "Fill every neighbour until the boundary colour is reached.",
                 COL_WHITE, false);
    }

    // ===========================================================
    //  PHASE 2 - FLOOD FILL
    // ===========================================================
    else if (t < T_SCAN) {
        float lt = t - T_FLOOD;
        drawLabel(300, WIN_HEIGHT - 100,
                  "2. FLOOD FILL  -  replace every pixel matching the seed's colour");

        int cols = 22, rows = 16; float cell = 18, gx = 300, gy = 150;
        gridLines(gx, gy, cols, rows, cell);

        // A connected "old colour" blob (the target region).
        std::vector<bool> isOld(cols * rows, false);
        float bcx = 11, bcy = 8;
        for (int j = 0; j < rows; j++)
            for (int i = 0; i < cols; i++) {
                float d = (i - bcx) * (i - bcx) / 36.0f + (j - bcy) * (j - bcy) / 16.0f;
                if (d < 1.0f) isOld[j * cols + i] = true;
            }
        // A separate patch of a DIFFERENT colour - flood must NOT touch it.
        for (int j = 3; j <= 5; j++)
            for (int i = 16; i <= 19; i++) isOld[j * cols + i] = false;

        // Draw old-colour region (pink) and the other patch (orange).
        for (int j = 0; j < rows; j++)
            for (int i = 0; i < cols; i++)
                if (isOld[j * cols + i]) fillCell(gx, gy, i, j, cell, COL_PINK);
        for (int j = 3; j <= 5; j++)
            for (int i = 16; i <= 19; i++) fillCell(gx, gy, i, j, cell, COL_ORANGE);

        // Flood fill: enter only cells that MATCH the seed's (old) colour.
        int si = 11, sj = 8;
        std::vector<int> order = bfsFill(cols, rows, isOld, si, sj);

        int shown = (int)(lt / 1000.0f);   // slow: one BFS ring per ~1s
        for (int j = 0; j < rows; j++)
            for (int i = 0; i < cols; i++) {
                int o = order[j * cols + i];
                if (o >= 0 && o <= shown) fillCell(gx, gy, i, j, cell, COL_SCENE2);
            }
        fillCell(gx, gy, si, sj, cell, COL_YELLOW);

        drawText(300, gy - 26,
                 "Like the paint-bucket: only the matching colour is replaced.",
                 COL_WHITE, false);
        drawText(300, gy - 44,
                 "(The orange patch is a different colour, so it is untouched.)",
                 COL_LIGHT_GREY, false);
    }

    // ===========================================================
    //  PHASE 3 - SCAN-LINE FILL
    // ===========================================================
    else if (t < T_DONE) {
        float lt = t - T_SCAN;
        drawLabel(300, WIN_HEIGHT - 100,
                  "3. SCAN-LINE FILL  -  sweep rows, fill between edge crossings");

        // A pentagon to fill.
        const int n = 5;
        float vx[n] = {430, 560, 520, 360, 320};
        float vy[n] = {420, 330, 180, 180, 330};
        float yTop = 420, yBot = 180;

        // Outline.
        setColor(COL_WHITE);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < n; i++) glVertex2f(vx[i], vy[i]);
        glEnd();

        // Sweep line moves top -> bottom over ~12s, then rests at bottom.
        float sweepY = yTop - (lt / 12000.0f) * (yTop - yBot);
        if (sweepY < yBot) sweepY = yBot;

        // Fill all rows from the top down to the sweep line.
        scanFillPolygon(vx, vy, n, yTop, yBot, sweepY, COL_SCENE2);

        // Draw the moving yellow scan line.
        setColor(COL_YELLOW);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(300, sweepY); glVertex2f(590, sweepY);
        glEnd();
        drawText(595, sweepY - 4, "scan line", COL_YELLOW, false);

        drawText(300, yBot - 26,
                 "No recursion - this is how GPUs fill triangles today.",
                 COL_WHITE, false);
    }

    // ===========================================================
    //  FINAL - FULLY COLOURED HOUSE (stays)
    // ===========================================================
    else {
        drawLabel(330, WIN_HEIGHT - 100, "The house is fully coloured!");
        drawColoredHouse(440, 230, 80);
        drawText(330, 150, "Beautiful! But what if I want to move it?",
                 COL_YELLOW, false);
    }

    if (t > T_DONE + 3000.0f) {
        drawText(WIN_WIDTH/2 - 80, 45,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
