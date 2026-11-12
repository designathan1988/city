---
name: promo-video-pipeline
description: "How the promo video was recorded (2026-11-08): clean HEAD export, --fixed-fps, tick_hz = fps, frames raw to ffmpeg"
metadata:
  node_type: memory
  type: project
  originSessionId: 7db88cf1-50be-4105-b08c-4f877928a423
  modified: 2026-11-08T05:27:55.907Z
---

Promo video made on 2026-11-08: `dist/promo/roadcraft_promo.mp4` (1080p30, 69 s) and `roadcraft_promo_leve.mp4` (light copy). The tooling lived in the session scratchpad, not in the repo.

**Why:** the user asked for a promo video "fast and good". The working tree was mid-change by another session and its scripts did not compile, so the game was recorded from a clean `git archive HEAD project` copy, with `.godot` copied in and `--import` run first.

**How to apply (if asked again):**
- Run a director scene from outside the repo: Godot loads an absolute-path `.tscn`/`.gd` with `--path <project>`. Launch it hidden through `tools/tasks/proc.run(..., hidden=True)`.
- Use `--fixed-fps 30` rather than Movie Maker. Boot with `catalog.tuning.sim.tick_hz = 30`, open the map paused and call `CityStore.step_ticks(1)` each frame.
- Sleep about 75 ms after each step. The render samples poses at steady_clock minus `pose_delay_ticks`, so without the sleep the cars jitter.
- Grab frames on `RenderingServer.frame_post_draw`, write them raw as rgb24 and assemble with ffmpeg (xfade, drawtext in Bahnschrift).
- The hidden window comes out 1920x1061: crop to 16:9 and scale to 1080p.
- Downtown is dense with towers. Keep low shots in the residential ring (r > 350 m on the town map).
- ffmpeg is installed via winget (Gyan.FFmpeg.Essentials), with the user's OK.
