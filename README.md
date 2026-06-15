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

**Windows (MSYS2 + MinGW-w64 — recommended):**
1. Install MSYS2 from https://www.msys2.org
2. Open the **MSYS2 MinGW64** shell and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut make
   ```
3. `cd` to this project folder and run `make`, then `./cg_project`.

The Makefile auto-detects MSYS2/MinGW and links `-lfreeglut -lopengl32 -lglu32`.
(Alternative: download freeglut from
https://www.transmissionzero.co.uk/software/freeglut-devel/ for plain MinGW.)

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



## Concepts checklist

- [x] Member 1: Points, Lines (Bresenham), Circles (Midpoint), Polygons, OpenGL Setup
- [x] Member 2: Boundary Fill, Flood Fill, Scan-Line Fill
- [x] Member 3: Translation, Rotation, Scaling, Reflection, Shearing
- [x] Member 4: Window/Viewport, W-to-V Mapping, Cohen-Sutherland, Liang-Barsky
- [x] Member 5: 3D Transforms, Projection (Ortho + Perspective), Z-Buffer, Animation
