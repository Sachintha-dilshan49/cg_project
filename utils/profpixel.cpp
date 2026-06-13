// ============================================================
//  utils/profpixel.cpp  -  MEMBER 1 OWNS THIS FILE
//  Shared Prof. Pixel character. All members call:
//    drawProfPixel(x, y, scale, waving, mouthOpen);
// ============================================================
#include "../common.h"

static void filledCircle(float cx, float cy, float r, int seg=24) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i=0;i<=seg;i++){
        float a=2.0f*M_PI*i/seg;
        glVertex2f(cx+r*cos(a),cy+r*sin(a));
    }
    glEnd();
}
static void drawArc(float cx,float cy,float r,float s,float e,int seg=16){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=seg;i++){
        float a=(s+(e-s)*i/seg)*M_PI/180.0f;
        glVertex2f(cx+r*cos(a),cy+r*sin(a));
    }
    glEnd();
}

void drawProfPixel(float cx,float cy,float scale,bool waving,float mouthOpen){
    float s=scale;
    float headR=20*s, headCY=cy+80*s;
    float bodyTopY=headCY-headR, bodyBotY=cy+30*s;

    glLineWidth(2.0f*s);

    // Head
    setColor(COL_YELLOW);
    filledCircle(cx,headCY,headR);
    setColor(COL_WHITE);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<24;i++){float a=2*M_PI*i/24;
        glVertex2f(cx+headR*cos(a),headCY+headR*sin(a));}
    glEnd();

    // Eyes + glasses
    setColor({0.1f,0.1f,0.1f});
    filledCircle(cx-7*s,headCY+5*s,3*s);
    filledCircle(cx+7*s,headCY+5*s,3*s);
    setColor(COL_CYAN);
    for(int side=-1;side<=1;side+=2){
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<16;i++){float a=2*M_PI*i/16;
            glVertex2f(cx+side*7*s+5*s*cos(a),headCY+5*s+5*s*sin(a));}
        glEnd();
    }
    glBegin(GL_LINES);
    glVertex2f(cx-2*s,headCY+5*s); glVertex2f(cx+2*s,headCY+5*s);
    glEnd();

    // Mouth
    setColor({0.1f,0.1f,0.1f});
    glLineWidth(1.5f);
    if(mouthOpen<0.1f) drawArc(cx,headCY-5*s,8*s,200,340);
    else filledCircle(cx,headCY-7*s,5*s*mouthOpen);

    // Hat
    setColor(COL_DARK_GREY);
    glBegin(GL_QUADS);
    glVertex2f(cx-22*s,headCY+headR-2*s); glVertex2f(cx+22*s,headCY+headR-2*s);
    glVertex2f(cx+22*s,headCY+headR+3*s); glVertex2f(cx-22*s,headCY+headR+3*s);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(cx-14*s,headCY+headR+3*s);  glVertex2f(cx+14*s,headCY+headR+3*s);
    glVertex2f(cx+14*s,headCY+headR+16*s); glVertex2f(cx-14*s,headCY+headR+16*s);
    glEnd();
    setColor(COL_YELLOW);
    glBegin(GL_LINES);
    glVertex2f(cx+14*s,headCY+headR+16*s);
    glVertex2f(cx+20*s,headCY+headR+10*s);
    glEnd();
    filledCircle(cx+20*s,headCY+headR+9*s,2*s);

    // Body
    setColor(COL_DARK_GREY);
    glBegin(GL_QUADS);
    glVertex2f(cx-14*s,bodyBotY); glVertex2f(cx+14*s,bodyBotY);
    glVertex2f(cx+12*s,bodyTopY); glVertex2f(cx-12*s,bodyTopY);
    glEnd();
    setColor(COL_WHITE);
    glBegin(GL_QUADS);
    glVertex2f(cx-4*s,bodyBotY); glVertex2f(cx+4*s,bodyBotY);
    glVertex2f(cx+3*s,bodyTopY); glVertex2f(cx-3*s,bodyTopY);
    glEnd();
    // Bow tie
    setColor(COL_SCENE4);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx,bodyTopY-4*s);
    glVertex2f(cx-8*s,bodyTopY-8*s); glVertex2f(cx-8*s,bodyTopY);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(cx,bodyTopY-4*s);
    glVertex2f(cx+8*s,bodyTopY-8*s); glVertex2f(cx+8*s,bodyTopY);
    glEnd();

    // Arms
    glLineWidth(3.0f*s);
    setColor(COL_DARK_GREY);
    // Left arm + pointer
    glBegin(GL_LINES);
    glVertex2f(cx-12*s,bodyTopY-10*s);
    glVertex2f(cx-28*s,bodyTopY-30*s);
    glEnd();
    setColor(COL_ORANGE);
    glBegin(GL_LINES);
    glVertex2f(cx-28*s,bodyTopY-30*s);
    glVertex2f(cx-48*s,bodyTopY-60*s);
    glEnd();
    // Right arm
    setColor(COL_DARK_GREY);
    float rEY=waving?headCY+20*s:bodyTopY-30*s;
    glBegin(GL_LINES);
    glVertex2f(cx+12*s,bodyTopY-10*s);
    glVertex2f(cx+28*s,rEY);
    glEnd();
    setColor(COL_YELLOW);
    filledCircle(cx+28*s,rEY,4*s);

    // Legs
    setColor(COL_DARK_GREY);
    glBegin(GL_LINES);
    glVertex2f(cx-6*s,bodyBotY); glVertex2f(cx-8*s,cy+10*s);
    glVertex2f(cx-8*s,cy+10*s);  glVertex2f(cx-12*s,cy);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(cx+6*s,bodyBotY); glVertex2f(cx+8*s,cy+10*s);
    glVertex2f(cx+8*s,cy+10*s);  glVertex2f(cx+12*s,cy);
    glEnd();
    glLineWidth(1.0f);
}
