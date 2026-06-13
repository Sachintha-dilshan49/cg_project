# START HERE — Team Overview

Welcome! This folder is your group's Computer Graphics project. Read this first,
then open YOUR personal guide.

---

## Find your guide

| You are... | Open this file | You edit this code file |
|------------|----------------|--------------------------|
| Member 1 | `MEMBER_1_GUIDE.md` | `scenes/scene1.cpp` (+ project setup) |
| Member 2 | `MEMBER_2_GUIDE.md` | `scenes/scene2.cpp` |
| Member 3 | `MEMBER_3_GUIDE.md` | `scenes/scene3.cpp` |
| Member 4 | `MEMBER_4_GUIDE.md` | `scenes/scene4.cpp` |
| Member 5 | `MEMBER_5_GUIDE.md` | `scenes/scene5.cpp` |

Not sure about teamwork/Git? Read `VERSION_CONTROL_GUIDE.md`.

---

## The 8 golden rules (everyone follows)

1. **Only edit your own `sceneX.cpp` file.** Nothing else.
2. **Never edit `common.h` or `main.cpp`** unless you are Member 1 (and tell the group).
3. **Window is 800 x 600.** Keep all drawing within x: 0–800, y: 0–600.
4. **Use the shared colors** from `common.h` (COL_SCENE1, COL_YELLOW, etc.).
5. **Use `drawText` / `drawLabel` / `drawHeading`** for text. No other fonts.
6. **Use `t`** (the time parameter) for all animations. Never your own timer.
7. **Mark all your helper functions `static`** so names don't clash with others.
8. **Only `#include "../common.h"`** — never include another member's file.

---

## The whole project in one picture

```
cg_project/
│
├── START_HERE.md          ← you are here (in member_guides/)
├── README.md              ← build instructions & rules
├── Makefile               ← run "make" to build everything
├── .gitignore             ← for Git users
│
├── common.h               ← SHARED: colors, helpers, constants (Member 1)
├── main.cpp               ← SHARED: render loop, scene switcher (Member 1)
│
├── utils/
│   └── profpixel.cpp      ← SHARED: the Prof. Pixel character (Member 1)
│
├── scenes/
│   ├── scene_intro.cpp    ← Member 1
│   ├── scene1.cpp         ← Member 1  (Drawing Primitives)
│   ├── scene2.cpp         ← Member 2  (Filling Algorithms)
│   ├── scene3.cpp         ← Member 3  (2D Transformations)
│   ├── scene4.cpp         ← Member 4  (Viewing & Clipping)
│   ├── scene5.cpp         ← Member 5  (3D & Animation)
│   └── scene_outro.cpp    ← Member 1
│
└── member_guides/         ← all the guide files (this folder)
```

---

## How it all connects

1. `main.cpp` opens the window and starts the render loop (60 fps)
2. Every frame, it calls the active scene's function: `scene1(t)`, `scene2(t)`, etc.
3. `t` is the time in milliseconds since that scene started — you animate with it
4. Press `SPACE` to move to the next scene, or `1`–`5` to jump
5. Each scene draws Prof. Pixel teaching one set of concepts

The story: a single point → shapes → filled shapes → transformed → clipped → 3D world.
Each member's scene continues the story from the last.

---

## How to build & run

```bash
# Install GLUT first (one time):
#   Ubuntu:  sudo apt-get install freeglut3-dev
#   macOS:   brew install freeglut

make          # compile
make run      # compile and run
make clean    # remove compiled files
```

---

## How to merge (Member 1 does this)

1. Each member sends their `sceneX.cpp`
2. Member 1 drops each file into `scenes/`
3. Run `make`
4. If it compiles → done. If not, the error names the file to fix.

(See `VERSION_CONTROL_GUIDE.md` for the Git way, which automates this.)

---

## Final checklist before submission

- [ ] All 5 scenes implemented and working
- [ ] `make` compiles with no errors
- [ ] Pressing 1–5 shows each scene correctly
- [ ] After Scene 5, pressing 1–4 still works (Member 5 popped matrices correctly)
- [ ] All 20 concepts visibly demonstrated (see README checklist)
- [ ] Screen-record the full run for the video submission
