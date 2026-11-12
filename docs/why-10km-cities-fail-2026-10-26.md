# Why a 10 km city does not fit yet: the cause, the 4 GB question, the engines, and the fix

2026-10-26. Measurements from Chrome (headless, D3D11) at 1920×1080 on the owner's machine. The cause comes from reading the code, with no new runs: the owner stopped the tests. Every outside claim links its source.

## Short answer

The cause is **architecture**, not the renderer, the engine or the triangle counts. The GPU frame stays at 7–14 ms at every city size measured. What breaks is the page's CPU memory and time. The page keeps the **whole map** as JavaScript objects and builds the whole map's derived data inside **one V8 cage of 4 GB per process**, which the page and its Web Workers share. The city is also generated on **one thread**. As the map grows, the page heap grows with it until the process dies. Each extra city size also leaves less room in the cage for workers, so it gets slower as well as heavier.

The fix was already designed in `reports/Arquitetura para cidades grandes.md`, phases 1–3:

- a columnar document;
- derived data per cell, bounded by the view, under a memory budget;
- parallel generation.

It was never carried out. Only the buildings moved to a binary table. Everything done since has tuned the GPU side, which was never the limit, so the gains were small.

## What was measured

| | Large city (r 1.6 km) | 3× large (r 2.77 km) | 10 km × 10 km |
|---|---|---|---|
| Buildings | 8,102 | 24,690 | 111,274 |
| Time until the city shows | 32 s | 44 s | 153 s (95 s generating on one worker) |
| Page JS heap, peak (`usedJSHeapSize`) | 1,436 MB | 2,308 MB | 3,039 MB while still opening, then the page died (twice) |
| Page + workers heap, peak | 2,221 MB | 3,295 MB | (not reached) |
| All Chrome processes, peak | 6.9 GB | 9.5 GB | (not reached) |
| GPU frame p50 across 12–13 views | 7.5–13.3 ms | 7.3–14.3 ms | (not reached) |
| Main-thread frame, worst | 20 ms | 422 ms | (not reached) |
| Draw calls / triangles per frame | 286–540 / 1.4–6.5 M | 285–711 / 1.6–12.3 M | (not reached) |

The heap timeline of the 10 km city (`x10km.log` in the session's scratchpad) shows where the memory goes:

- **During generation (0–95 s):** the page heap stays at 410–500 MB, because the generator is a worker.
- **When the city arrives (95–102 s):** it jumps to **1,568 MB**.
- **While the page builds the world (102–134 s):** it climbs to **3,039 MB** within 32 s, still "opening", and the process then died.

**Correction (same day).** An earlier version of this report said the 1,568 MB jump was the document applied into objects. That was an inference, not a measurement, and it was wrong. A sampling heap profile of the same city (one run, afterwards) gives these numbers:

- **The document's live objects:** about 80 MB.
- **The page's whole live JS heap after a collection:** 0.54–0.74 GB.
- **The heap peaks are mostly garbage.** A forced collection took the heap from 1,871 MB to 541 MB in 7 s.
- **The page's process grew from 2.7 GB to 7.8 GB** during the 66 s of the whole-map fill after the reveal (OS private bytes), then died without the camera moving.

So the memory that kills the page comes from building the whole map's derived data (cause 2 below), not from the document. The verdict does not change.

**How the architecture is judged.** From here on, the architecture is judged from the code, not from measurements. Any page code that walks, builds or keeps something for the whole map, rather than what is near the camera, is wrong for 10 km cities.

## The cause in the code

### 1. The document lives in the page as one JS object per thing

These document parts are `Map`s of objects:

- road nodes and segments (`src/world/doc.ts:153-154`);
- poles and spans (`:160-161`);
- barriers (`:168`);
- landscape items (`:176`);
- lots (`src/world/lotStore.ts:35`).

The 10 km city holds this many of them:

| Part | Objects |
|---|---|
| Landscape items | 228,558 |
| Lots | 111,274 |
| Poles | 63,952 |
| Segments | 23,296 |
| Nodes | 11,760 |

Spatial indexes sit on top of them as more `Map`s of arrays (`src/world/cells.ts:64-67`, `src/world/poleIndex.ts:27-29`, `src/world/pointFiling.ts:22`). Only the buildings follow the binary layout (`src/world/buildings/recordTable.ts:302`, `items.ts`), and that is the layout the rest needs.

### 2. The page builds and keeps the whole map's derived data

- **One elevation solve for the whole network, on the page:** `src/render/renderer.ts:2425-2432`.
- **The whole network after the reveal:** the fill builds every road tile of the network's whole extent (`renderer.ts:2393-2395`). Its own comment records "the 3.4 GB peak of the metropolis".
- **Text keys per block:** the fill's blocks are keyed by strings `${i},${j}` (`renderer.ts:2354-2358`).
- **Only distance-limited meshes:** road blocks drop their full mesh when far (`src/render/mesh/roadResidency.ts`) and building masses go by cell. The data they are remade from stays resident for the whole map.

### 3. One cage for the page and its workers

`src/jobs/jobPool.ts:296-310` sizes the worker pool by the room left in the cage. Its comment records that the page reached 2.7 GB and that workers on top of it took the cage past 4 GB, after which Chrome killed the page (2026-10-18). The bigger the city, the fewer workers fit, so heavy cities also get slower.

The desktop build already moves two long job workers into Electron utility processes, each with a cage of its own (`desktop/main.cjs:263-271`). The page itself still holds everything.

### 4. Serial generation and main-thread stalls

`openCityNow` (`src/main.ts:2179-2193`) asks one worker for the whole city, waits for the network bake, then applies the whole snapshot on the page (`openCity`, `src/main.ts:2217-2231`). That took 95 s for 111k buildings. In the 3× city the main thread stalled for up to 422 ms while the world filled in.

## The 4 GB limit: real, per process, and only for JS objects

The limit is real and not a setting that was missed:

- **Electron enabled pointer compression in Electron 14**, and "the V8 heap is limited to a maximum size of 4GB" ([Electron: V8 memory cage](https://www.electronjs.org/blog/v8-memory-cage)).
- **The whole process shares one cage.** Since V8 9.2, "all Isolates within a process share the same 4GB virtual memory cage". The cost: "the total V8 heap size across all threads in a process is capped at a maximum 4GB" ([V8 v9.2](https://v8.dev/blog/v8-release-92)). That process covers the page and every Web Worker.
- **Flags cannot lift it.** `--max-old-space-size` cannot go past the cage. Opting out is a build flag (`v8_enable_pointer_compression_shared_cage = false`, or pointer compression off), so it needs a custom Electron build.

It is not a ceiling on the game's memory:

- **Binary data sits outside the cage.** "ArrayBuffers are counted separately from the rest of the V8 heap" ([Electron](https://www.electronjs.org/blog/v8-memory-cage)). Typed-array data does not count against the 4 GB.
- **Each process has its own cage.** An Electron [`utilityProcess`](https://www.electronjs.org/docs/latest/api/utility-process) gets a separate one, which the game already uses for two job processes.
- **Electron documents two more ways out:** a Node child built without pointer compression, or a custom Electron build ([Electron](https://www.electronjs.org/blog/v8-memory-cage)).
- **WebAssembly can go past 4 GB.** Memory64 shipped in Chrome 133 ([SpiderMonkey on Memory64](https://spidermonkey.dev/blog/2025/01/15/is-memory64-actually-worth-using.html)). It runs about 10% to over 100% slower than 32-bit wasm, so it only pays when over 4 GB is truly needed.

Node.js hit the same wall and fixed it with per-isolate cages (IsolateGroups), which Electron does not ship ([Platformatic](https://blog.platformatic.dev/we-cut-nodejs-memory-in-half)).

In short: 4 GB is the limit for **JS objects per process**. Engines in C++ or C# do not keep 100 km² of world as loose objects either. They use compact arrays and keep only the derived data near the camera.

## Engines and languages

Changing the engine does not remove the cause, because the limit is the data layout. Any engine needs the same compact data and view-bounded derived data. What changes is the cost of getting there:

| Option | What it gives | What it costs |
|---|---|---|
| **Stay: TypeScript + three.js + Electron, rebuild the data layout** | GPU already fine (7–14 ms). The buildings' table proves the pattern. utilityProcess and job pool already exist. | Weeks per phase (the previous report estimates 4–8 for the document, 3–5 for derived data). No rewrite. |
| **Godot 4** | Native C++ renderer. MultiMesh, visibility ranges (HLOD), occlusion culling, large-world coordinates. | Nodes cost too much for "tens of thousands of instances": the docs send that to the servers API ([Godot docs](https://docs.godotengine.org/en/stable/tutorials/performance/using_servers.html)). No GPU-driven renderer in mainline: [reduz's proposal](https://gist.github.com/reduz/c5769d0e705d8ab7ac187d63be0099b5) is a design, deferred in 2023. No TS→GDScript/C# converter, so ~212k lines are ported by hand, including the 87k-line renderer and its shaders. [GodotJS](https://github.com/ialex32x/GodotJS) runs TypeScript on V8 but calls itself "still under testing", with workers "experimental". |
| **Unity 6 (DOTS/Entities + GPU Resident Drawer)** | The most proven stack for this map size: Cities: Skylines II covers 159 km² ([Wikipedia](https://en.wikipedia.org/wiki/Cities:_Skylines_II)). Data-oriented ECS in native memory, Burst jobs. | Full rewrite in C#. CS2 shipped on this very stack with crippling performance, from its own choices: props without LODs, no occlusion culling, shadows at 72% of draws ([Huhtala](https://blog.paavo.me/cities-skylines-2-performance/)). |
| **Unreal 5** | World Partition, HLOD, Mass, Nanite. | Full rewrite in C++. Built around authored worlds rather than worlds generated at run time. Heaviest toolchain. |
| **Rust (Bevy or wgpu) / custom C++ (bgfx)** | Full control, no GC, no cage. NIMBY Rails draws the whole world in C++ with bgfx, one developer ([retrospective](https://carloscarrasco.com/nimby-rails-retrospective/)). | Full rewrite plus an engine to write. Bevy is pre-1.0 and breaks APIs every release. |
| **Hybrid: hot cores in Rust/C++ as WASM or a native process, three.js kept** | Memory outside the JS heap, faster solvers and generation. | Only pays after the data is columnar. Native addons in Electron must copy buffers into the cage ([Electron](https://www.electronjs.org/blog/v8-memory-cage)). |

What real games show:

- **Vampire Survivors** left Phaser for Unity for performance ([GamingOnLinux](https://www.gamingonlinux.com/2023/07/vampire-survivors-switching-to-new-game-engine-on-august-17/)). It cost almost a year.
- **CS2** shows that an engine built for scale still fails when content and culling are not disciplined.
- **NIMBY Rails and Transport Fever** show that large maps come from preprocessed, compact data and level of detail, not from the engine's name.

## Decision: rebuild the data architecture in the current stack

These steps are the ones in `reports/Arquitetura para cidades grandes.md`, now confirmed by measurement. The document stays whole in memory (the owner rejected streaming the document), but compact.

1. **Columnar document.** Lots, nodes, segments, poles, spans, barriers and landscape move to typed-array columns with dense ids, the way the buildings already are. Variable-length data goes into one pool per kind, and objects exist only as short-lived views for tools. This puts the document outside the 4 GB heap and makes it an order of magnitude smaller.
2. **Derived data per cell, bounded by the view.** This covers road tiles, the elevation solve, ground and landscape layers. Each cell has a stage, a byte budget and LRU eviction for both CPU and GPU, plus a disk cache by hash. The whole-extent fill and the whole-network solve go away.
3. **Parallel generation by block,** with counter-based random numbers, in separate processes with their own cages (utilityProcess). The page only receives transferred buffers.
4. **The simulation as owner of its state** in its own process, publishing deltas for visible cells.

Acceptance, measured once at the end on the 10 km city:

- the page heap stays flat when the map doubles with the same view (target ≤ 1.3 GB);
- the city opens and is playable without dying;
- no main-thread frame over 50 ms while it fills in;
- nothing disappears or changes appearance.

## Confirmed vs not confirmed

- **Confirmed by measurement:** the GPU times; the heap curve and the page dying on the 10 km city; the 95 s serial generation; the 422 ms stalls.
- **Confirmed by the code:** the object maps, the whole-network solve, the whole-extent fill, the one-worker generation and the cage-bound worker count.
- **Confirmed by official sources:** the 4 GB cage per process, shared with workers.
- **Not measured:** how the ~3 GB splits among those structures. That needs one heap profile (`perf/heap.mjs` in the scratchpad is ready), which was not run because the owner stopped the tests.
- **Not retaken:** photos framing each city whole, for the same reason.
