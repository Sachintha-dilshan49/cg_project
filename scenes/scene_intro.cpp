// ============================================================
//  scenes/scene_intro.cpp  -  MEMBER 1 OWNS THIS FILE
//  Opening sequence: a point grows into a world.
// ============================================================
#include "../common.h"

void sceneIntro(float t) {
    // Phase 1 (0-2s): single point appears
    // Phase 2 (2-4s): extends into a line
    // Phase 3 (4-6s): line becomes a house outline
    // Phase 4 (6-8s): title text fades in
    // Phase 5 (8s+):  Prof. Pixel walks in, waves

    drawHeading(WIN_WIDTH/2 - 160, WIN_HEIGHT/2 + 60,
                "COMPUTER GRAPHICS");
    drawText(WIN_WIDTH/2 - 130, WIN_HEIGHT/2 + 30,
             "Prof. Pixel's Classroom", COL_LIGHT_GREY, false);

    if (t > 2000) {
        // Show a growing point -> line animation
        float progress = (t - 2000) / 3000.0f;
        if (progress > 1.0f) progress = 1.0f;
        setColor(COL_YELLOW);
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        glVertex2f(WIN_WIDTH/2, WIN_HEIGHT/2 - 30);
        glEnd();
        setColor(COL_WHITE);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(WIN_WIDTH/2, WIN_HEIGHT/2 - 30);
        glVertex2f(WIN_WIDTH/2 + progress * 100, WIN_HEIGHT/2 - 30);
        glEnd();
    }

    if (t > 6000) {
        float wave = (fmod(t, 800.0f) < 400.0f);
        drawProfPixel(WIN_WIDTH - 150, 100, 1.0f, wave, 0.7f);
        drawText(WIN_WIDTH - 220, 270,
                 "Press SPACE to begin!", COL_YELLOW, false);
    }
}
