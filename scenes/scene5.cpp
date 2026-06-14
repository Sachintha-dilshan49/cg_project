// ============================================================
//  scenes/scene5.cpp  -  MEMBER 5
//  Topic : 3D Graphics and Animation
//  Concepts implemented:
//    1. 3D Transformations  - rotate around X/Y/Z axes
//    2. Projection          - orthographic vs perspective
//    3. Z-Buffer            - depth testing correct vs incorrect
//    4. Animation           - moving scene with render pipeline stages
// ============================================================
#define GL_SILENCE_DEPRECATION
#include "../common.h"
#include <cstdio>
#include <cmath>

// ── Sub-scene timeline (milliseconds) ────────────────────────
static const float T_TRANSFORM = 1000.0f;
static const float T_PROJ      = 12000.0f;
static const float T_ZBUF      = 24000.0f;
static const float T_ANIM      = 36000.0f;

// ── Content area (right side, leaving left for Prof. Pixel) ──
// Prof takes roughly x=0..220, so content starts at x=230
static const float CX0 = 230.0f;   // content left edge
static const float CX1 = 780.0f;   // content right edge
static const float CY0 =  60.0f;   // content bottom edge
static const float CY1 = 520.0f;   // content top edge
static const float CCX = (CX0+CX1)*0.5f;  // content centre X
static const float CCY = (CY0+CY1)*0.5f;  // content centre Y

// ── 3D viewport sits inside the content area ─────────────────
static const int V3X = (int)CX0;
static const int V3Y = (int)CY0;
static const int V3W = (int)(CX1-CX0);
static const int V3H = (int)(CY1-CY0);

// ── Utilities ─────────────────────────────────────────────────
static float s5_smoothstep(float t){
    if(t<=0.f)return 0.f; if(t>=1.f)return 1.f;
    return t*t*(3.f-2.f*t);
}
static float s5_clamp01(float t){ return t<0.f?0.f:t>1.f?1.f:t; }

// ── 2D helpers (same style as scene4) ────────────────────────
static void s5_solidLine(float x0,float y0,float x1,float y1,Color c,float lw){
    setColor(c);
    glLineWidth(lw);
    glBegin(GL_LINES); glVertex2f(x0,y0); glVertex2f(x1,y1); glEnd();
    glLineWidth(1.f);
}
static void s5_dashedRect(float x0,float y0,float x1,float y1,Color c,float lw){
    setColor(c);
    glLineWidth(lw);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2,0x00FF);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x0,y0); glVertex2f(x1,y0);
    glVertex2f(x1,y1); glVertex2f(x0,y1);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}
static void s5_filledRect(float x0,float y0,float x1,float y1,Color c){
    setColor(c);
    glBegin(GL_QUADS);
    glVertex2f(x0,y0); glVertex2f(x1,y0);
    glVertex2f(x1,y1); glVertex2f(x0,y1);
    glEnd();
}
static void s5_badge(float x,float y,float w,float h,Color border){
    // dark fill
    glColor4f(0.04f,0.04f,0.10f,0.88f);
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    // border
    setColor(border);
    glLineWidth(2.f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    glLineWidth(1.f);
}

// ── Map a logical (800x600) rectangle to the ACTUAL window ────
// Lets the 3D panels fill the window correctly when it is resized
// or maximised, and reports the true pixel aspect ratio so the 3D
// projection does not stretch.
static void s5_setViewport(int lx, int ly, int lw, int lh, float* outAspect)
{
    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);
    int px = lx * W / WIN_WIDTH,  py = ly * H / WIN_HEIGHT;
    int pw = lw * W / WIN_WIDTH,  ph = lh * H / WIN_HEIGHT;
    glViewport(px, py, pw, ph);
    if (outAspect) *outAspect = (ph > 0) ? (float)pw / (float)ph : 1.0f;
}

// ── begin3D: set viewport + projection for RIGHT panel ────────
static void begin3D_right(bool perspective)
{
    float aspect;
    s5_setViewport(V3X, V3Y, V3W, V3H, &aspect);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(perspective){
        float n=1.5f, f=80.f, fovY=52.f;
        float top = n*tanf(fovY*0.5f*(float)M_PI/180.f);
        float right = top*aspect;
        glFrustum(-right,right,-top,top,n,f);
    } else {
        float s=3.5f;
        glOrtho(-s*aspect,s*aspect,-s,s,-60,60);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// begin3D with custom viewport (for split-panel demos)
static void begin3D_vp(int vx,int vy,int vw,int vh,bool perspective)
{
    float aspect;
    s5_setViewport(vx,vy,vw,vh,&aspect);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(perspective){
        float n=1.5f,f=80.f,fovY=52.f;
        float top=n*tanf(fovY*0.5f*(float)M_PI/180.f);
        float right=top*aspect;
        glFrustum(-right,right,-top,top,n,f);
    } else {
        float s=3.5f;
        glOrtho(-s*aspect,s*aspect,-s,s,-60,60);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ── Restore clean 2D state after 3D rendering ─────────────────
static void restore2D_s5()
{
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,WIN_WIDTH,0,WIN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(1.f);
    glPointSize(1.f);
}

// ── 3D Geometry ───────────────────────────────────────────────
static void s5_colorCube(float s, Color col)
{
    float r=col.r, g=col.g, b=col.b;
    // Front
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
    glVertex3f(-s,-s,s); glVertex3f(s,-s,s);
    glVertex3f(s,s,s);   glVertex3f(-s,s,s);
    glEnd();
    // Back
    glColor3f(r*.45f,g*.45f,b*.45f);
    glBegin(GL_QUADS);
    glVertex3f(-s,-s,-s); glVertex3f(-s,s,-s);
    glVertex3f(s,s,-s);   glVertex3f(s,-s,-s);
    glEnd();
    // Right
    glColor3f(r*.75f,g*.75f,b*.75f);
    glBegin(GL_QUADS);
    glVertex3f(s,-s,-s); glVertex3f(s,s,-s);
    glVertex3f(s,s,s);   glVertex3f(s,-s,s);
    glEnd();
    // Left
    glColor3f(r*.55f,g*.55f,b*.55f);
    glBegin(GL_QUADS);
    glVertex3f(-s,-s,-s); glVertex3f(-s,-s,s);
    glVertex3f(-s,s,s);   glVertex3f(-s,s,-s);
    glEnd();
    // Top
    glColor3f(r*.90f,g*.90f,b*.90f);
    glBegin(GL_QUADS);
    glVertex3f(-s,s,-s); glVertex3f(-s,s,s);
    glVertex3f(s,s,s);   glVertex3f(s,s,-s);
    glEnd();
    // Bottom
    glColor3f(r*.35f,g*.35f,b*.35f);
    glBegin(GL_QUADS);
    glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s);
    glVertex3f(s,-s,s);   glVertex3f(-s,-s,s);
    glEnd();
}

static void s5_ground(float size, float step)
{
    glColor3f(0.10f,0.16f,0.12f);
    glBegin(GL_QUADS);
    glVertex3f(-size,-1.f,-size); glVertex3f(size,-1.f,-size);
    glVertex3f(size,-1.f,size);   glVertex3f(-size,-1.f,size);
    glEnd();
    glColor3f(0.22f,0.30f,0.24f);
    glLineWidth(1.f);
    glBegin(GL_LINES);
    for(float i=-size;i<=size;i+=step){
        glVertex3f(i,-0.99f,-size); glVertex3f(i,-0.99f,size);
        glVertex3f(-size,-0.99f,i); glVertex3f(size,-0.99f,i);
    }
    glEnd();
}

static void s5_axes(float L)
{
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glColor3f(1.f,0.25f,0.25f); glVertex3f(0,0,0); glVertex3f(L,0,0);
    glColor3f(0.25f,1.f,0.25f); glVertex3f(0,0,0); glVertex3f(0,L,0);
    glColor3f(0.35f,0.55f,1.f); glVertex3f(0,0,0); glVertex3f(0,0,L);
    glEnd();
    glLineWidth(1.f);
}

static void s5_house(float doorDeg)
{
    // Walls
    Color wc={0.92f,0.82f,0.60f}; s5_colorCube(1.0f,wc);
    // Roof gables
    glColor3f(0.78f,0.14f,0.18f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(0,2,1);
    glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(0,2,-1);
    glEnd();
    // Roof slopes
    glColor3f(0.88f,0.18f,0.20f);
    glBegin(GL_QUADS);
    glVertex3f(-1,1,1); glVertex3f(0,2,1); glVertex3f(0,2,-1); glVertex3f(-1,1,-1);
    glEnd();
    glColor3f(0.68f,0.10f,0.14f);
    glBegin(GL_QUADS);
    glVertex3f(1,1,1); glVertex3f(0,2,1); glVertex3f(0,2,-1); glVertex3f(1,1,-1);
    glEnd();
    // Ridge
    glColor3f(0.50f,0.08f,0.10f);
    glLineWidth(2.f);
    glBegin(GL_LINES);
    glVertex3f(0,2,1); glVertex3f(0,2,-1);
    glEnd();
    glLineWidth(1.f);
    // Windows
    glColor3f(0.25f,0.60f,0.95f);
    glBegin(GL_QUADS);
    glVertex3f(-0.75f,-0.15f,1.01f); glVertex3f(-0.30f,-0.15f,1.01f);
    glVertex3f(-0.30f,0.35f,1.01f);  glVertex3f(-0.75f,0.35f,1.01f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(0.30f,-0.15f,1.01f); glVertex3f(0.75f,-0.15f,1.01f);
    glVertex3f(0.75f,0.35f,1.01f);  glVertex3f(0.30f,0.35f,1.01f);
    glEnd();
    // Door frame
    glColor3f(0.35f,0.20f,0.08f);
    glBegin(GL_QUADS);
    glVertex3f(-0.20f,-1,1.01f); glVertex3f(0.20f,-1,1.01f);
    glVertex3f(0.20f,0.3f,1.01f); glVertex3f(-0.20f,0.3f,1.01f);
    glEnd();
    // Door leaf
    glPushMatrix();
    glTranslatef(-0.20f,-1,1.01f);
    glRotatef(doorDeg,0,1,0);
    glColor3f(0.50f,0.28f,0.10f);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0); glVertex3f(0.40f,0,0);
    glVertex3f(0.40f,1.30f,0); glVertex3f(0,1.30f,0);
    glEnd();
    glColor3f(1.f,0.85f,0.15f);
    glBegin(GL_QUADS);
    glVertex3f(0.32f,0.55f,0.02f); glVertex3f(0.36f,0.55f,0.02f);
    glVertex3f(0.36f,0.72f,0.02f); glVertex3f(0.32f,0.72f,0.02f);
    glEnd();
    glPopMatrix();
    // Chimney
    glPushMatrix();
    glTranslatef(-0.55f,1.6f,-0.3f);
    glScalef(0.20f,0.55f,0.20f);
    Color ch={0.55f,0.45f,0.35f}; s5_colorCube(1.f,ch);
    glPopMatrix();
}

static void s5_cylinder(float r,float h,int seg)
{
    float h2=h*.5f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,h2,0);
    for(int i=0;i<=seg;i++){
        float a=i*(float)M_PI*2.f/seg;
        glVertex3f(cosf(a)*r,h2,sinf(a)*r);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0,-h2,0);
    for(int i=seg;i>=0;i--){
        float a=i*(float)M_PI*2.f/seg;
        glVertex3f(cosf(a)*r,-h2,sinf(a)*r);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i=0;i<=seg;i++){
        float a=i*(float)M_PI*2.f/seg;
        float cx=cosf(a)*r,cz=sinf(a)*r;
        glVertex3f(cx,-h2,cz); glVertex3f(cx,h2,cz);
    }
    glEnd();
}

static void s5_car(float wheelSpin)
{
    // Body
    glPushMatrix(); glTranslatef(0,0.15f,0); glScalef(1.80f,0.55f,0.85f);
    Color bc={0.90f,0.10f,0.12f}; s5_colorCube(1.f,bc); glPopMatrix();
    // Cabin
    glPushMatrix(); glTranslatef(0.10f,0.68f,0); glScalef(1.0f,0.45f,0.72f);
    Color cc={0.96f,0.22f,0.24f}; s5_colorCube(1.f,cc); glPopMatrix();
    // Wheels
    float wx[]={0.65f,0.65f,-0.65f,-0.65f};
    float wz[]={0.46f,-0.46f,0.46f,-0.46f};
    for(int i=0;i<4;i++){
        glPushMatrix();
        glTranslatef(wx[i],-0.28f,wz[i]);
        glRotatef(90.f,1,0,0);
        glRotatef(wheelSpin,0,0,1);
        glColor3f(0.10f,0.10f,0.10f); s5_cylinder(0.28f,0.22f,14);
        glColor3f(0.65f,0.65f,0.68f); s5_cylinder(0.15f,0.24f,10);
        Color spk={0.55f,0.55f,0.58f};
        for(int s=0;s<4;s++){
            glPushMatrix(); glRotatef(s*90.f,0,0,1); glScalef(0.04f,0.26f,0.04f);
            s5_colorCube(1.f,spk); glPopMatrix();
        }
        glPopMatrix();
    }
}

static void s5_tree(float x,float z,float sc)
{
    glPushMatrix(); glTranslatef(x,-1.f,z); glScalef(sc,sc,sc);
    glPushMatrix(); glTranslatef(0,0.5f,0); glScalef(0.18f,1.0f,0.18f);
    Color tk={0.40f,0.22f,0.09f}; s5_colorCube(1.f,tk); glPopMatrix();
    float ly[]={1.2f,1.7f,2.1f};
    float lr[]={0.75f,0.60f,0.42f};
    float lc[3][3]={{.10f,.52f,.20f},{.12f,.58f,.22f},{.14f,.64f,.26f}};
    for(int l=0;l<3;l++){
        glColor3fv(lc[l]);
        glPushMatrix(); glTranslatef(0,ly[l],0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,lr[l]*0.9f,0);
        for(int j=0;j<=10;j++){
            float a=j*(float)M_PI*2.f/10.f;
            glVertex3f(cosf(a)*lr[l],0,sinf(a)*lr[l]);
        }
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,0,0);
        for(int j=10;j>=0;j--){
            float a=j*(float)M_PI*2.f/10.f;
            glVertex3f(cosf(a)*lr[l],0,sinf(a)*lr[l]);
        }
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();
}

static void s5_road()
{
    glColor3f(0.14f,0.14f,0.16f);
    glBegin(GL_QUADS);
    glVertex3f(-7,-0.98f,1.8f); glVertex3f(7,-0.98f,1.8f);
    glVertex3f(7,-0.98f,3.2f);  glVertex3f(-7,-0.98f,3.2f);
    glEnd();
    glColor3f(1,1,0.75f);
    glLineWidth(3.f);
    glBegin(GL_LINES);
    for(float x=-6.5f;x<6.5f;x+=1.2f){
        glVertex3f(x,-0.96f,2.5f); glVertex3f(x+0.6f,-0.96f,2.5f);
    }
    glEnd();
    glLineWidth(1.f);
}

// =============================================================
//  scene5  — main entry
// =============================================================
void scene5(float t)
{
    int W=WIN_WIDTH, H=WIN_HEIGHT;

    // ── Heading ──────────────────────────────────────────────
    drawHeading(20, H-35, "3D Graphics and Animation");

    // ── Prof. Pixel (left side, same as scene4) ───────────────
    float wave = (t > 1200.0f) ? 0.0f : 1.0f;
    float talk = (fmodf(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f;
    drawProfPixel(110, 110, 0.9f, wave > 0.5f, talk);

    // ── Save matrix stacks before any 3D ─────────────────────
    glMatrixMode(GL_PROJECTION); glPushMatrix();
    glMatrixMode(GL_MODELVIEW);  glPushMatrix();

    // =========================================================
    //  1. 3D TRANSFORMATIONS   0 – T_PROJ
    // =========================================================
    if(t < T_PROJ)
    {
        float lt = t - T_TRANSFORM;
        if(lt < 0) lt = 0;
        int   seg = (int)(lt / 3500.f) % 3;
        float a   = lt * 0.038f;

        // ── 3D panel ──────────────────────────────────────────
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        begin3D_right(true);
        gluLookAt(0.0,2.2,7.5, 0.0,0.5,0.0, 0,1,0);

        s5_ground(5.f,0.8f);
        s5_axes(2.0f);

        glPushMatrix();
        glRotatef(12.f,1,0,0);
        if(seg==0)      glRotatef(a,1,0,0);
        else if(seg==1) glRotatef(a,0,1,0);
        else            glRotatef(a,0,0,1);
        s5_house(0.f);
        glPopMatrix();

        // ── Back to 2D ────────────────────────────────────────
        restore2D_s5();

        // Content area border
        s5_dashedRect(CX0,CY0,CX1,CY1,COL_SCENE5,1.5f);

        // Section label
        drawText(30, H-95,
            "1. 3D TRANSFORMATIONS  -  rotate around X / Y / Z axes",
            COL_YELLOW, true);

        // Axis name badge
        const char* axName[]={"Rotate X  -  tilts forward / back",
                               "Rotate Y  -  spins left / right",
                               "Rotate Z  -  rolls in plane"};
        float axCol[3][3]={{1,.28f,.28f},{.22f,1,.38f},{1,.82f,.18f}};
        Color axc={axCol[seg][0],axCol[seg][1],axCol[seg][2]};

        s5_badge(CX0+10, CY0+60, 340, 28, axc);
        drawText(CX0+18, CY0+73, axName[seg], axc, false);

        char abuf[48];
        snprintf(abuf,sizeof(abuf),"angle = %.0f deg",fmodf(a,360.f));
        drawText(CX0+18, CY0+44, abuf, COL_LIGHT_GREY, false);

        // Axis legend (top-left of 3D panel)
        drawText(CX0+10, CY1-20, "Axes:", COL_WHITE, false);
        drawText(CX0+55, CY1-20, "X", {1,.25f,.25f}, false);
        drawText(CX0+75, CY1-20, "Y", {.25f,1,.25f}, false);
        drawText(CX0+95, CY1-20, "Z", {.35f,.55f,1}, false);

        // Bottom explanation
        drawText(CX0+10, CY0+10,
            "4x4 matrix: T*R*S applied per vertex (right-to-left).",
            COL_WHITE, true);
    }

    // =========================================================
    //  2. PROJECTION   T_PROJ – T_ZBUF
    // =========================================================
    else if(t < T_ZBUF)
    {
        float lt = t - T_PROJ;
        float a  = lt * 0.022f;

        // Split the 3D panel: LEFT = ortho, RIGHT = perspective
        int halfW = V3W/2;

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        // LEFT half – orthographic
        begin3D_vp(V3X, V3Y, halfW, V3H, false);
        glTranslatef(0,-0.4f,-9.f);
        glRotatef(14.f,1,0,0);
        glRotatef(a,0,1,0);
        s5_ground(5.f,0.8f);
        s5_house(0.f);

        glClear(GL_DEPTH_BUFFER_BIT);

        // RIGHT half – perspective
        begin3D_vp(V3X+halfW, V3Y, halfW, V3H, true);
        gluLookAt(0,2,8.5, 0,0.4,0, 0,1,0);
        glRotatef(a,0,1,0);
        s5_ground(5.f,0.8f);
        s5_house(0.f);

        // ── Back to 2D ────────────────────────────────────────
        restore2D_s5();

        s5_dashedRect(CX0,CY0,CX1,CY1,COL_SCENE5,1.5f);
        drawText(30, H-95,
            "2. PROJECTION  -  orthographic (left) vs perspective (right)",
            COL_YELLOW, true);

        // Divider between the two halves
        float divX = CX0 + (CX1-CX0)*0.5f;
        s5_solidLine(divX, CY0+40, divX, CY1-10, COL_DARK_GREY, 2.f);

        // Sub-labels inside each half
        s5_badge(CX0+10, CY1-52, 180, 26, COL_SCENE5);
        drawText(CX0+18, CY1-40, "ORTHOGRAPHIC", COL_SCENE5, false);

        s5_badge(divX+10, CY1-52, 168, 26, COL_SCENE5);
        drawText(divX+18, CY1-40, "PERSPECTIVE", COL_SCENE5, false);

        // Bottom explanation
        drawText(CX0+10, CY0+36,
            "Ortho: parallel lines stay parallel.",
            COL_WHITE, true);
        drawText(CX0+10, CY0+10,
            "Perspective: far objects shrink to a vanishing point.",
            COL_WHITE, true);
    }

    // =========================================================
    //  3. Z-BUFFER   T_ZBUF – T_ANIM
    // =========================================================
    else if(t < T_ANIM)
    {
        float lt = t - T_ZBUF;
        float a  = lt * 0.018f;
        Color c1={0.30f,0.55f,1.00f};   // blue
        Color c2={1.00f,0.48f,0.06f};   // orange
        Color c3={0.22f,0.80f,0.30f};   // green

        int halfW = V3W/2;

        // LEFT – depth OFF (painter's order, wrong)
        glDisable(GL_DEPTH_TEST);
        begin3D_vp(V3X, V3Y, halfW, V3H, true);
        glTranslatef(0,0,-7.5f);
        glRotatef(14.f,1,0,0);
        glRotatef(a,0,1,0);
        s5_ground(4.f,0.7f);
        glPushMatrix(); glTranslatef(-0.9f,0,-0.9f); s5_colorCube(0.85f,c1); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.9f,0, 0.9f); s5_colorCube(0.85f,c2); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.0f,0, 0.0f); s5_colorCube(0.85f,c3); glPopMatrix();

        // RIGHT – depth ON (correct)
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        begin3D_vp(V3X+halfW, V3Y, halfW, V3H, true);
        glTranslatef(0,0,-7.5f);
        glRotatef(14.f,1,0,0);
        glRotatef(a,0,1,0);
        s5_ground(4.f,0.7f);
        glPushMatrix(); glTranslatef(-0.9f,0,-0.9f); s5_colorCube(0.85f,c1); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.9f,0, 0.9f); s5_colorCube(0.85f,c2); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.0f,0, 0.0f); s5_colorCube(0.85f,c3); glPopMatrix();

        // ── Back to 2D ────────────────────────────────────────
        restore2D_s5();

        s5_dashedRect(CX0,CY0,CX1,CY1,COL_SCENE5,1.5f);
        drawText(30, H-95,
            "3. Z-BUFFER  -  which surface is actually in front?",
            COL_YELLOW, true);

        float divX = CX0 + (CX1-CX0)*0.5f;
        s5_solidLine(divX, CY0+40, divX, CY1-10, COL_DARK_GREY, 2.f);

        float red[3]={1,.28f,.28f};
        float grn[3]={.22f,1,.38f};

        s5_badge(CX0+10, CY1-52, 200, 26, {red[0],red[1],red[2]});
        drawText(CX0+18, CY1-40, "NO DEPTH TEST", {red[0],red[1],red[2]}, false);

        s5_badge(divX+10, CY1-52, 180, 26, {grn[0],grn[1],grn[2]});
        drawText(divX+18, CY1-40, "Z-BUFFER ON", {grn[0],grn[1],grn[2]}, false);

        drawText(CX0+10, CY0+36,
            "Left: draw order decides overlap (wrong).",
            {red[0],red[1],red[2]}, true);
        drawText(CX0+10, CY0+10,
            "Right: closest surface wins per pixel (correct).",
            {grn[0],grn[1],grn[2]}, true);
    }

    // =========================================================
    //  4. ANIMATION   T_ANIM onward
    // =========================================================
    else
    {
        float lt = t - T_ANIM;

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        begin3D_right(true);

        // Orbiting camera
        float camAngle = lt * 0.00025f;
        float camR     = 9.0f;
        gluLookAt(sinf(camAngle)*camR, 3.0f, cosf(camAngle)*camR,
                  0.0, 0.2, 0.0, 0,1,0);

        s5_ground(8.f,0.8f);
        s5_road();

        // Trees
        s5_tree(-3.5f,-2.8f,0.70f);
        s5_tree( 3.5f,-2.8f,0.80f);
        s5_tree(-5.0f,-1.5f,0.60f);
        s5_tree( 5.0f,-1.5f,0.65f);

        // House with animated door
        float door = 0.f;
        if(lt > 1000.f){
            float dp = s5_clamp01((lt-1000.f)/3000.f);
            door = -85.f * s5_smoothstep(dp);
        }
        glPushMatrix();
        glTranslatef(0,0,-1.5f);
        s5_house(door);
        glPopMatrix();

        // Car driving along the road
        float speed = 0.0018f;
        float carX  = fmodf(lt*speed,14.f)-7.f;
        float wSpin = lt*speed*200.f;
        glPushMatrix();
        glTranslatef(carX,-0.72f,2.45f);
        glScalef(0.55f,0.55f,0.55f);
        s5_car(wSpin);
        glPopMatrix();

        // ── Back to 2D ────────────────────────────────────────
        restore2D_s5();

        s5_dashedRect(CX0,CY0,CX1,CY1,COL_SCENE5,1.5f);
        drawText(30, H-95,
            "4. ANIMATION  -  camera orbits, door opens, car drives",
            COL_YELLOW, true);

        // Render pipeline stage ticker (bottom strip)
        const char* steps[]={"Clear","Transform","Project","Clip","Z-test","Display"};
        float sc[6][3]={
            {1,.30f,.30f},{.40f,.90f,.40f},{.40f,.60f,1},
            {1,.80f,.20f},{1,.50f,.10f},{.70f,.40f,1}
        };
        int active=(int)(lt/380.f)%6;
        float bx=CX0+10, by=CY0+8, bw=88, bh=24;
        for(int i=0;i<6;i++){
            bool act=(i==active);
            Color bc = act
                ? Color{sc[i][0]*.35f, sc[i][1]*.35f, sc[i][2]*.35f}
                : Color{0.05f,0.05f,0.05f};
            s5_filledRect(bx+i*(bw+4), by, bx+i*(bw+4)+bw, by+bh, bc);
            if(act){
                Color border={sc[i][0],sc[i][1],sc[i][2]};
                s5_badge(bx+i*(bw+4), by, bw, bh, border);
            }
            Color textCol = act
                ? Color{sc[i][0],sc[i][1],sc[i][2]}
                : COL_DARK_GREY;
            drawText(bx+i*(bw+4)+6, by+7, steps[i], textCol, false);
        }
    }

    // ── Prompt after scene settles ────────────────────────────
    if(t > T_ANIM + 4000.f){
        drawText(W/2-80, 45,
            "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }

    // ── Restore EVERYTHING for other scenes ───────────────────
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glLineWidth(1.f);
    glPointSize(1.f);
    glColor3f(1,1,1);
}