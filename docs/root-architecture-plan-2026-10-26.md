# Root re-architecture for 10 km cities: the plan

2026-10-26. The goal, in the owner's words: create, open, save and play a real 10 km city fast on any mid-range PC, everything real and working, no cooked city, no stand-ins. The only metric is correct code with the right optimisation. This plan comes from reading the code on the path of each step, and from one timed reopen of the owner's 10 km save in the open game window (249,122 buildings, 52,333 roads, 24-thread machine with 32 GB of RAM). It builds on `reports/Arquitetura para cidades grandes.md` and `reports/why-10km-cities-fail-2026-10-26.md`.

## What the reopen shows

The city appears after 190-207 s. It never appeared before commit `92296e0b`. In order:

| Step | Time | Where it runs | Cause in the code |
|---|---|---|---|
| Save read, squares decoded and joined | 2-21 s | page main thread | `editor/chunkedSave.ts` `readSquares`, `joined` (`copyRows` row by row) |
| Restore, whole network rebuilt | 21-29 s | page main thread | `editor/history.ts:185` `net.rebuild({ full: true })`: every junction and ribbon of the map |
| World waits for the roads' stations | 30-50 s (up to 77 s) | **one** job worker | see below |
| Road surfaces of the view | 65-133 s | the page idle, waiting on workers | see below |
| Buildings: every plate's mass base, the view's cells | 133-190 s | workers | `render/buildings/layer.ts` `coarsePending` waits for `basesWaiting`: every base of the map |
| Simulation starts | ~100 s | the simulation's process, fed by the page's pool | `main.ts:678` `sim.start`, its bays and conflicts on the page's lanes (`simLanes`) |

### The root cause: the thread architecture

Read in the pool's state during the reopen: **the pool had one worker**, with 155 lane messages waiting on it, while 22 cores stood idle.

- `jobs/jobPool.ts` `jobWorkers` sizes the whole pool by the page's V8 cage (`POOL_CAGE_SHARE` of 4 GB: 9 workers), even on the desktop, where the long side runs in job processes that each have a cage of their own (Electron `utilityProcess`, [docs](https://www.electronjs.org/docs/latest/api/utility-process); V8 shares one cage between the isolates of one process, [V8 v9.2](https://v8.dev/blog/v8-release-92)).
- Of those 9, the long side takes 7, and they serve only the long jobs and the road tiles (`PROCESS_LANES = ['roadTiles']`). The short side keeps 2 in the page, one kept for the save: **`laneWorkers()` is 1**. Every other kind's lanes queue on that one worker: the simulation's lot bays and conflict pairs (`main.ts` `simLanes`), the roads' stations, the ground's shapes, the plates' ground, ecology, terrain light, people, fracture.
- `desktop/main.cjs:287` makes **2 job processes** whatever the machine, the long threads shared between them.

So a 10 km opening runs on one thread. No optimisation of any one step can fix that: the threads have to be re-architected.

### The second cause: whole-map work on the opening's critical path

The opening waits for work that covers the whole map, and part of it runs on the page:

- the whole network rebuilt on the page (`history.ts:185`). The network is derived data. The page only needs the window near the view. `world/network.ts` already supports a window (`scope`, `holds`, `complete`) and junctions solved off the page (`offerBake`).
- the far roads asked once for the whole map, built from the page's whole network (`render/networkFarRoads.ts` `request` → `farRoadInput` over `net.ribbons`). This is why the page network window (3867fcae) had to be reverted: it moved the whole-network solve into one job, and the far level waited on it.
- every plate's mass base before the curtain (`layer.ts` `coarsePending`).
- the save decoded and joined on the main thread.

### Fixed in this session

- `92296e0b`: the plants' and covers' tests were run for the whole map in one frame. They are now done per cell, nearest the view first, under a per-frame budget.
- `ce13133a`: the terrain atlas no longer decodes every record to find its plates.

## Principles (every phase follows them)

1. **The page holds the document and the view.** Anything derived (network pieces, road meshes, far levels, building levels, plants, heights) is a pure function of a cell and the document, computed off the page and handed back as transferred buffers. It is held under a budget near the view, and kept on disk by hash.
2. **No step covers the whole map, on the page or off it.** A whole-map level is the union of per-cell results, built progressively and nearest first. Nothing waits for all of them.
3. **Threads come from the machine, not from the page's cage.** On the desktop, work runs in job processes sized by the cores and the RAM. The page keeps only the workers that need its memory (IndexedDB, the save).
4. **The opening waits only for what its first picture draws.** Everything else follows, nearest first, without a frame over budget.
5. **Every step is shown in the game window with photos.** Nothing is claimed that was not seen.

## The phases, in dependency order

### Phase 1: threads (now)

- Size the desktop pool by the cores and the RAM, outside the page's cage: `jobWorkers` on the desktop = min(cores - 2, RAM budget / a worker's peak). The page side stays bounded by `pageRoomWorkers`.
- Scale the job processes with the threads: enough processes that each holds a few threads (a 4 GB cage each), not a fixed 2. The shell gets the count from the page.
- Run every lane whose messages carry what they read in the job processes, not only the road tiles: the simulation's bays and conflicts, the roads' coarse levels and stations, the ground's shapes and plates, ecology, terrain light, landscape strokes, fracture, the people's levels and the citizens' bake. Only the save and the derived-cache writer stay in the page, because they use its IndexedDB.
- Accept when the pool in the game window shows the lanes spread over the job threads during the 10 km reopen, the world starts without waiting behind one worker, and nothing fails to load in Node. Seen with photos.

### Phase 2: the page holds the network near the view

- The page's `Network` holds the window (the view's road cells, the simulation's micro window, the tool's pointer), as the simulation's twin already does.
- Junctions and ribbons are solved per tile on the job threads (stateless, from the tile's roads and those at their ends).
- The far roads are made **per far cell** from those tile bakes, nearest the view first, and never from a whole-network solve. `NetworkFarRoads` already makes single cells (`only`). The input changes from the page's network to the tile bakes.
- `restoreInto` no longer rebuilds the whole network on the page.

### Phase 3: the opening's critical path is the view's

- The save is read, decompressed and joined in a worker. The page receives the tables by transfer. Lots get their box columns as they are joined.
- The mass bases come off the critical path: those in the view first, the rest after the curtain, nearest first, kept on disk by hash.
- The simulation starts from the opened tables, its lanes in the job processes (phase 1).

### Phase 4: creation in parallel

- The city is laid out once, then grown tile by tile on the job threads (`editor/cityTiles.ts`, `cityInTiles.ts`, `cityGrowth.worker.ts`, written and not yet tested in the game). The edge buildings are handed over as tables.
- The new city then opens through the same view-bounded path as a reopened one.

### Phase 5: the rest of the document in columns

- Nodes, segments, barriers and the remaining lists move to tables like the buildings, lots and landscaping.
- Undo becomes a log of diffs.
- Saving writes only the changed squares, in a worker.

## Acceptance, on the 10 km city, in the game window, on a mid-range profile too

- Create, save, reopen and play the real 10 km city. Every element is real and nothing is missing: compared by photo.
- No frame of the page blocked for more than 50 ms while the city opens or fills in.
- The opening waits only for the first view. The rest arrives nearest first.
- Checked once on this machine and once with the weak-machine profile (`core/weakMachine.ts`: fewer threads), so the threads' sizing holds on a mid-range PC.
