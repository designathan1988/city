// The audit's rule catalog: each rule pairs a wrong methodology the graph can detect with the correct methodology,
// its counterpart in the target (Godot + C++) architecture, and the primary sources that establish both. A rule
// without a source does not enter the catalog.

export interface Source {
  readonly title: string;
  readonly url: string;
}

export interface Rule {
  readonly id: string;
  readonly title: string;
  readonly group: 'architecture' | 'state' | 'performance' | 'determinism';
  readonly problem: string;
  readonly methodology: string;
  readonly target: string;
  readonly sources: readonly Source[];
}

const S = {
  parnas: { title: 'Parnas, On the Criteria to Be Used in Decomposing Systems into Modules (CACM 1972)', url: 'https://dl.acm.org/doi/10.1145/361598.361623' },
  eaddy: { title: 'Eaddy et al., Do Crosscutting Concerns Cause Defects? (IEEE TSE 2008)', url: 'https://thomas-zimmermann.com/publications/files/eaddy-tse-2008.pdf' },
  redux: { title: 'Redux, Three Principles (single source of truth, read-only state, changes by actions)', url: 'https://redux.js.org/understanding/thinking-in-redux/three-principles' },
  command: { title: 'Nystrom, Game Programming Patterns: Command', url: 'https://gameprogrammingpatterns.com/command.html' },
  ccfinder: { title: 'Kamiya, Kusumoto, Inoue, CCFinder: token-based code clone detection (IEEE TSE 2002)', url: 'https://sel.ist.osaka-u.ac.jp/lab-db/en/betuzuri/contents/387/' },
  winnowing: { title: 'Schleimer, Wilkerson, Aiken, Winnowing: Local Algorithms for Document Fingerprinting (SIGMOD 2003)', url: 'https://theory.stanford.edu/~aiken/publications/papers/sigmod03.pdf' },
  fiedler: { title: 'Fiedler, Fix Your Timestep!', url: 'https://gafferongames.com/post/fix_your_timestep/' },
  gameLoop: { title: 'Nystrom, Game Programming Patterns: Game Loop (fixed update time step, variable rendering)', url: 'https://gameprogrammingpatterns.com/game-loop.html' },
  shift: { title: 'ECMAScript, Array.prototype.shift (every remaining element is moved one index down)', url: 'https://tc39.es/ecma262/multipage/indexed-collections.html#sec-array.prototype.shift' },
  fastProps: { title: 'V8, Fast properties in V8 (delete and dynamic keys send objects to dictionary mode)', url: 'https://v8.dev/blog/fast-properties' },
  elements: { title: 'V8, Elements kinds in V8 (new Array(n) is holey forever; typed arrays for numbers)', url: 'https://v8.dev/blog/elements-kinds' },
  spatial: { title: 'Nystrom, Game Programming Patterns: Spatial Partition', url: 'https://gameprogrammingpatterns.com/spatial-partition.html' },
  teschner: { title: 'Teschner et al., Optimized Spatial Hashing for Collision Detection of Deformable Objects (VMV 2003)', url: 'https://cgweb.informatik.uni-freiburg.de/publications/2003_VMV_collisionDetectionHashing.pdf' },
  webgl: { title: 'MDN, WebGL best practices (synchronous calls stall; batch draw calls)', url: 'https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/WebGL_best_practices' },
  servers: { title: 'Godot, Optimization using Servers (RIDs freed manually; value-returning calls stall the server)', url: 'https://docs.godotengine.org/en/stable/tutorials/performance/using_servers.html' },
  transfer: { title: 'MDN, Transferable objects', url: 'https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Transferable_objects' },
  pool: { title: 'Nystrom, Game Programming Patterns: Object Pool', url: 'https://gameprogrammingpatterns.com/object-pool.html' },
  locality: { title: 'Nystrom, Game Programming Patterns: Data Locality', url: 'https://gameprogrammingpatterns.com/data-locality.html' },
  gc: { title: 'V8, Trash talk: the Orinoco garbage collector (cost scales with surviving objects)', url: 'https://v8.dev/blog/trash-talk' },
  threeLots: { title: 'three.js manual, Optimize Lots of Objects (each Mesh is a draw request; merge or instance)', url: 'https://github.com/mrdoob/three.js/blob/dev/manual/pages/optimize-lots-of-objects.html' },
  threeDispose: { title: 'three.js manual, How to dispose of Objects (GPU resources are not freed automatically)', url: 'https://github.com/mrdoob/three.js/blob/dev/manual/pages/how-to-dispose-of-objects.html' },
  multimesh: { title: 'Godot, Optimization using MultiMeshes', url: 'https://docs.godotengine.org/en/stable/tutorials/performance/using_multimesh.html' },
} as const;

export const RULES: readonly Rule[] = [
  {
    id: 'god-file', group: 'architecture', title: 'File larger than the target architecture allows',
    problem: 'One file holds several responsibilities, so a change to one concern touches code of others.',
    methodology: 'Decompose by responsibility (each module hides one design decision); one owner per concept.',
    target: 'Split along the core → sim → render → api layers; each file under the target rules\' max_lines.',
    sources: [S.parnas],
  },
  {
    id: 'god-function', group: 'architecture', title: 'Function longer than the target architecture allows',
    problem: 'A function doing several things cannot be owned, tested or converted as one responsibility.',
    methodology: 'Extract each step into a function with one responsibility, owned by the module of its concept.',
    target: 'Each C++ function under the target rules\' max_function_lines.',
    sources: [S.parnas],
  },
  {
    id: 'module-state', group: 'state', title: 'Mutable module-level variable: state without an owner',
    problem: 'Any code that imports the module can change it; nothing serialises, undoes or observes the change.',
    methodology: 'Keep state in one store (single source of truth), read-only to everyone else, changed only by actions.',
    target: 'A field of CityState (or the owning subsystem\'s state struct), mutated only by sim/commands or sim/simulation.',
    sources: [S.redux, S.command],
  },
  {
    id: 'scattered-writes', group: 'state', title: 'State written from many files',
    problem: 'The same state is mutated from several modules: no single owner, invariants enforced nowhere.',
    methodology: 'One owner mutates the state; everyone else sends it a command or action.',
    target: 'One mutator in sim/commands (journalled for undo) or the simulation step; others dispatch.',
    sources: [S.redux, S.command, S.eaddy],
  },
  {
    id: 'ui-writes-state', group: 'state', title: 'Interface code writes game state directly',
    problem: 'The UI holds or mutates the game\'s numbers, so the simulation is not the source of truth.',
    methodology: 'State is read-only to views; the only way to change it is to dispatch an action.',
    target: 'UI reads CityStore.snapshot and calls CityStore.dispatch (target gate ESTADO_NA_UI).',
    sources: [S.redux],
  },
  {
    id: 'duplicated-code', group: 'architecture', title: 'Identical code in two places: no single owner',
    problem: 'Two copies drift apart; a fix in one is missing from the other.',
    methodology: 'Extract the shared code into one owner and call it from both places.',
    target: 'One function in the owning module (target gate CODIGO_DUPLICADO, 80 identical tokens).',
    sources: [S.ccfinder, S.winnowing],
  },
  {
    id: 'duplicated-constant', group: 'architecture', title: 'The same tuning value typed in many files',
    problem: 'A value with one meaning lives in several places; changing it misses some of them.',
    methodology: 'Give the value one owner: a named constant in the owning module or a key in the data tables.',
    target: 'A key in project/data/** read through DataTables, or a named constexpr in the owner (gates NUMERO_MAGICO, VALOR_DUPLICADO).',
    sources: [S.parnas, S.eaddy],
  },
  {
    id: 'feature-scattering', group: 'architecture', title: 'A feature spread across many files',
    problem: 'Implementing or changing the feature means finding and editing many unrelated places; scattering correlates with defects.',
    methodology: 'Concentrate each feature in the modules that own it (low degree of scattering).',
    target: 'The feature\'s code in its owner\'s files; the modularize stage minimises its DOSC.',
    sources: [S.eaddy],
  },
  {
    id: 'nondeterminism-in-simulation', group: 'determinism', title: 'Simulation depends on wall-clock time or unseeded randomness',
    problem: 'Runs cannot be reproduced, replayed or compared; results depend on frame rate.',
    methodology: 'Fixed timestep driven by an accumulator, seeded random streams owned by the simulation.',
    target: 'sim/simulation fixed tick (sim_tick_hz) and core/random.h seeded generators.',
    sources: [S.fiedler, S.gameLoop],
  },
  {
    id: 'queue-shift', group: 'performance', title: 'Queue drained with Array.shift() in a loop',
    problem: 'shift() moves every remaining element down one index, so draining n items costs O(n²).',
    methodology: 'A ring buffer or a head index into the array (amortised O(1) dequeue).',
    target: 'A ring buffer or std::deque / index-based queue in C++.',
    sources: [S.shift],
  },
  {
    id: 'object-delete', group: 'performance', title: 'Properties removed with delete',
    problem: 'Adding and deleting properties sends objects to dictionary mode, where inline caches stop working.',
    methodology: 'A Map for dynamic keys, or a fixed shape with the slot set to undefined.',
    target: 'A struct with fixed fields or an indexed container; no dynamic members.',
    sources: [S.fastProps],
  },
  {
    id: 'holey-array', group: 'performance', title: 'Array preallocated with new Array(n)',
    problem: 'new Array(n) creates a holey array that stays holey after it is filled, on slower element paths.',
    methodology: 'An array literal filled with push, or a typed array of length n for numbers.',
    target: 'std::vector reserved or sized once.',
    sources: [S.elements],
  },
  {
    id: 'quadratic-proximity', group: 'performance', title: 'All-pairs distance search in nested loops',
    problem: 'Comparing every object with every other is O(n²) and dominates as the city grows.',
    methodology: 'A spatial partition: uniform grid or spatial hash, querying only nearby cells.',
    target: 'core/grid.h SpatialHash (the target\'s single spatial index).',
    sources: [S.spatial, S.teschner],
  },
  {
    id: 'sync-gpu-readback', group: 'performance', title: 'Synchronous GPU query on the calling thread',
    problem: 'readPixels, getError, get*Parameter and similar calls flush the pipeline and wait for the GPU.',
    methodology: 'Avoid them in production; use asynchronous readback (pixel buffer plus fence).',
    target: 'No value-returning RenderingServer calls per frame; keep CPU-side copies of what is needed.',
    sources: [S.webgl, S.servers],
  },
  {
    id: 'worker-copy', group: 'performance', title: 'Data sent to a worker without a transfer list',
    problem: 'postMessage without transferables clones the payload; big typed arrays are copied every time.',
    methodology: 'Transfer the ArrayBuffers (or share memory) instead of cloning them.',
    target: 'Threads share memory: PoseBuffer triple buffer and ChangeLog, no copies.',
    sources: [S.transfer],
  },
  {
    id: 'hot-allocation', group: 'performance', title: 'Allocation inside loops on the per-frame or per-tick path',
    problem: 'Objects allocated every frame that survive cost collector time; in C++ each is a heap allocation per tick.',
    methodology: 'Preallocate and reuse buffers, pool objects that are created and destroyed often, keep data contiguous.',
    target: 'Zero heap allocation in the tick: arenas (RouteArena), reused std::vector, contiguous structs.',
    sources: [S.pool, S.locality, S.gc],
  },
  {
    id: 'per-object-draw', group: 'performance', title: 'One mesh object created per item in a loop',
    problem: 'Each Mesh is at least one draw request; thousands of them make the CPU the bottleneck.',
    methodology: 'Merge static geometry or draw identical items with instancing.',
    target: 'MultiMesh (one draw for many instances), RenderingServer instances, merged chunk meshes.',
    sources: [S.threeLots, S.webgl, S.multimesh],
  },
  {
    id: 'undisposed-gpu-resource', group: 'performance', title: 'GPU resources created but never disposed in the file',
    problem: 'Geometries, materials, textures and render targets keep GPU memory until dispose() is called.',
    methodology: 'Dispose every GPU resource when it is no longer used (one owner releases it).',
    target: 'Every RID created through the RenderingServer is freed by its owner.',
    sources: [S.threeDispose, S.servers],
  },
];

export const RULE_BY_ID = new Map(RULES.map((rule) => [rule.id, rule]));
