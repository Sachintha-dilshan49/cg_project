# Member 1 — Drawing Primitives + OpenGL Setup
**Your file:** `scenes/scene1.cpp`
**You also own:** `main.cpp`, `common.h`, `utils/profpixel.cpp`, `scene_intro.cpp`, `scene_outro.cpp`
**Scene color:** Blue
**Time in video:** 0:30 – 2:30

---

## Your Special Role
You are the **project lead** for the code. You own the engine that everyone else plugs into:
- The OpenGL window and main render loop (`main.cpp`)
- The shared constants and helpers (`common.h`)
- The Prof. Pixel character (`utils/profpixel.cpp`)
- The intro and outro scenes
- Plus your own concept scene (`scene1.cpp`)

This means you should **set up the project first and share it** before anyone else starts coding.

---

## What You Must Implement (your concepts)

| # | Concept | Algorithm |
|---|---------|-----------|
| 1 | Points | `glVertex2f`, `glPointSize` |
| 2 | Lines | **Bresenham's Line Algorithm** |
| 3 | Circles | **Midpoint Circle Algorithm** |
| 4 | Polygons | `GL_POLYGON`, `GL_LINE_LOOP` |
| + | OpenGL Setup | GLUT window, render loop (already in `main.cpp`) |

---

## How Your Scene Should Flow

```
Sub-scene 1.0 (0-3s):   OpenGL setup explanation (show the render loop diagram)
Sub-scene 1.1 (3-6s):   Points — dots of different sizes on a grid
Sub-scene 1.2 (6-12s):  Lines — Bresenham pixel-by-pixel animation
Sub-scene 1.3 (12-16s): Circles — Midpoint, mirror across 8 octants
Sub-scene 1.4 (16-20s): Polygons — build a hollow house shape
```

The house you draw at the end is **handed off to Member 2** to fill with color.

---

## Code Skeleton For Your Scene

```cpp
void scene1(float t) {
    drawHeading(20, WIN_HEIGHT - 35, "Drawing Primitives");
    drawProfPixel(120, 120, 0.9f, false, 0.8f);

    // 1.1 Points
    if (t < 6000) {
        setColor(COL_YELLOW);
        glPointSize(8.0f);
        glBegin(GL_POINTS);
        glVertex2f(400, 300);
        glEnd();
        drawLabel(410, 305, "Point (400,300)");
    }
    // 1.2 Lines — Bresenham
    else if (t < 12000) {
        // Implement Bresenham here, light pixels one by one
        // Use (t-6000) to control how many pixels are drawn so far
    }
    // 1.3 Circles
    else if (t < 16000) {
        // Midpoint circle here
    }
    // 1.4 Polygons — draw the house
    else {
        // Draw hollow house: square + triangle roof
    }
}
```

---

## Bresenham's Line Algorithm (reference)

```cpp
static void bresenhamLine(int x0, int y0, int x1, int y1, int maxPixels) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int count = 0;

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    while (count < maxPixels) {
        glVertex2f(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
        count++;
    }
    glEnd();
}
// Animate: pass maxPixels = (int)((t - 6000) / 50)  so pixels appear over time
```

---

## Files You Should NOT Break
Since you own `common.h` and `main.cpp`, be careful — if you change a function
signature in `common.h`, it can break everyone else's code. If you must change
something shared, **tell the group first**.

---

## Checklist
- [ ] Project set up and shared with the group FIRST
- [ ] Points shown with different sizes
- [ ] Bresenham line animates pixel by pixel
- [ ] Midpoint circle with 8-octant symmetry
- [ ] Hollow house drawn (handed to Member 2)
- [ ] `make` compiles with no errors
