# Performance: where the 24 km city spends, fix by fix

The record of the performance line on the tree taken back to `a1a771c4` (2026-11-10). Every number is the 24 km city
(map `km24`, index 4, seed 1) through `tests/visual/look.tscn` at 1920x1080 with the tests' DLL, each view read only once
`render_pending` is 0. Views: centre (`setView [0, 0, 1.2, 30, 40]`) and 9 km out (`[-9000, 4000, 1.2, 210, 40]`).

## Base (commit a1a771c4, 2026-11-10)

| View | Video | Buffers | Godot static | GPU a frame | Core allocator committed |
|---|---|---|---|---|---|
| Centre | 5,252 to 5,454 MiB | 4,084 to 4,281 MiB | 1,309 to 1,351 MiB | 16.2 to 16.6 ms | 3,463 MiB |
| 9 km out | 3,773 to 4,009 MiB | 2,622 to 2,837 MiB | 868 to 898 MiB | 14.0 to 14.5 ms | 3,239 MiB |

The city opens in 20 to 25 s. The ranges are two runs with the views taken in a different order.

## P1 (2026-11-10): the core's allocator gives memory back

- Problem: the allocator (mimalloc v2.2.4, a static library in a DLL built by clang for MinGW) registers its load and
  thread-end hooks through MSVC pragmas this compiler ignores (`src/prim/windows/prim.c`). It never left its loading state:
  freed memory was only marked reusable (`MEM_RESET`), never given back, and the heap of a thread that ended was never freed.
- Test first: `tests/core/memory_test` (probe `memory_churn`). A thread takes 256 MiB in blocks of a MiB, frees all but one
  in sixteen and gives back. Before: 256 MiB still held for 16 MiB alive, and one heap left when the thread ended. After:
  17 MiB held, none left.
- Fix: `native/CMakeLists.txt` sets `MI_WIN_USE_FLS`, the allocator's own path outside MSVC (a constructor runs its start,
  a fiber-local callback each thread's end). `core/memory.h` is the allocator's interface for the core (`allocator_hold`,
  `release_freed_memory`); the probes `memory_churn` and `allocator_probe` read it.
- Measured, same plan, the build file's change set aside for the run before:

| Core allocator committed | Before | After |
|---|---|---|
| City just opened | 1,462 MiB | 1,484 MiB |
| Centre view drawn | 3,463 MiB | 2,118 MiB |
| 9 km out drawn | 3,239 MiB | 2,051 MiB |

- Draws, primitives, video memory, static memory and the opening time are the same in both runs; the city's digest is the
  base's (`4040792916cf7d78`). `job_pool_test` and `memory_test` pass.
- Left for the next problems: 2.0 GiB still held at the 9 km view after the centre was left (what the workers' heaps keep
  until they work again, and what lives), the video buffers (4.1 GiB at the centre) and Godot's static memory (1.3 GiB).
- Gates on the changed files: the findings left stand at the base commit and are not of this change (`JOB_SEM_CHAVE` on the
  pool probe's four submits, two `.uid` files without their script).

## P5 (2026-11-10): the memory the pool's workers hold follows the work, and what the frame frees goes back

- Problem: a keyed job went to any of the 23 workers (one condition, `notify_one`), so a thin stream of mesh jobs spread
  over every worker's heap; and what a job built is freed later by the frame's thread, after the upload, into the worker's
  heap, where the allocator gives it back only when that worker's own thread looks. A worker asleep never looked.
- Test first: `job_pool_probe` on a pool of its own (`tests/core/job_pool_test`): jobs one after the other, each building
  32 MiB its caller frees once the job ended. Before: 4 workers used, 382 MiB still held. After: 1 worker, 0 MiB, also for
  memory freed long after its job with the worker at rest.
- Fix (`core/job_pool.h`, `.cpp`): the idle workers wait in a stack, each on a condition of its own, and new work calls the
  one that worked last; a worker's thread starts the first time its place is called. A worker at rest whose heap still holds
  pages looks at it (`release_freed_memory`, `heap_held` of `core/memory.h`) a linger after its own work and again when the
  pool is told of a free (`JobPool::freed()`, called by `RenderWorld::sync` in a frame that handed built results to the
  engine), never more than once a linger (2 s); once its heap holds nothing it waits for work alone. No timer runs while
  nothing is freed.
- Two designs were tried on the way and dropped, each by what the test or the measurement showed: workers that end when idle
  (their abandoned segments are freed by nobody: 94 MiB left, with or without the allocator's reclaim-on-free option), and a
  blind timer whose wait grew (the old view's memory was looked at late: 2,735 MiB right after a view change).
- Measured, one plan, each view read 5 s after it was drawn, the pool's change set aside for the run before:

| Core allocator committed | Before | After |
|---|---|---|
| Centre | 2,057 MiB | 1,790 MiB |
| 9 km out | 1,996 MiB | 1,683 MiB |
| Centre again | 2,045 MiB | 1,744 MiB |

- The city opened in 16.4 to 16.6 s in three runs after, 19.7 to 19.9 s in three before. Draws, primitives, video and static
  memory are the same; the digest is the base's. `job_pool_test`, `memory_test`, `traffic_hash_test` and `e2e` pass.
- Gates on the changed files: what is left stands at the base (as above, and `NUMERO_MAGICO` on the indices of
  `render_world.cpp:291`).
