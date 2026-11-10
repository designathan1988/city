# Converter

Converts features of a TypeScript game (the source) into the architecture of the Godot + C++ game (the target),
and audits code for wrong methodologies. It never writes to the target game: it reads it, and everything it makes
goes to an output folder. Paths are always given on the command line or in a configuration file.

## Stages

| Command | What it does |
|---|---|
| `graph` | Traces a feature (or the whole source) through the source's own type checker: every declaration it uses, typed edges, external APIs, patterns, fingerprints. |
| `cppgraph` | The same graph for the target's C++, through clangd. |
| `audit` | Runs the methodology catalog over a graph: each finding says what is wrong, the correct methodology, its form in the target, and the primary sources. |
| `classify` | What a feature becomes in the target: roles from facts, mapped dependencies, what still blocks it. |
| `convert` | A feature carried as far as facts allow: a ledger decides every unit, the clean ones are translated with a spec found in the program, each output is judged, and the rest is listed with its correct method. Exit 1 while anything is open. |
| `translate` | Writes a feature as C++ in its owner folder (types, API, edits, queries, indexes); refuses when anything cannot be translated. |
| `differential` | Runs the translation and its source on the same random scenario and compares them (a double build that must agree exactly, and the target's float build). `--hand <native/src/...>` (repeatable) puts C++ written by hand in place of the translation's file of that path: the scenario still calls the API the spec's translation declares, so the hand-written code declares it in the same headers; only the float build runs. |
| `package` | The translation as a conversion package: its files, its registration where the target's gates look, its state member. |
| `sandbox` | Tests a conversion package in an isolated clone of the target: the target's own setup and verification before and after the package; prints the manual integration. |
| `baseline` | The target's own setup and verification on its base alone, which proves a sandbox configuration. |

Run with Node 23.6 or newer: `node src/cli.ts <command> --flag value`.

## A conversion package

One feature, stored outside the target:

- `manifest.json`: the source commit and the SHA-256 of every source file read, the target's base commit, the
  converter's commit (packaging refuses a converter with uncommitted changes) and the identity of the commits.
- `patches/`: the conversion as a series of commits, one per topic, each compiling alone (`git format-patch`).
- `evidence/`: every log and photo of the sandbox, before and after.

Integration into the target is manual: the sandbox prints the commands (fetch the reviewed branch, a worktree,
the target's usual integration checks). Nothing integrates by itself.

## Rules the conversion follows

The target's `CLAUDE.md` and gate rules are the authority; these are how the converter applies them.

1. **One owner per feature.** A feature is converted into the subsystem that owns it (one folder), entering the
   target only through the points the architecture designates: its state as a member of `CityState`, its time step
   called by `Simulation::tick`, the player's actions as `sim/commands`, its tunables in `data/**` with a schema read
   by `DataTables`, its drawing in `render/` (read only), its changes announced in the `ChangeLog`. No feature code is
   written into another owner's files.
2. **What another owner must provide is that owner's work.** When a feature needs a capability its neighbour does
   not offer yet, the package names it and is not complete; the converter never writes it into the neighbour, and
   never leaves a declaration without an implementation.
3. **Roles come from each unit's facts, not from its folder.** A source unit that mixes responsibilities (input,
   state, simulation, drawing) is split by responsibility; it is never carried over whole.
4. **Every constant needs a source.** A value moved into `data/**` carries where it came from; the package lists each
   one and is complete only when each has a measurement or a primary source.
5. **The correct methodology is written every time.** Each pattern of the audit catalog is converted to its correct
   form (instancing, reused buffers, a spatial index, a ring buffer, state in its owner), whatever its static rating.
6. **The target's gates judge the result** in the sandbox, with its build, tests and photos.

## Convert

`node scripts/port.ts convert editor:poles [--apply]` takes a feature of the inventory, traces it and decides each of
its units from facts (`src/convert/ledger.ts`):

| Decision | When |
|---|---|
| `exists` | a port's `cobertura.md` says the game has it |
| `translate` | no finding the translator cannot rewrite, in the unit or in anything it reaches |
| `method` | a wrong methodology in it or reached from it: the catalog's correct form must be written |
| `write` | drawing, interface, class members, parts of shared code: written in the owner, by the method given |

Only `translate` units reach the machine. Each output then passes the quarantine (`src/convert/quarantine.ts`): nothing
the ledger stopped was emitted, none of the catalog's wrong forms is in the C++, the target's limits hold, and it
compiles against the target's headers. What the game owns is in `configs/conversion-library.json`, written once.

Limit: the machine writes pure simulation code only. Classes, drawing, shaders and interface are not translated: they
stay open in `conversao.md` until a session writes them and the port's `cobertura.md` records it. `--apply` writes only
to the feature's clone. Every constant in the accepted files needs its measurement or primary source in
`<ports>/<feature>/constantes.json` (name → source); the command stays at exit 1 until each has one.

## How a translation decides types

What each value is in C++ has one owner, the flow of values (`src/translate/flow.ts`): every place a value lives is
a slot, every way it moves is a flow, and three facts per slot (integer or real, owned or referenced, which struct
shape) are solved together to a fixpoint. The type mapper and the emitters only read that solution. A spec
(`configs/<feature>.translate.json`) says what the translator may not guess: the owner folder, the state and its
member, which names are integers, the target's types and owned concepts (patterns), the names of literals the
target's gates forbid, and what stays with other owners.

## Feature

`node src/cli.ts feature --source-root <4.0 root> --entry <file>... --out <dir>` cuts a feature from the whole
program: its own declarations (by dominance: what the program reaches only through it), its slices of shared code
(forward slicing in the program's functions that name it or the state that holds its values), the dictionary
entries, style rules and non-code files it uses, the shared code it uses and where the program plugs it in. What a
call runs comes from the call graph of Jelly (`@cs-au-dk/jelly`, a points-to analysis: `--jelly <package dir>` or
`CONVERTER_JELLY`); style sheets are read with the source's PostCSS (`--postcss`).

Where the platform calls the feature (`src/feature/platform.ts`, `platform` in boundary.json): code no declaration of
the program calls, because a package does. An override of a member a package declares in the class's bases
(`ViewedRoot extends LOD` → `three:LOD.update`, called by three's renderer every frame) or a function stored in or
given to a package's declaration (`mesh.onBeforeRender = f`, `onUpload(f)`); the standard library's in-place
iterations (`Array.map`, `Array.from`...) are not hooks.

### Checking a trace against the running 4.0

A trace is judged by what the program did, in a copy of the 4.0 (its own dev server never writes into the source):

1. `node scripts/oracle.mjs <url> <out> <playwright package dir>`: Chrome on the copy's dev server, a small city,
   views near the poles; it writes V8's precise coverage, a CPU profile (sampled stacks), every shader given to
   `gl.shaderSource` and every code made by `Function`/`eval`.
2. `node scripts/check-trace.mjs <graph.json> <boundary.json> <out> <node_modules dir>`: each function V8 ran,
   placed in the graph's innermost unit through the dev server's source maps, against the trace: (A) every unit of
   the entry files that ran is in it; (B) every shared unit the feature's frames called is reached from its own units,
   and every way the program entered the feature is known: a hook, or `platform` when three.js called it; (C) every
   three.js function the feature called is among its external APIs; (D) where code from strings came from.

### The definitive test: complete call graph, verdict, controls

Ground truth by instrumentation, never by sampling, on a copy of the 4.0 (its source never written):

1. `scripts/oracle-instrument.mjs`, a Vite plugin in the copy's config: every function of the game and of three.js
   records entry and exit on a shadow stack, in the page and in every worker (async functions and generators leave it
   at each `await`/`yield`; field and parameter initializers, static blocks, derived constructors and modules awaiting
   at the top are frames too). A frame of a feature's **seed files** (the inventory's `sementes`) raises the feature's
   bit, so the truth never depends on the trace under test. New edges are streamed to the server as they happen (a
   worker the game ends takes nothing with it).
2. `node scripts/oracle-calls.mjs <url> <out> <playwright dir> [seconds]`: a city, views, simulated time; it writes
   every edge caller → callee with the features on the stack, and checks each realm's stack is back at its root.
3. `node scripts/verdict.mjs <graph> <out> <features.json> <features dir>`: PASS or FAIL per feature by four rules —
   R1 every unit of a seed file that ran is in the trace; R2 everything run while the feature was on the stack is in
   it; R3 every entry into the feature from outside is known (a graph edge, a hook, or `platform`); R4 every three.js
   function the feature called is among its external APIs.
4. Controls, which must FAIL: `--strip-platform`, `--drop-own <fraction> --seed <n>` (reports how many removed units
   that ran were caught), and the traces of an older converter.

## Render: three.js → Godot (shaders) and → Godot C++ (the RenderingServer API)

**Shaders** (`src/render/godot-shader.ts`): a program three linked (vertex and fragment GLSL, chunks and
`onBeforeCompile` edits in) parsed with the ShaderFrog GLSL parser (`@shaderfrog/glsl-parser`, given by `--glsl` or
`CONVERTER_GLSL`) and written as one Godot spatial shader, `unshaded` (three's own lighting, exact). The rules come
from Godot's documentation and engine source and each was measured by the parity test: three's matrices and
attributes are Godot built-ins; `gl_Position` → `POSITION`; `gl_FragCoord` in three's convention (Godot's origin is
top-left and its depth reversed); `gl_FragDepth` → `DEPTH = 1 - d`; the output decoded when three writes display
values; struct uniform arrays one array per field; `sampler2DShadow` → `step(ref, depth)`; `matN` of scalars → of
columns; integer operators and constructors given explicit types (`glsl-types.ts`); helpers receive the built-ins
they read (Godot exposes only TIME, EXPOSURE, IN_SHADOW_PASS to them); the fragment main a helper (no `return` in
`fragment()`); vertex helpers that assign varyings written inline; varyings packed into vec4s when many (Godot's 32
locations include its own); attributes in CUSTOM0–3, INSTANCE_CUSTOM, bones, or a data texture by VERTEX_ID /
INSTANCE_ID; `instanceMatrix` the identity (Godot puts a MultiMesh transform in MODEL_MATRIX).

- `node scripts/oracle.mjs <url> <out> <playwright dir>` also writes `programs.json`: every program the 4.0 linked,
  with the attributes it reads per instance (the GL divisor).
- `node scripts/shaders.ts <programs.json> <out> --glsl <dir>`: each program translated, and a Godot project that
  compiles and draws each one; `node scripts/shaders-verdict.mjs <out>`: Godot's judgment, per program.
- `node scripts/parity.mjs <out> <three build dir> <playwright dir> <glsl dir>`: the same scenes drawn by three.js
  (Chrome) and by Godot from the translated program, with the uniforms and textures three uploaded (every `gl.uniform*`
  and texture upload captured) and the mesh as the translation places its attributes; compared pixel by pixel.

Translator options beyond the defaults: `matrices: 'three'` (three's matrices as uniforms, its clip position
converted at the end of vertex(): exact for any camera), `depth: 'reversed'` (reversedDepthBuffer), `output: 'raw'`
(the values the program wrote, for a render target), `shadowCompare`/`shadowFilter` (the hardware's compare and 2×2
filter, by hand), `fragmentOutput` (one pass per MRT location), `floatBitsSamplers` (32-bit integer textures as float
words, read back with `floatBitsToUint`: Godot 4.7 maps no 32-bit integer texture). Always: three's tone mapping is
its own code (Godot's ACES/AgX are other curves), `dFdy` changes sign (Godot's window y points down), a vector
indexed at run time is read through a comparison helper (Godot's SPIR-V pass refuses OpVectorExtractDynamic).

**A real frame, replayed** — the test that proves translations on the game's own data:

- `node scripts/frame-capture.mjs <url> <out> <playwright dir>`: one frame of the running 4.0, every draw with its GL
  state (programs, every active uniform, samplers with their full state, attributes, elements, target, viewport,
  blend/depth/cull), the buffers and every texture level and layer read back from the GPU (integer textures read two
  independent ways and compared; a refused read is an error, never zeros).
- `node scripts/frame-replay.mjs <capture> <out> <playwright dir> <glsl dir> [--only <draws>] [--probe <expr>]`: each
  draw alone at its own resolution, by WebGL (three's GL commands again: the reference; a GL error voids it) and by
  Godot (`scripts/replay.gd`: the translated program, the mesh as the translation placed its attributes, textures
  through the RenderingDevice in the GPU's formats), compared strictly: *equal* (every pixel within 2/255), *near*
  (≤1 % of pixels differ), *different*, *empty* (both blank: proves nothing, never counted as equal). Depth-only draws
  are judged by their depth (a 24-bit code). `--probe <expr>` makes both engines output an intermediate value of the
  same program (bisects a difference in seconds). The WebGL reference and the translations are cached by content.
- `node scripts/draw-coverage.mjs <capture> <draws...>`: an independent check of a draw's screen coverage (its
  vertices projected in JS), so an *empty* verdict is known to be an off-screen draw.

**C++ port** (`cpp/runtime`, `scripts/cpp-scene.mjs <replay dir>`): every captured draw written as C++ for godot-cpp
(uniforms as typed Godot values, textures and mesh arrays and instances as their data files), compiled with
llvm-mingw against the prebuilt godot-cpp (files in parallel, objects cached by content; generated files at -O0)
into a GDExtension (`ThreeReplay`), and run in Godot; `node scripts/replay-compare.mjs <replay dir> <images>
--same-as <other images>` compares it with WebGL and, byte for byte, with the GDScript replay.

**C++ catalog** (`configs/three-godot-catalog.json`): each three.js class the 4.0's features use → its Godot C++ form (the
RenderingServer calls of godot-cpp) and the rule that carries values (column-major matrices, reversed winding,
MultiMesh rows, linear colors, sampler hints...). `node scripts/catalog-check.mjs <catalog> <features dir> <godot>`
checks it is complete for the features' traces and that every Godot class and method it names exists.

## Child processes

Every process the converter starts (Jelly, the graph builds, each compile, the differential's builds and runs, the
sandbox steps, Godot in the replay, parity and catalog checks) is a task of the repository's orchestration owner
(`python tools/tasks/cli.py serve`, through `src/common/tasks.ts`): its own deadline (the whole process tree is stopped
past it), no window on the user's desktop when it draws, and a result keyed by the content of what it reads, so it is not
made again while its inputs stay the same.

The GLSL parser runs from a copy whose peggy memo table is a `Map` (`scripts/glsl-parser-fast.cjs <parser.js>`):
the generated object, keyed position × rules, falls into dictionary mode and took 96 % of the time; the 277 programs
translate byte-identically, 4× faster (14.5 s cold on 22 workers).

## Tests

`node --test "test/**/*.test.ts"`: every tool (TypeScript, Jelly, PostCSS, the GLSL parser, clangd, the C++ compiler) comes
from `configs/roadcraft-machine.json` through `src/common/machine.ts`; an environment variable (`CONVERTER_TYPESCRIPT`,
`CONVERTER_JELLY`, `CONVERTER_POSTCSS`, `CONVERTER_GLSL`, `CONVERTER_CLANGD`, `CONVERTER_CXX`) only overrides one. Type check with
`node scripts/typecheck.ts --typescript <dir> --types <node_modules/@types dir>`.
