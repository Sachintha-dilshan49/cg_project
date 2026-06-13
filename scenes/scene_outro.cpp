// ============================================================
//  scenes/scene_outro.cpp  -  MEMBER 1 OWNS THIS FILE
//  Closing sequence: the 5-stage pipeline appears left-to-right,
//  then Prof. Pixel bows and the group details fade in.
// ============================================================
#include "../common.h"

void sceneOutro(float t) {

    drawHeading(WIN_WIDTH/2 - 150, WIN_HEIGHT - 60, "From a Point to a World");

    // ── Pipeline: 5 stages revealed one by one, left to right ──
    const char* stages[] = {
        "Drawing", "Filling", "Transforms", "Clipping", "3D + Anim"
    };
    Color cols[] = { COL_SCENE1, COL_SCENE2, COL_SCENE3, COL_SCENE4, COL_SCENE5 };

    float boxW = 130, gap = 20, totalW = 5 * boxW + 4 * gap;
    float startX = WIN_WIDTH/2 - totalW/2;
    float boxY = 360, boxH = 60;

    for (int i = 0; i < 5; i++) {
        if (t < i * 700.0f) break;           // staggered reveal
        float x = startX + i * (boxW + gap);
        // Box
        setColor(cols[i]);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x, boxY); glVertex2f(x + boxW, boxY);
            glVertex2f(x + boxW, boxY + boxH); glVertex2f(x, boxY + boxH);
        glEnd();
        drawText(x + 12, boxY + boxH/2 - 4, stages[i], COL_WHITE, false);
        // Arrow to the next stage
        if (i < 4 && t > (i + 1) * 700.0f) {
            setColor(COL_LIGHT_GREY);
            glLineWidth(2.0f);
            float ax = x + boxW, ay = boxY + boxH/2;
            glBegin(GL_LINES);
                glVertex2f(ax, ay); glVertex2f(ax + gap, ay);
                glVertex2f(ax + gap, ay); glVertex2f(ax + gap - 6, ay + 4);
                glVertex2f(ax + gap, ay); glVertex2f(ax + gap - 6, ay - 4);
            glEnd();
        }
    }

    // ── Prof. Pixel bows ──────────────────────────────────────
    if (t > 4000) {
        bool bow = (fmod(t - 4000, 2000.0f) < 1000.0f);
        // a small downward bob to suggest a bow
        float by = bow ? 80 : 100;
        drawProfPixel(WIN_WIDTH/2, by, 1.0f, !bow, bow ? 0.0f : 0.6f);
    }

    // ── Closing text + group details fade in ──────────────────
    if (t > 5500) {
        drawText(WIN_WIDTH/2 - 230, 290,
                 "From a single point - to lines, shapes, colour, motion, and a 3D world.",
                 COL_WHITE, false);
        drawText(WIN_WIDTH/2 - 90, 268,
                 "That's the graphics pipeline.", COL_LIGHT_GREY, false);
    }
    if (t > 7000) {
        drawText(WIN_WIDTH/2 - 70, 230, "Thanks for watching!", COL_YELLOW, true);
        drawText(WIN_WIDTH/2 - 150, 205,
                 "Computer Graphics Project  -  Prof. Pixel's Classroom",
                 COL_LIGHT_GREY, false);
        drawText(WIN_WIDTH/2 - 80, 185,
                 "Group of 5  -  OpenGL / GLUT", COL_LIGHT_GREY, false);
    }
}
