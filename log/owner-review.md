# The owner's review of the game: findings, causes and fixes

The working record of the list in `log/pendencias.md` ("The owner's review of the game", 2026-11-09): each problem as it is
found, every candidate cause with its evidence, the cause that held, the fix and its verification. Written as the work goes,
so nothing found is lost between steps. A problem leaves `pendencias.md` when its fix is committed; its record stays here.

Method (the owner's brief): locate the lab function by the ledger, compare both versions line by line, name the difference
with file:line on both sides before editing; only when they are equivalent, one measurement that separates the hypotheses.
Verification in the live game (`godot --path project res://game/play.tscn -- --live=<dir>`, tests/visual/live_driver.gd),
town map (index 0), seed 7, simulation running at 1x, photos near, mid and far, low and high.

## 1. Walkers drawn as black boxes from about 50 m

**Seen (2026-11-09, live, town seed 7, 07:30):** at 60-150 m along a street, standing black rectangles (rgb about 5, 10, 15)
twice as tall as wide on the footways; the walkers nearer (near and mid mesh levels) are right. With `agents.json`
`people.blob.from_m` set to 5 for one run, every walker became such a box, even at 10 m: the far card, not the mid mesh.

**Lab:** `src/render/people/personImpostor.ts` (createPersonImpostor, VERTEX_MAIN, MAP_MAIN), chosen by
`src/render/riggedCitizens.ts` citizenLevelOf (under IMPOSTOR_PIXELS = 8 device pixels).
**Game:** `project/render/shaders/people_far.gdshader`, the strip from `native/src/render/agents/people_asset.cpp` read_card,
set at `native/src/render/agents/people_boot.cpp:191`.

**Differences listed:**
1. The strip (x, y, a frame's width and height in uv) was a `godot::Color` set on a `vec4` uniform. Godot converts a Color
   set on any non-texture uniform from sRGB to linear unless the uniform is hinted `color_conversion_disabled`
   (servers/rendering/renderer_rd/storage_rd/material_storage.cpp, update_uniform_buffer and convert_to_vector). The file's
   strip (0, 0.9375, 0.03125, 0.0625) reached the shader as about (0, 0.863, 0.0024, 0.0050): every card read one texel near
   row 884, inside an opaque black region of the atlas. The lab passes a Vector4 (`uFrame`). **The cause.**
   Measured once: the card shader drew `vec3(v_uv, texel.a)` and the colour was the same over the whole card (the uv did not
   vary), alpha 1.
2. The lab clamps the card's mip to log2(frame texels) - 2 so the next frame never bleeds in (MAP_MAIN); the game sampled the
   whole mip chain. Followed.
3. The lab's card is as wide as its frame's aspect (`uCard.x = height * frame.width / frame.height`); the game had a constant
   CARD_ASPECT 0.5 (equal for these 32 x 64 frames, a second owner of the strip's shape). Now strip.z / strip.w.
4. The v axis: the lab flips v (`1 - (y + h) / size`, its KTX2 rows bottom first); the game's atlas PNGs hold the strip
   upside down at the bottom rows (figures' feet at row 960, heads at 1023), and the game's v grows with the card's height
   from strip.y: equivalent, checked on the decoded atlas (layer 0).
5. Level choice: the lab by the figure's pixels (card under 8 px), the game by distance (`lod_m` 40, card past `blob.from_m`
   90 m). Left: the mid mesh is right at 40-90 m in the photos and the card is now a figure.

**Fix:** `PeopleAsset::card_strip` is a `godot::Vector4` (people_asset.h/.cpp); people_far.gdshader clamps the mip and takes
the width from the strip. No other `material_set_param` in `native/src` sends a Color to a uniform that is not a colour
(checked: signature_bodies albedo/emission, agent paints and helmet colours are colours).

**Verification:** both DLLs; live photos at the same spots (street at -75.5, 175.6, yaw 90: pitch 12 and 40, distance 30,
120, 250): no black box, the far walkers are small coloured figures. Probe: people_test `_card` checks that the material's
strip is a Vector4 equal to the file's (fails with a Color). people_test passes.

## 9. The picture shakes; vehicles jerk and leap forward abruptly

**Seen:** by the owner, watching the live game (2026-11-09). Not yet investigated.

**Leads to read (not causes yet):**
- `pose_stats` in the live game: `late 6`, `gap_max_ms 5164.8` between pose publications (`publications 698`,
  `frames 2080`). A vehicle drawn between two poses that arrive late or after a long gap moves in a jump: the pose
  interpolation and the simulation thread's ticks (sim/state/pose_buffer.h, render PoseSample) are the first place to read.
- The sun moves with the clock at 1x: if its direction is set every frame, the shadow cascades' texel grid turns every frame
  and every shadow edge crawls (a "shaking" picture with the camera still). Daylight (project/render/daylight.gd) to read.
- The automatic tier's governor (game/settings/frame_governor.gd) switches tiers on the GPU's p90; each switch redraws shadows
  and detail. Only in a run that is not driven by tests.
