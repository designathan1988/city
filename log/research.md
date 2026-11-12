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
