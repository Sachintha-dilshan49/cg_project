#ifndef COMMON_H
#define COMMON_H

// ============================================================
//  common.h  —  Shared by ALL members. Do NOT modify without
//               telling the whole group.
// ============================================================

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <string>

// ── Window ──────────────────────────────────────────────────
#define WIN_WIDTH   800
#define WIN_HEIGHT  600
#define WIN_TITLE   "Computer Graphics - Prof. Pixel's Classroom"

// ── Scene IDs ───────────────────────────────────────────────
#define SCENE_INTRO   0
#define SCENE_1       1   // Member 1 - Drawing Primitives
#define SCENE_2       2   // Member 2 - Filling Algorithms
#define SCENE_3       3   // Member 3 - 2D Transformations
#define SCENE_4       4   // Member 4 - Viewing & Clipping
#define SCENE_5       5   // Member 5 - 3D Graphics & Animation
#define SCENE_OUTRO   6

// ── Shared Color Palette ────────────────────────────────────
struct Color { float r, g, b; };

const Color COL_BG         = {0.05f, 0.05f, 0.10f};
const Color COL_SCENE1     = {0.18f, 0.42f, 0.92f};
const Color COL_SCENE2     = {0.09f, 0.63f, 0.24f};
const Color COL_SCENE3     = {0.82f, 0.53f, 0.04f};
const Color COL_SCENE4     = {0.86f, 0.07f, 0.24f};
const Color COL_SCENE5     = {0.49f, 0.16f, 0.87f};
const Color COL_WHITE      = {1.00f, 1.00f, 1.00f};
const Color COL_LIGHT_GREY = {0.75f, 0.75f, 0.80f};
const Color COL_DARK_GREY  = {0.25f, 0.25f, 0.30f};
const Color COL_YELLOW     = {1.00f, 0.90f, 0.10f};
const Color COL_CYAN       = {0.10f, 0.90f, 0.90f};
const Color COL_ORANGE     = {1.00f, 0.55f, 0.10f};
const Color COL_PINK       = {1.00f, 0.40f, 0.70f};

inline void setColor(Color c) { glColor3f(c.r, c.g, c.b); }

// ── Global State ─────────────────────────────────────────────
// animTime: milliseconds since current scene started. READ ONLY for members 2-5.
// currentScene: which scene is active. READ ONLY for all members except Member 1.
extern float animTime;
extern int   currentScene;
inline void resetTimer() { animTime = 0.0f; }

// ── Text Rendering ───────────────────────────────────────────
// Use ONLY these functions for text. Do not use other fonts.
inline void drawText(float x, float y, const std::string& text,
                     Color c = COL_WHITE, bool large = false) {
    setColor(c);
    glRasterPos2f(x, y);
    void* font = large ? (void*)GLUT_BITMAP_HELVETICA_18
                       : (void*)GLUT_BITMAP_HELVETICA_12;
    for (char ch : text) glutBitmapCharacter(font, ch);
}
inline void drawLabel(float x, float y, const std::string& text) {
    drawText(x, y, text, COL_YELLOW, false);
}
inline void drawHeading(float x, float y, const std::string& text) {
    drawText(x, y, text, COL_WHITE, true);
}

// ── Grid & Axes Helpers ──────────────────────────────────────
inline void drawGrid(int step = 50) {
    setColor(COL_DARK_GREY);
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    for (int x = 0; x <= WIN_WIDTH;  x += step) {
        glVertex2f(x, 0); glVertex2f(x, WIN_HEIGHT);
    }
    for (int y = 0; y <= WIN_HEIGHT; y += step) {
        glVertex2f(0, y); glVertex2f(WIN_WIDTH, y);
    }
    glEnd();
}
inline void drawAxes(float ox, float oy, float len) {
    glLineWidth(1.5f);
    setColor({1.0f, 0.3f, 0.3f});
    glBegin(GL_LINES); glVertex2f(ox,oy); glVertex2f(ox+len,oy); glEnd();
    drawLabel(ox+len+5, oy-5, "x");
    setColor({0.3f, 1.0f, 0.3f});
    glBegin(GL_LINES); glVertex2f(ox,oy); glVertex2f(ox,oy+len); glEnd();
    drawLabel(ox-15, oy+len+5, "y");
}

// ── Prof. Pixel (implemented in utils/profpixel.cpp) ────────
void drawProfPixel(float cx, float cy, float scale = 1.0f,
                   bool waving = false, float mouthOpen = 0.0f);

// ── Scene Declarations (each member implements their own) ────
void sceneIntro (float t);
void scene1     (float t);
void scene2     (float t);
void scene3     (float t);
void scene4     (float t);
void scene5     (float t);
void sceneOutro (float t);

#endif
