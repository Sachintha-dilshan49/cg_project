# Computer Graphics Project — Prof. Pixel's Classroom
**University of Moratuwa | OpenGL + GLUT**

---

## Who owns what

| File | Owner | Do NOT touch if you are not this member |
|------|-------|-----------------------------------------|
| `main.cpp` | Member 1 | Scene switcher, render loop, keyboard |
| `common.h` | Member 1 | Shared constants — discuss before editing |
| `utils/profpixel.cpp` | Member 1 | Prof. Pixel character |
| `scenes/scene_intro.cpp` | Member 1 | Opening sequence |
| `scenes/scene_outro.cpp` | Member 1 | Closing sequence |
| `scenes/scene1.cpp` | **Member 1** | Drawing Primitives + OpenGL Setup |
| `scenes/scene2.cpp` | **Member 2** | Filling Algorithms |
| `scenes/scene3.cpp` | **Member 3** | 2D Transformations |
| `scenes/scene4.cpp` | **Member 4** | Viewing & Clipping |
| `scenes/scene5.cpp` | **Member 5** | 3D Graphics & Animation |

---

---

## How to build and run

### Install dependencies

**Ubuntu/Linux:**
```bash
sudo apt-get install freeglut3-dev
```

**macOS:**
```bash
brew install freeglut
```

**Windows (MinGW):**
Download freeglut from https://www.transmissionzero.co.uk/software/freeglut-devel/

### Build
```bash
make
```

### Run
```bash
make run
# or
./cg_project
```

### Controls
| Key | Action |
|-----|--------|
| `SPACE` | Next scene |
| `0` | Intro |
| `1` – `5` | Jump to scene 1–5 |
| `6` | Outro |
| `ESC` | Quit |

---

## How to implement your scene

Open your `scenes/sceneX.cpp`. Your function signature is:

```cpp
void sceneX(float t) {
    // t = milliseconds since this scene started
}
```

Use `t` to animate over time:

```cpp
void scene3(float t) {
    // Sub-scene A: first 5 seconds
    if (t < 5000) {
        float angle = t * 0.05f;   // grows over time
        // draw rotation demo...
    }
    // Sub-scene B: 5 to 10 seconds
    else if (t < 10000) {
        // draw scaling demo...
    }
}
```

Draw Prof. Pixel anywhere in your scene:
```cpp
drawProfPixel(120, 120, 0.9f, false, 0.8f);
//            x    y   scale  wave  mouthOpen
```

Draw text:
```cpp
drawHeading(20, 550, "My Scene Title");   // large white text
drawLabel(200, 300, "angle = 45 deg");   // small yellow label
```

---


## Concepts checklist

- [ ] Member 1: Points, Lines (Bresenham), Circles (Midpoint), Polygons, OpenGL Setup
- [ ] Member 2: Boundary Fill, Flood Fill, Scan-Line Fill
- [ ] Member 3: Translation, Rotation, Scaling, Reflection, Shearing
- [ ] Member 4: Window/Viewport, W-to-V Mapping, Cohen-Sutherland, Liang-Barsky
- [ ] Member 5: 3D Transforms, Projection (Ortho + Perspective), Z-Buffer, Animation
