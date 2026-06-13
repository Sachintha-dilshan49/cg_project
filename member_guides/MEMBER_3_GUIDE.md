# Member 3 — 2D Transformations
**Your file:** `scenes/scene3.cpp` *(this is the ONLY file you edit)*
**Scene color:** Amber/Orange
**Time in video:** 4:15 – 6:00

---

## What You Receive
Member 2 hands you a **fully colored house**. Your job is to **move, rotate, scale,
reflect, and shear** it — demonstrating all 5 transformations with the matrix shown
on screen.

---

## What You Must Implement

| # | Concept | Matrix idea |
|---|---------|-------------|
| 1 | **Translation** | Add tx, ty to coordinates |
| 2 | **Rotation** | Use sin/cos around a pivot |
| 3 | **Scaling** | Multiply by sx, sy |
| 4 | **Reflection** | Negate x or y (mirror) |
| 5 | **Shearing** | Slant along an axis |

You have the **most concepts (5)**, but they all share the same matrix pattern — so
once you build the matrix framework, each one is a small change. **Start early.**

---

## How Your Scene Should Flow

```
Sub-scene 3.1 (0-4s):    Translation — house slides across screen
Sub-scene 3.2 (4-8s):    Rotation — house spins around its center
Sub-scene 3.3 (8-12s):   Scaling — house grows and shrinks
Sub-scene 3.4 (12-16s):  Reflection — house mirrors across axes
Sub-scene 3.5 (16-20s):  Shearing — house slants
Finale (20s+):           All transforms chained together
```

---

## The Smart Approach — Build Once, Reuse

Use OpenGL's built-in matrix functions. They ARE transformation matrices:

```cpp
glPushMatrix();
    glTranslatef(tx, ty, 0);     // Translation
    glRotatef(angle, 0, 0, 1);   // Rotation around Z
    glScalef(sx, sy, 1);         // Scaling
    drawHouse();                 // your house drawing function
glPopMatrix();
```

OR implement the matrices manually to show the math (better for the video):

```cpp
// Apply a 2D transform manually to each vertex
static void transformPoint(float x, float y, float m[3][3],
                           float &outX, float &outY) {
    outX = m[0][0]*x + m[0][1]*y + m[0][2];
    outY = m[1][0]*x + m[1][1]*y + m[1][2];
}
```

**Tip:** Show the matrix numbers on screen with `drawLabel()` so the examiner sees
you understand the math, not just the OpenGL shortcut.

---

## Code Skeleton

```cpp
void scene3(float t) {
    drawHeading(20, WIN_HEIGHT - 35, "2D Transformations");
    drawText(20, WIN_HEIGHT - 55, "Member 3", COL_SCENE3, false);
    drawProfPixel(120, 120, 0.9f, false, 0.8f);

    // 3.1 Translation
    if (t < 4000) {
        float tx = (t / 4000.0f) * 200;  // slides right over time
        glPushMatrix();
        glTranslatef(tx, 0, 0);
        drawHouse(400, 300);
        glPopMatrix();
        drawLabel(300, 500, "Translation: tx grows");
    }
    // 3.2 Rotation
    else if (t < 8000) {
        float angle = (t - 4000) / 4000.0f * 90;
        glPushMatrix();
        glTranslatef(400, 300, 0);   // move to pivot
        glRotatef(angle, 0, 0, 1);
        glTranslatef(-400, -300, 0); // move back
        drawHouse(400, 300);
        glPopMatrix();
    }
    // ... continue for scaling, reflection, shearing
}

// Your reusable house — same shape Member 1 & 2 used
static void drawHouse(float cx, float cy) {
    // square body + triangle roof, filled
}
```

---

## Reflection & Shearing Matrices (reference)

```
Reflect across X-axis:   [1  0]   (negate y)      glScalef(1, -1, 1)
                         [0 -1]

Reflect across Y-axis:   [-1 0]   (negate x)      glScalef(-1, 1, 1)
                         [0  1]

Shear (horizontal):      [1 shx]  x' = x + shx*y
                         [0  1 ]
```

For shearing, OpenGL has no direct function — apply a custom matrix with
`glMultMatrixf()` or transform vertices manually.

---

## Rules
- Only edit `scenes/scene3.cpp`
- Keep drawing within x: 0–800, y: 0–600
- Use shared colors from `common.h`
- Use `t` for all animations
- Mark helper functions `static`
- ALWAYS pair `glPushMatrix()` with `glPopMatrix()` or you'll break the next frame

---

## Checklist
- [ ] Translation animates (house slides)
- [ ] Rotation around a pivot point
- [ ] Scaling (uniform AND non-uniform)
- [ ] Reflection across at least 2 axes
- [ ] Shearing (horizontal and/or vertical)
- [ ] Matrix values shown on screen with labels
- [ ] Finale chains multiple transforms
- [ ] Your file compiles with `make`
