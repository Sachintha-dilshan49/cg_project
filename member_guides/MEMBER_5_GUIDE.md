# Member 5 — 3D Graphics & Animation
**Your file:** `scenes/scene5.cpp` *(this is the ONLY file you edit)*
**Scene color:** Purple
**Time in video:** 7:45 – 9:30

---

## What You Receive
This is the **grand finale**. You take everything 2D and bring it into **3D** —
the house becomes a 3D model, you add projection and depth, then animate the whole
scene so all 5 members' work is visible at once.

---

## What You Must Implement

| # | Concept | What it does |
|---|---------|--------------|
| 1 | **3D Transformations** | Translate/rotate/scale in 3D (4x4 matrices) |
| 2 | **Projection Methods** | Orthographic vs Perspective |
| 3 | **Z-Buffer Algorithm** | Depth test — which surface is visible |
| 4 | **Animation System** | The final animated scene tying it all together |

**Heads up:** You have the **heaviest technical load**. Start with the Z-Buffer — it's
the most complex piece. The animation loop itself is already built by Member 1 in
`main.cpp` (you get `t` for free) — you just fill in the 3D content.

---

## Important: Switching to 3D Mode

`main.cpp` sets up 2D orthographic projection by default. In YOUR scene, you need to
switch to 3D, then switch back at the end:

```cpp
void scene5(float t) {
    // ... 2D text labels first (still in 2D mode) ...

    // Switch to 3D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, (double)WIN_WIDTH/WIN_HEIGHT, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(0,0,400,  0,0,0,  0,1,0);  // camera

    glEnable(GL_DEPTH_TEST);  // Z-buffer ON

    // ... draw your 3D scene ...

    // Switch back to 2D so other scenes are not affected
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
```

**IMPORTANT:** Always pop the matrices back, or the next scene will be broken!

---

## How Your Scene Should Flow

```
Sub-scene 5.1 (0-5s):    3D Transformations — house becomes 3D box, rotates on X/Y/Z
Sub-scene 5.2 (5-10s):   Projection — Ortho vs Perspective side by side
Sub-scene 5.3 (10-15s):  Z-Buffer — overlapping objects, depth test shown
Sub-scene 5.4 (15s+):    Animation finale — everything alive at once
```

---

## Z-Buffer — The Key Concept

The good news: **OpenGL has a built-in Z-buffer.** You enable it with:

```cpp
glEnable(GL_DEPTH_TEST);
```

For the video, **demonstrate** what it does:
- Draw 2 overlapping 3D objects (sphere in front of cube)
- First show WITHOUT depth test (`glDisable(GL_DEPTH_TEST)`) — wrong overlap
- Then WITH depth test enabled — correct overlap
- Show a depth buffer visualization grid beside it (optional but impressive)

```cpp
// Demo: toggle depth test based on time to show the difference
if (t > 12000) glEnable(GL_DEPTH_TEST);
else           glDisable(GL_DEPTH_TEST);
```

---

## Projection Demo (reference)

```cpp
// Orthographic
glOrtho(-100, 100, -100, 100, 1, 1000);

// Perspective
gluPerspective(60.0, aspect, 1.0, 1000.0);
```

Show both with the same cube so the difference is obvious (perspective shrinks
far objects, ortho keeps them the same size).

---

## Code Skeleton

```cpp
void scene5(float t) {
    // 2D labels (before switching to 3D)
    drawHeading(20, WIN_HEIGHT - 35, "3D Graphics & Animation");
    drawText(20, WIN_HEIGHT - 55, "Member 5", COL_SCENE5, false);

    // --- enter 3D mode (see above) ---

    if (t < 5000) {
        // 3D transformations: rotating box
        float ax = t * 0.05f;
        glRotatef(ax, 1, 1, 0);
        drawCube();
    }
    else if (t < 10000) {
        // projection comparison
    }
    else if (t < 15000) {
        // z-buffer demo
    }
    else {
        // animation finale: house door opens, car drives, camera orbits
    }

    // --- exit 3D mode (pop matrices) ---
}

static void drawCube() {
    // 6 colored faces using GL_QUADS
}
```

---

## Animation Finale Ideas
- House door rotates open (rotation transform)
- A car (simple box) translates along a road
- Camera slowly orbits using `gluLookAt` with time-based angle
- Z-buffer keeps all depth correct automatically

This final scene is what the examiner remembers — make it show **all concepts together**.

---

## Rules
- Only edit `scenes/scene5.cpp`
- ALWAYS push/pop projection AND modelview matrices (or you break other scenes)
- Re-enable `GL_DEPTH_TEST` since you need it; main.cpp already enabled it globally
- Use `t` for all animations
- Mark helper functions `static`
- Use shared colors from `common.h`

---

## Checklist
- [ ] 3D transformations: rotate around X, Y, Z
- [ ] Orthographic vs Perspective shown side by side
- [ ] Z-Buffer demo: with vs without depth test
- [ ] Animation finale with multiple moving parts
- [ ] Matrices popped back at the end (test by pressing 1-4 after your scene)
- [ ] Your file compiles with `make`
