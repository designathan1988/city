# Research: every question, its searches, its sources and its proof

Opened 2026-11-11 (night) at the owner's order. One entry per question, written when the research is made. A question is
`proven` only with all four: the problem reproduced with a before measure or image; the solution applied in a test (in an
isolated folder outside the game tree); the after measure or image meeting the criterion; the steps to repeat it with the real
output. Anything less is `open`, with what failed written down. The questions come from `log/specification.md`.

## Entry format

- **Question** (and the pending item of `log/pendencias.md` it serves)
- **Searches** (the queries, context7 libraries and pages read, whole)
- **Sources** (links)
- **Found**
- **Test** (folder, steps, before, after, real output)
- **Conclusion** and **status** (`open`, `proven`, `failed: ...`)

## Entries

### Q1. What is a "median computer", and what does x86-64-v3 cost? (pending item M1; 2026-11-11, session 2)

- **Searches:** WebSearch "Steam Hardware & Software Survey October 2026 most common GPU CPU cores RAM VRAM resolution";
  "Steam hardware survey AVX2 support percentage"; then the primary page read whole in the browser pane, its detail tables
  read from the page's DOM (`.stats_row` and the row after it).
- **Source:** https://store.steampowered.com/hwsurvey/Steam-Hardware-Software-Survey-Welcome-to-Steam (October 2026). Secondary,
  agreeing: https://www.guru3d.com/story/steam-hardware-survey-october-2026-16-gb-ram-regains-lead-over-32-gb/
- **Found (October 2026, all platforms; percentages of surveyed machines):**
  - System RAM: 4 GB 1.35, 8 GB 7.52, 12 GB 2.45, **16 GB 41.42**, 24 GB 2.31, 32 GB 37.06, 64 GB 3.94. Cumulative to 12 GB:
    11.3 %; to 16 GB: 52.8 %. Median 16 GB; the 10th percentile is 12 GB; 8 GB or less is 8.9 %.
  - Physical cores: 2: 2.84, 4: 12.36, **6: 27.29**, **8: 28.68**, 10: 7.48, 12: 4.97, 14: 4.74, 16: 5.86, 20: 2.16, 24: 2.91.
    Cumulative to 4: 15.3 %; to 6: 42.8 %; to 8: 71.5 %. Median 8 cores; 25th percentile 6; 10th percentile 4.
  - VRAM: 512 MB 2.81, 1 GB 1.46, 2 GB 3.78, 3 GB 1.30, 4 GB 5.54, 6 GB 5.20, **8 GB 26.92**, 10 GB 1.84, 11 GB 0.94, 12 GB 12.91,
    16 GB 26.35, 24 GB 5.40. Cumulative to 4 GB: 14.9 %; to 6 GB: 20.1 %; to 8 GB: 47.0 %; to 11 GB: 49.8 %. Median at the
    8/12 GB boundary; mode 8 GB; 10th percentile 4 GB.
  - Primary display: **1920x1080 50.23**, 2560x1440 21.82, 2560x1600 5.46, 3840x2160 5.05, 1366x768 2.25. Median 1920x1080.
  - Most common video card: NVIDIA GeForce RTX 3060 3.62 %, then RTX 4060 Laptop 3.40, RTX 3050 3.06, RTX 5060 2.97,
    RTX 5060 Ti 2.55, RTX 5060 Laptop 2.52, RTX 4060 Ti 2.17, RTX 3060 Ti 2.03. Integrated: Intel(R) UHD Graphics 1.19,
    AMD Radeon(TM) Graphics 0.95, Intel UHD Graphics 0.71, UHD 620 0.47, Radeon 610M 0.40, Vega 8 0.33.
  - CPU features ("Other Settings"): SSE2 97.90 (the ceiling: the rest are machines that report no x86 features), SSE4.2 97.78,
    AVX 97.00, F16C 96.41, **BMI1 95.39, FMA 95.36, AVX2 95.17, BMI2 95.12**, AVX512F 24.28.
- **Conclusions:**
  - The **median profile** is: 6 to 8 physical cores, 16 GB RAM, a GPU of the RTX 3060 class with 8 GB of VRAM, 1920x1080.
    The owner's machine (i9-12900K 8P+8E, RTX 3060, 32 GB) has exactly the median GPU, and more CPU and RAM than the median.
  - The **floor profile** (about the 10th to 15th percentile): 4 cores, 8 GB RAM, 4 GB VRAM or an integrated GPU, 1920x1080.
  - **x86-64-v3** (AVX2, FMA, BMI1, BMI2): the least supported of the four is BMI2 at 95.12 % of all surveyed machines, 97.2 %
    of those reporting x86 features (95.12 / 97.90). So the DLL's requirement excludes about **2.8 % of x86 Steam machines**
    (pre-Haswell Intel, pre-Excavator AMD, Atom/Celeron/Pentium without AVX2), none of which meets the floor profile's GPU and
    core count in practice. It does not touch the median. A machine without it must get a clear message at start, not a
    crash at the first AVX2 instruction (to check in the code: pending item M2).
- **Status: `proven`** for the profile's definition (primary data above). How to measure it: Q2.

### Q2. Can the profiles be measured without depending on the strength of the owner's machine? (pending item T4; 2026-11-11, session 2)

- **Problem (before):** every number of `log/performance.md` was taken on the i9-12900K's 24 threads and the RTX 3060. Nothing
  says what a 4-core machine with an integrated GPU does.
- **Searches:** `godot --help` of the installed 4.7.2 (`--gpu-index <device_index>`: "Use a specific GPU (only available on the
  Forward+/Mobile renderers; run with --verbose to get a list of available devices)"); context7 `/godotengine/godot-docs`
  (`RenderingServer.get_video_adapter_name`, `viewport_set_measure_render_time`, `get_rendering_info`); the machine
  (`Win32_VideoController`: Intel UHD Graphics 770 and NVIDIA GeForce RTX 3060; i9-12900K: logical processors 0-15 are the 8
  P-cores' threads, 16-23 the 8 E-cores); `tools/tasks/proc.py` (`run(..., hidden=True, cores=[...])`: a Job Object with an
  affinity, on the private desktop).
- **Test (isolated folder, outside the game tree):** `dist/research/q2/` (out of git): `proj/` (a Godot project: 20,000 boxes in one
  MultiMesh, a sun with shadows, 1920x1080, vsync off, 60 frames to settle, 180 sampled, the viewport's measured GPU time) and
  `run.py <gpu index|default> <cores|all> <tag>` (runs it through `tasks.proc.run`, hidden, 90 s deadline). Real output:

  | Run | Adapter (Godot's own line) | Affinity (checked by `aff.py`, `GetProcessAffinityMask`) | GPU ms p50 / p90 | Frame ms p50 / p90 |
  |---|---|---|---|---|
  | `default all` | Device #0: NVIDIA GeForce RTX 3060 | 0xffffff (24) | 1.66 / 1.87 | 1.73 / 1.85 |
  | `0 0..11` (six P-cores) | Device #0: NVIDIA GeForce RTX 3060 | 0xfff (12) | 1.33 / 1.80 | 1.52 / 1.52 |
  | `1 all` | Device #1: Intel(R) UHD Graphics 770 | 0xffffff (24) | 9.79 / 9.96 | 10.09 / 10.42 |
  | `1 16,17,18,19` (four E-cores) | Device #1: Intel(R) UHD Graphics 770 | 0xf0000 (4) | 10.15 / 10.68 | 10.61 / 11.11 |

- **After (criterion met):** the same scene is rendered, hidden, by the integrated GPU (5.9 times the RTX 3060's GPU time)
  and on four E-cores, with no window on the owner's screen and nothing installed. The floor profile is run in hardware:
  `--gpu-index 1`, cores 16-19; the median profile: `--gpu-index 0`, cores 0-11.
- **What it does not give, found by the test:** `OS.get_processor_count()` still answers 24 under the affinity, and the core's
  job pool sizes itself by `std::thread::hardware_concurrency()` (`native/src/core/job_pool.cpp:32`), which is the machine's
  count: on four cores it would start as many workers as on 24. A faithful floor run needs the pool to count the processors
  the process may run on (pending item M3); memory is capped by the Job Object's limit, not yet exercised here (owed: a run
  with `JOB_OBJECT_LIMIT_JOB_MEMORY` at 8 GB once the game is the subject).
- **Steps to repeat:** `cd dist/research/q2 && python run.py 1 16,17,18,19 igpu_4e` (log beside it, `igpu_4e.log`).
- **Status: `proven`** for GPU and processor selection; `open` for the memory cap and the pool's count.

### Q16. Why are a line's wires drawn from one view and not from another? (pending item V1; 2026-11-12, session 2)

- **Problem (before, an image):** `dist/trace/port-from-nov7/photos/00-tjoint_near_a_day.png` (four line poles, no conductor)
  against `01-tjoint_near_b_day.png` (the same poles' wires drawn), one run, the camera turned 180 degrees round one target
  (`plan_tjoint.json`: `setView [540, 468, 25, 30, 35]` and `[540, 468, 25, 210, 35]`).
- **Read in the code (not yet a trace):** a span is one instance of a per-chunk MultiMesh (`render/props/street_props.cpp`
  `push_span`); the chunk's wires are drawn twice, `wires_near` and `wires_far`, each with the chunk's box as custom AABB and a
  visibility range: `range(NEVER_END, near_end)` and `range(near_end, far_end)` (`prop_renderer.cpp:96-102`; `streets.json`
  `tube_end_m` 220, `far_end_m` 700), the margins 3 % of each bound (`rs_util.h` `RANGE_LINGER_SHARE`), fade disabled.
- **Searches:** context7 `/godotengine/godot-docs`, `tutorials/3d/visibility_ranges.rst` and `class_geometryinstance3d.rst`:
  the range's distance is measured "from the camera to the center of the instance's AABB"; with fade disabled the margins are
  a hysteresis ("will only change its visibility state when it goes over or under the threshold by this amount").
- **Hypothesis:** inside the hysteresis band round 220 m of a chunk's box centre, a camera that finds both levels new (the
  chunk just streamed in or rebuilt) sees neither. **Test:** `dist/research/q16/` (`proj/probe.gd`: two RenderingServer
  instances of one mesh, a 256 m box as custom AABB, ranges 0..220 and 220..700 with 3 % margins, a fresh process per
  first-sight distance; `python run.py ranges <distances>`).
  - **My first run was wrong and said the opposite** (`objects_drawn=1` at all thirteen distances): the probe moved the
    instance, and the engine measures to the centre of the instance's *transformed* box, so every distance was 124 m short and
    the band was never visited. Recorded because the wrong "failed" stood in this file for an hour (commit 62fe932d).
  - **Corrected probe (the instance at the origin), real output:**

    ```
    d_to_box_centre=200.0 objects_drawn=1      d_to_box_centre=222.0 objects_drawn=0
    d_to_box_centre=212.0 objects_drawn=1      d_to_box_centre=224.0 objects_drawn=0
    d_to_box_centre=214.0 objects_drawn=0      d_to_box_centre=226.0 objects_drawn=0
    d_to_box_centre=216.0 objects_drawn=0      d_to_box_centre=228.0 objects_drawn=1
    d_to_box_centre=218.0 objects_drawn=0      d_to_box_centre=240.0 objects_drawn=1
    d_to_box_centre=220.0 objects_drawn=0      d_to_box_centre=720.0 objects_drawn=0
    ```
    **The problem is reproduced:** from 214 to 226 m of the box's centre, at first sight, the engine draws neither level
    (each instance, not drawn before, waits to be inside its own range by its margin: 220 - 6.6 and 220 + 6.6).
- **In the game** (read whole in the trace `pole-no-wires`, `dist/trace/pole-no-wires/`): the chunk's box is the chunk grown
  by 63 m, from y -100 over 700 m (`rs_util.h:19`), its centre at y = 250. The photo's camera stands about 62 m from its
  target (zoom 25 px/m: half 21.2 m, lens about 37 degrees, `camera_profile.gd`), some 36 m over the ground, so about 193 m
  under the box's centre: the band 213.4 to 226.6 m is, in plan, a ring 91 to 119 m round the chunk's centre. The T joint is
  131 m from its chunk's centre in plan and the two shots stand 51 m either side of it (80 to 182 m): one of them falls in the
  ring, the other does not. That is the photo with no wires and the photo with them.
- **Solution applied in the test (after):** `python run.py owner <distances>`: no engine range; one owner shows exactly one
  level by its own rule each frame. Real output: `objects_drawn=1` at 150, 214, 216, 218, 220, 222, 224, 226, 240 and 400 m.
- **Conclusion:** the cause of V1 is a design decision: the level of a thing drawn is left to two engine instances that each
  decide alone, with a hysteresis of their own, by the distance to a box's centre; nothing guarantees one of them is drawn.
  The same decision swaps the poles' shadows (`prop_renderer.cpp:94-95`) and gives the buildings' prop casts a range with a
  begin (`building_renderer_reach.cpp:88`, to read). The fix (Phase 3, trace `pole-no-wires` P1, P2): one owner of the level
  drawn (`render/detail.h`, by pixels on the live lens, measured to the nearest point of the box), shown by instance
  visibility; a probe of the wire instances drawn from the band's distances as the failing test.
- **Status: `proven`** (the problem reproduced with its before, the solution with its after, the steps above).

### Q6. How is input injected so that the game cannot tell it from a player's? (pending items T3, F1; 2026-11-12, session 2)

- **Searches:** context7 `/godotengine/godot-docs`: `classes/class_input.rst` (`parse_input_event`: "Feeds an InputEvent to the
  game ... has no influence on the operating system ... will not move the OS mouse cursor"), `classes/class_viewport.rst`
  (`push_input`: "does not remap the specified event based on project settings"; the order `_input`, `_gui_input`,
  `_shortcut_input`, `_unhandled_key_input`, `_unhandled_input`), `tutorials/inputs/inputevent.rst` (the DisplayServer reads the
  operating system's events and feeds them to the root window: `parse_input_event` enters at that point). The game's driver:
  `project/tests/visual/ui_driver.gd` (`move`, `button`, `key` set a position, `pressed` and a keycode, nothing else).
- **Problem reproduced (before):** `dist/research/q6/` (`proj/probe.gd`: a Button, a LineEdit, a pad that drags only while the
  motion event says the left button is held, as a game tool does; the events sent by `Input.parse_input_event`, flushed, a
  process and a physics frame awaited after each; run hidden). `python run.py bare` (events as `ui_driver.gd` makes them):

  ```
  mode=bare window_focused=true
  click: button_presses=1 polled_down_while_held=true
  drag: dragged=(0.0, 0.0) motions_with_mask=0 motions_without_mask=21 max_velocity=0 last_polled_velocity=(0.0, 0.0)
  type: line_has_focus=true text=""
  ground: wheel_factor_sum=1.0 double_clicks=0 shift_clicks=0 unhandled_keys=["R", "U", "A", "Space", "7", "X", "X", "PageUp"]
  ```
  A drag moves nothing; typing into a focused text field writes nothing and every letter falls through to the game as a
  shortcut; the wheel's factor, a double click and a Shift-click are not seen. So a capture "driven as the player" by the
  present driver cannot drag a handle, type a name or zoom by a notch as a player does: features pass or fail for reasons a
  player never meets.
- **Solution applied in the test (after):** `python run.py full`: every field the DisplayServer fills: `button_mask` held
  through the drag, `relative`, `screen_relative`, `velocity`, `screen_velocity`; `double_click`, modifiers and the wheel's
  `factor` on buttons; `physical_keycode`, `key_label`, `unicode` and `echo` on keys:

  ```
  mode=full window_focused=true
  click: button_presses=1 polled_down_while_held=true
  drag: dragged=(200.0, 0.0) motions_with_mask=20 motions_without_mask=1 max_velocity=625 last_polled_velocity=(428.6952, 0.0)
  type: line_has_focus=true text="Rua 7xx"
  ground: wheel_factor_sum=2.5 double_clicks=1 shift_clicks=1 unhandled_keys=["PageUp"] key_polled_while_held=true
  ```
  The drag is the 200 px sent, the text is what was typed (the held key's echo included), the wheel, the double click and the
  Shift-click arrive, only the key nobody handles reaches `_unhandled_input`, and the polled state (`is_mouse_button_pressed`,
  `is_key_pressed`, `get_last_mouse_velocity`) agrees.
- **Focus and the owner's desk:** the probe ran on the private desktop with no window on the owner's screen; nothing moved the
  operating system's cursor (the documentation's own guarantee). Still to prove on a window shown without focus on the
  owner's desktop (with Q4).
- **A limit found by the test:** `get_viewport().get_mouse_position()` answered `(192928352, -206)`: the viewport asks the
  operating system's cursor, which synthetic events do not move. Code that polls it instead of reading the event's position
  would see the owner's real cursor. The game polls a mouse position in one place (`grep`: `project/ui/hud_tip.gd` or
  `project/ui/junction_marks.gd`; to read in the tool's trace): that reader must take the position from the events.
- **Conclusion:** one owner of synthetic input that builds events with every field (the design of
  `log/live-instance-design.md` Part E stands, proven on the engine); `ui_driver.gd` moves onto it. **Status: `proven`** for
  the input path; `open` for the unfocused visible window and for recording and replaying a play.

### Q4. Can a C++ change reach a running game of the official engine, with no engine build of our own? (pending items T1, L5; 2026-11-12, session 2)

- **Problem (before):** every C++ change costs a new process, the city generated again and every chunk streamed again.
  `log/live-instance-design.md` Part A.1 answers with a development build of the engine patched to let a game process reload
  extensions. Is the patch needed?
- **Searches:** context7 `/godotengine/godot-docs` `classes/class_gdextensionmanager.rst`: `reload_extension` ("You can only
  reload extensions in the editor; in release builds, this method always fails"), `load_extension`, `unload_extension`
  (no such restriction written), `is_extension_loaded`.
- **Test:** `dist/research/q4/`: `src/probe_ext.cpp` (a GDExtension class `ProbeCounter` that owns a thread, joined in its
  destructor, as the core owns its simulation thread), compiled twice against the game's own built godot-cpp
  (`native/build-win-debug/bin/libgodot-cpp.windows.template_debug.x86_64.a`) with `-DPROBE_VERSION=1` and `=2`; `proj/`
  runs on the installed Godot 4.7.2 (`editor_hint=false`), hidden. `python run.py` (first part, the extension loaded at run
  time), real output:

  ```
  load: OK (5.3 ms)
  first: version=1 ticks_after_100ms=42
  copy over a loaded library -> copy v2 -> bin/probe.dll: Can't open file
  ERROR: GDExtension reloading is disabled.   at: reload_extension (core/extension/gdextension_manager.cpp:162)
  engine reload_extension: FAILED (2.0 ms)
  unload: OK (0.3 ms); copy v2 -> bin/probe.dll: OK (2.0 ms); load: OK (4.1 ms)
  after unload, replace, load: version=2 ticks_after_100ms=57
  20 more cycles done; static memory 44.8 MiB
  ```
- **Second part, as the game is** (the extension loaded at the engine's start through `.godot/extension_list.cfg`, a GDScript
  typed against its class, `typed_user.gd`: `var counter: ProbeCounter`), `python run.py typed`:
  - Without more, the typed script **crashes** after the swap (signal 11 in `make`, `typed_user.gd:6`): the compiled script
    keeps what it resolved in the old library.
  - With `script.reload(true)` after the load (`python run.py typed_dropped`, and `typed_alive --alive` with an instance of the
    script kept alive across it):

    ```
    unload: OK; replace: OK; load: OK (2.2 ms in all)
    script.reload(keep_state) with its instance alive: OK
    typed script after its own reload: version=2
    new instance of the cached script, after: version=2
    a script compiled after the reload: reload=OK   its version=2
    unload with an object of the class alive: OK; object still valid: true
    ```
  - The last line is a trap, not a success: the engine unloads a library while an object of its class lives and says OK; the
    process then dies with an access violation when that object is destroyed (exit 3221225477 in both runs). A script typed
    against an extension's class and compiled before a run-time `load_extension` of an extension that was **not** loaded at
    start does not compile at all ("Native class not found"): the swap works only for an extension loaded at start.
- **Conclusion:** on the official 4.7.2, with no patch: (1) the engine's own reload is refused in a game; (2) `unload_extension`,
  a new library, `load_extension` works in about 2 to 6 ms; (3) it is sound only if **no object of the extension's classes is
  alive at the unload** (the core torn down whole: its threads joined, its RIDs freed, the city carried as the save's bytes)
  and **every GDScript that names an extension class is reloaded after the load** (`reload(true)` keeps its instances); (4) a
  loaded library is locked on disk, so the build writes beside it and the swap copies it between unload and load. The
  engine build with the reload patch is not needed for the reload; whether it is needed for the engine's own Tracy zones is
  Q5. **Status: `proven`** for the mechanism in isolation; `open` for the game itself (a teardown that leaves nothing, the
  time to carry and rebuild the town and km24, Terrain3D across it, and the scripts, shaders and data reloaded in place).

### Q5. Which sources give the process's GPU, memory and CPU live, from outside it? (pending item T2; 2026-11-12, session 2)

- **Problem (before):** the in-game readings (F7, F9, Shift+F9) live in the window; the budgets are of the whole process
  (GPU use, video memory, RAM, CPU by thread), which the engine's own monitors do not give.
- **Searches:** the machine's own counters (`Get-Counter -ListSet 'GPU*'`: `GPU Engine`, `GPU Process Memory`); context7
  `/godotengine/godot-docs` `class_renderingserver.rst` (`RENDERING_INFO_VIDEO_MEM_USED`: what the renderer tracks, "always
  greater than the sum of texture and buffer memory").
- **Test:** `dist/research/q5/` (`proj/`: the Q2 scene rendering hidden for 24 s, printing its PID; `run.py <gpu index>`
  samples by PID from another process five times: PDH `\GPU Engine(pid_N_*)\Utilization Percentage`,
  `\GPU Process Memory(pid_N_*)\Dedicated Usage` and `Shared Usage`, the working set and each thread's processor time).
  Real output (`python run.py 0`):

  ```
  [0] sample_ms=1775 gpu_engines: ..._eng_0_engtype_3d=71.7 | dedicated_mb=340 shared_mb=109 | ws_mb=501 cpu_s=2.1 threads=55 top_thread_ms: t24932=1406 t20156=281 ...
  [2] sample_ms=1475 gpu_engines: ..._eng_0_engtype_3d=73.8 | dedicated_mb=340 shared_mb=109 | ws_mb=501 cpu_s=6.9 threads=55 top_thread_ms: t24932=3328 t20156=3109 ...
  [4] sample_ms=1476 gpu_engines: ..._eng_0_engtype_3d=53.1 | dedicated_mb=340 shared_mb=109 | ws_mb=502 cpu_s=12.7 threads=55 top_thread_ms: t20156=6109 t24932=6031 ...
  engine: pid=6828 static_mb=56
  engine: gpu_ms p50=1.14 p90=1.36 | frame_ms p50=1.19 p90=1.39 | video_mb=109
  ```
- **Found:** (1) the operating system gives, by PID and with nothing compiled into the game: the GPU's 3D engine use, the
  dedicated and shared GPU memory, the working set, the CPU time of every thread. (2) **The engine's own numbers understate
  the budgets' quantities**: video memory 109 MB by `RENDERING_INFO_VIDEO_MEM_USED` against 340 MB dedicated on the card;
  static memory 56 MB against a 501 MB working set. A budget of memory checked on the engine's monitors would pass a game
  that fails on a real 8 GB machine: the measures of `log/specification.md` (video 2.0 to 2.8 GB at km24) are the engine's
  and must be taken again by PID. (3) A PowerShell `Get-Counter` sample costs about 1.5 s: fine for a measure's summary, too
  slow for a live monitor, which must query PDH natively (`pdh.dll`: `PdhOpenQuery`, `PdhAddEnglishCounter`,
  `PdhCollectQueryData`, milliseconds a sample) from the supervisor, outside the game, so the game pays nothing.
- **Status: `proven`** for the sources and for what they change in the budgets' measure; `open` for the native sampler's cost,
  for GPU time by pass and lock waits (inside the game: `RenderingServer` measured times, the core's own stamps), and for
  Tracy (whether the engine's zones are worth an engine build, now that Q4 shows the reload does not need one).

### Q12. What does the `low` tier pay per pixel on an integrated GPU? (pending item M5; 2026-11-12, session 2) — first attempt, not answered

- **Tried:** a look plan outside the game tree (`dist/research/q12/plan.json`: the town at three views, `{"passes": true}`,
  `{"info": true}`, a shot each) run on the floor profile with `--preset=low`
  (`python dist/research/measure/run_game.py floor q12_low_floor 560 res://tests/visual/look.tscn --preset=low --plan=... --capture-dir=...`).
- **Real output:** GPU 208.9, 198.4 and 134.5 ms at the three views, with 452 to 553 shadow draws and up to 6.0 M shadow
  primitives, though `low` has `shadows: false`; `passes` gave one entry (`vp_begin`); the log opens with an engine error
  (pending item V9).
- **What it showed instead of the answer:** `look.tscn` does not read `--preset=`: it applies `PairCamera.quality`
  (`look.gd:50`), a tier with shadows and casting lamps. The run measured that tier, not `low`; and the plan's pass step does
  not split the frame. Both are defects of the diagnostic tool (pending item T8), found because the numbers did not fit
  `tier_gpu`'s (35 to 63 ms at `low`).
- **Status: `failed: the tool measured another tier`.** To answer Q12 the plan must name its tier and the stamps by pass must
  be on (T8 first), or `tier_gpu` must print the passes; then: `low` with the render scale at 1, 0.75 and 0.5, each pass's
  time, and the photos of each by the image protocol.

## Open questions (each becomes an entry above when researched; the pending item it serves in the last column)

| Q | Question | Serves |
|---|---|---|
| Q3 | Which library prints "SetThreadGroupAffinity failed" under an affinity, and does it matter? | the measure's noise |
| Q4 | What gives a change seen in a running game in seconds: Godot 4.7's own reload of scripts, shaders and resources in a running game; GDExtension reload outside the editor (`reloadable`, the engine's reload being the editor's); a restart from a save with the view kept; a development engine build. Which is fastest, lightest and exact, proven on this game? | T1, L5 |
| Q5 | Which sources give CPU per thread, GPU time and use, VRAM, RAM, tick time and lock waits live, at a cost under a few percent: Godot's `Performance` monitors and `EngineDebugger` profilers, `RenderingServer` measured times, Windows PDH GPU counters, Tracy? | T2 |
| Q6 | How is input injected so that the game cannot tell it from a player's: `Input.parse_input_event` and `Viewport.push_input` with every field the DisplayServer fills (button mask, relative, velocity, modifiers, double click, wheel factor, unicode, echo), flushed and awaited; what GdUnit4's scene runner does; how focus stays with the owner? | T3, F1 |
| Q7 | How must a city's traffic be simulated so that it is the city's and not the camera's, at 24 km: agents with trips; mesoscopic queues far, microscopic (IDM, MOBIL) near, with state kept across the change of detail; hierarchical and asynchronous routing; time-sliced updates; determinism | S1, S6, R3 |
| Q8 | How are crowds kept apart correctly and cheaply (ORCA / RVO2, social forces, position-based crowds, flow fields), deterministic on a fixed tick | S5, R3 |
| Q9 | How do city builders simulate growth, demand, economy and land value (RCI demand, budgets, upkeep), and which of it this game wants | S4 |
| Q10 | Fixed-step simulation at multiples of real time: sub-stepping against a budget, dropped time counted, interpolation of poses | S3 |
| Q11 | Damage and fire stepped apart from the render's locks: a schedule that does not depend on try-locks | S2 |
| Q12 | A low tier for integrated GPUs in Godot 4.7 Forward+ and Mobile: render scale (`scaling_3d_scale`, FSR 1/2), half-resolution passes, the Mobile renderer's limits, shader variants per tier; what costs per pixel in this game's ground, road and building shaders | M5, M4 |
| Q13 | Sun shadows over a 24 km city: cascades by distance, static shadow caching, what Godot 4.7 offers; screen passes at reduced resolution | M4 |
| Q14 | Terrain3D at 24 km: regions, LOD, collision, what its own documentation says about cost | M4 |
| Q15 | Loading: what the 23.6 s of km24 are made of (read first), then pipelines precompiled, generation on jobs, a city opened aside | M6 |
| Q16 | Thin geometry seen from every side: how wires are drawn (ribbons facing the camera, cylinders, line width under a pixel, culling and double-sided materials, MSAA) | V1 |
| Q17 | A check of the CPU at start for AVX2/FMA/BMI before the DLL's first instruction that needs them: where it can run (a launcher, the `.gdextension` entry compiled at a lower level) | M2 |
| Q18 | A guard for photos at the commit: where the analysis is recorded, how a hook knows a theme's photos and their analyses | T6, L4 |
| Q19 | One process, many checks: scenes sharing one generated city, a city cache keyed by the generator's inputs, so that a test does not pay a town each | L5 |
| Q20 | The probes on the release DLL without exporting: a `.gdextension` choice by feature tag or a `--release-core` run | T5 |
