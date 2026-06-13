// ============================================================
//  scenes/scene5.cpp  -  MEMBER 5 OWNS THIS FILE
//  Topic : 3D Graphics and Animation
//  Concepts implemented:
//    1. 3D Transformations - extrude the house, rotate about X/Y/Z
//    2. Projection         - Orthographic vs Perspective (side by side)
//    3. Z-Buffer           - overlapping objects, depth test OFF vs ON
//    4. Animation          - door opens, car drives, camera orbits
//
//  CRITICAL: this scene switches to a 3D projection. It saves BOTH
//  the PROJECTION and MODELVIEW matrices (glPushMatrix) and restores
//  them, the viewport, and depth-test state before returning, so
//  scenes 1-4 keep working when revisited.
// ============================================================

#include "../common.h"
#include <cstdio>    // snprintf

// ── Sub-scene timeline (milliseconds) ────────────────────────
static const float T_3D   = 1000.0f;
static const float T_PROJ = 12000.0f;
static const float T_ZBUF = 23000.0f;
static const float T_ANIM = 34000.0f;

// ── Configure a 3D viewport + projection (no GLU dependency) ──
// Operates on the CURRENTLY pushed PROJECTION/MODELVIEW matrices.
static void begin3D(int vx, int vy, int vw, int vh, bool perspective) {
    glViewport(vx, vy, vw, vh);
    float aspect = (float)vw / (float)vh;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (perspective) {
        // glFrustum gives a perspective (vanishing-point) projection.
        float n = 2.0f, f = 60.0f, top = n * 0.6f, right = top * aspect;
        glFrustum(-right, right, -top, top, n, f);
    } else {
        // glOrtho keeps parallel edges parallel (blueprint look).
        float s = 3.2f;
        glOrtho(-s * aspect, s * aspect, -s, s, -60, 60);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ── A solid coloured cube of half-size s, centred at origin ──
static void colorCube(float s, Color base) {
    // Each face slightly shaded so the 3D form reads without lighting.
    float r = base.r, g = base.g, b = base.b;
    // Front (+z)
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex3f(-s,-s, s); glVertex3f( s,-s, s); glVertex3f( s, s, s); glVertex3f(-s, s, s);
    glEnd();
    // Back (-z)
    glColor3f(r*0.5f, g*0.5f, b*0.5f);
    glBegin(GL_QUADS);
        glVertex3f(-s,-s,-s); glVertex3f(-s, s,-s); glVertex3f( s, s,-s); glVertex3f( s,-s,-s);
    glEnd();
    // Right (+x)
    glColor3f(r*0.8f, g*0.8f, b*0.8f);
    glBegin(GL_QUADS);
        glVertex3f( s,-s,-s); glVertex3f( s, s,-s); glVertex3f( s, s, s); glVertex3f( s,-s, s);
    glEnd();
    // Left (-x)
    glColor3f(r*0.6f, g*0.6f, b*0.6f);
    glBegin(GL_QUADS);
        glVertex3f(-s,-s,-s); glVertex3f(-s,-s, s); glVertex3f(-s, s, s); glVertex3f(-s, s,-s);
    glEnd();
    // Top (+y)
    glColor3f(r*0.9f, g*0.9f, b*0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-s, s,-s); glVertex3f(-s, s, s); glVertex3f( s, s, s); glVertex3f( s, s,-s);
    glEnd();
    // Bottom (-y)
    glColor3f(r*0.4f, g*0.4f, b*0.4f);
    glBegin(GL_QUADS);
        glVertex3f(-s,-s,-s); glVertex3f( s,-s,-s); glVertex3f( s,-s, s); glVertex3f(-s,-s, s);
    glEnd();
}

// ── A 3D house: cuboid body + triangular-prism roof + door ───
static void house3D(float doorOpenDeg) {
    // Body
    colorCube(1.0f, COL_SCENE5);

    // Roof: triangular prism from y=1 up to apex y=2, extruded in z.
    setColor(COL_SCENE4);
    // Two triangular gable ends (front z=+1, back z=-1)
    glBegin(GL_TRIANGLES);
        glVertex3f(-1, 1,  1); glVertex3f( 1, 1,  1); glVertex3f(0, 2,  1);
        glVertex3f(-1, 1, -1); glVertex3f( 1, 1, -1); glVertex3f(0, 2, -1);
    glEnd();
    // Two slanted roof faces
    setColor(COL_ORANGE);
    glBegin(GL_QUADS);
        glVertex3f(-1, 1, 1); glVertex3f(0, 2, 1); glVertex3f(0, 2, -1); glVertex3f(-1, 1, -1);
        glVertex3f( 1, 1, 1); glVertex3f(0, 2, 1); glVertex3f(0, 2, -1); glVertex3f( 1, 1, -1);
    glEnd();

    // Door on the front face, hinged on its left edge, swings open.
    glPushMatrix();
        glTranslatef(-0.45f, -1.0f, 1.001f);     // hinge at front-left of door
        glRotatef(doorOpenDeg, 0, 1, 0);         // swing about vertical axis
        setColor(COL_YELLOW);
        glBegin(GL_QUADS);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.5f, 0.0f, 0.0f);
            glVertex3f(0.5f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
        glEnd();
    glPopMatrix();
}

// ── A simple 3D car (body + wheels) ──────────────────────────
static void car3D() {
    glPushMatrix();
        glScalef(1.0f, 0.45f, 0.6f);
        colorCube(1.0f, COL_SCENE1);
    glPopMatrix();
    // Cabin
    glPushMatrix();
        glTranslatef(0.1f, 0.5f, 0);
        glScalef(0.5f, 0.35f, 0.5f);
        colorCube(1.0f, COL_CYAN);
    glPopMatrix();
    // Wheels
    Color tyre = {0.1f, 0.1f, 0.1f};
    float wx = 0.7f, wz = 0.62f, wy = -0.5f;
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2) {
            glPushMatrix();
                glTranslatef(sx * wx, wy, sz * wz);
                glScalef(0.25f, 0.25f, 0.12f);
                colorCube(1.0f, tyre);
            glPopMatrix();
        }
}

// Draw the X/Y/Z axes in 3D.
static void axes3D(float L) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(1, 0.3f, 0.3f); glVertex3f(0,0,0); glVertex3f(L,0,0);
        glColor3f(0.3f, 1, 0.3f); glVertex3f(0,0,0); glVertex3f(0,L,0);
        glColor3f(0.4f, 0.4f, 1); glVertex3f(0,0,0); glVertex3f(0,0,L);
    glEnd();
}

// ── scene5 ─────────────────────────────────────────────────
void scene5(float t) {

    // ---- 2D overlay (drawn while still in main's 2D state) ----
    drawHeading(20, WIN_HEIGHT - 35, "3D Graphics and Animation");

    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);

    // =========================================================
    //  ENTER 3D  -  save BOTH matrix stacks + depth state
    // =========================================================
    glMatrixMode(GL_PROJECTION); glPushMatrix();
    glMatrixMode(GL_MODELVIEW);  glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // ---------------------------------------------------------
    //  1. 3D TRANSFORMATIONS  -  rotate about X, then Y, then Z
    // ---------------------------------------------------------
    if (t < T_PROJ) {
        begin3D(0, 0, W, H, true);
        glTranslatef(0, -0.3f, -6.0f);

        float lt = t - T_3D;
        int   seg = ((int)(lt / 3500.0f)) % 3;
        float a   = lt * 0.04f;
        glRotatef(20, 1, 0, 0);                  // slight tilt to see depth
        if (seg == 0) glRotatef(a, 1, 0, 0);
        else if (seg == 1) glRotatef(a, 0, 1, 0);
        else glRotatef(a, 0, 0, 1);

        axes3D(2.5f);
        house3D(0.0f);
    }

    // ---------------------------------------------------------
    //  2. PROJECTION  -  Orthographic (left) vs Perspective (right)
    // ---------------------------------------------------------
    else if (t < T_ZBUF) {
        float a = (t - T_PROJ) * 0.025f;
        // Left half: orthographic
        begin3D(0, 0, W / 2, H, false);
        glTranslatef(0, -0.3f, -8.0f);
        glRotatef(20, 1, 0, 0); glRotatef(a, 0, 1, 0);
        house3D(0.0f);
        // Right half: perspective
        begin3D(W / 2, 0, W / 2, H, true);
        glTranslatef(0, -0.3f, -6.0f);
        glRotatef(20, 1, 0, 0); glRotatef(a, 0, 1, 0);
        house3D(0.0f);
    }

    // ---------------------------------------------------------
    //  3. Z-BUFFER  -  depth test OFF (left) vs ON (right)
    // ---------------------------------------------------------
    else if (t < T_ANIM) {
        float a = (t - T_ZBUF) * 0.02f;

        // Left half: depth test OFF -> draw order decides (wrong).
        glDisable(GL_DEPTH_TEST);
        begin3D(0, 0, W / 2, H, true);
        glTranslatef(0, 0, -7.0f);
        glRotatef(a, 0, 1, 0);
        glPushMatrix(); glTranslatef(-1.0f, 0, -1.0f); colorCube(0.9f, COL_SCENE4); glPopMatrix();
        glPushMatrix(); glTranslatef( 1.0f, 0,  1.0f); colorCube(0.9f, COL_SCENE2); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.0f, 0,  0.0f); colorCube(0.9f, COL_SCENE3); glPopMatrix();

        // Right half: depth test ON -> closest surface wins (correct).
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        begin3D(W / 2, 0, W / 2, H, true);
        glTranslatef(0, 0, -7.0f);
        glRotatef(a, 0, 1, 0);
        glPushMatrix(); glTranslatef(-1.0f, 0, -1.0f); colorCube(0.9f, COL_SCENE4); glPopMatrix();
        glPushMatrix(); glTranslatef( 1.0f, 0,  1.0f); colorCube(0.9f, COL_SCENE2); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.0f, 0,  0.0f); colorCube(0.9f, COL_SCENE3); glPopMatrix();
    }

    // ---------------------------------------------------------
    //  4. ANIMATION  -  orbiting camera, swinging door, moving car
    // ---------------------------------------------------------
    else {
        float lt = t - T_ANIM;
        begin3D(0, 0, W, H, true);

        // Orbiting camera: translate back, tilt, then rotate the world.
        glTranslatef(0, -0.5f, -8.0f);
        glRotatef(20, 1, 0, 0);
        glRotatef(lt * 0.02f, 0, 1, 0);          // camera orbit

        // Ground plane
        setColor(COL_DARK_GREY);
        glBegin(GL_QUADS);
            glVertex3f(-6, -1, -6); glVertex3f(6, -1, -6);
            glVertex3f(6, -1, 6);   glVertex3f(-6, -1, 6);
        glEnd();

        // House with a door that swings open and shut.
        float doorDeg = 60.0f * (0.5f * (1 + sin(lt * 0.002f)));
        glPushMatrix();
            glTranslatef(0, 0, 0);
            house3D(doorDeg);
        glPopMatrix();

        // A car driving past in a loop.
        float carX = fmod(lt * 0.0016f, 10.0f) - 5.0f;
        glPushMatrix();
            glTranslatef(carX, -0.55f, 3.0f);
            glScalef(0.6f, 0.6f, 0.6f);
            car3D();
        glPopMatrix();
    }

    // =========================================================
    //  EXIT 3D  -  restore EVERYTHING for scenes 1-4
    // =========================================================
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glViewport(0, 0, W, H);
    glColor3f(1, 1, 1);

    // ---- Prof. Pixel drawn AFTER 3D so he is always visible on top ----
    float wave = (t > 1200.0f) ? 0.0f : 1.0f;
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(80, 110, 0.7f, wave > 0.5f, talk);

    // ---- 2D labels on top (2D state is restored now) ----
    if (t < T_PROJ) {
        drawLabel(250, WIN_HEIGHT - 95,
                  "1. 3D TRANSFORMATIONS  -  4x4 matrices, rotate about X / Y / Z");
        float lt = t - T_3D;
        int seg = ((int)(lt / 3500.0f)) % 3;
        const char* ax[3] = { "Rotating about X", "Rotating about Y", "Rotating about Z" };
        drawText(WIN_WIDTH/2 - 50, 40, ax[seg], COL_YELLOW, false);
    } else if (t < T_ZBUF) {
        drawLabel(60, WIN_HEIGHT - 95,
                  "2. PROJECTION  -  Orthographic vs Perspective");
        drawText(WIN_WIDTH/4 - 70, 40, "ORTHOGRAPHIC (parallel edges)", COL_CYAN, false);
        drawText(3*WIN_WIDTH/4 - 70, 40, "PERSPECTIVE (vanishing point)", COL_ORANGE, false);
        // Divider
        setColor(COL_DARK_GREY);
        glLineWidth(1.0f);
        glBegin(GL_LINES); glVertex2f(WIN_WIDTH/2, 60); glVertex2f(WIN_WIDTH/2, WIN_HEIGHT-110); glEnd();
    } else if (t < T_ANIM) {
        drawLabel(60, WIN_HEIGHT - 95,
                  "3. Z-BUFFER  -  hidden-surface removal by depth");
        drawText(WIN_WIDTH/4 - 60, 40, "DEPTH TEST OFF (wrong overlap)", COL_SCENE4, false);
        drawText(3*WIN_WIDTH/4 - 60, 40, "DEPTH TEST ON (closest wins)", COL_SCENE2, false);
        setColor(COL_DARK_GREY);
        glLineWidth(1.0f);
        glBegin(GL_LINES); glVertex2f(WIN_WIDTH/2, 60); glVertex2f(WIN_WIDTH/2, WIN_HEIGHT-110); glEnd();
    } else {
        drawLabel(250, WIN_HEIGHT - 95,
                  "4. ANIMATION  -  transformation over time @ 60 fps");
        drawText(WIN_WIDTH/2 - 130, 40,
                 "Door swings, car drives, camera orbits - that is computer graphics!",
                 COL_YELLOW, false);
    }

    if (t > T_ANIM + 5000.0f) {
        drawText(WIN_WIDTH/2 - 80, 45,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
