# src:main

Grupo: raiz. Entrada: ligação. Gerado de `records/map40/graph.json` (commit do 4.0 22357c928fb9) por `tools/map40/features.cjs`; a lista de código abaixo é rastreável: arquivo e linhas de cada declaração própria.

- Sementes: `src/main.ts`
- Unidades próprias (só esta funcionalidade alcança): 1021 em 68 arquivo(s), 14310 linhas
- Alcance total: 12193 unidades; usa 11172 compartilhadas de outras; é dona principal de 0 compartilhadas

## O que o jogador vê e faz
- Textos da interface (`t()`): `confirm.newLargeWorld`, `confirm.newMap`, `confirm.newWorld`, `health.context.lastChange`, `health.context.opening`, `health.context.tool`, `health.msg.invariant`, `health.msg.invariantDetail`, `health.state.simulation`, `health.world.rects`, `health.world.whole`, `map.loading`, `palette.lanes.median`, `rule.short.…`, `terrain.level`, `zone.lot.size`
- Eventos de entrada: `emite:new CustomEvent<LabSubject>(INVESTIGATE_EVENT, { bubbles: tr`, `emite:new Event('change')`, `ouve:beforeunload`, `ouve:blur`, `ouve:change`, `ouve:click`, `ouve:contextmenu`, `ouve:dblclick`, `ouve:keydown`, `ouve:keyup`, `ouve:lostpointercapture`, `ouve:pagehide`, `ouve:pointercancel`, `ouve:pointerdown`, `ouve:pointerleave`, `ouve:pointermove`, `ouve:pointerup`, `ouve:resize`, `ouve:samplebufferfull`, `ouve:toggle`, `ouve:visibilitychange`, `ouve:webglcontextlost`, `ouve:webglcontextrestored`, `ouve:wheel`
- Chaves de localStorage: `escreve:roadcraft.lab.tab`, `le:roadcraft.lab.tab`

## Entradas, saídas e interações
- Mensagens de worker (envia/recebe): `envia:?`, `recebe:building`, `recebe:cancel`, `recebe:click`, `recebe:dblclick`, `recebe:down`, `recebe:leave`, `recebe:lost`, `recebe:move`, `recebe:node`, `recebe:person`, `recebe:segment`, `recebe:terrain`, `recebe:up`, `recebe:vehicle`, `recebe:wheel`
- Workers: -
- Ganchos do three.js: `onBeforeCompile`
- Timers: 14
- Interage com: `core:vec` (302), `world:doc` (209), `render:renderer` (202), `sim:host` (177), `view:viewport` (106), `editor:persistence` (95), `ui:toolChoices` (91), `src:effects` (90), `render:gpu` (81), `ui:overlay` (80), `world:changes` (72), `view:camera` (57)

## Estado (campos do documento/estado da simulação)
- Lê: `?.code`, `?.detail`, `?.subject`, `?.tick`, `?.x`, `?.y`, `RoadDoc.buildings`, `RoadDoc.changes`, `RoadDoc.clouds`, `RoadDoc.economy`, `RoadDoc.elements`, `RoadDoc.fogDabs`, `RoadDoc.fogSettings`, `RoadDoc.gullyAuto`, `RoadDoc.gullyDabs`, `RoadDoc.markingStyle`, `RoadDoc.nature`, `RoadDoc.nodes`, `RoadDoc.people`, `RoadDoc.segments`, `RoadDoc.terrainStamps`, `RoadDoc.transit`, `RoadDoc.treeClearings`, `RoadDoc.trees`, `RoadDoc.weather`, `RoadDoc.zones`, `RoadNode.control`, `RoadNode.crossing`, `RoadNode.heightOffset`, `RoadNode.id`, `RoadNode.incident`, `RoadNode.smooth`, `RoadNode.x`, `RoadNode.y`, `RoadSegment.a`, `RoadSegment.b`, `RoadSegment.curve`, `RoadSegment.direction`, `RoadSegment.type`, `SerializedDoc.buildings`, `SerializedDoc.nature`, `SerializedDoc.nodes`, `SerializedDoc.poles`, `SerializedDoc.segments`, `SerializedDoc.terrain`, `SimStats.completedTrips`, `SimStats.controllers`, `SimStats.entryDemandLost`, `SimStats.lotsMoving`, `SimStats.measured`, `SimStats.people`, `SimStats.populationShare`, `SimStats.stepMs`, `SimStats.vehicles`
- Escreve: `RoadDoc.nature`, `RoadDoc.terrainRelief`, `RoadNode.smooth`

## Dados e recursos
- Recursos (JSON, modelos, texturas, fetch): `/__lab/report`, `/__lab/reports`, `/__lab/source-find?q=…`, `/__open-in-editor?file=…`, `file.startsWith('/src/') ? `${file}?raw` : file`, `new URL(ref, url)`, `url`
- Bibliotecas externas: -

## Testes do 4.0 que a exercitam
- nenhum spec importa os arquivos desta funcionalidade

## Estado no jogo (lido de ports/*/coverage.md)
- porte `demolition`: portado 2
- porte `economy`: portado 2
- porte `interface`: outra sessão 4
- porte `interior`: outro porte 4
- porte `landscape`: portado 70
- porte `street-furniture`: portado 2
- porte `traffic`: não se aplica 3
- porte `transit`: portado 2
- porte `people`: não se aplica 1

## Código próprio (arquivo:linhas — declaração — comentário)

### src/lab/code/aggregate.ts
- 4-10 `ProfilerTrace` (interface): The trace `Profiler.stop()` gives (https://wicg.github.io/js-self-profiling/#the-profilertrace-dictionary).
- 13-19 `FunctionId` (interface): A function as the profiler names it: its script, the 1-based line and column it is defined at, its name.
- 21-35 `FunctionStat` (interface)
- 37-42 `EdgeStat` (interface)
- 44-50 `StackStat` (interface)
- 52-72 `Aggregate` (interface)
- 75-75 `STACKS_KEPT` (variable): Stacks kept at most (the rest summed as dropped).
- 77-77 `STACK_KEY_DEPTH` (variable): Frames of a stack that make its key (the leaf's nearest).
- 78-78 `GC_NAME` (variable)
- 80-86 `createAggregate` (function)
- 89-177 `addTrace` (function): `trace` folded into `agg`: each sample weighs the time to the next (at most twice the interval), the last the interval. `frames`: the game's frames drawn in the window.

### src/lab/code/analysis.ts
- 8-8 `Category` (type): `game`: the game; `browser`: a native browser frame; `engine`: an engine-internal frame. `lab`: the debugging lab's own code (this sampler among it), apart so it can be filtered out.
- 10-22 `FunctionRow` (interface)
- 24-31 `Suspect` (interface)
- 34-34 `num` (function): A number as the lab shows them (en-US).
- 37-49 `categoryOf` (function): The category of a script by its URL (and a native frame by its name).
- 52-52 `shortPath` (function): A server path short: no origin, no leading slash, no Vite query.
- 54-58 `whereOf` (function)
- 61-74 `functionRows` (function): Every function of `agg` as a row, placed through `store` (null places until its map is in).
- 77-79 `recursionRows` (function): The functions found more than once in a stack.
- 82-124 `suspects` (function): What deserves a look: hot leaves, recursion, deep stacks, the garbage collector, code always present for nothing, the engine's idle products, costly library calls by their game caller.
- 127-137 `reportOf` (function): The report's section: the top functions, the recursion, the suspects.

### src/lab/code/codeTab.ts
- 11-11 `Section` (type): THE "CODE" TAB: the page's JavaScript sampled live, by function (self and total time, its TypeScript file and line), the selected function's callers and callees, the hot stacks, recursion and the suspects; the code of whatever is clicked.
- 13-13 `EdgeRow` (interface)
- 14-14 `StackRow` (interface)
- 16-16 `ms2` (function)
- 17-17 `pct` (function)
- 19-30 `fnColumns` (function)
- 32-263 `createCodeTab` (function)

### src/lab/code/coverageTab.ts
- 9-14 `DeclaredRow` (interface): THE "COVERAGE" TAB: the functions the game's modules declare against the ones the sampling profiler saw this session - an approximate runtime coverage (a sampler misses what runs briefly: "not seen" is a lead, not a proof). The exact one co
- 15-21 `FileRow` (interface)
- 23-23 `ENTRY` (variable)
- 24-24 `COMMAND` (variable)
- 26-32 `CoverageResult` (interface)
- 34-39 `CoverageTab` (interface)
- 41-172 `createCoverageTab` (function)

### src/lab/code/declared.ts
- 3-8 `DeclaredFunction` (interface): THE FUNCTIONS A SOURCE DECLARES, found by their shape line by line (no parser in the page): declarations, named function expressions and arrows, class and object methods. Approximate: a signature spread over lines, a callback passed inline,
- 10-10 `IDENT` (variable)
- 11-11 `DECL` (variable)
- 12-12 `ARROW` (variable)
- 13-13 `PROP_ARROW` (variable)
- 14-14 `CLASS_FIELD` (variable)
- 15-15 `METHOD` (variable)
- 16-16 `KEYWORD` (variable)
- 17-17 `TYPE_BLOCK` (variable)
- 20-61 `declaredFunctions` (function): The functions `text` declares, by line.

### src/lab/code/index.ts
- 10-42 `createCodeTabs` (function): THE LAB'S CODE TABS: "Code" (the page's JavaScript sampled live, mapped to its TypeScript) and "Coverage" (the functions declared against the ones seen). Their numbers go into every report (`code`) and to the probes (`window.__labCode`).

### src/lab/code/sampler.ts
- 6-10 `JsProfiler` (interface): THE PAGE SAMPLING ITS OWN JAVASCRIPT (JS Self-Profiling API, `new Profiler`; the dev server sends `Document-Policy: js-profiling`): back-to-back windows of a few seconds, each folded into the last window's, the session's and, while recordin
- 11-11 `ProfilerCtor` (type)
- 13-13 `SampleView` (type)
- 15-34 `CodeSampler` (interface)
- 36-129 `createCodeSampler` (function)

### src/lab/code/sourceMap.ts
- 4-10 `SourceLocation` (interface): A place in an original source: the file's path on the server (`/src/...ts`), 1-based line and column.
- 13-19 `DecodedMap` (interface): A module's map decoded: per generated line, its segments as [column, source, line, column, name] (-1: none).
- 21-21 `BASE64` (variable)
- 22-26 `DIGIT` (variable)
- 29-75 `decodeMappings` (function): The `mappings` string decoded (Base64 VLQ: 5 bits a digit, 32 the continuation, the first digit's lowest bit the sign; the generated column relative within a line, the rest across the whole string).
- 77-83 `RawMap` (interface)
- 86-98 `decodeMap` (function): A map's JSON decoded; its sources resolved against `baseUrl` (the module's URL) as server paths.
- 101-117 `lookup` (function): The original place of a generated position (0-based line and column): the segment at or before it on its line, else the line's first.
- 120-140 `mapOfModule` (function): The map a module's text points to (inline `data:` or a file next to it), or null.

### src/lab/code/sourceStore.ts
- 5-9 `ModuleEntry` (interface): THE CODE'S SOURCES, as the dev server serves them: each module fetched once, its map decoded, its original text kept (the map's `sourcesContent`). Owned by the code tabs.
- 11-26 `SourceStore` (interface)
- 28-28 `CRAWL_PARALLEL` (variable)
- 30-30 `IMPORT` (variable): Vite's rewritten imports: `from "/src/x.ts"`, `import "/src/x.ts"`, `import("/src/x.ts")`.
- 32-138 `createSourceStore` (function)

### src/lab/code/state.ts
- 9-9 `ProductCount` (interface): THE CODE TABS' SHARED STATE: the sampler, the sources, what is shown, the engine's products counted (idle refreshes).
- 11-23 `CodeState` (interface)
- 25-61 `createCodeState` (function)

### src/lab/code/viewer.ts
- 5-10 `CodeViewer` (interface): THE CODE SHOWN: the original source around a line, the line marked, and a link that opens it in the editor (Vite's dev server `/__open-in-editor?file=path:line:column`, launch-editor resolving the path from the project's folder).
- 13-13 `AROUND` (variable): Lines shown each side of the one marked.
- 16-19 `openInEditor` (function): Asks the dev server to open `file` at `line` in the editor it finds running (or $LAUNCH_EDITOR).
- 21-97 `createCodeViewer` (function)

### src/lab/doctor/findings.ts
- 6-6 `Severity` (type): THE DIAGNOSIS'S FINDINGS: the snapshot read as causes - the bottleneck, the passes and layers the GPU spends on, the objects that weigh most, the shadows, the draw calls, the CPU's systems and functions, the hitches, the memory - each with 
- 8-25 `Finding` (interface)
- 28-47 `LAYER_SWITCH` (variable): A layer of the scene (the capture's `layer`) and the switch that hides it (`effectSwitches.ts`).
- 48-50 `PASS_SWITCH` (variable)
- 52-55 `layerSwitch` (function)
- 57-57 `BUDGET_MS` (variable)
- 58-58 `fmtN` (function)
- 59-59 `big` (function)
- 60-60 `pct` (function)
- 61-61 `severityOf` (function)
- 62-62 `isShadow` (function)
- 63-63 `isQuad` (function)
- 65-65 `Verdict` (interface)
- 67-74 `verdictOf` (function)
- 77-77 `LayerShare` (interface): Each layer's share of the frame's draws (the composer's own quads left out).
- 79-94 `layersOf` (function)
- 96-210 `findingsOf` (function)

### src/lab/doctor/index.ts
- 9-11 `PASS_COLOUR` (variable): THE DIAGNOSIS (the lab's first tab): it measures by itself - the frame, every GPU pass, one frame's every draw, the CPU's systems and functions - and lists the CAUSES by weight, each with its figures, why it matters, a one-click A/B test of
- 12-12 `fmt` (function)
- 14-235 `createDoctorTabs` (function)

### src/lab/doctor/measure.ts
- 5-5 `PassTime` (interface): THE DIAGNOSIS'S MEASURES: one snapshot of where the frame goes (the GPU's passes, one frame's every draw, the CPU's systems and functions), and the quick A/B of one change (off, measured, back on, measured), from the other areas' probes.
- 6-9 `Draw` (interface)
- 10-10 `ProfileFn` (interface)
- 12-27 `Snapshot` (interface)
- 29-34 `GpuProbe` (interface)
- 35-35 `CodeProbe` (interface)
- 37-37 `gpuProbe` (function)
- 38-38 `codeProbe` (function)
- 40-45 `median` (function)
- 46-46 `wait` (function)
- 49-61 `framesFor` (function): The frames drawn for `ms`, every frame asked for (the game draws on demand: a still view would give none).
- 64-112 `snapshot` (function): Where the frame goes now: the passes timed for a while, one frame captured, the CPU sampled meanwhile.
- 114-122 `AbResult` (interface)
- 125-135 `settled` (function): The frames drawn until no shader program has been made for 600 ms (a change compiles them), at most 6 s.
- 138-167 `quickAb` (function): A, change, B, back, A again: B against the mean of the two A (Δ under their gap is noise).

### src/lab/gpu/caps.ts
- 5-9 `CapRow` (interface): THE CONTEXT'S CAPABILITIES: its limits (`getParameter`, the WebGL 2 names), its extensions (`getSupportedExtensions`), its attributes (`getContextAttributes`) and what three r186 made of them (`renderer.capabilities`).
- 11-18 `LIMITS` (variable)
- 20-20 `show` (function)
- 22-50 `readCapabilities` (function)

### src/lab/gpu/controller.ts
- 11-20 `PASS_LABELS` (variable): THE LAB'S GPU AREA, ITS STATE: the per-pass timing mode, the frame capture, the live counters by layer, the debug views, the programs and the GPU's objects. The tabs (`passesTab.ts`, `frameTab.ts`, `objectsTab.ts`) draw it; the report and `
- 21-21 `PASS_ORDER` (variable)
- 23-32 `PassRow` (interface)
- 34-38 `Capture` (interface)
- 40-49 `AggregateRow` (interface)
- 51-76 `GpuLab` (interface)
- 79-94 `aggregate` (function): Records grouped by `key`.
- 96-96 `CAPTURE_GIVE_UP_FRAMES` (variable)
- 98-98 `DEPTH_PASSES` (variable): The chain's passes that read the scene's depth (`effectSwitches.ts` POST_SWITCHES).
- 99-99 `TOP` (variable)
- 101-294 `createGpuLab` (function)

### src/lab/gpu/dom.ts
- 3-8 `el` (function): Small DOM helpers of the GPU area's tabs.
- 10-16 `button` (function)
- 18-22 `bytes` (function)
- 24-24 `ms` (function)
- 27-29 `PASS_COLOURS` (variable): One colour per pass, steady across frames.
- 32-37 `filterOf` (function): A table's filter from a tab's `focus` params: only what was given.

### src/lab/gpu/frameTab.ts
- 9-16 `GROUPS` (variable): THE "FRAME" TAB: one frame's every draw call (`renderer.renderBufferDirect`, the shadow maps' and the passes' quads too), and the same draws grouped by layer, material, program, pass or target.
- 18-120 `createFrameTab` (function)

### src/lab/gpu/index.ts
- 10-35 `createGpuTabs` (function): THE LAB'S GPU AREA: the GPU time of each pass, one frame's every draw, the programs, the GPU's objects, the debug views and the context's capabilities. `window.__labGpu` for the browser checks.

### src/lab/gpu/memory.ts
- 7-18 `TextureRow` (interface): THE GPU'S OBJECTS AS THE SCENE HOLDS THEM: textures (the materials' fields, their uniforms and the uniforms three compiled them with), render targets (the shadow maps and every target the last captured frame drew into), geometries (their bu
- 20-28 `TargetRow` (interface)
- 30-40 `GeometryRow` (interface)
- 42-46 `GpuObjects` (interface)
- 48-48 `Attribute` (type)
- 50-50 `FORMATS` (variable)
- 51-51 `TYPES` (variable)
- 53-53 `formatOf` (function)
- 54-58 `sizeOf` (function)
- 60-149 `scanGpuObjects` (function)

### src/lab/gpu/objectsTab.ts
- 11-11 `age` (function): THE "GPU OBJECTS" TAB (the programs and the textures, targets and geometries alive) and THE "WEBGL" TAB (the context's limits, extensions and attributes, its losses).
- 13-118 `createObjectsTab` (function)
- 120-147 `createWebglTab` (function)

### src/lab/gpu/passesTab.ts
- 9-9 `BAR_FRAMES` (variable): THE "GPU" TAB: the GPU time of each pass (avg/p95/max and the frames as stacked bars), the debug views, and the draws by layer live.
- 11-149 `createPassesTab` (function)

### src/lab/gpu/passTimer.ts
- 6-9 `TimerExtension` (interface): THE GPU TIME OF EACH PASS (the lab's per-pass mode): the frame cut into segments one after another, each a TIME_ELAPSED query of its own. Khronos EXT_disjoint_timer_query(_webgl2): only ONE query of a target is active at a time (beginQuery 
- 12-12 `PassFrame` (type): One frame's GPU ms by pass.
- 14-32 `PassTimer` (interface)
- 34-34 `Issued` (interface)
- 36-150 `createPassTimer` (function)
- 153-158 `spread` (function): avg, p95 and max of a list of ms.

### src/lab/gpu/probe.ts
- 8-34 `DrawRecord` (interface): One draw call as the frame capture saw it.
- 37-37 `LayerCount` (interface): Draws and triangles by layer, one frame.
- 39-55 `GpuProbe` (interface)
- 57-57 `SIDES` (variable)
- 58-58 `PASS_NAMES` (variable)
- 61-65 `shortHash` (function): FNV-1a of a string, 8 hex digits: a program's cache key shown short.
- 67-224 `createGpuProbe` (function)

### src/lab/gpu/programs.ts
- 7-16 `ProgramRow` (interface): THE PROGRAMS THE RENDERER HOLDS (`renderer.info.programs`, three r186 `WebGLPrograms.programs`: one per cache key, `usedTimes` the materials using it, let go at 0): every new one told with when and what came just before it - a count that ke
- 18-26 `ProgramTracker` (interface)
- 28-28 `Program` (interface)
- 30-30 `POLL_MS` (variable)
- 31-31 `COUNTS_KEPT` (variable)
- 33-33 `TRIGGER_MS` (variable): An event counts as the trigger when this recent.
- 35-79 `createProgramTracker` (function)

### src/lab/gpu/views.ts
- 10-10 `DebugView` (type): THE LAB'S DEBUG VIEWS. Overdraw, wireframe and flat normals are drawn with each object's OWN material, changed (`substitute`, called by the probe for every draw of the scene pass): most of the game's materials place their vertices in their 
- 12-19 `DEBUG_VIEWS` (variable)
- 21-29 `DebugViews` (interface)
- 32-62 `checkerTexture` (function): A checker of `size` (a power of two), every mip level given, coloured by its level when `colourLevels`.
- 65-68 `atEnd` (function): `code` put at the very end of the shader's `main` (its last brace).
- 69-69 `OVERDRAW` (variable)
- 70-70 `NORMAL` (variable)
- 71-71 `VIEW_POSITION` (variable)
- 74-78 `recolourable` (function): Whether a material's fragment can be recoloured: three's own GLSL (where gl_FragColor is defined), one output.
- 80-173 `createDebugViews` (function)

### src/lab/icons.ts
- 3-39 `PATHS` (variable): THE LAB'S ICONS: line drawings on a 24-unit grid, stroked in the text's colour (no font, no package).
- 42-59 `icon` (function): The icon `name` (a blank square if unknown), `size` px.
- 62-74 `tabIcon` (function): The icon a tab is shown with, by its id (or a word in it).

### src/lab/inspector/consoleTab.ts
- 12-12 `AsyncFunction` (variable): The constructor of async functions: the code given awaits freely.
- 14-14 `HISTORY_KEY` (variable)
- 15-15 `HISTORY_KEPT` (variable)
- 18-33 `SNIPPETS` (variable): Ready-made snippets: the questions asked most of a scene.
- 35-35 `Entry` (interface)
- 37-188 `createConsoleTab` (function)

### src/lab/inspector/describe.ts
- 13-16 `FILTERS` (variable): Names of three's numeric constants, as the inspector shows them.
- 17-17 `SIDES` (variable)
- 18-18 `WRAPS` (variable)
- 19-19 `BLENDINGS` (variable)
- 20-20 `FORMATS` (variable)
- 21-21 `TYPES` (variable)
- 23-23 `named` (function): A constant's name by its table, else its number.
- 26-30 `pathOf` (function): The scene's path down to an object: each ancestor's name (its type where unnamed).
- 33-37 `layerOf` (function): The top-level group under the scene it hangs from.
- 39-39 `v3` (function)
- 40-40 `deg` (function)
- 43-47 `decomposed` (function): A matrix decomposed: position, rotation (degrees) and scale.
- 50-61 `instanceMatrix` (function): The matrix of an instance (or a batch's item) in the world.
- 63-74 `AttributeRow` (interface)
- 76-92 `attributeRows` (function)
- 94-103 `geometrySummary` (function)
- 106-106 `ownCompile` (function): Whether a material's `onBeforeCompile` is its own (the shader edited when compiled).
- 108-115 `TextureInfo` (interface)
- 117-134 `textureInfo` (function)
- 137-151 `texturesOf` (function): Every texture a material holds: its fields, and the uniforms it was compiled with (a texture an `onBeforeCompile` put there).
- 154-162 `ProgramInfo` (interface): What three keeps per material (`WebGLProperties`): the program it is drawn with and the uniforms it was compiled with.
- 164-169 `programOf` (function)
- 172-176 `SHADER_OF` (variable): The shader of three's own a built-in material compiles from (`ShaderLib`): its uniforms are set from the material's fields every frame.
- 179-186 `editableUniforms` (function): The uniforms an edit can change: a ShaderMaterial's own; for a built-in material, the ones its `onBeforeCompile` added (three refreshes the rest from the fields).
- 189-192 `materialProps` (function): A material's every own property (not the private ones) for the full listing.
- 195-198 `materialsOf` (function): The materials of an object, an array's each.
- 203-221 `trianglesUnder` (function): Triangles under an object (its subtree) as three submits them in one pass (r186 `WebGLRenderer.renderBufferDirect`): visible objects only, the draw range from its start, instances counted, a BatchedMesh by the lists of its last draw. DO NO

### src/lab/inspector/dom.ts
- 5-10 `el` (function): Small DOM helpers of the inspector's tabs: elements, buttons, key-value grids and an expandable JSON view.
- 13-22 `button` (function): A button: an icon (from `icons.ts`, '' none) and/or its text; `title` its tooltip and accessible name.
- 24-28 `bytes` (function)
- 30-31 `num` (function)
- 34-39 `heading` (function): A section heading.
- 42-52 `kv` (function): A two-column grid of names and values (a value may be an element).
- 55-61 `fold` (function): A collapsible block (`<details>`), open or not.
- 64-82 `preview` (function): A value's one-line preview, as a console shows it.
- 85-100 `entriesOf` (function): The entries of a value a tree shows when it is opened (at most `limit`).
- 102-102 `expandable` (function)
- 105-132 `valueTree` (function): A value as an expandable tree (each level made when it is opened, at most 300 entries).
- 135-150 `safeJson` (function): JSON of anything the game holds: Maps and Sets as arrays, typed arrays summarised, cycles cut.
- 153-155 `copyText` (function): Text to the clipboard, a failure logged.

### src/lab/inspector/edit.ts
- 13-13 `Holder` (type): THE LIVE EDITS: every field of the selected object, its materials, their textures and uniforms and its geometry's draw range, set from the panel to try a change without touching the code. Each goes through the edit log (`state.ts`): undone
- 16-24 `check` (function): A checkbox bound to `holder[key]`.
- 27-48 `numberField` (function): A number: a slider over `min`-`max` and a field for any value.
- 51-56 `rangeOf` (function): A range guessed from a value: 0-1 for a share, 0 to four times it otherwise.
- 59-69 `colourField` (function): A colour (`Color`) edited with the browser's colour input.
- 72-90 `vectorField` (function): A vector's components, each a number field (the whole vector set again on a change).
- 93-101 `selectField` (function): A select of named numbers (a side, a filter).
- 103-103 `objectLabel` (function)
- 106-139 `objectEditor` (function): The object's own fields, and the buttons that hide it, hide everything else, or bring the camera to it.
- 142-156 `isolateObject` (function): Everything but the object hidden (each sibling on its way up to the scene), the lights and the inspector's own kept; one undo shows them again.
- 159-159 `COLOURS` (variable): Material fields worth a control, by kind: colours, shares 0-1, other numbers, switches.
- 160-160 `SHARES` (variable)
- 161-161 `NUMBERS` (variable)
- 162-162 `SWITCHES` (variable)
- 163-163 `TEXTURE_SLOTS` (variable)
- 165-212 `materialEditor` (function)
- 215-229 `uniformField` (function): One uniform's control by the type of its value; null for what cannot be edited here (a texture, an array).
- 232-246 `textureEditor` (function): A texture's anisotropy and filters, sent again with the texture (`needsUpdate`) where its image is still on the CPU.
- 249-260 `geometryEditor` (function): The geometry's draw range: how much of it is drawn.

### src/lab/inspector/entity.ts
- 12-23 `GameProbe` (interface): What the inspector reads of the development probe (`main.ts` `__roadcraft`).
- 25-25 `gameProbe` (function)
- 28-32 `namesUp` (function): The top-level group under the scene an object hangs from (its layer), and the names on its way.
- 35-35 `toMap` (function): Three's world point (x, height, -y) as the map's (x, y, z up).
- 37-37 `pointRay` (variable)
- 39-43 `offRay` (function): How far a map point at height z lies from the ray, and how far along it.
- 46-64 `agentsNear` (function): The vehicle and the person nearest the line of sight, within their size, in front of what the ray met first (`before`).
- 67-102 `entitiesAt` (function): Every entity under a pixel, the first the one the selection takes: the layer of the 3D object hit first says which kind leads.
- 105-111 `EntityInfo` (interface): An entity's title, its main fields and its whole record, and where it stands on the map.
- 113-113 `fixed` (function)
- 115-207 `entityInfo` (function)
- 210-219 `entityLabel` (function): An entity in one line (lists, the report).

### src/lab/inspector/index.ts
- 15-65 `createInspectorTabs` (function): THE LAB'S INSPECTOR, Chrome's element picker for the game: "Inspector" (pick in the game and see everything about it - the game's entity, the 3D object, its geometry, materials, program, switches, draws, memory and the code that makes it -

### src/lab/inspector/inspectorTab.ts
- 13-146 `createInspectorTab` (function): THE INSPECTOR TAB: the picker's button, then everything about the selection - the game's entity (its record, its lot, its traffic), the 3D object, its geometry and materials with their live edits, what relates to it (switches, optimisation

### src/lab/inspector/pick.ts
- 11-11 `POINTER_EVENTS` (variable): THE ELEMENT PICKER, FOR THE GAME: while on, what is under the cursor is outlined (its box drawn over everything) with a tooltip, and a click selects it. The game never sees those clicks: listeners on the window in the capture phase take ev
- 13-13 `HOVER_MS` (variable): At most one hover pick in this many ms: a pick walks the scene.
- 15-16 `isGameCanvas` (function)
- 19-30 `selectionAt` (function): The selection under a canvas pixel (CSS px from its corner).
- 32-36 `Highlight` (interface)
- 39-71 `createHighlight` (function): A box drawn over everything (depth test off), in the inspector's colour; never picked itself.
- 73-79 `Picker` (interface)
- 81-191 `createPicker` (function)

### src/lab/inspector/raycast.ts
- 11-11 `INSPECTOR_OWN` (variable): Objects of the inspector's own (the highlight), never picked.
- 14-19 `rayAtPixel` (function): The ray through a canvas pixel (CSS px from the canvas's corner), in three's world (x, height, -y); null where the view has no line of sight.
- 22-27 `hasArray` (function): Whether an attribute still has its array on the CPU (`gpuOnly.ts` lets it go after the upload).
- 30-38 `testable` (function): Whether three can test the object's triangles: its positions, its index and (instanced) its matrices on the CPU.
- 41-49 `shaderPosed` (function): Whether what is drawn is moved in the vertex shader (a crowd posed on the GPU, wind): the CPU's triangles may not be where it is drawn.
- 51-54 `drawable` (function)
- 56-56 `boxHit` (variable)
- 57-57 `box` (variable)
- 58-58 `local` (variable)
- 59-59 `world` (variable)
- 62-87 `boundsHits` (function): The ray against an object's bounds: each instance's box for an instanced mesh, else the whole object's.
- 90-130 `hitsAlong` (function): Every visible object the ray meets, nearest first: by its triangles where three can test them, by its bounds otherwise (and for the ones posed in their shader, both).
- 134-138 `primaryHit` (function): The hit the selection takes: the nearest solid met by its triangles; else the smallest solid met by its bounds (the roads and buildings let their arrays go); else what is seen through.
- 141-143 `solidExact` (function): The nearest solid hit met by its triangles; null when only bounds (or see-through things) were met.
- 146-150 `seeThrough` (function): A glow, a light's cone or a pool of light (transparent, writing no depth): seen through, the solid behind it is what was pointed at (it stays in "Under the cursor").
- 153-161 `smallest` (function): The hit with the smallest bounds among the nearest few.
- 164-171 `ENTITY_LAYERS` (variable): The groups each kind of the game's entities is drawn in (the scene as `renderer.ts` builds it).
- 175-183 `hitForEntity` (function): Among the hits, the one drawing the game's entity (found by the game's own pick) where no solid was met by its triangles: a road's or a building's meshes let their arrays go, and their bounds are whole blocks; their layer says which is the
- 185-189 `boundsRadius` (function)
- 192-217 `selectionBox` (function): The world box of what is selected: one instance's, a batch's, or the whole object's; null when it has none.

### src/lab/inspector/related.ts
- 8-22 `LAYER_BY_NAME` (variable): Each layer switch by the names of the groups it hides (`renderer.ts` `switchedOff`, the scene as built: `world`, `agent-parts`, `buildings-layer`...).
- 25-31 `BUILDING_ROOTS` (variable): The buildings layer's roots (`BuildingLayer.roots`, named in `buildings/layer.ts`) and the switch of each.
- 34-43 `buildingKindSwitch` (function): A piece of a building by its mesh's name (`buildingSwitches.ts` KIND_SWITCH).
- 45-45 `SwitchLink` (interface)
- 48-65 `switchesOf` (function): The switches that hide the object or what it hangs from.
- 68-79 `optimizationSystemsOf` (function): The lab's optimisation systems (`optimizations.ts` OptSystem) that bear on the object's layer; the global ones always.
- 82-84 `memoryOf` (function): The object's own memory (its subtree): buffers and textures, CPU and GPU.
- 87-87 `GpuDraw` (interface): The GPU tab's last captured draws of an object (`window.__labGpu.drawsOf`); null when no capture or no GPU tab.
- 88-97 `gpuDrawsOf` (function)
- 100-110 `SourceHit` (interface): One place in the sources a name is made (the dev server's search).
- 113-124 `findSource` (function): The places in `src/` that make a name (`GET /__lab/source-find`, dev server only); an error's text when the server cannot answer.
- 127-131 `openInEditor` (function): The file opened in the editor at its line (Vite's `/__open-in-editor`, the `launch-editor` middleware, dev server only).

### src/lab/inspector/relatedView.ts
- 9-9 `OptRow` (interface): THE INSPECTOR'S "RELATED": the switches that hide the object (flipped from here), the optimisations of its system, its draws in the GPU tab's last capture, its memory, and where in the code it is made, each place opened in the editor.
- 11-104 `relatedSection` (function)
- 106-132 `showSources` (function)

### src/lab/inspector/sceneTab.ts
- 12-12 `MAX_CHILDREN` (variable): THE SCENE TAB: the scene's tree, opened a level at a time (its rows made only when shown), filtered by name or type; each node with its children and triangles, the hidden ones dimmed, an eye to hide or show it (an edit, undone in the Inspe
- 13-13 `MAX_FOUND` (variable)
- 15-142 `createSceneTab` (function)

### src/lab/inspector/sections.ts
- 14-18 `reselect` (function): The selection moved to another object (an ancestor, another hit), the game's entity kept.
- 20-63 `objectSection` (function)
- 66-84 `instanceBlock` (function): One instance's own data: its colour, its batch's geometry.
- 87-104 `hitsBlock` (function): Every object the ray met, nearest first: a click selects it.
- 106-129 `geometrySection` (function)
- 131-137 `materialsSection` (function)
- 139-181 `materialBlock` (function)
- 183-190 `safeKey` (function)

### src/lab/inspector/state.ts
- 7-13 `EntityRef` (type): The game's own thing under the cursor, by its id in the document or the simulation.
- 16-24 `Hit` (interface): A 3D object the ray met: by its triangles (`exact`), or only by its bounds (its arrays let go to the GPU, or posed in its shader).
- 26-38 `Selection` (interface)
- 41-50 `EditEntry` (interface): One live edit: undone alone (`undo`), described for the report.
- 52-59 `InspectorState` (interface)
- 61-68 `SetOptions` (interface)
- 70-80 `EditLog` (interface)
- 82-82 `Copyable` (interface)
- 84-88 `copyable` (function): A value edited in place (a colour, a vector): never a texture or a material, which are swapped whole.
- 91-100 `shown` (function): A value as the log shows it.
- 102-181 `createEditLog` (function)
- 183-208 `createInspectorState` (function)

### src/lab/investigate.ts
- 7-7 `INVESTIGATE_EVENT` (variable): The DOM event a table (or anything) sends up to the lab's window to have a subject investigated.
- 9-11 `sendInvestigate` (function)
- 13-13 `SourceHit` (interface)
- 14-14 `ProfileRow` (interface)
- 15-15 `OptRow` (interface)
- 17-17 `FILE_REF` (variable)
- 20-23 `parts` (function): What the subject's name is, without its kind ("system: drawing" → "drawing"), and that kind.
- 26-40 `queriesOf` (function): The searches that find where in the code the subject is made or measured, by where it was measured.
- 43-60 `explain` (function): What the subject is, in a sentence, by where it was measured.
- 62-62 `words` (function)
- 64-68 `Investigator` (interface)
- 70-292 `createInvestigator` (function)

### src/lab/lab.ts
- 14-23 `LabDeps` (interface): THE DEBUGGING LAB (F7): one window of tabs over the game - the switches and optimisations, what every system costs, the code that runs, the GPU's passes, the recordings and the tests. Every tab is handed the same context (`labTypes.ts`).
- 25-33 `Lab` (interface)
- 35-35 `EVENTS_KEPT` (variable)
- 37-140 `createLab` (function)

### src/lab/labTypes.ts
- 10-16 `LabEvent` (interface): Something that happened, on the page's clock (`performance.now()`): a switch flipped, a test begun, a long task, a hitch.
- 19-33 `LabSample` (interface): One frame as the lab saw it: its times, what it drew, the heap.
- 35-64 `LabContext` (interface)
- 67-74 `LabSubject` (interface): Anything clicked to be investigated (`investigate.ts`): its name, and every field it showed.
- 77-88 `LabTab` (interface): A tab of the lab: built once, mounted when shown, refreshed a few times a second while shown.
- 90-90 `LAB_REFRESH_MS` (variable)

### src/lab/optimizations/index.ts
- 10-21 `OptRow` (interface): An optimisation as the tests, the inspector and the reports read it: plain JSON.
- 23-32 `rowOf` (function)
- 34-222 `createOptimizationTabs` (function)

### src/lab/recorder/analysis.ts
- 6-6 `SETTLE_MS` (variable): After a change, this long is settling (programs compiled, targets made): its frames out of the stretch's figures, its worst reported apart.
- 8-8 `SETTLE_RUN` (variable): Settling goes on past `SETTLE_MS` while the frames are still frozen (a burst of compiles can stop the game for seconds): it ends where this many frames in a row come in calm.
- 10-10 `SETTLE_MAX_MS` (variable): No calm run this long after the change: the stretch never settled (its figures left out, said so).
- 12-12 `COALESCE_MS` (variable): Changes this close together (one `setMany`, a group switched) are one cut.
- 14-14 `AUTOCORRELATION` (variable): Frames lean on the frames before them (a slow one delays the next): every this many count as one when the noise is guessed.
- 16-16 `NOISE_FLOOR_MS` (variable): Below this a difference in ms is nothing the timers can tell apart.
- 19-29 `Dist` (interface): A distribution of one measure over a stretch's frames.
- 31-40 `Settle` (interface)
- 42-62 `Segment` (interface)
- 64-64 `Verdict` (type)
- 66-74 `Delta` (interface)
- 76-86 `Comparison` (interface)
- 88-93 `quantile` (function)
- 95-104 `distribution` (function)
- 107-114 `slopePerSecond` (function): Least squares slope of y over t (ms), per second.
- 117-126 `boundaries` (function): The cuts: every event that splits, those close together as one.
- 129-141 `describeChange` (function): A change said as a person would: "turn off bShadows", "bTriangles = 0.5".
- 144-158 `settleEnd` (function): When the settling after a change at `startT` ends: the first run of `SETTLE_RUN` calm frames finishing past `SETTLE_MS` (calm: interval and CPU under 50 ms, or twice the stretch before's p95 on a machine that is slow anyway).
- 160-222 `segment` (function)
- 225-230 `noiseOf` (function): How far a figure must move to be more than the noise: twice the standard error of the difference, the frames thinned for their dependence. The median's error is 1.253 σ/√n; the tail's (p95) taken as 2.5 σ/√n, a deliberate overestimate.
- 232-241 `METRICS` (variable)
- 243-243 `fmtNum` (function)
- 244-244 `signed` (function)
- 245-245 `unit` (function)
- 247-250 `describeDelta` (function)
- 252-275 `compare` (function)
- 277-289 `compareAll` (function)

### src/lab/recorder/deliver.ts
- 4-6 `DesktopLab` (interface): The desktop shell's call (`desktop/preload.cjs`), when the page runs in it.
- 8-12 `ServerReport` (interface)
- 15-24 `download` (function): A file offered to the player (MDN: a Blob's object URL on an `<a download>`, revoked once clicked).
- 27-31 `openText` (function): A text opened in a new tab (the Markdown read in place).
- 34-45 `postToServer` (function): Written by the dev server into the repository's `lab-reports/`: the paths written, or null (no dev server, refused).
- 48-57 `saveDesktop` (function): Written by the desktop app into its data folder: the folder, or null (not the desktop app).
- 60-70 `listServer` (function): The reports the dev server keeps (`lab-reports/`), newest first; empty without one.
- 72-72 `serverUrl` (function)

### src/lab/recorder/index.ts
- 12-17 `SessionReport` (interface): THE LAB'S RECORDER (tabs "Recording" and "Reports"): the game recorded as it is set, cut at every change, each stretch held against the one before; on Stop a report for an AI (JSON + Markdown) downloaded, written by the dev server into `lab
- 19-29 `LabRecorderProbe` (interface)
- 32-32 `LIVE_SEGMENTS_MS` (variable): The live segments are cut again at most this often while shown.
- 34-42 `button` (function)
- 43-48 `row` (function)
- 49-54 `card` (function)
- 55-59 `heading` (function)
- 60-67 `verdictOf` (function)
- 69-355 `createRecorderTabs` (function)

### src/lab/recorder/recording.ts
- 5-12 `RecEvent` (interface): ONE RECORDING: every frame the lab sees, every event, the camera, the simulation's counts and the settings at each change, kept from Start to Stop. Times are ms since the recording began (page clock).
- 15-28 `FrameColumns` (interface): The frames as columns (one array a measure): compact in memory and in the JSON.
- 30-38 `ViewSample` (interface)
- 40-47 `SimSample` (interface)
- 49-54 `SettingsSnapshot` (interface)
- 56-56 `RecState` (type)
- 58-74 `Recording` (interface)
- 76-83 `RecordingHandle` (interface)
- 86-86 `SPLITS` (variable): Events that cut the recording: the switches, the optimisations, the quality, the Inspector's edits, a marker, a resumption.
- 88-88 `SETTING_CHANGE` (variable): Settings read again after these.
- 90-90 `MAX_FRAMES` (variable): About an hour at 60 Hz: past it the recording stops itself.
- 91-91 `VIEW_EVERY_MS` (variable)
- 92-92 `SIM_EVERY_MS` (variable)
- 94-94 `VIEW_STILL_MS` (variable): A view still this long, then moved, is a new camera move (one event).
- 96-96 `round` (function)
- 98-107 `readOptimizations` (function)
- 110-241 `startRecording` (function): Starts a recording now; `onStop` told once, whatever stopped it.

### src/lab/recorder/report.ts
- 8-8 `REPORT_FORMAT` (variable): THE RECORDING'S REPORT, written for an AI to read: the JSON with everything, and a Markdown that says what was changed, what it did, and how sure it is.
- 10-14 `Anomaly` (interface)
- 16-26 `LabReport` (interface)
- 28-37 `gpuName` (function)
- 39-39 `s` (function)
- 40-40 `ms` (function)
- 41-41 `int` (function)
- 44-76 `anomaliesOf` (function): The frames before the first of each segment's settled frames: what is left out.
- 79-98 `suspectsOf` (function): The other areas' sections, a few lines each: a summary string if they have one, else their costliest rows.
- 100-100 `distJson` (function)
- 102-262 `buildReport` (function)

### src/lab/recorder/timeline.ts
- 5-9 `SERIES` (variable): THE RECORDING DRAWN: frame interval, CPU and GPU over time (each pixel column the worst of its frames), a line at every change and marker.
- 11-23 `timelineLegend` (function)
- 25-89 `drawTimeline` (function)

### src/lab/resources/collector.ts
- 10-19 `FramePoint` (interface): One frame kept for the overview's graphs.
- 22-27 `LongFrame` (interface): A long animation frame as kept (W3C Long Animation Frames: `duration`, `blockingDuration`, `scripts`).
- 30-38 `PlatformReadings` (interface): A slow reading of the platform, taken now and then: storage, the user agent's memory, the battery.
- 41-47 `GlPoint` (interface): One reading of the GPU's objects (`renderer.info`), for the growth checks.
- 50-50 `FRAMES_KEPT_MS` (variable): How long the overview's frames are kept, ms.
- 52-52 `GL_POINTS_KEPT` (variable): Readings of the GPU's objects kept (one a second): ten minutes.
- 54-54 `POLL_MS` (variable): The slow readings' period: the pool, the ledger, the targets each second; storage every `STORAGE_EVERY` of them.
- 55-55 `STORAGE_EVERY` (variable)
- 57-57 `UA_MEMORY_MEAN_MS` (variable): The user agent's memory measured at random times, a mean of this apart (MDN `measureUserAgentSpecificMemory`: a Poisson process, never in step with what the player does).
- 59-59 `OPFS_MOST` (variable): The origin's private file system walked at most this many entries deep in all.
- 61-61 `Heap` (type)
- 62-62 `ScriptEntry` (type)
- 63-63 `LoafEntry` (type)
- 64-64 `ResourceEntry` (type)
- 67-77 `shortUrl` (function): A URL shown short: its path without the query, the host only when it is not this page's.
- 80-86 `ownerLabel` (function): A GPU ledger's owner named: a string as it is (its first two `|` parts), an object by its name or its type.
- 88-108 `Collector` (interface)
- 110-448 `createCollector` (function)

### src/lab/resources/consumers.ts
- 9-9 `WINDOW_LABEL` (variable): THE LAB'S CONSUMERS ("Consumers"): one table of everything that costs - main thread, workers, GPU, video memory, the page's memory, disk, network - each row a meter (`meter.ts`) read over 5 s, 30 s or the whole session
- 11-14 `num` (function)
- 17-31 `columns` (function): The columns; `perSecond` the window's seconds (the time a cost in ms takes of each second: the one figure comparable across rows of time).
- 33-36 `ConsumersView` (interface)
- 38-113 `createConsumersTab` (function)

### src/lab/resources/diagnostics.ts
- 11-11 `Verdict` (type): THE LAB'S DIAGNOSTICS ("Checks"): the machine, the browser and the game's resources checked one by one, each with its verdict (ok, warning, problem), what it read and why it matters, and the tab where to look further
- 13-23 `Check` (interface)
- 26-26 `TAB_IDS` (variable): The lab's tabs the checks lead to, by title.
- 28-28 `RANK` (variable)
- 31-31 `SOFTWARE` (variable): A GPU named by software (no hardware drawing): every frame on the CPU.
- 34-39 `growth` (function): A count of the GPU's objects (`renderer.info`) growing over the readings of the last `ms` while the world stood: the slope a minute and the rise.
- 42-225 `runChecks` (function): Every check, now (the storage, the battery and the user agent's memory as last read: `collector.platform`).
- 227-278 `createDiagnosticsTab` (function)

### src/lab/resources/index.ts
- 11-65 `createResourceTabs` (function): THE LAB'S RESOURCES: the overview (the lab's first tab), every consumer in one table, and the diagnostics - one collector behind them (`collector.ts`), begun the first time the lab opens (nothing is observed before: a game never debugged pa

### src/lab/resources/meter.ts
- 4-4 `ResourceCategory` (type): What a consumer spends: the main thread, a worker's, the GPU's time, video memory, the page's memory, the disk, the network.
- 7-7 `MeterKind` (type): `event`: each sample a cost that happened (a measure, a frame's system) - its total means something; `gauge`: each sample a reading of a level (bytes held, jobs waiting) - its last is what stands now.
- 10-10 `ResourceWindow` (type): A window of time read: the last 5 s, the last 30 s, or since the session's start (or the last reset).
- 12-12 `WINDOW_MS` (variable)
- 15-32 `MeterReading` (interface): One meter read: what the consumers' table shows of it.
- 35-35 `RING` (variable): Samples kept per meter for the windows: past this the oldest go (a 30 s window at 68 Hz and more reads only its last 2048).
- 37-37 `RESERVOIR` (variable): Samples kept at random for the session's 95th percentile (Vitter's reservoir, algorithm R).
- 40-40 `ranked` (function): The P-th percentile (nearest rank) of values already sorted.
- 42-153 `Meter` (class)
- 44-44 `Meter.times` (field): The ring: times and values, `head` the next slot, `size` how many are held.
- 45-45 `Meter.values` (field)
- 46-46 `Meter.head` (field)
- 47-47 `Meter.size` (field)
- 49-49 `Meter.count` (field): The whole session (since `reset`): count, sum, largest, newest, and a reservoir of values.
- 50-50 `Meter.sum` (field)
- 51-51 `Meter.largest` (field)
- 52-52 `Meter.newest` (field)
- 53-53 `Meter.reservoir` (field)
- 54-54 `Meter.note` (field)
- 56-63 `Meter.constructor` (method)
- 65-81 `Meter.add` (method)
- 84-95 `Meter.grow` (method): The ring doubled, its samples kept in order.
- 98-106 `Meter.since` (method): The values sampled since `since`, oldest first.
- 113-121 `Meter.reset` (method)
- 124-152 `Meter.read` (method): Read over `window` ending `now`: the session's from its running sums and its reservoir, a window's from the ring.
- 156-188 `MeterBook` (class): Every meter by key, made on first use.
- 157-157 `MeterBook.meters` (field)
- 160-164 `MeterBook.meter` (method): The meter `key`, made with these traits the first time.
- 166-168 `MeterBook.get` (method)
- 170-172 `MeterBook.all` (method)
- 175-177 `MeterBook.reset` (method): Every meter emptied (the gauges' last level too: read again at the next poll).
- 180-187 `MeterBook.read` (method): Every meter read over `window`; those with no sample in it left out (a gauge always kept while it has a level).
- 191-200 `slope` (function): The slope of `y` over `t` (least squares), per the unit of `t`; 0 with fewer than 3 points.
- 203-206 `percentileOf` (function): Nearest-rank percentile of unsorted values; null when none.

### src/lab/resources/overview.ts
- 9-18 `Kpi` (interface): One graph of the overview: what it reads of a frame, its unit, its colour.
- 21-40 `OverviewSummary` (interface): The overview's verdicts, read by the report and the diagnostics too.
- 42-50 `KPIS` (variable)
- 53-53 `HEAP_GROWS_MB_MIN` (variable): The heap growing faster than this, MB a minute, over the window: a leak suspected (a game standing still holds its heap level between collections).
- 55-55 `spread` (function)
- 58-111 `summarize` (function): What the frames of the last `windowMs` say.
- 113-113 `n1` (function)
- 114-114 `big` (function)
- 117-147 `drawSpark` (function): A graph of `values` over the last `spanMs` into `canvas`, its budget line dashed.
- 149-258 `createOverviewTab` (function)
- 261-275 `settingsSummary` (function): The settings that change the numbers, in one paragraph.
- 277-290 `table` (function)
- 292-297 `line` (function)

### src/lab/table.ts
- 6-17 `LabColumn` (interface): THE LAB'S TABLE: rows sorted by any column (a click on its head, again to reverse), filtered by text and by category, copied as CSV or JSON. Every tab lists what it measures with it.
- 19-32 `LabTableOptions` (interface)
- 34-40 `LabTable` (interface)
- 42-42 `fmt` (function)
- 44-216 `createLabTable` (function)

### src/lab/tabs.ts
- 12-32 `createLabTabs` (function): THE LAB'S TABS, in the order shown: each area builds its own (one folder each).

### src/lab/tests/benchmarks.ts
- 13-13 `BenchmarkKind` (type): THE BENCHMARKS: the standard views against the plan's budgets (`VIEW_BUDGETS`, `__roadcraft.budget`), the simulation pushed (its speed, its traffic and people), the leak test (the camera moved for a while: the heap and the GPU's objects mu
- 14-17 `BENCHMARK_LABELS` (variable)
- 19-24 `BenchmarkOptions` (interface)
- 27-35 `BenchRow` (interface): One line of a benchmark: what was looked at, the figure, its budget or reference, a status.
- 37-45 `BenchmarkResult` (interface)
- 47-70 `runBenchmark` (function)
- 72-72 `cap` (function)
- 75-115 `views` (function): Each standard view: settled with the clock held, a window measured, the plan's table read (`__roadcraft.budget`).
- 118-155 `stress` (function): The simulation pushed: its speed 1, 2, 4×, then its traffic and people at the most (`MAX_INTENSITY` 3×), each a while on the fixed step.
- 159-225 `track` (function): A reading a second for `seconds`: the camera circled and turned round where it was (`moving`), or still with the game running. Then, moving, the camera back where it began and the scene settled: what it holds then against what it held at f
- 229-276 `hitch` (function): The flips that compile programs: a quality option of the optimisations' area when it has one, else the switches whose shaders differ. Each flipped and back, two seconds of frames after each: the hitches, the longest frame and the programs 

### src/lab/tests/index.ts
- 14-14 `TestResult` (type): THE LAB'S TESTS (F7): the A/B sweep of every switch and optimisation, the benchmarks, the probes. One test at a time; every result kept for the lab's report (`tests` section) and for a script (`window.__labTests`, `scripts/lab/sweep.mjs`).
- 16-23 `LabTests` (interface)
- 26-26 `KEPT` (variable): Results kept (each sweep and benchmark run).
- 28-85 `createTestTabs` (function)
- 87-96 `num` (function)
- 97-101 `labelled` (function)
- 102-109 `button` (function)
- 110-110 `warningsOf` (function)
- 112-225 `sweepTab` (function)
- 227-281 `benchTab` (function)

### src/lab/tests/items.ts
- 7-16 `SweepItem` (interface): WHAT THE A/B SWEEP CAN FLIP: every switch of the effects panel (`effectsWiring.ts` snapshot, set with `setMany`) and every optimisation the optimisations' area exposes (`window.__labOpt`, when there). Each item knows its value now, the val
- 19-24 `LabOptApi` (interface): The optimisations' area's surface (another area's file: read defensively, any shape it gives).
- 25-25 `LabOptEntry` (interface)
- 27-30 `labOpt` (function)
- 33-36 `SWITCH_GROUPS` (variable): The groups of the switches, as the panel shows them (`effectsWiring.ts` GROUPS).
- 38-42 `CHOICE_B` (variable): The buildings' settings of several values: B is the cheapest of them (`effectsWiring.ts` BUILDING_CHOICES).
- 44-44 `NOT_SWEPT` (variable): Left out: they change what is measured, not what it costs (the loop on demand: a frame every refresh is the measure itself).
- 46-50 `choiceOf` (function)
- 52-102 `sweepItems` (function)

### src/lab/tests/measure.ts
- 9-15 `RoadcraftProbe` (interface): The console's probes the tests use when they are there (`main.ts`, development and the probe build).
- 17-17 `probe` (function)
- 19-19 `sleep` (function)
- 22-22 `Cancel` (interface): Asked to stop: every wait looks at it.
- 24-26 `Cancelled` (class)
- 27-27 `check` (function)
- 31-36 `median` (function): ---- statistics
- 37-41 `quantile` (function)
- 43-47 `robustSigma` (function): The median absolute deviation scaled to a standard deviation (×1.4826, as robust regression detectors use it).
- 49-58 `slope` (function): The least-squares slope of y over x.
- 59-59 `round` (function)
- 64-72 `SceneSign` (interface): What makes the scene the scene (measureCity's signature): the buildings' levels chosen and shown, the vehicles, the people, the programs.
- 73-79 `sceneSign` (function)
- 81-82 `signText` (function): The signature as text; `withPrograms` false: a flip that compiled a program is still the same scene.
- 83-83 `programs` (function)
- 86-92 `Holdings` (interface): What the GPU and the page hold now (the leak test's readings).
- 93-101 `holdings` (function)
- 106-108 `Hold` (interface): A frame asked for every refresh while a test runs (the loop draws on demand), and the clock held (`__roadcraft.measure`) when asked.
- 109-129 `holdScene` (function)
- 133-133 `ViewId` (type): ---- the views
- 134-134 `VIEW_LABELS` (variable)
- 136-136 `VIEW_ZOOM` (variable): The zooms of the standard views (`scripts/look/views.json`: map 0.17, district 2.5, street 28).
- 141-141 `CameraAt` (interface)
- 142-145 `cameraAt` (function)
- 147-154 `goToView` (function): The camera at a standard view round `centre` (the camera's own centre if none); false when the probe is not there (`current` always true).
- 155-157 `restoreCamera` (function)
- 162-167 `FrameWindow` (interface): Every frame for `count` frames (or `maxMs`), the GPU's samples pushed meanwhile, the disjoint results thrown.
- 168-193 `collectFrames` (function)
- 196-209 `WindowStats` (interface): A window's figures: medians of the GPU, the CPU and the frame; the means of draw calls and triangles; the robust spread of each median.
- 211-211 `HITCH_MS` (variable): A frame a player sees as a hitch, ms (two refreshes and more at 60 Hz).
- 212-226 `windowStats` (function)
- 230-251 `settle` (function): Settled: nothing waiting to land, the scene's signature and its programs the same for `stableMs`, and the frames' times steady (two halves of the last frames within 20 %). Answers how long it took, and whether it gave up at `maxMs`.

### src/lab/tests/probes.ts
- 8-27 `SAFE` (variable): The calls known to only read (`main.ts` probes, the lab's own): anything else is listed, not run - it may move the camera, open a map, reset a reading (`__gpu(true)`) or grow a building.
- 32-40 `SPECS_NOTE` (variable): What the specs are and how they run: `vitest.config.ts`, `vitest.scale.config.ts`, `package.json`. A list of the files is not built into the page on purpose: `import.meta.glob` would put every spec into the game's build (Vite turns each ma
- 42-51 `resolvePath` (function)
- 54-69 `safeJson` (function): JSON of anything: Maps and Sets as arrays, cycles and DOM nodes named, long text cut.
- 71-121 `createProbesTab` (function)
- 124-135 `listProbes` (function): Every `window.__*` and, for objects, their functions and fields, one line each.

### src/lab/tests/sweep.ts
- 12-25 `SweepOptions` (interface): THE A/B SWEEP: for each item, the scene settled, a window measured as it stands (A), the item flipped and the scene settled again (its programs compiled, the frames steady), a window (B), flipped back, a window again (A): A/B/A, so a scene
- 27-27 `Verdict` (type)
- 29-55 `SweepRow` (interface)
- 57-69 `SweepResult` (interface)
- 71-71 `Rep` (interface)
- 73-73 `text` (function)
- 76-80 `pickItems` (function): The items a sweep's options pick.
- 82-145 `runSweep` (function)
- 148-182 `rowOf` (function): One item's row from its repetitions: the deltas are B against the mean of its two A windows, their median over the repetitions.

### src/lab/usage.ts
- 6-15 `UsageReading` (interface): THE LAB'S USAGE METERS: how much of each resource the game is taking now, as a share - the main thread, the GPU, the JS heap, the video memory, the disk - over the last second.
- 17-24 `UsageMeter` (interface)
- 26-26 `WINDOW_MS` (variable)
- 27-27 `HISTORY` (variable)
- 28-28 `mb` (function)
- 30-135 `createUsageMeter` (function)

### src/lab/window.ts
- 8-8 `Mode` (type): THE LAB'S WINDOW: docked at either side, centred large, maximised or reduced to its meters; resized from any edge, dragged by its head; tabs at the side when wide; the usage meters on top, each opening what consumes it; a status line at the
- 10-18 `Kept` (interface)
- 20-20 `KEY` (variable)
- 21-27 `MODES` (variable)
- 30-37 `GROUPS` (variable): The tabs' groups, in the order shown (a tab of no group goes to the last).
- 40-48 `METER_TARGET` (variable): Where each meter leads when clicked: the tab and what it is asked to show.
- 50-59 `readKept` (function)
- 61-68 `LabWindow` (interface)
- 70-364 `createLabWindow` (function)

### src/main.ts
- 80-80 `__CONFLICT_ZONES_HASH__` (variable)
- 81-81 `__CITY_GEN_HASH__` (variable)
- 154-154 `clearBulbs` (variable): The turning circles cleared after each edit, from the network's changes since the last one (`bulbClearing`).
- 156-156 `Alignment` (type)
- 159-159 `ExpressionStatement` (statement): The interface language is resolved and applied BEFORE anything reads a label
- 160-160 `ExpressionStatement` (statement)
- 163-163 `problemShell` (variable): FIELD PROBLEMS (`fieldReport.ts`) wired before anything can fail: to the desktop shell's local log when it runs in one, the console in a browser.
- 164-164 `ExpressionStatement` (statement)
- 166-166 `heapBytes` (function): The JS heap now, where the browser tells it (Chromium's `performance.memory`).
- 168-168 `canvas` (variable)
- 171-171 `health` (variable): The game's health (`core/health.ts`): what broke and what was slow, with what the player was doing.
- 172-172 `frameTimer` (variable)
- 173-187 `healthWatch` (variable)
- 189-189 `doc` (variable)
- 191-191 `NEW_RELIEF` (variable): A new map is a level plain (the player, 2026-10-09); a saved one keeps its own (restored below).
- 192-192 `ExpressionStatement` (statement)
- 194-194 `newNature` (function): A new map's ecosystem: the default biome, its patches laid by a seed of its own.
- 195-195 `ExpressionStatement` (statement)
- 196-196 `net` (variable)
- 197-197 `camera` (variable)
- 199-199 `booted` (variable): Whether the whole module has run.
- 200-200 `surface` (variable)
- 201-201 `history` (variable)
- 203-203 `ExpressionStatement` (statement): On the desktop the map is kept as files Steam Cloud syncs (`desktopSaves.ts`); in a browser, IndexedDB.
- 204-204 `desktopStores` (variable)
- 205-205 `persistence` (variable)
- 208-208 `desktopJobs` (variable): On the desktop the long job workers (the tiles' maker, the growth) run in the shell's job processes, a V8 cage each (`desktopJobs.ts`); before any worker is made.
- 209-209 `IfStatement` (statement)
- 210-218 `IfStatement` (statement)
- 220-220 `ExpressionStatement` (statement)
- 223-223 `surfaceBake` (variable): ------------------------------------------------------------------ boot
- 225-225 `ZONES_PREFIX` (variable): The conflict zones measured in earlier sessions
- 226-226 `IfStatement` (statement)
- 228-228 `keptZones` (variable): DO NOT read every kept zone at the start (the store grows with every junction the city ever had): the simulation's worker reads the pairs it needs (`ConflictIndex.lookup`).
- 229-229 `ExpressionStatement` (statement)
- 231-231 `STREAMED_MARK` (variable): The mark a world by tiles left open at its last close: DO NOT open it again - the player, 2026-10-18: no city by tiles any more; the mark is let go, and the map saved as a map opens.
- 232-232 `TryStatement` (statement)
- 233-233 `streamedAtBoot` (variable)
- 236-236 `TABLE_RESTORE_BUILDINGS` (variable): Buildings from which a saved map is restored through one table (`prepareCity`): a small map loads as it did.
- 237-237 `ExpressionStatement` (statement)
- 241-241 `bootSave` (variable): The autosave read and restored into the document; what the boot reads on - the save's settings, whether the player had a map, whether it failed to load. DO NOT keep the loaded save in the module's scope: it held the save's whole map (every
- 243-243 `savedSession` (variable): The boot's save without its map: its settings, read on through the boot.
- 245-245 `hadSavedMap` (variable): Whether the player had a map of their own at the boot (the start screen's continue).
- 246-246 `bootFailed` (variable)
- 248-276 `restoreBootSave` (function)
- 279-293 `clearOldMapsOnce` (function): The cities saved before 2026-10-04 (the shipped town, the player's own maps, the copies set aside) are removed, once
- 295-295 `sim` (variable): The simulation, in a worker of its own (`sim/host/simHost.ts`): the page sends it the map's changes and reads its frames.
- 297-297 `maxPopulationShare` (variable): The population the machine sustains (`sim/populationGovernor.ts`, run in the worker), never more than the device's own share.
- 301-301 `measure` (variable): A measure running (`__roadcraft.measure`, `scripts/lib/measureCity.mjs`): `on` a frame drawn every refresh; `step` the simulation going on a fixed step of `MEASURE_STEP_S` a frame, whatever the wall clock did, so two runs of a scene step t
- 303-303 `PROBES` (variable): The console's probes (`__roadcraft`, `__frames`, `__gpu`, `__state`, `__changes`): in development, and in a build made to be measured (`npm run build:probe`, mode `probe`) - the product's code measured, without the development panels (repor
- 305-305 `MEASURE_STEP_S` (variable): One display frame at 60 Hz, s: the simulation's step a frame in `measure('running')`.
- 306-308 `syncPopulationShare` (function)
- 310-310 `startPopulationShare` (variable): Where the population starts, by the machine's threads (the run's pretended ones, `core/weakMachine.ts`): the governor fills it from there when its steps are cheap.
- 311-311 `ExpressionStatement` (statement)
- 314-316 `bootZoomBounds` (function): The zoom range the renderer we are about to boot can actually represent.
- 319-319 `bootSettings` (variable): A streamed world's camera, where the player left it (its map is its tiles, read round it).
- 320-328 `IfStatement` (statement)
- 331-355 `isUntouchedStarter` (function): Whether an autosave is exactly one of the starter scenarios an earlier build seeded, never edited.
- 357-370 `worldBounds` (function)
- 374-374 `BrushMode` (type): The terrain brush: a land stamp (`TerrainMode`), or painting the ground (`terrainPaint.ts`).
- 376-376 `Landform` (type): The landforms: a shape and its ROCK in one tool
- 377-382 `LANDFORMS` (variable)
- 383-383 `landformOf` (function)
- 384-384 `hardnessByMode` (variable)
- 385-385 `ExpressionStatement` (statement)
- 386-386 `ExpressionStatement` (statement)
- 387-387 `ExpressionStatement` (statement)
- 388-388 `ExpressionStatement` (statement)
- 389-389 `ExpressionStatement` (statement)
- 391-391 `IfStatement` (statement): The saved city's time of day, not 06:30 again (`SavedSettings.minutes`).
- 393-430 `gameState` (variable): The game's state - the tool in hand, the pause and speed, the quality, the selection
- 432-432 `game` (variable): The game's state, read-only: `game.tool`, `game.paused`, `game.selectedSegment`...
- 434-437 `tools` (variable): The tool in hand (`editor/toolMachine.ts`): the only way it changes.
- 439-439 `gullyStrength` (variable): The gully brush's strength, % (Paisagem > Relevo > Ravinas).
- 441-441 `cloudScatter` (variable): How many clouds "spread over the sky" lays, and how much they vary, %.
- 443-455 `sessionSettings` (function)
- 458-486 `roadTool` (variable): The road tool (`editor/roadTool.ts`): the stroke, the chain, the curve waiting for its bend, the road settling.
- 488-498 `poleTool` (variable): The pole tool (`editor/poles.ts`): the stretch being dragged and the line's last pole.
- 500-500 `framePolePlan` (variable): The pole run planned for this frame (`PoleTool.plan`), shared by the 3D preview and the overlay.
- 502-513 `streetscapeTool` (variable): The landscaping tool (`editor/streetscapeTool.ts`): items on the footways.
- 515-528 `barrierTool` (variable): The walls tool (`editor/barriers.ts`): the wall, fence or hedge being traced.
- 530-543 `lotTool` (variable): The Zoning tool (`editor/lotTool.ts`): the lots the player draws, edits and zones, with its gestures' own state.
- 544-544 `hoverAnchor` (variable)
- 546-550 `select` (function): What is selected - a road (and where along it) or a junction - written through the game's state with why (`gameState`).
- 553-559 `cameraHand` (variable): The camera moved by hand: the pointers, a pan, an orbit, a pinch (`view/cameraGestures.ts`).
- 561-561 `KEY_TURN` (variable): Camera turn per press of the camera panel's buttons, rad.
- 563-567 `cameraMotion` (variable): The keys held and the wheel's notches, spent as a glide frame by frame (`view/cameraMotion.ts`).
- 569-577 `mover` (variable): The Move tool's node drag (`editor/nodeMover.ts`), previewed live and dropped as one undo step.
- 579-589 `terrainBrush` (variable): The terrain brush (`editor/terrainBrush.ts`): the stroke in progress and what each dab does.
- 591-605 `dabSettings` (function): The brush in hand, as one dab reads it (`editor/terrainBrush.ts` `DabSettings`).
- 606-606 `ExpressionStatement` (statement)
- 608-608 `view` (variable)
- 612-612 `overlayCanvas` (variable): The editor's overlay gets its OWN canvas, and must.
- 613-613 `overlayCtx` (variable)
- 614-614 `previewAsphaltPattern` (variable)
- 617-638 `asphaltPreviewPattern` (function): Asphalt grain used by the live 3D-editor preview.
- 640-640 `canvas3d` (variable)
- 641-641 `ExpressionStatement` (statement)
- 642-642 `ExpressionStatement` (statement)
- 643-643 `ExpressionStatement` (statement)
- 644-644 `ExpressionStatement` (statement)
- 647-647 `fieldSession` (variable): The session's summary told to the field log as the page closes (`fieldReport.ts`): the GPU, the tiers, the frames' times, the heap's peak.
- 648-648 `graphics` (variable)
- 649-649 `ExpressionStatement` (statement)
- 650-650 `ExpressionStatement` (statement)
- 652-652 `effects` (variable): The effects panel (F7, `effectsWiring.ts`): every effect and heavy system switched live, to measure what each costs.
- 654-654 `achievements` (variable): The city's milestones told to Steam when the desktop shell runs under it (`steamAchievements.ts`).
- 656-656 `ExpressionStatement` (statement): The graphics are what the player chose: the game lowers only the resolution, and only with "dynamic resolution" on.
- 657-657 `ExpressionStatement` (statement)
- 658-658 `ExpressionStatement` (statement)
- 660-660 `engineFlags` (variable): The traffic's worker, given the map as it opened and the conflict zones measured in earlier sessions.
- 662-662 `simLanes` (function): Its lots' bays and conflict pairs on this page's pool, talked to straight (DO NOT let the worker open a pool of its own: more isolates in the one V8 cage).
- 664-666 `ExpressionStatement` (statement): DO NOT wait for the traffic's worker to stand before the world is built: its map read and its traffic laid out took 1.8 s of a reopened city's boot, the page idle meanwhile (2026-10-21). Its messages queue behind its `init` and each frame w
- 667-667 `ExpressionStatement` (statement)
- 669-669 `engine` (variable): The engine that brings every derived product up to date, its ledger the work each did (`core/engine.ts`).
- 670-670 `ExpressionStatement` (statement)
- 671-671 `scene` (variable)
- 672-672 `ExpressionStatement` (statement)
- 674-691 `comesBackFromLostContext` (function): A WebGL context lost for good leaves a black canvas with the game still running behind it (Chrome after hours in a tab, a driver reset): a tab the browser does not give its context back comes back by itself, the session saved first.
- 692-692 `ExpressionStatement` (statement)
- 694-694 `ExpressionStatement` (statement): The context lost and back, and a shader that failed (reported while three checks them: `debug.checkShaderErrors`, development builds), told to the field log.
- 695-695 `ExpressionStatement` (statement)
- 698-698 `profileFrameEnd` (variable): Decision 5.4: the profiler, the F9 monitor and the console's hooks in a development build only (`scripts/look.mjs` runs on the dev server).
- 699-741 `wireProfiler` (function)
- 744-744 `ExpressionStatement` (statement): The traffic's topology is not built here: it is built a slice a frame while the opening puts the town together
- 745-745 `ExpressionStatement` (statement)
- 746-746 `ExpressionStatement` (statement)
- 749-749 `ExpressionStatement` (statement): No loading screen (player, 2026-10-02): the town plays at once and each person appears as their body is ready
- 750-750 `ExpressionStatement` (statement)
- 751-752 `ExpressionStatement` (statement)
- 753-753 `ExpressionStatement` (statement)
- 754-754 `ExpressionStatement` (statement)
- 756-756 `ExpressionStatement` (statement)
- 757-757 `ExpressionStatement` (statement)
- 760-806 `buildings` (variable): Modular buildings
- 808-810 `restoreOrbit` (function): Puts the camera back at a saved bearing and tilt; a save without one gets the default view.
- 812-821 `syncViewFromFlatCamera` (function)
- 823-826 `fitView` (function)
- 828-834 `syncFlatCameraFromView` (function)
- 837-839 `mutate` (function): ------------------------------------------------------------- mutations
- 842-851 `mutateRoads` (function): A road edit made in place a split, a class, lanes, parking, a section, a direction, a bend, a join, a copy, a node's
- 854-860 `transitEditor` (variable): The public transport tool (`editor/transitTools.ts`): each edit one undo step.
- 861-861 `ExpressionStatement` (statement)
- 864-864 `docText` (variable): The document as text for the autosave, kept while nothing in it has moved
- 865-872 `serializedDoc` (function)
- 874-874 `ExpressionStatement` (statement)
- 876-911 `mutateBuilt` (function)
- 915-928 `applySnapshot` (function): `table`: an imported map's buildings' table, prepared by the city's worker (`editor/preparedCity.ts`).
- 931-931 `deferSessionSave` (variable): A large city just opened: its session save waits for the fill after the curtain, instead of running on top of it (the 3.4 GB peak of the metropolis).
- 933-933 `SAVE_AFTER_FILL_MOST_MS` (variable): Most the deferred save waits for the fill after the curtain once the town is shown.
- 935-941 `saveOnceShown` (function): The large city's session save once its town is shown and its fill has landed (or `SAVE_AFTER_FILL_MOST_MS` after the reveal). DO NOT wait a fixed time from the map's arrival: a city of 70,000 buildings was still behind its curtain after 30 
- 942-953 `snapshotApplied` (function)
- 959-971 `worldAtScreen` (function): The world point under a screen position, ON THE SURFACE THAT IS THERE.
- 974-985 `groundHeightUnder` (function): The height of what is drawn under a screen point
- 988-990 `sceneHeightAt` (function): Whatever the player can see at a world point: a road deck, or the ground.
- 993-993 `PICK_TOP` (variable): Highest surface the pick looks for; step of the march down the ray.
- 994-994 `PICK_STEP` (variable)
- 997-997 `roadTop` (variable): DO NOT walk `doc.nodes` here on each road edit: `RoadTop` reads only the cells the edit reached.
- 998-1002 `surfaceTop` (function)
- 1005-1022 `firstSurfaceAt` (function): Where the ray under the cursor first meets what is drawn - a deck, or the ground - marched down from above.
- 1025-1031 `pointerWorld` (function): `rect`: the canvas's box when the caller has already read it for this event (a layout read each).
- 1034-1037 `coalescedPath` (function): Every position a move went through since the last event, CSS px of the canvas, at its own time stamp
- 1041-1053 `cancelGestures` (function): Ends every gesture in progress without committing it
- 1056-1073 `currentGesture` (function): The gesture in progress, named by its dictionary key
- 1074-1078 `syncGesture` (function)
- 1080-1080 `syncGestureSoon` (variable): After the event's own handlers: a task of its own, after the whole dispatch.
- 1081-1083 `ForOfStatement` (statement)
- 1085-1085 `NO_CAUSE` (variable): What the diary says a change came from (`world/changes.ts` `causeNext`)
- 1086-1092 `ForOfStatement` (statement)
- 1094-1102 `caused` (function): Runs `fn` with the diary's cause set to `cause`, then puts the one before back.
- 1105-1114 `bulldozer` (variable): The bulldozer (`editor/bulldozer.ts`): a click removes what is under it, a box everything inside.
- 1117-1119 `gestureInProgress` (function): Whether anything is being drawn or dragged right now.
- 1122-1126 `releaseHeld` (function): A press whose release will never arrive - the window lost focus, the pointer capture was taken
- 1129-1131 `cancelMove` (function): Cancels a node drag without leaving its live preview in the document.
- 1136-1136 `cloudStepOpen` (variable): A step the clouds' tool opened and its gesture has not closed yet.
- 1137-1150 `cloudTool` (variable)
- 1152-1162 `endTerrainStroke` (function): Ends the brush stroke (`TerrainBrush.end`), warning when the land's stamps near their cap.
- 1166-1170 `CanvasInput` (type): The canvas's input, taken as the browser dispatches it and handled at the start of the next frame, in order
- 1171-1171 `canvasInput` (variable)
- 1172-1172 `take` (function)
- 1175-1186 `handleInput` (function): One input handled (`frame`, first thing).
- 1188-1188 `ExpressionStatement` (statement)
- 1189-1189 `ExpressionStatement` (statement)
- 1190-1196 `ExpressionStatement` (statement)
- 1197-1201 `ExpressionStatement` (statement)
- 1202-1202 `ExpressionStatement` (statement)
- 1203-1203 `ExpressionStatement` (statement)
- 1204-1204 `ExpressionStatement` (statement)
- 1205-1205 `ExpressionStatement` (statement)
- 1207-1212 `doubleClick` (function)
- 1213-1216 `singleClick` (function)
- 1217-1396 `pointerDown` (function)
- 1398-1398 `ExpressionStatement` (statement)
- 1399-1402 `captureLost` (function)
- 1404-1475 `pointerMove` (function)
- 1477-1477 `HOVER_DRAWN` (variable): The tools whose overlay follows the hover.
- 1480-1482 `poleReach` (function): Pick radius for a pole, in world units at the current zoom (`PoleTool.reach`: one definition).
- 1484-1513 `endPointer` (function)
- 1516-1516 `ForOfStatement` (statement): The camera let go: the pointer's own look again.
- 1518-1523 `pointerLeft` (function): Nothing to aim at off the map: no hover preview left behind on it.
- 1524-1524 `ExpressionStatement` (statement)
- 1526-1534 `ExpressionStatement` (statement)
- 1535-1550 `wheel` (function)
- 1556-1556 `roadClassPictures` (variable): Each class's picture, drawn once (`roadSwatch` paints a canvas).
- 1558-1566 `roadClasses` (function): Every road class as the upgrade tool shows it: name, picture, total width in whole metres.
- 1569-1569 `MEDIAN_CLASS` (variable): The lanes a road is laid at.
- 1570-1575 `LANE_CHOICES` (variable)
- 1577-1585 `laneChoices` (function): The lane choices, the one the next road will be laid as lit: with no count chosen, the class's own.
- 1586-1594 `setLanes` (function)
- 1596-1602 `selectRoadType` (function)
- 1604-1609 `setAlignment` (function)
- 1611-1611 `ExpressionStatement` (statement): A key rebound: every sentence that names it is written again.
- 1614-1614 `ROUNDABOUT_RADIUS_M` (variable): The roundabout's radius, in whole metres.
- 1617-1630 `setZone` (function): A lot or zone setting changed (Zoning): the use, the density, the eraser, the lot verb, the cut.
- 1633-1633 `BRUSH_MODES` (variable): Every brush of the terrain tool, in the order of its number keys.
- 1634-1640 `setTerrainMode` (function)
- 1643-1643 `TERRAIN_RADIUS_RANGE` (variable): One writer for each brush number
- 1644-1644 `TERRAIN_STRENGTH_RANGE` (variable)
- 1645-1648 `setTerrainRadius` (function)
- 1649-1652 `setTerrainStrength` (function)
- 1655-1668 `mapSlider` (function): A slider of the map's own settings (the fog over the map, the weather, the gullies of the land)
- 1670-1671 `brushSlider` (function): A slider of a brush, kept between sessions (`ui/toolChoices.ts`): not the map's, no undo.
- 1674-1674 `ExpressionStatement` (statement): THE WEATHER (Landscape > Sky, `world/weather.ts`) and the water under the river brush: the map's, one undo step a drag.
- 1675-1676 `weatherSlider` (function)
- 1679-1679 `ATMOSPHERE_KEY` (variable): The sky the player sets: the mist over everything, and (from before every cloud was the map's) the cloud cover.
- 1680-1680 `atmosphere` (variable)
- 1681-1689 `TryStatement` (statement)
- 1690-1701 `applyAtmosphere` (function)
- 1703-1711 `IfStatement` (statement): The sky made no clouds of its own any more (every cloud is the map's, to move or take away)
- 1712-1712 `ExpressionStatement` (statement)
- 1715-1771 `SLIDERS` (variable): Every slider the interface shows, by name: its range and how the game reads and takes it.
- 1774-1776 `mapBiome` (function): The map's biome (`world/ecology.ts`): one gives an old map its ecosystem too; "none" takes it away. Undoable.
- 1777-1786 `setMapBiome` (function)
- 1789-1797 `wholeMapEdit` (function): One edit of the whole map, asked first and undoable: what each "clear" and "spread" button does.
- 1798-1817 `MAP_ACTIONS` (variable)
- 1820-1831 `takeTool` (function): Puts a tool in hand: what it does to the game (gestures dropped, the selection let go, the Builder switched on or off).
- 1834-1838 `showTool` (function): What follows the tool in hand outside the interface: the pointer's cursor, the Builder's mode, the hint.
- 1839-1839 `ExpressionStatement` (statement)
- 1842-1844 `setTool` (function): Takes a tool in hand (a panel's own choice, a road class key).
- 1847-1849 `freeSelection` (function): Puts everything down: no tool in hand, nothing picked, no panel.
- 1852-1854 `pickTool` (function): A tool button or key: picks the tool, or puts it down when it is already in hand.
- 1857-1857 `shell` (variable): The interface's handle: the road inspector's eyedropper opens the road tool through it.
- 1859-1946 `mountInterface` (function): The interface (`ui/v2/shell.ts`): its HUD, dock, drawer and selection panel, handed the game's state and its commands.
- 1948-1948 `compass` (variable): The compass in a corner of the interface (`mountInterface`).
- 1950-1952 `updateCompass` (function): Points the compass where north lies on screen: up the map.
- 1954-1954 `ExpressionStatement` (statement): Mounted after this module has finished evaluating
- 1956-1962 `setPaused` (function)
- 1964-1971 `setSpeed` (function)
- 1974-1978 `setDemand` (function): How much traffic the city asks for: Low, Normal or Peak.
- 1980-1983 `toggleCongestion` (function)
- 1984-1984 `ExpressionStatement` (statement)
- 1985-1985 `ExpressionStatement` (statement)
- 1986-1986 `ExpressionStatement` (statement)
- 1993-1993 `growthPool` (variable): THE GROWTH (`editor/growthFeed.ts`): the generated city's buildings and the zones' (`growZones`), one at a time, each designed in the growth's worker (`growthPool.ts`, `growth.worker.ts`) and written here when its answer comes - outside the
- 1994-1996 `growth` (variable)
- 1998-2012 `growthAnswered` (function): The worker's answer: the building written, stamped as grown now. False: not now - nothing is written in the middle of a drag (an authoring preview); the pool offers it again at the next frame (`GrowthPool.pump`).
- 2015-2027 `newMap` (function)
- 2029-2033 `newWorld` (function): A NEW CITY from the menu: the small city of Roadcraft 2.0 (`buildSmallCityMap`: straight blocks, medium size) laid out as ONE map, no tiles - the player, 2026-10-18: the tiles were for a city over the whole map, which never worked.
- 2035-2039 `newLargeWorld` (function): A LARGE CITY: the small city's generator over a larger radius (`largeCity.ts`), every block built, opened at a district while the rest of the world fills in behind the curtain.
- 2041-2041 `generatingCity` (variable): A city being made off the page (`cityGen.worker.ts`): the curtain over the map while the worker builds it, no frame of it on the page.
- 2043-2043 `cityOpening` (variable): The opening asked for while one is made: the same promise, never a second city over the first's answer.
- 2044-2044 `cityAskId` (variable)
- 2047-2047 `CITY_HASH` (variable): The generated cities kept from earlier sessions, by the generator's code (`cook-plugin.ts` DERIVED: its whole import closure): the same seed makes the same city with the same code, so one made before is read, not made again.
- 2048-2048 `CITY_PREFIX` (variable)
- 2049-2049 `IfStatement` (statement)
- 2051-2051 `ExpressionStatement` (statement): The next new cities made ahead from the start (`cityAhead`), once the boot's work is behind.
- 2053-2053 `KeptCity` (interface): A kept city: its document without buildings and its buildings' table (`prepareCity`), as the worker answered them.
- 2056-2076 `cityData` (function): The city's document made by the worker (the game's own generator), or read as an earlier session kept it (`CITY_PREFIX`); without a job queue (a test) it is made on the page, as before.
- 2079-2083 `openedOf` (function): A city's reply as the page opens it.
- 2090-2106 `networkBakeOf` (function): A document's network solved by a job worker (`networkBake.worker.ts`), as the page will restore it (`restoreInto`); null without a job queue (a test) or when the worker failed: the page then solves it itself. DO NOT solve a large city's eve
- 2109-2127 `cityJob` (function): The city's worker asked for a city (`priority`: the queue's, 0 first), its answer kept under `key` for the next opening of this seed.
- 2134-2134 `NEXT_CITY_KEY` (variable): THE NEXT NEW CITY MADE AHEAD: each size's next seed chosen and kept (`NEXT_CITY_KEY`), its city grown in the background once the game stands idle and kept (`cityJob`), so the menu's new city opens as a kept one (`newWorld`, `newLargeWorld` 
- 2135-2135 `citiesAhead` (variable)
- 2136-2145 `nextCitySeed` (function)
- 2146-2162 `cityAhead` (function)
- 2164-2166 `citiesAheadLater` (function): Both sizes' next cities made ahead, the large first, once the opening's work is behind (`cityAhead` waits for the world to stand). DO NOT wait a fixed 15 s more: on the first launch the empty map stands in seconds, and the player's first la
- 2169-2173 `openCityOffPage` (function): A city made off the page and put in place of the map: the curtain over the map while its worker builds it, then the real map opened whole. One at a time, in the order asked.
- 2175-2200 `openCityNow` (function)
- 2203-2205 `openSmallCity` (function): The small city of a seed put in place of the map (the menu's, and the probes' with a seed of their own).
- 2208-2210 `openLargeCity` (function): The large city of a seed put in place of the map (the menu's, and the probes').
- 2213-2227 `openCity` (function): `table`: its buildings' table, prepared by the city's worker (`editor/preparedCity.ts`).
- 2230-2240 `fitOpeningView` (function): The view a large city opens at: a district round its middle, in the detail its roads and buildings are drawn for; the whole map comes as the camera moves, the rest of the world filled after the curtain.
- 2242-2242 `SKY_KEY` (variable): The sky: always day (the default), always night, or the residents' clock; kept.
- 2243-2243 `SKY_MODES` (variable)
- 2244-2248 `setSky` (function)
- 2249-2252 `TryStatement` (statement)
- 2253-2253 `ExpressionStatement` (statement)
- 2254-2254 `ExpressionStatement` (statement)
- 2257-2260 `saveMap` (function): DO NOT say "saved" without asking `exportToFile`
- 2263-2271 `openImported` (function): Loads a picked map.
- 2273-2293 `openSession` (function): A picked map's session put in place of the map.
- 2294-2296 `openMap` (function)
- 2299-2299 `PERSPECTIVE_KEY` (variable): Perspective or the isometric (orthographic) view, the player's choice, kept.
- 2300-2303 `setPerspective` (function)
- 2305-2307 `showPerspective` (function): The camera as the game's state has it.
- 2308-2308 `ExpressionStatement` (statement)
- 2310-2315 `ExpressionStatement` (statement): At boot, once the whole file has run: switching the camera asks for a frame, and the frame loop is set up further down.
- 2318-2318 `TILT_STEP` (variable): The camera menu's buttons: a step per press; north and reset put the camera back (reset frames the map too).
- 2319-2332 `cameraCommand` (function)
- 2335-2343 `undo` (function): An undo can change something far off screen, so the game says it happened.
- 2344-2351 `redo` (function)
- 2353-2363 `afterRestore` (function): The game after an undo or a redo put records back (`editor/history.ts`): the network rebuilt only when the roads moved
- 2366-2369 `updateHistoryButtons` (function): Whether there is something to undo and to redo, in the game's state.
- 2372-2391 `restoreSettings` (function): Restores the user-visible state stored beside a map without touching topology.
- 2392-2392 `ExpressionStatement` (statement)
- 2395-2399 `closeInspectorPanel` (function): The road inspector closed (its × , Escape, Delete): nothing picked.
- 2400-2400 `ExpressionStatement` (statement)
- 2402-2404 `closeInspector` (function): Closes the road inspector's panel (`ui/inspector.ts`).
- 2406-2406 `ExpressionStatement` (statement): Open or closed, by a command or by itself (what it showed is gone): in the game's state.
- 2409-2417 `CONTROL_CYCLE` (variable): Junction control, as a tool rather than as a form field.
- 2419-2437 `cycleNodeControl` (function)
- 2440-2440 `hintListeners` (variable): Who shows the tool's hint and the game's answers (the interface, `onHint`).
- 2442-2442 `earlyFlashes` (variable): The answers said before anyone listened (a map that failed to open at boot): told to the first listener.
- 2444-2448 `flashHint` (function): A transient answer of the game - saved, refused, undone, a road that cannot be laid
- 2451-2458 `hintKey` (function): The tool's hint: what a click does now.
- 2460-2460 `hintShown` (variable)
- 2461-2466 `updateHint` (function)
- 2468-2473 `listenHint` (function): A listener of the hint: told the hint now, and any answer said before it came.
- 2474-2474 `IfStatement` (statement)
- 2477-2477 `TERRAIN_KEY_MODES` (variable): The terrain brush's operations in the number row's order (1 = raise).
- 2479-2555 `gameKeys` (variable): Every key the game answers (`editor/gameKeys.ts`): one command a key.
- 2556-2563 `ExpressionStatement` (statement)
- 2564-2564 `ExpressionStatement` (statement)
- 2566-2566 `ExpressionStatement` (statement): A key let go elsewhere never comes up here: nothing stays held.
- 2570-2570 `frameClock` (variable): The frame asked for, and the time between frames (`frameLoop.ts`).
- 2571-2571 `ExpressionStatement` (statement)
- 2572-2572 `ExpressionStatement` (statement)
- 2574-2574 `lab` (variable): The debugging lab (F7, `lab/lab.ts`): the switches, what each system costs, the code that runs, recordings and tests.
- 2576-2576 `ExpressionStatement` (statement): For the probes and for an AI reading the game: the lab's context (events, frames, every switch).
- 2578-2578 `flatCameraDue` (variable): The flat camera kept up with the view (what the tools measure their pixels with): ten times a second.
- 2580-2580 `panelsDue` (variable): The status bar, the inspector, the simulation's checks.
- 2582-2582 `zonesDue` (variable): The conflict zones measured since, kept for the next session (`keptZones`).
- 2584-2584 `actionsWarm` (variable): The Actions' physics asked for (`frame`).
- 2587-2587 `topologyAfterDraw` (variable): Set by an edit: draw the new geometry first, rebuild the simulation after.
- 2589-2591 `requestDraw` (function)
- 2594-2594 `growthDue` (variable): DO NOT go back to a `setInterval` on the wall clock.
- 2596-2596 `zoneGrowthHold` (variable): Nothing grows before this simulation time (rubble of a collapse lying, `strikeAt`); `at` the time it was set.
- 2597-2606 `growZones` (function)
- 2610-2610 `waitingForTab` (variable): Waiting for another tab to let this world go (`openStreamedWorld`): the curtain stays up, saying so.
- 2611-2611 `loadingCurtain` (variable)
- 2612-2612 `ExpressionStatement` (statement)
- 2613-2613 `ExpressionStatement` (statement)
- 2614-2614 `ExpressionStatement` (statement)
- 2615-2616 `ExpressionStatement` (statement)
- 2617-2617 `ExpressionStatement` (statement)
- 2618-2618 `ExpressionStatement` (statement)
- 2620-2620 `programsWaited` (variable): The opening's programs the curtain waits for (`SceneHandle.programsReady`), and whether they are compiled.
- 2621-2621 `programsCompiled` (variable)
- 2622-2637 `syncLoadingCurtain` (function)
- 2639-2639 `drawnOnce` (variable): A frame has been drawn, and the first world has been put in place (the opening builds it in parts).
- 2641-2641 `frameTimes` (variable): Every frame's whole time, the last 40 s (`sampleRing.ts`): the CPU the process spends a frame, sampled while it runs (`__roadcraft.usage`).
- 2642-2642 `worldShown` (variable)
- 2644-2745 `frame` (function): @perFrame One frame: input, the topology catching up, the simulation, the drawing, the panels.
- 2748-2750 `simShowsMotion` (function): Whether anything the simulation moves can be seen moving: vehicles, cars parking, people, signals, public transport.
- 2755-2755 `SCREEN_REACH` (variable): How far from the view's centre the screen's ground is read, each way: the far ground's ring (`flatAtlas.ts`, 7 plates), the world having no edge to stop at.
- 2757-2760 `screenReach` (function): The square round the view's centre the screen's ground is held to.
- 2762-2780 `groundOnScreen` (function): The world's rectangle the screen can show
- 2783-2783 `BARRIER_PICK_PIXELS` (variable): Pick radius for a barrier under a shift-click, screen pixels.
- 2786-2805 `drawBarrierPlan` (function): The run being traced (`editor/barriers.ts` `BarrierTool.plan`), as it will stand: red where it cannot be built.
- 2808-2847 `drawStreetscapeHover` (function): Where the landscaping tool would put its item: a ring on the footway, red where it cannot go.
- 2849-2898 `drawPolePlan` (function)
- 2901-2901 `showChanges` (variable): F8: the diary's last changes drawn where they happened (`world/changes.ts`).
- 2903-2903 `CHANGE_SHOWN` (variable): How long a change stays drawn, ms.
- 2904-2909 `CHANGE_COLOURS` (variable)
- 2912-2940 `drawChanges` (function): The rectangles of the changes of the last seconds, each in its kind's colour and labelled with what it was and why
- 2942-3410 `drawOverlayScreen` (function)
- 3413-3419 `BUILD_FILL` (variable): How each stretch of the road in hand will be built, drawn between its deck and the natural ground
- 3420-3420 `PIER_EVERY` (variable)
- 3421-3459 `drawBuildModes` (function)
- 3461-3475 `drawTunnelStretches` (function)
- 3478-3486 `buildSummary` (function): The ways the road in hand is built, beside its length: "ponte 120 m · aterro 40 m".
- 3489-3505 `TERRAIN_BRUSH_COLOUR` (variable): The brush's colour, by what it does to the ground.
- 3507-3523 `TERRAIN_BRUSH_FILL` (variable)
- 3526-3534 `CONTROL_GLYPH` (variable): One glyph and one colour per control mode.
- 3536-3544 `CONTROL_COLOUR` (variable)
- 3546-3571 `setNodeHeightMetres` (function)
- 3573-3818 `showInspector` (function)
- 3821-3832 `duplicateSelectedSegment` (function): Duplicates the inspected road and keeps the copy selected for immediate editing.
- 3835-3835 `statusListeners` (variable): Who shows the city's numbers (the interface, `onStatus`).
- 3837-3856 `currentStatus` (function): The city's numbers now: the clock, the traffic, the money, the junctions that cannot be built.
- 3858-3862 `updateStatus` (function): Tells the interface the city's numbers (a few times a second while the city runs, and after an edit).
- 3865-3869 `desktopShown` (variable): The desktop window minimized or hidden (`desktopShown.ts`): drawn no more and saved, as a hidden tab; drawn again when back.
- 3871-3873 `pageHidden` (function): Nobody can see the game: the tab hidden, or the desktop window minimized or hidden.
- 3875-3881 `ExpressionStatement` (statement)
- 3884-3884 `CLOSE_WAIT_MS` (variable): Closing the game saves it.
- 3885-3898 `saveOnClose` (function)
- 3899-3899 `ExpressionStatement` (statement)
- 3901-3903 `ExpressionStatement` (statement): A hidden or discarded tab may never see `beforeunload` (phones, tab freezing).
- 3904-3904 `ExpressionStatement` (statement)
- 3907-3907 `saveWarnedAt` (variable): A failed write says so, at most once a minute, instead of losing the map at the next reload in silence.
- 3908-3913 `ExpressionStatement` (statement)
- 3915-3915 `ExpressionStatement` (statement)
- 3918-3918 `ExpressionStatement` (statement): The flat camera put where the view is before the first frame
- 3923-3930 `ExpressionStatement` (statement): Anything rendered from script rather than from markup has to be re-rendered when the language changes
- 3933-3941 `setMarkingStyle` (function): The map's road paint (`world/roads/markingStyle.ts`)
- 3943-3943 `ExpressionStatement` (statement)
- 3944-3946 `setQuality` (function)
- 3948-3952 `IfStatement` (statement): The game's state in the console (`core/gameState.ts`): `__state()` its values now and the last changes
- 3954-3954 `IfStatement` (statement): The game's health in the console (`core/health.ts`): what broke or was slow, newest first.
- 3956-3957 `IfStatement` (statement): The last frames per system (`core/health.ts` `frameStats`)
- 3960-3966 `gpuLines` (function): The F9 monitor's GPU lines (state tab, the whole width)
- 3967-3972 `IfStatement` (statement)
- 3975-3975 `issuesSeen` (variable): The simulation's own checks (`sim/invariants.ts`, run every 60 ticks) in the health log
- 3976-3984 `noteSimulationIssues` (function)
- 3986-3986 `valueText` (function)
- 3987-4002 `healthPanel` (variable)
- 4004-4019 `IfStatement` (statement): The diary of changes in the console (`world/changes.ts`): `__changes()` lists the last ones
- 4022-4024 `compiledUniforms` (function): The uniforms the renderer compiled a material with: `onBeforeCompile` puts textures there that are in no field of the material (report TST-09).
- 4027-4033 `pacedFps` (function): Frames a second of the last 2 s, from the frames' own start times; null when the game did not draw every refresh in them (the loop on demand: a smoothed 1/delta then told how often it drew, not how fast it could - report UI-15 d).
- 4036-4231 `IfStatement` (statement): Diagnostic surface for browser-driven checks.
- 4234-4243 `densify` (function): A polyline with points added so no step is longer than `step`.
- 4247-4250 `refreshShell` (function): Asks the interface to redraw its panels (a state it shows changed in the game).
- 4253-4253 `decayLook` (variable): Buildings run down without maintenance (`world/buildings/decayWatch.ts`)
- 4254-4254 `decayWatch` (function)
- 4256-4256 `decayDue` (variable): The buildings' decay looked at every 3 s of the city's time (`frame`), not on the wall clock.
- 4259-4274 `actions` (variable): The Actions (`actionsWiring.ts`): the pistol, the bomb, the buildings it breaks and the fires it leaves.
- 4277-4277 `ExpressionStatement` (statement): Everything is set up: the first frame may be drawn.
- 4278-4278 `ExpressionStatement` (statement)
- 4282-4288 `IfStatement` (statement): autosave holds. The probes' own `generateCity` asks the same (`openSmallCity`).
