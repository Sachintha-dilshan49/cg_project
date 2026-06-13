// ============================================================
//  scenes/scene5.cpp  -  MEMBER 5 OWNS THIS FILE
//  Topic : 3D Graphics and Animation
//  Concepts to implement:
//    3D Transformations, Projection Methods, Z-Buffer, Animation
//
//  HOW TO USE:
//    - Your entry point is: void scene5(float t)
//    - t = milliseconds since this scene started (from animTime)
//    - Use t to drive animations (e.g. float angle = t * 0.05f)
//    - Call drawProfPixel() to show the character
//    - Call drawHeading() / drawLabel() for text
//    - Use setColor() with the shared palette from common.h
//    - Do NOT use hardcoded pixel values outside 0-800 (x) / 0-600 (y)
// ============================================================

#include "../common.h"

// ── Internal helpers (only used in this file) ────────────────
// Put your helper functions here. Keep them static so they
// don't conflict with other members' functions.



// ── scene5 ─────────────────────────────────────────────────
// Called every frame (~60fps) by main.cpp display().
// t = elapsed time in milliseconds for this scene.
void scene5(float t) {

    // ── Scene title ──────────────────────────────────────────
    drawHeading(20, WIN_HEIGHT - 35, "3D Graphics and Animation");
    drawText(20, WIN_HEIGHT - 55,
             "Member 5", COL_SCENE5, false);

    // ── Prof. Pixel ──────────────────────────────────────────
    // Adjust position and animation as needed.
    float wave = (t > 1000.0f) ? 0.0f : 1.0f;   // waves for first second
    float talk = (fmod(t, 1200.0f) < 600.0f) ? 0.8f : 0.0f; // talking pulse
    drawProfPixel(120, 120, 0.9f, wave > 0.5f, talk);

    // ── YOUR IMPLEMENTATION GOES HERE ───────────────────────
    // TODO: Implement your concepts below.
    // Use t to animate things over time.
    // Example structure:
    //
    //   Sub-scene A (0 - 5000ms):
    //   if (t < 5000) { ... draw concept A ... }
    //
    //   Sub-scene B (5000 - 10000ms):
    //   else if (t < 10000) { ... draw concept B ... }
    //
    //   Sub-scene C (10000ms+):
    //   else { ... draw concept C ... }



    // ── Transition hint ──────────────────────────────────────
    if (t > 15000) {  // after 15 seconds, hint to move on
        drawText(WIN_WIDTH/2 - 80, 15,
                 "Press SPACE for next scene", COL_LIGHT_GREY, false);
    }
}
