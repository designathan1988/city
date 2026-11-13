# How every ported feature is built here (the backbone), and what never comes from the reference

Specification 003, 2026-11-13. The owner's orders: the port is complete (everything the reference has) and right; the reference's
behaviour is mostly right, its code carries performance, architecture and engineering faults that are identified and never
brought; one must always know where a feature lives, who owns its state, how a change reaches what depends on it, and where an
error happened.

## What stands today (checked in the code, 2026-11-13)

- One source of truth exists: `CityState` / `CityData` hold the whole city (native/src/sim/state/city_state.h:61-136), the render
  reads it through `MapRead` (city_state.h:140-154), the C++ layers are `core -> sim -> render -> api`.
- Change propagation exists but is written by hand: `ChangeLog` is a bag of twenty lists (native/src/sim/state/change_log.h:20-53);
  each producer must know every consumer's list, and nothing checks that a change reached all who depend on it.
- Ownership is declared for 9 concepts (tools/gates/owners.json) in a tree of about 1,190 files; features have no map:
  finding one takes reading and tracing.
- Record ids are reused most recently freed first with no generation (native/src/core/slot_ids.h:1-4): a reference kept past
  its record's removal attaches to whatever takes the id next (lane links and bans after a road removal, flow statistics and
  the control advisor after a junction is rebuilt: agents D and E of sheets/roads.md).
- No invariant is checked while the game runs (no check in native/src outside the api's test probes).
- Failures go unseen: a refusal with no text key shows the raw key (`hint.road.invalid`), Apply past 256 custom roads does
  nothing, a grade-limited end is never told (sheets/roads.md).

## The backbone (each ported feature lands in it)

1. **One error channel.** Every engine and extension error and warning is caught by a `Logger` registered with `OS.add_logger`
   (Godot 4.5+, Context7 /godotengine/godot-docs, tutorials/scripting/logging: `_log_error` gets function, file, line, rationale
   and type, from any thread, so it holds a mutex). The core's own failures (an invariant broken, a reference found stale, a
   command refused for a reason with no text) go to one sink in the core read by the same channel. The count shows in a debug
   overlay; every test scene fails on any entry.
2. **Every command returns its outcome.** Done, refused with a reason key and its arguments, or done in part; the interface
   shows a refusal in one place (the store's dispatch), never per tool; a gate checks every reason key a command can return
   exists in `project/data/i18n`.
3. **Records by handle.** A slot keeps a generation that moves when it is freed; a reference to another record holds the id
   and the generation, and reading it through a stale generation is a reported fault, never a silent hit on the new record
   (the slot map with generations: P0661, https://open-std.org/JTC1/sc22/wg21/docs/papers/2017/p0661r0.pdf; generational
   indices as every ECS handle uses them, https://root-11.codeberg.page/intro-book/trunk/10_stable_ids_and_generations.html).
4. **A record's removal reaches every table that refers to it.** One removal path per kind of record: each table holding
   references to that kind is a declared dependent and drops or remaps them in the same command (the links and bans of a
   road, a junction's flow statistics and control choice, a transit stop on a segment).
5. **Invariants per feature.** Each feature declares the checks that must always hold (no link to a dead road, every
   approach lane has an exit, every junction plate at its node's height, nothing stands in a wall); they run after every
   command and every batch of ticks in the debug DLL and always in the tests, and a failure names the feature, the check and
   the records.
6. **The feature map.** A machine-read registry: each feature with its files, its state (the CityState fields it owns), its
   commands, its simulation steps, what is derived from it and how it is told, its interface entry, its data keys, its tests
   and its invariants. A gate holds it true: every file of `native/src` and `project/{game,ui,render}` belongs to one feature,
   every symbol it names exists, every derived product names the change it rebuilds on. "Where is X" is one lookup.
7. **Declared propagation.** A derived product names its inputs (the records and the ChangeLog list it rebuilds on); a
   producer marks by the kind of record changed, and the dependents of that kind are told (the separation of what is
   rebuilt from how dependencies are tracked: Mokhov, Mitchell, Peyton Jones, Build Systems a la Carte, ICFP 2018,
   https://www.microsoft.com/en-us/research/publication/build-systems-la-carte).
8. **Cost declared and measured.** Each feature states when it works (per edit, per tick, per frame, per view change) and
   what it holds, and its check at its end measures it against `doc/specification.md`'s budgets.

Order: 1, 2 and 5's runner first (they make every later check see what goes wrong), then each piece where the port first needs
it: handles and removal with the roads' links and bans, the map growing feature by feature as each is ported and closing with
the gate once every area is in, propagation declared as each area's products are rebuilt.

## What never comes from the reference (its own reports and the agents' reading)

From the reference's own reports (read whole 2026-11-13: reports/why-10km-cities-fail-2026-10-26.md 1-149, .whole-review/FINDINGS.md
1-147, ARQUITETURA.md 1-97, docs-optimization-brief.md 1-27):

- The whole map kept as loose objects and the whole map's derived data built at once: one elevation solve for the whole network
  (render/renderer.ts:2425-2432), every road tile of the network's extent after the reveal (renderer.ts:2393-2395), string keys per
  block (renderer.ts:2354-2358). Here: records in typed tables, derived products per chunk, bounded by the view and a budget.
- Every road edit's rules solving the whole map's heights again (editor/editRules.ts:66, F-0013), repeated at each pause of the
  pointer by the dry run (editor/roadTool.ts:412, F-0019). Here: a draft is solved on its own reach.
- Generation serial on one thread, its result applied whole on the page (main.ts:2179-2231: 95 s, 422 ms stalls). Here: the job
  pool, deterministic per cell.
- Every product refreshed each frame though nothing moved (core/engine.ts:100, F-0005); queues re-sorted each frame
  (editor/lotTool.ts:807, F-0014); keys rebuilt as strings each frame (lotTool.ts:941, F-0015; roadTool.ts:393).
- Swallowed errors (core/clipper.ts:134, core/derivedCache.ts:41 and nine more, editor/persistence.ts:132, editor/roadParking.ts:12:
  F-0002, F-0004, F-0016, F-0018). Here: the error channel above.
- Diagnostics running in the shipped game: the profiler on from the start capturing stack traces (core/profiler.ts:75, F-0010), a
  Map a frame for the frame timer (core/health.ts:153, F-0008).
- Quadratic work: the offset's self-intersection pruning (core/offset.ts:129, F-0009), the network's change history
  (world/networkDelta.ts:101-110), marks re-merged per shape (render/roadMarks.ts:164), street numbers (world/roads/streetNames.ts:188).
- Copies and validation on opening: the whole document validated on the page (editor/persistence.ts:191, F-0017), every square's
  table copied twice (editor/squaresDecode.ts:193, F-0021).
- A mid-range RTX x060 started on the ultra tier (core/gpuTier.ts:9, F-0007): the median player's card.
- Geometry heavier than it shows: rounded boxes, closed cylinders whose caps never show, non-indexed cards, hidden faces drawn,
  32-bit indices under 65,536 vertices (docs-optimization-brief.md; reports/polygon-geometry-audit.md, its per-file list in the
  agent's extract below).

The agents' area reports add each area's own (sheets/<area>.md, "Reference faults never brought").
