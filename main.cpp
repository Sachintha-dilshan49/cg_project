// ============================================================
//  main.cpp  —  MEMBER 1 OWNS THIS FILE
//  Do NOT edit unless you are Member 1.
//  This is the entry point and scene manager for the whole project.
// ============================================================

#include "common.h"
#include <iostream>

// ── Global state (declared extern in common.h) ───────────────
float animTime    = 0.0f;
int   currentScene = SCENE_INTRO;

// ── Timer callback ───────────────────────────────────────────
// Called every TIMER_MS milliseconds. Advances animTime and
// triggers a redraw. Do NOT modify.
#define TIMER_MS 16   // ~60 fps

void timerCallback(int) {
    animTime += TIMER_MS;
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timerCallback, 0);
}

// ── Scene switcher ───────────────────────────────────────────
void switchScene(int scene) {
    currentScene = scene;
    resetTimer();
    glutPostRedisplay();
}

// ── Keyboard controls ────────────────────────────────────────
// 0 = Intro, 1-5 = scenes, 6 = Outro
// SPACE = next scene, ESC = quit
void keyboardCallback(unsigned char key, int, int) {
    switch (key) {
        case '0': switchScene(SCENE_INTRO);  break;
        case '1': switchScene(SCENE_1);      break;
        case '2': switchScene(SCENE_2);      break;
        case '3': switchScene(SCENE_3);      break;
        case '4': switchScene(SCENE_4);      break;
        case '5': switchScene(SCENE_5);      break;
        case '6': switchScene(SCENE_OUTRO);  break;
        case ' ':
            // Auto-advance to next scene
            if (currentScene < SCENE_OUTRO)
                switchScene(currentScene + 1);
            break;
        case 27: // ESC
            exit(0);
    }
}

// ── Display callback ─────────────────────────────────────────
void display() {
    // Clear screen with shared background color
    glClearColor(COL_BG.r, COL_BG.g, COL_BG.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset to 2D orthographic projection for all 2D scenes
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw the scene label bottom-left (always visible)
    const char* sceneNames[] = {
        "INTRO",
        "Scene 1: Drawing Primitives",
        "Scene 2: Filling Algorithms",
        "Scene 3: 2D Transformations",
        "Scene 4: Viewing & Clipping",
        "Scene 5: 3D Graphics & Animation",
        "OUTRO"
    };
    drawText(10, 10, sceneNames[currentScene], COL_DARK_GREY, false);
    drawText(10, 25, "SPACE = next scene  |  0-6 = jump to scene  |  ESC = quit",
             COL_DARK_GREY, false);

    // Route to the correct scene function
    switch (currentScene) {
        case SCENE_INTRO:  sceneIntro(animTime);  break;
        case SCENE_1:      scene1(animTime);       break;
        case SCENE_2:      scene2(animTime);       break;
        case SCENE_3:      scene3(animTime);       break;
        case SCENE_4:      scene4(animTime);       break;
        case SCENE_5:      scene5(animTime);       break;
        case SCENE_OUTRO:  sceneOutro(animTime);   break;
    }

    glutSwapBuffers();
}

// ── Reshape callback ─────────────────────────────────────────
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// ── Main ─────────────────────────────────────────────────────
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(WIN_TITLE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(TIMER_MS, timerCallback, 0);

    std::cout << "Prof. Pixel's Classroom loaded.\n";
    std::cout << "Press 0=Intro  1-5=Scenes  6=Outro  SPACE=Next  ESC=Quit\n";

    glutMainLoop();
    return 0;
}
