# Member 4 — Viewing & Clipping
**Your file:** `scenes/scene4.cpp` *(this is the ONLY file you edit)*
**Scene color:** Red
**Time in video:** 6:00 – 7:45

---

## What You Receive
After Member 3's transformations, you **zoom out to a big world** with many objects,
then show how a Window and Viewport work, and how lines get **clipped** to fit.

---

## What You Must Implement

| # | Concept | What it does |
|---|---------|--------------|
| 1 | **Window & Viewport** | Two rectangles — world region vs screen region |
| 2 | **Window-to-Viewport Mapping** | Formula to map world coords to screen coords |
| 3 | **Cohen-Sutherland Clipping** | Region codes, clip lines at boundary |
| 4 | **Liang-Barsky Clipping** | Parametric clipping, more efficient |

**Heads up:** You have **two full clipping algorithms** — the second-heaviest load.
Code Cohen-Sutherland first (more intuitive), then Liang-Barsky.

---

## How Your Scene Should Flow

```
Sub-scene 4.1 (0-4s):    Window & Viewport — show both rectangles, label them
Sub-scene 4.2 (4-8s):    W-to-V Mapping — map a point, then whole scene
Sub-scene 4.3 (8-14s):   Cohen-Sutherland — region codes, clip 3 example lines
Sub-scene 4.4 (14-20s):  Liang-Barsky — parametric, show t_enter / t_leave
```

---

## Window-to-Viewport Mapping Formula

```cpp
// Map a point from window (world) space to viewport (screen) space
static float mapX(float wx, float wxmin, float wxmax,
                  float vxmin, float vxmax) {
    return (wx - wxmin) / (wxmax - wxmin) * (vxmax - vxmin) + vxmin;
}
static float mapY(float wy, float wymin, float wymax,
                  float vymin, float vymax) {
    return (wy - wymin) / (wymax - wymin) * (vymax - vymin) + vymin;
}
```

---

## Cohen-Sutherland Region Codes (reference)

```cpp
// 4-bit codes:  TOP=8, BOTTOM=4, RIGHT=2, LEFT=1
static int computeCode(float x, float y,
                       float xmin, float ymin, float xmax, float ymax) {
    int code = 0;
    if (x < xmin) code |= 1;       // LEFT
    else if (x > xmax) code |= 2;  // RIGHT
    if (y < ymin) code |= 4;       // BOTTOM
    else if (y > ymax) code |= 8;  // TOP
    return code;
}

// Algorithm:
//   code0, code1 = codes of both endpoints
//   if (code0 | code1) == 0  -> trivially ACCEPT (draw it)
//   if (code0 & code1) != 0  -> trivially REJECT (discard)
//   else -> find intersection with a boundary, replace outside point, repeat
```

---

## Liang-Barsky (reference)

```cpp
// Line from (x0,y0) to (x1,y1), clip to window (xmin,ymin,xmax,ymax)
// dx = x1-x0, dy = y1-y0
// p[] = {-dx, dx, -dy, dy}
// q[] = {x0-xmin, xmax-x0, y0-ymin, ymax-y0}
// Find t_enter (max of entering t's) and t_leave (min of leaving t's)
// If t_enter > t_leave -> line is fully outside (reject)
// Else clip line to t_enter .. t_leave
```

---

## Code Skeleton

```cpp
void scene4(float t) {
    drawHeading(20, WIN_HEIGHT - 35, "Viewing & Clipping");
    drawText(20, WIN_HEIGHT - 55, "Member 4", COL_SCENE4, false);
    drawProfPixel(120, 120, 0.9f, false, 0.8f);

    // 4.1 Window & Viewport
    if (t < 4000) {
        // Draw a big world with objects, then a window rect (blue dashed)
        // and a viewport rect (orange) on the screen
    }
    // 4.2 Mapping
    else if (t < 8000) {
        // Highlight a point, show mapping formula, animate the mapping
    }
    // 4.3 Cohen-Sutherland
    else if (t < 14000) {
        // Draw viewport with 9 zones labeled with codes
        // Draw 3 lines: one accept (green), one reject (red), one clip (yellow)
    }
    // 4.4 Liang-Barsky
    else {
        // Show parametric clip with t_enter / t_leave highlighted
    }
}
```

---

## Rules
- Only edit `scenes/scene4.cpp`
- Keep drawing within x: 0–800, y: 0–600
- Use shared colors (`COL_ORANGE` for viewport, `COL_SCENE4` red, etc.)
- Use `t` for all animations
- Mark helper functions `static`
- For dashed lines, use `glLineStipple` (enable `GL_LINE_STIPPLE` first)

---

## Checklist
- [ ] Window and Viewport drawn and clearly labeled
- [ ] Mapping formula shown + point mapped visually
- [ ] Cohen-Sutherland: region codes shown, 3 line cases (accept/reject/clip)
- [ ] Liang-Barsky: parametric clip with t_enter / t_leave
- [ ] Comparison of the two methods shown
- [ ] Your file compiles with `make`
