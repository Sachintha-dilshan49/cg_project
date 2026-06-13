// ============================================================
//  scenes/scene_intro.cpp  -  MEMBER 1 OWNS THIS FILE
//  Opening sequence: a single point grows into a whole world.
//    point -> line -> square -> filled square -> 3D box -> title
//  Then Prof. Pixel walks in and waves.
// ============================================================
#include "../common.h"

// Centre of the morphing shape.
static const float CX = 400.0f, CY = 330.0f;
static const float HS = 60.0f;             // half-size of the square

void sceneIntro(float t) {

    // ── Title (always visible, fades in via the shapes below) ──
    drawHeading(WIN_WIDTH/2 - 130, WIN_HEIGHT - 70, "COMPUTER GRAPHICS");
    drawText(WIN_WIDTH/2 - 110, WIN_HEIGHT - 92,
             "Prof. Pixel's Classroom", COL_LIGHT_GREY, false);

    // ── Phase 1 (0 - 1.5s): a glowing point ───────────────────
    setColor(COL_YELLOW);
    glPointSize(8.0f + 4.0f * sin(t * 0.01f));
    glBegin(GL_POINTS); glVertex2f(CX - HS, CY - HS); glEnd();
    glPointSize(1.0f);

    // ── Phase 2 (1.5 - 3s): extend into a line ────────────────
    if (t > 1500) {
        float p = (t - 1500) / 1500.0f; if (p > 1) p = 1;
        setColor(COL_WHITE); glLineWidth(2.5f);
        glBegin(GL_LINES);
            glVertex2f(CX - HS, CY - HS);
            glVertex2f(CX - HS + p * 2 * HS, CY - HS);
        glEnd();
    }

    // ── Phase 3 (3 - 4.5s): trace a square outline ────────────
    if (t > 3000) {
        float p = (t - 3000) / 1500.0f; if (p > 1) p = 1;
        setColor(COL_SCENE1); glLineWidth(2.5f);
        // Draw the 4 edges progressively.
        float perim = p * 4.0f;
        glBegin(GL_LINE_STRIP);
            glVertex2f(CX - HS, CY - HS);
            if (perim > 1) glVertex2f(CX + HS, CY - HS);
            if (perim > 2) glVertex2f(CX + HS, CY + HS);
            if (perim > 3) glVertex2f(CX - HS, CY + HS);
            if (perim >= 4) glVertex2f(CX - HS, CY - HS);
        glEnd();
    }

    // ── Phase 4 (4.5 - 6s): fill the square with colour ───────
    if (t > 4500) {
        float p = (t - 4500) / 1500.0f; if (p > 1) p = 1;
        setColor(COL_SCENE2);
        glBegin(GL_QUADS);
            glVertex2f(CX - HS, CY - HS);
            glVertex2f(CX + HS, CY - HS);
            glVertex2f(CX + HS, CY - HS + p * 2 * HS);
            glVertex2f(CX - HS, CY - HS + p * 2 * HS);
        glEnd();
    }

    // ── Phase 5 (6 - 8s): extrude into a 3D box (oblique) ─────
    if (t > 6000) {
        float p = (t - 6000) / 2000.0f; if (p > 1) p = 1;
        float d = p * 40.0f;                 // depth offset
        setColor(COL_SCENE5); glLineWidth(2.0f);
        // Back square
        glBegin(GL_LINE_LOOP);
            glVertex2f(CX - HS + d, CY - HS + d);
            glVertex2f(CX + HS + d, CY - HS + d);
            glVertex2f(CX + HS + d, CY + HS + d);
            glVertex2f(CX - HS + d, CY + HS + d);
        glEnd();
        // Connecting edges
        glBegin(GL_LINES);
            glVertex2f(CX - HS, CY - HS); glVertex2f(CX - HS + d, CY - HS + d);
            glVertex2f(CX + HS, CY - HS); glVertex2f(CX + HS + d, CY - HS + d);
            glVertex2f(CX + HS, CY + HS); glVertex2f(CX + HS + d, CY + HS + d);
            glVertex2f(CX - HS, CY + HS); glVertex2f(CX - HS + d, CY + HS + d);
        glEnd();
    }

    // ── Phase 6 (8s+): Prof. Pixel walks in and waves ─────────
    if (t > 8000) {
        // Walk in from the right edge to a resting spot.
        float p = (t - 8000) / 1500.0f; if (p > 1) p = 1;
        float px = WIN_WIDTH + 50 - p * 230;
        bool wave = (fmod(t, 800.0f) < 400.0f);
        drawProfPixel(px, 90, 1.0f, wave, 0.7f);
        if (p >= 1) {
            drawText(WIN_WIDTH/2 - 160, 90,
                     "Welcome to my classroom - just pixels and mathematics.",
                     COL_WHITE, false);
            drawText(WIN_WIDTH/2 - 90, 68,
                     "Five lessons. Let's begin!", COL_YELLOW, false);
            drawText(WIN_WIDTH/2 - 80, 45,
                     "Press SPACE to begin", COL_LIGHT_GREY, false);
        }
    }
}
