# Member 2 — Filling Algorithms
**Your file:** `scenes/scene2.cpp` *(this is the ONLY file you edit)*
**Scene color:** Green
**Time in video:** 2:30 – 4:15

---

## What You Receive
Member 1 draws a **hollow house** (just outlines). Your job is to **fill it with color**
using three different algorithms, showing how each one works.

---

## What You Must Implement

| # | Concept | What it does |
|---|---------|--------------|
| 1 | **Boundary Fill** | Fills from a seed point, stops at a boundary color |
| 2 | **Flood Fill** | Fills from a seed point, replaces a target color |
| 3 | **Scan-Line Fill** | Sweeps horizontal lines, fills between edge intersections |

---

## How Your Scene Should Flow

```
Sub-scene 2.1 (0-5s):    Boundary Fill — color spreads from seed, stops at walls
Sub-scene 2.2 (5-10s):   Flood Fill — replaces matching color, compare with Boundary
Sub-scene 2.3 (10-16s):  Scan-Line Fill — horizontal sweep filling a polygon row by row
```

At the end, the house is **fully colored** and handed off to Member 3 to transform.

---

## Important Note on Fill Algorithms in OpenGL

Real recursive Boundary/Flood Fill reads pixels with `glReadPixels`, which is **slow
and tricky to animate**. For a teaching video, you have two options:

**Option A (recommended for visuals):** *Simulate* the fill on a grid of cells you
control in an array. Draw each cell as a small quad. This makes the animation smooth
and easy to control with `t`. You demonstrate the *logic* clearly.

**Option B (real pixels):** Use `glReadPixels` / `glDrawPixels` for an authentic
implementation. More correct but harder to animate step by step.

For the assignment, **Option A is fine** — the examiner wants to see you understand
and demonstrate the algorithm, which a grid simulation shows clearly.

---

## Code Skeleton

```cpp
void scene2(float t) {
    drawHeading(20, WIN_HEIGHT - 35, "Filling Algorithms");
    drawText(20, WIN_HEIGHT - 55, "Member 2", COL_SCENE2, false);
    drawProfPixel(120, 120, 0.9f, false, 0.8f);

    // 2.1 Boundary Fill
    if (t < 5000) {
        drawLabel(300, 500, "Boundary Fill: stops at boundary color");
        // Draw shape outline, then animate fill spreading from seed
        // Use (t/animation_speed) to control how far the fill has spread
    }
    // 2.2 Flood Fill
    else if (t < 10000) {
        drawLabel(300, 500, "Flood Fill: replaces target color");
        // Similar, but show the "replace color" logic
    }
    // 2.3 Scan-Line Fill
    else {
        drawLabel(300, 500, "Scan-Line Fill: sweep top to bottom");
        // Animate a horizontal line sweeping down
        // Fill spans between edge intersections at each y-level
    }
}
```

---

## Scan-Line Fill Logic (reference)

```cpp
// For a polygon defined by vertices, at each scan-line y:
//   1. Find all x-positions where polygon edges cross this y
//   2. Sort those x-intersections
//   3. Fill pixels between pairs: (x0->x1), (x2->x3), ...
//
// Animate by sweeping y from top down based on t:
//   int scanY = TOP - (int)((t - 10000) / 30);
//   draw a yellow horizontal line at scanY
//   fill all rows above scanY that have been processed
```

---

## Grid-Simulation Helper (Option A starter)

```cpp
// A simple 2D grid you fill cell by cell
static const int GW = 40, GH = 30;     // grid cells
static int grid[GW][GH];               // 0 = empty, 1 = boundary, 2 = filled

static void drawGridCells(float ox, float oy, float cell) {
    for (int i = 0; i < GW; i++)
        for (int j = 0; j < GH; j++) {
            if (grid[i][j] == 1) setColor(COL_WHITE);
            else if (grid[i][j] == 2) setColor(COL_PINK);
            else continue;
            glBegin(GL_QUADS);
            glVertex2f(ox+i*cell,     oy+j*cell);
            glVertex2f(ox+(i+1)*cell, oy+j*cell);
            glVertex2f(ox+(i+1)*cell, oy+(j+1)*cell);
            glVertex2f(ox+i*cell,     oy+(j+1)*cell);
            glEnd();
        }
}
```

---

## Rules
- Only edit `scenes/scene2.cpp`
- Keep all drawing within x: 0–800, y: 0–600
- Use shared colors (`COL_PINK`, `COL_SCENE2`, etc.) from `common.h`
- Use `drawLabel()` / `drawHeading()` for text — no other fonts
- Use `t` for all animations, never your own timer
- Mark all your helper functions `static`

---

## Checklist
- [ ] Boundary Fill animates spreading from a seed point
- [ ] Flood Fill shown with the color-replace difference explained
- [ ] Scan-Line Fill sweeps top to bottom
- [ ] House ends up fully colored
- [ ] Your file compiles when Member 1 runs `make`
