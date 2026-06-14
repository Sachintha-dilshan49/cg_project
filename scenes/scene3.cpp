// ============================================================
//  scenes/scene3.cpp  -  MEMBER 3 OWNS THIS FILE
//  Topic : 2D Transformations
//  Concepts implemented (OpenGL matrices + a live matrix panel):
//    1. Translation  2. Rotation (about a pivot)
//    3. Scaling (uniform + non-uniform)
//    4. Reflection (x-axis, y-axis, y=x)
//    5. Shearing (horizontal + vertical)
//  Finishes by CHAINING several transforms together.
//
//  glPushMatrix / glPopMatrix are used around every transform so
//  the base coordinate frame is always restored.
// ============================================================

#include "../common.h"
#include <cstdio>    // snprintf

// ── Sub-scene timeline (milliseconds) ────────────────────────
static const float T_TRANS   = 1000.0f;
static const float T_ROT     = 10000.0f;
static const float T_SCALE   = 19000.0f;
static const float T_REFLECT = 28000.0f;
static const float T_SHEAR   = 39000.0f;
static const float T_COMBO   = 49000.0f;   // chained transforms (loops/stays)

// Base position: the house's "home" frame on screen.
static const float BX = 430.0f, BY = 300.0f;

// ── A small coloured house centred on the local origin ───────
// Drawn in local coordinates so the current GL matrix transforms it.
static void drawHouseLocal(bool ghost) {
    Color body = ghost ? COL_DARK_GREY : COL_SCENE3;
    Color roof = ghost ? COL_DARK_GREY : COL_SCENE4;

    // Wall
    setColor(body);
    glBegin(GL_QUADS);
        glVertex2f(-45, -45); glVertex2f(45, -45);
        glVertex2f(45, 45);   glVertex2f(-45, 45);
    glEnd();
    // Roof
    setColor(roof);
    glBegin(GL_TRIANGLES);
        glVertex2f(-45, 45); glVertex2f(45, 45); glVertex2f(0, 85);
    glEnd();
    if (!ghost) {
        // Door
        setColor(COL_YELLOW);
        glBegin(GL_QUADS);
            glVertex2f(-12, -45); glVertex2f(12, -45);
            glVertex2f(12, 5);    glVertex2f(-12, 5);
        glEnd();
        // Window
        setColor(COL_CYAN);
        glBegin(GL_QUADS);
            glVertex2f(15, 10); glVertex2f(40, 10);
            glVertex2f(40, 35); glVertex2f(15, 35);
        glEnd();
    }
    // Outline
    setColor(COL_WHITE);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-45, -45); glVertex2f(45, -45);
        glVertex2f(45, 45);   glVertex2f(0, 85); glVertex2f(-45, 45);
    glEnd();
}

// ── Live 3x3 matrix panel (homogeneous 2D form, row-major) ───
static void matPanel(const char* title, const float m[9]) {
    float px = 600.0f, py = 360.0f;
    drawLabel(px, py + 30, title);
    setColor(COL_WHITE);
    for (int r = 0; r < 3; r++) {
        char line[64];
        snprintf(line, sizeof(line), "| %6.2f  %6.2f  %6.2f |",
                 m[r * 3 + 0], m[r * 3 + 1], m[r * 3 + 2]);
        drawText(px, py - r * 18, line, COL_WHITE, false);
    }
}

// ── scene3 ─────────────────────────────────────────────────
void scene3(float t) {

    drawHeading(20, WIN_HEIGHT - 35, "2D Transformations");

    // Background first: faint reference grid + axes at the home frame.
    drawGrid(40);
    drawAxes(BX, BY, 120);

    // A grey "ghost" house at home shows the original position.
    glPushMatrix();
        glTranslatef(BX, BY, 0);
        drawHouseLocal(true);
    glPopMatrix();

    // Prof. Pixel drawn AFTER the background so he is never covered.
    float wave = (t > 1200.0f) ? 0.0f : 1.0f;
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(110, 110, 0.9f, wave > 0.5f, talk);

    // ===========================================================
    //  1. TRANSLATION
    // ===========================================================
    if (t < T_ROT) {
        drawText(30, WIN_HEIGHT - 95,
                 "1. TRANSLATION  -  add (tx, ty) to every coordinate",
                 COL_YELLOW, true);
        float p = sin((t - T_TRANS) * 0.0012f);     // -1..1
        float tx = p * 130.0f, ty = p * 40.0f;

        glPushMatrix();
            glTranslatef(BX + tx, BY + ty, 0);       // move the house
            drawHouseLocal(false);
        glPopMatrix();

        float m[9] = { 1,0,tx,  0,1,ty,  0,0,1 };
        matPanel("Translation matrix", m);
    }

    // ===========================================================
    //  2. ROTATION (about the house's pivot = home origin)
    // ===========================================================
    else if (t < T_SCALE) {
        drawText(30, WIN_HEIGHT - 95,
                 "2. ROTATION  -  spin about a pivot using sin & cos",
                 COL_YELLOW, true);
        float ang = (t - T_ROT) * 0.035f;            // degrees
        float r   = ang * M_PI / 180.0f;

        glPushMatrix();
            glTranslatef(BX, BY, 0);
            glRotatef(ang, 0, 0, 1);                  // rotate about pivot
            drawHouseLocal(false);
        glPopMatrix();

        float c = cos(r), s = sin(r);
        float m[9] = { c,-s,0,  s,c,0,  0,0,1 };
        matPanel("Rotation matrix", m);
    }

    // ===========================================================
    //  3. SCALING (uniform first, then non-uniform)
    // ===========================================================
    else if (t < T_REFLECT) {
        float lt = t - T_SCALE;
        float sx, sy;
        if (lt < 4500.0f) {                          // uniform
            float k = 0.6f + 0.4f * (1 + sin(lt * 0.0016f));
            sx = sy = k;
            drawText(30, WIN_HEIGHT - 95,
                     "3. SCALING  -  uniform: multiply x and y equally",
                     COL_YELLOW, true);
        } else {                                      // non-uniform
            sx = 0.6f + 0.6f * (1 + sin(lt * 0.0016f));
            sy = 0.8f;
            drawText(30, WIN_HEIGHT - 95,
                     "3. SCALING  -  non-uniform: stretch / squish",
                     COL_YELLOW, true);
        }
        glPushMatrix();
            glTranslatef(BX, BY, 0);
            glScalef(sx, sy, 1);
            drawHouseLocal(false);
        glPopMatrix();

        float m[9] = { sx,0,0,  0,sy,0,  0,0,1 };
        matPanel("Scaling matrix", m);
    }

    // ===========================================================
    //  4. REFLECTION (cycles x-axis, y-axis, y = x)
    // ===========================================================
    else if (t < T_SHEAR) {
        float lt = t - T_REFLECT;
        int   mode = ((int)(lt / 3500.0f)) % 3;      // which axis
        const char* names[3] = {
            "4. REFLECTION  -  flip across the X-axis",
            "4. REFLECTION  -  flip across the Y-axis",
            "4. REFLECTION  -  flip across the line y = x" };
        drawText(30, WIN_HEIGHT - 95, names[mode], COL_YELLOW, true);

        // Draw the mirror axis (red) through the home origin.
        setColor(COL_SCENE4);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        if (mode == 0)      { glVertex2f(BX - 160, BY); glVertex2f(BX + 160, BY); }
        else if (mode == 1) { glVertex2f(BX, BY - 160); glVertex2f(BX, BY + 160); }
        else                { glVertex2f(BX - 150, BY - 150); glVertex2f(BX + 150, BY + 150); }
        glEnd();

        float m[9];
        glPushMatrix();
            glTranslatef(BX, BY, 0);
            if (mode == 0) {                          // reflect across X (y -> -y)
                glScalef(1, -1, 1);
                float t0[9] = { 1,0,0,  0,-1,0,  0,0,1 };
                for (int i = 0; i < 9; i++) m[i] = t0[i];
            } else if (mode == 1) {                   // reflect across Y (x -> -x)
                glScalef(-1, 1, 1);
                float t0[9] = { -1,0,0,  0,1,0,  0,0,1 };
                for (int i = 0; i < 9; i++) m[i] = t0[i];
            } else {                                   // reflect across y = x (swap)
                // column-major 4x4 swapping x and y
                float swap[16] = { 0,1,0,0,  1,0,0,0,  0,0,1,0,  0,0,0,1 };
                glMultMatrixf(swap);
                float t0[9] = { 0,1,0,  1,0,0,  0,0,1 };
                for (int i = 0; i < 9; i++) m[i] = t0[i];
            }
            drawHouseLocal(false);
        glPopMatrix();

        matPanel("Reflection matrix", m);
    }

    // ===========================================================
    //  5. SHEARING (horizontal first, then vertical)
    // ===========================================================
    else if (t < T_COMBO) {
        float lt = t - T_SHEAR;
        float sh = 0.6f * sin(lt * 0.0016f);
        float m[9];
        glPushMatrix();
            glTranslatef(BX, BY, 0);
            if (lt < 5000.0f) {                       // horizontal: x' = x + sh*y
                float shear[16] = { 1,0,0,0,  sh,1,0,0,  0,0,1,0,  0,0,0,1 };
                glMultMatrixf(shear);
                float t0[9] = { 1,sh,0,  0,1,0,  0,0,1 };
                for (int i = 0; i < 9; i++) m[i] = t0[i];
                drawText(30, WIN_HEIGHT - 95,
                         "5. SHEARING  -  horizontal: x' = x + sh*y (italics!)",
                         COL_YELLOW, true);
            } else {                                  // vertical: y' = y + sh*x
                float shear[16] = { 1,sh,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1 };
                glMultMatrixf(shear);
                float t0[9] = { 1,0,0,  sh,1,0,  0,0,1 };
                for (int i = 0; i < 9; i++) m[i] = t0[i];
                drawText(30, WIN_HEIGHT - 95,
                         "5. SHEARING  -  vertical: y' = y + sh*x",
                         COL_YELLOW, true);
            }
            drawHouseLocal(false);
        glPopMatrix();

        matPanel("Shear matrix", m);
    }

    // ===========================================================
    //  CHAINED TRANSFORMS (translate * rotate * scale)
    // ===========================================================
    else {
        float lt = t - T_COMBO;
        drawText(30, WIN_HEIGHT - 95,
                 "CHAINING  -  multiply matrices to combine all transforms",
                 COL_YELLOW, true);
        float ang = lt * 0.03f;
        float k   = 0.7f + 0.25f * (1 + sin(lt * 0.0016f));
        float tx  = 90.0f * sin(lt * 0.0012f);

        glPushMatrix();
            glTranslatef(BX + tx, BY, 0);   // T
            glRotatef(ang, 0, 0, 1);        // * R
            glScalef(k, k, 1);              // * S
            drawHouseLocal(false);
        glPopMatrix();

        drawText(560, 320, "M = T . R . S", COL_YELLOW, true);
        drawText(180, 120,
                 "Each transform is a matrix - chain them by multiplying.",
                 COL_WHITE, true);
        drawText(180, 95, "But what if the scene is bigger than the screen?",
                 COL_YELLOW, true);
    }

    if (t > T_COMBO + 3000.0f) {
        drawText(WIN_WIDTH/2 - 80, 45,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
