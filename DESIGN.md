# Converter design: any feature of the source, converted whole, with no decision by hand

The input is a feature: one or more entry points of the source (a file, a symbol). Everything else is decided by
rules from facts of the two programs. A decision the converter cannot make from facts is a defect of the converter,
never a field for a person to fill.

## 1. One model of each program

- **Source:** the whole TypeScript program, opened once with its own compiler. The graph (units, typed edges,
  external APIs) and the flow of values (every slot, every flow) are built over **all** its files, so every call site,
  every caller and every shared helper is known. Equality comparisons are flows both ways (`s.id !== id` makes `id`
  the kind of `s.id`): integer kinds follow from the whole program, never from names.
- **Target:** the game's C++ graph (clangd), its GDScript, its gate rules (layers, limits, allowed numbers, owners,
  map), its data tables and their schemas.
- The flow is solved with a real worklist (each flow re-runs only when a slot it read changed), so the whole program
  solves in time proportional to its size.

## 2. The feature, traced whole

From the entry points, the closure over the graph: every unit reached, in every file. Shared code is part of the
closure; it is translated once, into its own owner, and reused by every feature that reaches it (a second feature
finds it already in the target and links to it).

## 3. Each unit's place in the target, decided by rules

| Decision | Rule |
|---|---|
| Layer | the unit's facts: state, time step, computation → `native/src/sim`; draws (three.js) → `native/src/render` (reads only); interface (DOM) → GDScript in `project/ui` through `CityStore`; tunable values → `project/data` with a schema; GLSL → `.gdshader`; workers → `JobPool` |
| Owner folder | units of one layer grouped by the graph's cohesion (modularization: maximize intra-cluster edges, minimize inter-cluster, TurboMQ), each group a subsystem named after its dominant source module |
| Target type | a source type whose shape (fields and operations) is a target type's (`Vec2 {x, y}` ↔ `V2`) maps to it; otherwise it is translated |
| Target concept | a source function with a candidate in the target of the same signature shape is run against it on generated inputs (differential); equal results → the target's owner is called; otherwise the source's is translated |
| Integer kinds | the solved flow over the whole program |
| Literal names | from the slot or call that receives the literal (`Math.min(20, v)` assigned to `vehicles` → `MAX_VEHICLES`), unique per owner |
| State | the types the feature's edits take and return become a struct held by `CityState`; edits become commands under the exclusive lock; indexes are rebuilt by each edit |
| Registration | layer rule, map line (from the unit's doc comment), state member: written where the gates read them |

## 4. Translation per layer

- **Simulation C++:** the current translator, extended to every construct of the source: classes (fields, methods,
  constructors, inheritance → structs with methods; dynamic dispatch only where the source overrides), generics
  (templates), enums, closures, Map/Set/typed arrays, async and workers (jobs on `JobPool`), exceptions (error results).
- **Render C++:** a fixed catalog from the three.js API to the game's render owners (BufferGeometry → `MeshBuilder`,
  InstancedMesh → MultiMesh through the RenderingServer, materials → the game's shaders and material tables,
  frustum/LOD → the game's streaming); written once, as the translator's library knowledge.
- **Shaders:** parsed with an existing GLSL parser (not a new one), rewritten to Godot's shading language by the
  rules of Godot's official conversion guide.
- **Interface:** DOM and CSS → Godot Controls built through `theme_builder.gd`, texts through `tr()` with the CSV
  catalog, numbers only from `CityStore.snapshot`, actions through `CityStore.dispatch`.

## 5. Proof, for every unit

- Unit tests of each rule, each with a mutation shown to be caught.
- Simulation: differential runs against the source (double build exact, float build within precision).
- Render: the same scene captured in the source and in the target, compared with a perceptual metric.
- Target: the package in an isolated clone with the game's gates, builds, tests, capture and soak.

## 5a. Nothing wrong reaches the machine

Translating syntax carries the source's methods with it, so the decision comes first: the ledger (`src/convert`) reads
the audit and the whole graph, and a unit is translated only if nothing wrong is in it or reached from it. Each run's
output is then judged on its own (translation validation), not trusted. A unit the machine cannot write stays open
with its correct method; it is never reported as converted. Items 4 and 5 of the order of work below are still to do:
until then those units are written by a session. `test/convert.test.ts` holds each decision and each refused form.

## 6. Order of work

1. Whole-program model: flow with a worklist over all files; equality flows; no name rules.
2. Whole-feature tracing across files; shared code into its own owner once; emission per owner.
3. Automatic decisions: target types by shape, target concepts by differential equivalence, literal names, owner
   folders by cohesion, map lines from doc comments. The spec shrinks to the entry points.
4. Classes, generics, enums, exceptions; async and workers.
5. Render catalog; shaders; interface.
6. The transit feature whole (network, vehicles, tools, drawing, panel) converted with none of the spec's manual
   fields, proved by every check above.
