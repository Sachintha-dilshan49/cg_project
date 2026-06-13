// ============================================================
//  scenes/scene_outro.cpp  -  MEMBER 1 OWNS THIS FILE
//  Closing sequence: pipeline recap, Prof. Pixel bows.
// ============================================================
#include "../common.h"

void sceneOutro(float t) {
    drawHeading(WIN_WIDTH/2 - 160, WIN_HEIGHT - 50,
                "From a Point to a World");

    // Show all 5 concept labels in their scene colors
    const char* labels[] = {
        "1. Drawing Primitives",
        "2. Filling Algorithms",
        "3. 2D Transformations",
        "4. Viewing & Clipping",
        "5. 3D & Animation"
    };
    Color cols[] = {
        COL_SCENE1, COL_SCENE2, COL_SCENE3, COL_SCENE4, COL_SCENE5
    };
    for (int i = 0; i < 5; i++) {
        float delay = i * 1000.0f;
        if (t > delay) {
            drawText(WIN_WIDTH/2 - 120,
                     WIN_HEIGHT - 120 - i * 50,
                     labels[i], cols[i], true);
        }
    }

    if (t > 6000) {
        bool bow = (fmod(t - 6000, 2000.0f) < 1000.0f);
        drawProfPixel(WIN_WIDTH/2, 100, 1.0f, !bow, 0.0f);
        drawText(WIN_WIDTH/2 - 80, 30,
                 "Thank you!", COL_YELLOW, true);
    }
}
