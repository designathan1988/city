# Feature tracer: research before finishing (owner's order, 2026-11-11 evening)

The owner's order: research on the internet until the solution of every problem the tracer has met is found and the
whole objective is proven feasible; record every finding here as it is found, with quick bounded scripts that verify
it; only then go on. Nothing in this file is deleted: findings are appended, a wrong one is marked wrong with why.

The objective: one generic tracer that follows the complete flow of any feature through GDScript (with scenes and
config), C++ (GDExtension included) and JavaScript/TypeScript (JSX, TSX), from the languages' rules and the platforms'
official declarations only, converging (finite, fast enough to run in seconds to a few minutes), never naming a program.

## The problems met so far

- P1. Processes: a run of the new core on one lab file ran for minutes with no output and was moved to the background;
  the owner forbids unwatched, slow or heavy processes and a process blocking another.
- P2. The old core never converged: hand rules per case (51 tables of names), measurement used as specification.
- P3. Unbounded abstractions: closures' templates without end (C4''); Window's index signature making one declared
  object per level (RecursionError); risk: allocation sites built from call nodes that carry objects.
- P4. The solver applies a subscription's transfer recursively inside `subscribe`: the stack reached 4,323 frames on
  the lab (probe_stack.py, 2026-11-11 17:40), cycling structural -> callback -> consume -> taken -> structural.
- P5. Structural consumption of a program object as a declared type reads every member the type declares (hundreds in
  the DOM) and is redone per call site, environment and site path.
- P6. The platform's behaviour: declarations say types, not effects (what dispatchEvent calls, what a library stores).
- P7. Events and listeners: what calls a listener added with addEventListener.
- P8. JSX: factories, runtimes, children.
- P9. Grammar gaps of tree-sitter-typescript 0.23.2 (global augmentation in ambient modules).
- P10. Acceptance: how to compare the new core's call graph with the old one and with the truth.
- P11. GDScript and Godot's engine API for Task 8; P12. C++ (virtual calls, function pointers, templates) for Task 9.

## Findings

### F1. The solver's discipline (P4) — 2026-11-11 17:50
- Pearce, Kelly, Hankin, "Online cycle detection and difference propagation for pointer analysis", SCAM 2003
  (https://whileydave.com/publications/PKH03_SCAM_preprint.pdf): Andersen's analysis as set constraints solved by a
  worklist; difference propagation pushes only the change of each node's set. Every unit of work is a worklist item:
  nothing is applied recursively.
- Hardekopf, Lin, "The ant and the grasshopper", PLDI 2007 (https://cs.utexas.edu/~lin/papers/pldi07.pdf): lazy and
  hybrid cycle detection collapse nodes with identical sets, the fastest variant they studied. Pereira and Berlin's
  wave propagation (CGO 2009) orders propagation topologically.
- What it says of this tracer: `heap/solver.py` applies a new subscription's transfer to the source's current objects
  inside `subscribe`, so wiring that subscribes recursively nests Python frames (4,323 on the lab). The cause-level fix
  is the worklist discipline: a new subscription's first application is a queued item like any difference.

### F2. How the analyzers that scale treat JavaScript (P5, P6) — 2026-11-11 17:55
- Jelly (Aarhus, https://github.com/cs-au-dk/jelly): "Its core is a flow-insensitive control-flow and points-to
  analysis, together with access paths for tracking library usage"; it treats the Node.js standard library as unknown
  code; with all dependencies "can take a long time", and offers indirection bounding (`--max-indirections`) or a
  timeout for partial results; it compares call graphs (static against dynamic, via NodeProf) for precision and
  recall. Its design comes from JAM (modular call graphs, ISSTA 2021), TAPIR (OOPSLA 2020) and ACG (ICSE 2013).
- Averroes (Ali, Lhotak, ECOOP 2013, https://plg.uwaterloo.ca/~olhotak/pubs/ecoop13.pdf): a placeholder library that
  over-approximates the library; call graphs as sound as with the whole library, 4.3x-12x faster; merging the library's
  pointers costs precision (a later study: 59% of the whole-program precision on average).
- TAJS's DOM model (Jensen, Madsen, Moller, ESEC/FSE 2011, https://cs.au.dk/~amoeller/papers/dom): event handlers are
  collected as registered and triggered non-deterministically by the event loop (a hand model of ~250 abstract objects,
  500 properties, 200 functions).
- What it says of this tracer: the analyzers that finish on real programs are context-insensitive at the core and model
  the library either by hand (forbidden here) or as unknown code with access paths and callbacks assumed called. None
  enumerates every member of a declared type on every object handed to it: that is this tracer's design error (P5).

### F3. What keeps an analysis of real programs inside a budget (P1, P3, P5) — 2026-11-11 18:05
- Zipper (Li, Tan, Moller, Smaragdakis, OOPSLA 2018, https://cs.au.dk/~amoeller/papers/zipper): a fast
  context-insensitive pre-analysis builds a precision flow graph; the methods through which objects flow in and back out
  (direct, wrapped, unwrapped flows) are the precision-critical ones (38% on average); 2-object sensitivity applied only
  to them keeps essentially all its precision and runs much faster. Finding them is graph reachability, negligible time.
- Introspective analysis (Smaragdakis, Kastrinis, Balatsouras, PLDI 2014) and Scaler (TOPLAS 2020, "scalability-first
  pointer analysis with self-tuning context-sensitivity"): context sensitivity chosen per method from a cheap first pass;
  Scaler's aim is to finish any program within a given budget.
- Indirection-bounded call graphs (Chakraborty, Gnanakumar, Sridharan, Moller, ECOOP 2024,
  https://cs.au.dk/~amoeller/papers/bounded): code has few levels of indirection; a wave propagation that follows the
  levels finds the true edges first; bounding the levels gave ~2x speed for 5% recall.
- ACG (Feldthaus et al., ICSE 2013): a field-based flow analysis builds approximate call graphs fast enough for an IDE;
  "Static JavaScript Call Graphs: A Comparative Study" (SCAM 2018) and Jelly's `--compare-callgraphs` measure precision
  and recall against dynamic call graphs (the truth a static graph is judged by).
- What it says of this tracer: context sensitivity everywhere (Task 3's 1-call-site and 2-object with heap context on
  every function) is the cost the literature avoids; the costliest correct design is the two-phase one — a
  context-insensitive run, then contexts only for the precision-critical functions its flow graph names — and every run
  reports progress and finishes inside a budget it is given (Scaler's discipline, the owner's order on processes).

### F4. The evidence on the lab (P4, P5) — probe_stack.py, 2026-11-11 17:40
- A watchdog printing the analysing thread's stack every 6 s: depth 2,409 -> 4,323 -> 2,970 -> 1,974, the frames
  repeated most `solver.subscribe` (1,070), `values.consume` (539), `declared.taken` (536), `declared.callback` (535),
  `declared.structural` (533): one object given to a declared type is consumed structurally — every member the type
  declares read on it, each method called back, each result consumed into its declared type, structurally again — all
  inside `subscribe`'s immediate application. The solver's step counter never advanced past the first item's wiring.
- Causes: (a) the solver applies transfers recursively (F1); (b) structural consumption reads the type's members (the
  DOM's hundreds), not the members the object has, and is keyed per call, environment and site, so the same (object,
  type) pair is consumed again and again; (c) a callback's call node carries its environment's objects into the static
  site an object made under it is keyed by (a promise chain through async callbacks would nest without end).

### F5. Godot's engine API, for GDScript (P11) — 2026-11-11 18:15 (context7 /godotengine/godot-docs; local check)
- `godot --dump-extension-api` writes `extension_api.json`, the engine API for GDExtension bindings (command-line
  reference). The repository has the dumps of 4.3 to 4.8 in `native/godot-cpp/gdextension/`; 4.7: 1,036 classes, 41
  singletons. A class: name, inherits, is_instantiable, is_refcounted, api_type, methods, properties, signals, enums,
  constants. A method: name, is_const, is_vararg, is_static, is_virtual, arguments (name, type, default_value, meta),
  return_value (type); a property: type, name, setter, getter; a signal: name, arguments. Builtin classes (38: Array,
  String, Callable, Signal...) and 114 utility functions are declared the same way.
- Verified (a 1-second script): Node's virtual methods are declared (`_process`, `_physics_process`, `_enter_tree`,
  `_exit_tree`, `_ready`, `_input`, ...); Object declares `connect(signal: StringName, callable: Callable)`,
  `emit_signal`, `call(method: StringName, ...vararg) -> Variant`, `call_deferred`, `set`, `get`.
- What it says of this tracer: the engine's callbacks are the methods the API declares virtual (the old core's
  `ENGINE_CALLBACKS` list is replaced by the declaration); `connect` and `emit_signal` on one object are the platform's
  kept state (a callable kept, called back by an emit with the arguments its parameters admit) — the same generic rule
  as addEventListener and dispatchEvent; `call(method)` is reflection by name on the receiver (Task 5's summary must
  count the receiver among the objects a string may name a member of); `Variant` is an open type.

### F6. An independent reference for JavaScript, measured (P10) — 2026-11-11 18:25
- Jelly 0.13.0 is installed at `C:/Projects/toolchains/jelly` (`node .../@cs-au-dk/jelly/lib/main.js`). On the web
  fixture: 63 ms, 27 functions, 40 calls, 28 call->function edges, its JSON call graph (`files`, `functions`, `calls`,
  `fun2fun`, `call2fun`). On the lab's `src` with every dependency: a V8 fatal error after 42 s (its README: raise
  `--max-old-space-size`). With `--ignore-dependencies` and a 6 GB heap: 22.3 s, 3,289 MB, 19,725 functions, 99,094
  calls, 58,087 call->function edges, 39.8% of the calls to native or external code, 77.8% of the functions reachable.
- What it says: an independent call graph of the lab exists to measure the new core's recall and precision against
  (Task 7), as Jelly's own `--compare-callgraphs` does; and a whole-program, context-insensitive analysis in optimized
  V8 needs 22 s and 3.3 GB for the lab's own code: a whole-program fixpoint in Python, with contexts, on every trace
  cannot be fast. A trace asks about one feature: the analysis must answer that question, not solve the program.

### F7. Answering one query instead of solving the program (P1, P10) — 2026-11-11 18:30
- Demand-driven points-to analysis performs "only the work necessary to answer a query": Heintze and Tardieu (PLDI
  2001, the first on-demand Andersen analysis, built to compute call graphs of C with function pointers); Sridharan,
  Gopan, Shan, Bodik (OOPSLA 2005) and Sridharan, Bodik (PLDI 2006, refinement-based, context sensitivity refined per
  query) as CFL-reachability; Boomerang (Spath, Nguyen Quang Do, Ali, Bodden, ECOOP 2016,
  https://drops.dagstuhl.de/entities/document/10.4230/LIPIcs.ECOOP.2016.22), flow-, field- and context-sensitive on
  demand.
- Reuse across runs: per-function summaries computed once and applied at every call site (diff graphs, ICOOOLPS 2017);
  POLYMER (OOPSLA 2016) reuses facts of shared code across programs: 2.6x faster call graphs, 5.2x points-to.
- What it says of this tracer: a trace is a query (what a feature runs, what reaches it). Two costs must both be small:
  the work per query (demand: only the units the query reaches, the writes a read it needs can see) and the work shared
  by every query (the facts and the platform's declarations, read once and kept by content, as the readers' facts are).

### F8. The three causes fixed and verified (P3, P4, P5) — 2026-11-11 18:45
- The solver now follows the worklist discipline (F1): a new subscription's first application is a queued item; no
  transfer runs inside another. Structural consumption happens once per (object, type, environment), only for objects
  the program made (the platform's objects are its own), only on the members the object has, besides index signatures
  and call signatures. Every call's allocation site is an explicit (static site, heap context): a callback's node, which
  carries its environment's objects, is never a site.
- Verified (probe_growth.py, bounded, live): the lab's main.ts with its 130 declaration files, which ran for minutes
  without end, finishes in 2.1 s: 141,795 worklist items, 19,279 objects (7,530 declared, 3,160 declared functions),
  deepest object key nesting 14. The 71 heap tests pass.

### F9. The whole program does not fit; the query does (P1) — measured 2026-11-11 19:00
- Bounded probes on the lab's whole `src` (1,000 files with the declarations), 85 s each, live output:
  context-sensitive, 691,624 items, not finished, the rate falling (250,000 items in 8 s, the next 250,000 in 36 s;
  15,469 function objects cloned per context); context-insensitive (Zipper's pre-analysis), 334,847 items, not finished
  (46 s for 250,000: larger sets, heavier items). A sampling profile (probe_sample.py, 1,352 samples): `analysis.call`
  ~53% of the analysing thread, the trees' transfers ~34%, Task 5's generic summaries ~28% (reflection by name alone
  ~16%), `taken` ~17%. CPython does not cache a tuple's hash: a node keyed by a 12-deep tree hashes 8x slower than an
  int (a 1-second micro-benchmark), a cost, not the cause (~120 microseconds per item overall).
- With module code entered, a query from `LotTool.advanceProposal` is the whole program again: the lab's `main.ts`
  builds the whole application at module level. With the seed alone, 598 items, 8 units, 21 links in 0.0 s: too shallow,
  because the seed's `this`, its fields and its parameters are fed by code no forward walk runs.
- On-demand call graphs for JavaScript (SAS 2023, https://link.springer.com/10.1007/978-3-031-44245-2_20): start from an
  empty call graph, interleave backward data-flow queries, resolve call edges as needed; it explores a smaller part of
  the program. ACG's "optimistic DEMAND" propagates only along edges that may end at a call site. Doop on Souffle
  (Datalog compiled to C++) is an order of magnitude faster than a hand-built Java framework.
- What it says of this tracer: a trace must be a demand-driven query over the same inclusion analysis — forward from
  the seed as now, and every node that needs its sources (a parameter, a function's `this`, a field, a variable)
  finding them through static indexes (the call sites that may call the function, the writes of the member's name, the
  `new`s of the class) and wiring only those expressions, never whole units nor all module code.

### F10. What inflates the work, measured (P1, P5, P6) — 2026-11-11 19:20
- The same whole-lab probe, context-insensitive, with Task 5's pool removed and unknown code treated as Jelly treats
  it (what it is handed is called back, nothing spread into fields): 3,487,665 items in 85 s against 334,847 — ten
  times the work done in the same time; objects level off near 50,000 (converging); nodes ~890,000 still growing.
  The Averroes pool (every open argument's fields one level down, the callbacks' results, all spread into every open
  object's fields and called back with everything) makes every set large and every item heavy, and its precision is
  the literature's 59% (F2).
- The demand-driven prototype (probe_query.py): 5.1 M items in 85 s at ~60,000 per second, but pulling by name
  (callers of `get`, writes of common field names) drags in 12,789 units and 678,215 links; Jelly reaches 10,570 of
  19,725 functions from that same seed: a central feature's closure is half the program, so a query does not avoid
  the program's scale.
- A synthetic benchmark of Python against Node on the same worklist algorithm was inconclusive (the random graph formed
  one giant component: Python 5 M items, 16.5 M facts in 60 s; Node, with string field keys, ran out of its 4 GB):
  a runtime is no shortcut; the algorithm's work is the cost.

## Decision (2026-11-11 19:25): what the research proves the tracer must be

1. One inclusion analysis over the whole program, solved by the worklist discipline (F1), context-insensitive first,
   with contexts only for the functions a precision flow graph names (Zipper, F3) — the costliest correct design, not
   contexts everywhere.
2. The platform from its declarations as Task 4 reads them (precise where they type), and where they say nothing,
   Jelly's and JAM's treatment of unknown code (F2): what it is handed is called back with what its declared
   parameters give; a value given in an open position with a string naming a member reaches that member of the call's
   open object arguments (defineProperty's shape); a member a passed string names is read and called on the open
   arguments themselves (reflection); no pool spread into every field, no callback called with everything.
3. The platform's kept state only where a declaration pairs it: a function kept by a call on an object is called back
   by a later call on that object that hands no function and passes a value of a named type the kept function's
   declared parameter admits (an Event to an EventListener; a Signal's emit to its connected callables) — never by a
   type parameter's value (an array's push does not call its forEach callbacks again).
4. Speed by engineering the same algorithm: nodes interned to integers, cycles of identity edges collapsed (Hardekopf
   and Lin, F1), the facts and the solved analysis kept by the content of their inputs so a second trace reads them;
   every run prints its progress and finishes inside the budget it is given (Scaler, F3); acceptance against Jelly's
   call graph of the lab (F6) and the old core's links.

### F11. Narrowed summaries, measured (P5, P6) — 2026-11-11 19:50
- With the summaries of the Decision in the code (no Averroes pool; kept state paired by declared types), the
  whole-lab probe did 8.9 M items in 85 s; the window of its last 40 s (probe_tail.py): 4.1 M of 5.2 M items were
  reflection reads (`MEMBER_OF`) of one object each — `console.log("text", obj)`-shaped calls reading `obj["text"]`.
  Cause: any string in any position was taken for a member name. A string names a member only where its position is
  declared with a property-key type (ECMA-262: a key is a string or a symbol; TypeScript's `PropertyKey`, `keyof T`,
  a union of string literals), as `defineProperty(o, p: PropertyKey, ...)` and `Reflect.get(t, k: PropertyKey)`
  declare it; a plain `string` is text. After the fix: 4.9 M items in 85 s and the analysis reaches further (945,000
  nodes); the window is now the declared objects' production (`PRODUCED`, 565,000 items) and fields.
- Still open: the whole lab does not finish within 85 s; the next measured steps are the Decision's item 4 (nodes as
  integers, cycles of identity edges collapsed, the solved analysis kept by its inputs' content).

### F12. Nodes per receiver for what reads no receiver (P1) — 2026-11-11 20:05
- Nodes by kind at 55 s (probe_nodes.py): 416,626 `PRODUCED` nodes, 299,569 of them a primitive type (`: number`,
  `: string`) made again for every environment and site, 80,783 named types keyed by an environment they never read.
  Fix: the language's values are one node for all, a type parameter is its place itself, and a type with no type
  parameter and no `this` in it is keyed without the environment. `PRODUCED` fell to 29,434, all nodes at 63 s from
  ~990,000 to ~620,000; the rate of links did not change: the time is elsewhere.
- Cycles are not it (probe_scc.py): 1,257,113 identity edges, 8,536 of 462,290 nodes in cycles, the largest 1,505.

### F13. The facts are strings (P1) — 2026-11-11 20:10
- probe_facts.py at 51 s: 13,138,388 facts; 19,884 nodes hold more than 100 objects; the largest sets (1,100-1,363
  objects) are mostly known strings (`str`: 988 of 1,363; 1,165 of 1,166): every string literal is an object of its
  own, and all of them flow together through the program's text helpers, so every edge carries a thousand objects.
- What it says: a string's text matters only where it names a member (a computed key `o[k]`, a property-key
  argument); there it is known from the code around it (a literal, a constant initialized with one, a union of those),
  as the scalable analyzers do (ACG ignores dynamic keys; Jelly takes them from local constants and approximate
  interpretation's hints). In the heap a string is the language's string, one object; the texts a key position can
  have are read from its expression and its constants' definitions, locally.

### F14. Dictionary reads, enumerated keys, and the whole program reached (P1) — 2026-11-11 20:30
- probe_facts.py after F13: the largest sets were a generic helper's parameters (`value`, `args`: 590-613 objects of
  every kind) read with keys no text names, each such read reading every named member of every object it held. ACG
  and Jelly leave a read through an unknown key unresolved, and the old core's own T4' says an unknown key reads no
  property. A key no text names now reads the object as a dictionary (its elements and what unknown keys wrote), and
  a key `for...in` enumerated from an object (ECMA-262 EnumerateObjectProperties) reads that object's named members.
  `Object.keys(o)` stays unresolved: the lib declares `keys(o: object): string[]`, no link from the strings to `o`.
- Measured after: the analysis reaches the whole program — 31,662 units active (3,234 before), 344,765 links at 76 s
  (the old core: 223,604 on the lab app) — and does not finish in 85 s: 1.9 M items queued, 87,000 declared objects
  (ArrayBuffer 11,073, ArrayIterator 10,389, Map 8,859, Array 8,816...), 455,000 interned site identifiers.
- What it says: the analysis is now correct in shape and reaches what the old core reached and more; what remains is
  the throughput of CPython against a whole program (Jelly: the same reach in 22 s on V8, F6). The Decision's item 4
  is no longer about waste: it is about the runtime of the solver.

## State of the research (2026-11-11 20:35) — what is proven and what is not yet
- Proven, with bounded scripts: the hang's three causes and the four inflations (pool, reflection on any string, nodes
  per receiver, strings as objects, every-member reads) were the reasons the analysis never converged; each fix was
  measured; the analysis now reaches the whole lab and every expectation of the old core's JS tests except
  `Object.keys` (undeclared) holds on it; a declared platform (130 real lib files) is read and used.
- Not yet proven: a whole-program run inside a short budget. Measured throughput: CPython ~60,000-100,000 worklist items
  per second on this analysis; the lab needs several million. The options the research leaves: run the same solver on
  a faster runtime (PyPy, a download that needs the owner's permission; or the solver ported to V8/Node or C++, both
  present on this machine), and keep the solved analysis by its inputs' content so only the first run pays.

### F15. The cost is intrinsic to the whole program, and the runtime decides (P1) — 2026-11-11 20:50
- A fresh sampling profile (after F11-F14): no hot spot; the solver's loop is ~89% of the analysing thread, spread over
  interning nodes (hashing deep tuples), subscriptions, node creation, a linear `written` check, lookups.
- A part of the lab (`src/sim`, 91 files with the declarations) does not finish in 85 s either; its queue
  (probe_queue.py) is the intrinsic cost of a whole-program, context-insensitive analysis, not a loop: the return
  node of a function called from 6,414 sites gets ~5,800 items, each growth copied to 6,414 edges; the shared `number`
  node has 23,406 edges; member reads multiply by call sites times receivers. Each step costs CPython a fraction of a
  microsecond to microseconds, and there are tens of millions.
- Decision (owner's rule: the costliest solution that solves the cause; no download without the owner's permission):
  the analysis's semantics run in a native engine in C++ (the project's language; llvm-mingw and CMake 4.4 present),
  which takes the facts and the name resolutions Python computes, interns trees and types as integers (hash-consing),
  solves on integer worklists and returns the links; the Python semantics stay the specification and the oracle: the
  same tests run against the engine. Plan: log/trace-engine-plan.md.

### F16. The native path has no impediment (P1) — 2026-11-11 21:00
- llvm-mingw's clang++ builds a C++20 program in 1.1 s; linked `-static` it runs from PowerShell (a dynamic build
  misses the toolchain's libc++ outside its folder); Python 3.12's `include/Python.h` and `libs/python312.lib` are
  present for an extension module if the engine is one. CMake 4.4 is present.

### F17. The cyclic garbage collector, not the reading, was most of the tracer's Python time (P1) — 2026-11-11 21:40
- Measured (e1_read_cached.py, e1_split.py): the lab's read with the facts cache warm took 12.8 s; the same loads and
  the same index with the cyclic collector disabled take 2.2 s and 0.17 s. Cold (no cache): 32 s.
- Why (CPython InternalDocs/garbage_collector.md, "Collecting the oldest generation", via context7 /python/cpython;
  Doc/library/gc.rst): every `threshold0` allocations a young collection examines the new objects, and a full
  collection traverses every long-lived object once the pending ones pass 25% of them; the tracer builds millions of
  long-lived objects (facts, index, nodes, sets) that are never garbage during a run, so each is traversed several
  times for nothing. mypy, a whole-program analyser in CPython, met the same cost and changed its collector settings
  (python/mypy#18306: incremental check of torch 27% faster). The gc docs give `gc.disable()` and `gc.freeze()` (move
  every tracked object to a permanent generation that later collections ignore) for programs that build long-lived data.
- The whole lab on the new core with the collector disabled (probe_whole_gc.py, 85 s deadline): 195,623 links at 10 s
  (before: 344,765 at 76 s), 369,847 at 80 s; nodes 2.06 M, objects 700 k, the queue still growing to 2.84 M, 6.4 GB. The
  collector was a large constant factor; the intrinsic cost of F15 stands, and so does the Decision (the native engine).
- The fix at the cause: the tracer declares its building phases to the runtime (one owner in `program.py`): no cyclic
  collection while a phase builds the data the run keeps, and what it built frozen out of later collections.

### F18. The engine's input is the facts' own format (P1) — 2026-11-11 21:50
- Measured (e1_lab_nogc.py): an FTE1 file written by Python (every tree hash-consed in Python) costs 15 s for 1.85 M
  terms and 5 s for the bytes on the lab, 103.5 MB; the tables themselves 3.6 s.
- The facts already have one serialized form: the reader's cache, pickle protocol 5. `pickle.dumps` (C-accelerated) of
  every file's facts after the index: 1.6 s, 157 MB (e1_dumps.py). The cached facts use 25 opcodes only (e1_opcodes.py,
  via `pickletools.genops`): PROTO, FRAME, MEMOIZE, BINGET, LONG_BINGET, MARK, STOP, NONE, NEWTRUE, NEWFALSE, BININT,
  BININT1, BININT2, SHORT_BINUNICODE, BINUNICODE, EMPTY_TUPLE, TUPLE1-3, TUPLE, EMPTY_LIST, APPEND(S), EMPTY_DICT,
  SETITEM(S), EMPTY_SET, ADDITEMS, STACK_GLOBAL, NEWOBJ, BUILD; the globals are the model's four dataclasses.
- Decision: the engine's input is one pickle (protocol 5) of the units, the classes and the resolution tables, written
  by `pickle.dump`; the engine reads that documented subset (Lib/pickletools.py describes each opcode; PEP 3154 the
  framing, PEP 574 protocol 5) and hash-conses the trees itself. A second serialization of the facts (FTE1) is dropped:
  the facts keep one format. The model's field names reach the engine in the file's vocabulary (`dataclasses.fields`).

### F19. The engine is the Python semantics, and it shows where the work goes (P1) — 2026-11-12 00:30
- The new core ported to C++ (tools/analysis/engine/src, one file per heap/ module, each Python closure one kind of
  transfer): on all nine fixture sets (heap core, contexts, protos, platform, summaries, both JSX transforms, web,
  jsvalues) its links are the Python core's, link for link, and every heap test passes on both cores (437 tests).
- The lab through the engine (engine_lab.py, 150 s deadline, stopped by PID at it): with contexts, 49.8 M worklist
  items in 149 s (330 k/s), 8.7 M nodes, 876 k objects, the queue level at 100-180 k; without, 17.3 M items in 146 s,
  5.5 M nodes, the queue growing. Neither converges in the budget: the engine is not the limit, the analysis's size is.
- Measured inside the engine (its own reports: nodes and objects made by tag, the nodes holding the most objects and
  what they hold, the nodes whose deliveries cost the most):
  - an O(n^2) of the port: a call's targets, lines and relays were scanned linearly (Python kept the targets in a set);
    indexed by hash now;
  - `this` in a declared type made one node per receiver, call and site to hold the receiver alone (the largest kind of
    node made); it is the receiver's own node now, in both cores.

### F20. Three program-wide merges the language does not have (P1) — 2026-11-12 00:50
- The costliest node of the whole run (11.4 M deliveries in 6 s) was the `[[any]]` field of the primitive `undefined`:
  2,774 values written by unknown keys into whatever may be undefined, read back by every member read of every value
  that may be undefined. ECMA-262: GetValue and PutValue on undefined or null throw (RequireObjectCoercible); a property
  set on another primitive is set on a wrapper made for that access and gone after. A primitive keeps no property now:
  a read gives its wrapper prototype's declared members only; writes, stores and kept platform state on it, nothing.
- The unknown object kept what was written to it and gave it back to every iteration and unknown-key read of the
  unknown object (its `[[elements]]`, `[[any]]` and `every` nodes among the costliest). A read of the unknown object
  is the unknown object; what is written into it is handed to code the program does not have: an open call at the
  write (Averroes: what escapes may be called back), its callbacks linked from the writing line (`refs.escape`).
- The object a structural argument is read through (`BOUND`) was one per declared type for the whole program, so every
  call taking an `Iterable<T>`, `ArrayLike<T>` or `PromiseLike<T>` shared one place T (417-436 values each); it is one
  per call site and type now, as the declared objects a call returns are.
- Tests (red on the engine with the old semantics, green on both after): test_a_primitive_keeps_no_property,
  test_what_is_written_into_unknown_code_is_called_back, test_a_structural_argument_reaches_only_its_own_call,
  test_a_method_returning_this_gives_its_receiver (a refactor's pin: green before and after).

### F21. What still keeps the lab from converging (P1) — 2026-11-12 01:10 — open
- Without contexts, after F20: the costliest nodes are `[[any]]` fields of ordinary objects (literals, arrays, Maps,
  functions) each holding the same ~875 values: a store by an unknown key (`dst[k] = src[k]`) inside a generic helper
  whose parameters hold thousands of objects writes everything into every object's dictionary part. With contexts the
  helpers are told apart, but every run multiplies the nodes (8.7 M).
- What the literature does (F3; Zipper, Scaler, introspective analysis): contexts only for the methods whose precision
  matters (objects flowing from parameters to returns or into parameters' fields), none for the rest. To be settled:
  the selection criterion computed from each function's own flows (no rule for a program or a library), measured on the
  lab with the engine.

### F22. Where the lab's cost is: one pair of files, contexts, and the engine's own tables (P1) — 2026-11-12 02:30
- Bisected with the engine (engine_part.py, each run under its own deadline): every folder of the lab converges alone in
  1-7 s without contexts except `src/world`; inside it, `doc.ts` with `rescale.ts` alone (two program files and the 130
  declaration files) does not converge in 15 s without contexts, while each converges alone in under 1 s.
- On that pair without contexts the returns of generic functions hold "everything of doc.ts" (its arrays, Maps,
  iterators, the RoadDoc objects, lambdas): `copyItem = <T>(value: T): T => structuredClone(value)` and its kind are
  called with every value of the document, and a context-insensitive analysis gives each caller what all the others
  passed (185 k links). With contexts the pair converges in 1.8 s with 5,759 links: contexts are what makes it both
  precise and finite in practice.
- The engine profiled by the project's sampler (tools/analysis/sampler.py, a RelWithDebInfo build; the sampler now reads
  from each module's PE debug directory whether its .pdb exists, so a system module without one is named by offset
  instead of failing: tools/analysis/pe_debug.py): on the pair, terms made only to key a memo (`typesys.lookup`, the
  index's lookup), the call site rebuilt at every call, and the solver torn down at exit. Keyed directly now: the pair
  6.9 s -> 1.8 s, the same 446,413 items and 5,759 links.
- On the whole lab with contexts the rate stayed ~330 k items/s (48.5 M items in 148 s, 8.7 M nodes, 876 k objects,
  348 k links, the queue near 100 k): there the profile is the standard library's node-based hash sets (each insert
  allocates; ~35%), making key terms (16%) and allocation (9%). Next: flat open-addressing sets for the solver's and the
  analysis's keys.

### F23. A declaration has no run; what is left is the dictionary part (P1) — 2026-11-12 03:30
- Flat open-addressing sets and maps for the solver's and the analysis's keys (engine/src/flat.h): the pair 1.8 s ->
  1.33 s, the same result.
- The functions run in the most contexts on the lab were declared library methods (`Array[Symbol.iterator]` 18,968
  runs, `Iterator.next` 18,100, `Map.set`/`get`, `Array.push`/`map`, `Set.add`...): a declaration has no body, so a
  context, parameters, `this` and a result of its own are nodes nothing reads; what it does is `declared.apply`, in its
  receiver's environment. A call of a declaration is its link and what its declaration says now, in both cores, and a
  declaration is no entry point. Runs 232 k -> 127 k; the lab with contexts at 470 k items/s (69 M in 146 s), 8.4 M
  nodes, 833 k objects, links stable at ~348.7 k from about 100 s, the queue still ~260 k: it does not converge in 150 s.
- What still grows: values spreading through objects' dictionary parts (`[[any]]`, written by stores whose key is not
  known) and the implicit coercion reads (`toString`, `valueOf`, `Symbol.toPrimitive`) that read them: e.g. 307 values
  in the `[[any]]` of a declared ArrayIterator, read by every coercion of it. Open: what the language says of a store by
  a key whose value is known only by kind (a number names an index, a for-in key the property it enumerates), measured.

### F24. The state of the whole-program run, and what is next (P1) — 2026-11-12 04:00
- An object's types kept per object in the engine (they depend on its key only): the pair 1.33 s -> 0.86 s; the lab with
  contexts 535 k items/s, 79 M items in 147 s, 8.5 M nodes, 833 k objects, links 349.9 k and still rising slowly, the
  queue 160-290 k: not converged in 150 s.
- Measured (keykind probe, reverted): of the stores by a key whose text is not known, 30% have only numbers as keys
  (ECMA-262 ToPropertyKey: a number names an index, never a named property) and 70% strings or mixed keys.
- The causes still open, each with its evidence, in the order they cost:
  1. the dictionary part (`[[any]]`) read by every named read and coercion (`toString`, `valueOf`,
     `Symbol.toPrimitive`) of an object any unknown-key store reached (F23) — a number key writes the elements only;
  2. contexts for every function: 127 k runs; the literature's answer (Zipper, introspective analysis) is contexts only
     where values flow from parameters to results or into other parameters' fields, measured on the lab;
  3. the engine's constant factors (making key terms, allocation; an SCC collapse of copy cycles, Hardekopf & Lin).

### F25. A number names an index (P1) — 2026-11-12 04:40
- Both cores: a store or a read by a key whose text is not known goes by what each value of the key is (ECMA-262
  ToPropertyKey): a number or a BigInt names an index (the elements only), a key enumerating the same object every
  member, any other key the elements and the dictionary part; a name that is a canonical numeric string
  (CanonicalNumericIndexString, Number::toString: languages.canonical_numeric) is an index too, read and written in the
  elements. The engine gets those names from the input (`numeric_names`), the rule's one owner being Python.
- Test: test_a_number_key_names_an_index_never_a_named_property (red on both cores before, green after); 439 tests.
- The lab with contexts: 60.8 M items in 142 s, 8.7 M nodes, 838 k objects, 355.5 k links, the queue 120-210 k: not
  converged in 150 s. The dictionary part is not the main cost any more; contexts for every function are (F24, item 2).

### F26. Contexts where they change the result: correct, and most functions need them (P1) — 2026-11-12 05:10
- Built (log/trace-engine-plan.md, Task E7; heap/flows.py, both cores): a function's runs share one context when no
  output (return, yield, store into an input's object, call handing an input together with another object) carries an
  input's objects or objects it makes; inputs are parameters not typed as primitives, and `this`. Test:
  test_a_run_carrying_no_object_to_its_outputs_runs_once (red on both cores before; every earlier contexts test green;
  441 tests; fixture links identical across the cores).
- The lab: runs 127 k -> ~120 k, nodes 8.7 M -> 8.0 M, 75.8 M items in 150 s, 356 k links, not converged. Most of the
  lab's functions carry objects to their outputs, so the criterion keeps their contexts.
- What the evidence leaves (F21-F26): the analysis with a call-site context for nearly every function is the size of
  the whole lab's object flow per call site; the engine runs it at ~0.5 M items/s and it is still growing at 150 s.
  The two ways the literature bounds it are a cheaper context (call-site contexts only where the precision of call
  targets depends on them, judged by a pre-analysis that converges: the context-insensitive run does on every part of
  the lab but `src/world`) and a faster solver (collapsing the cycles the bidirectional places and copy loops make).

### F27. What a call of a value that is no function costs, and where the slow phase is (P1) — 2026-11-12 05:50
- The tail of the lab's run (tail.log): the transfer that calls what a member read holds received ~10 M objects every
  2 s, ~150 per delivery, most of them no function (values reaching member reads through dictionary parts); each went to
  `call_object` and `signatures` to find no call signature. A call of what is no function runs nothing (it throws), so
  the engine now asks once per object whether a call of it can run anything (a function, a declared function, the
  unknown object, a resolver, or an object one of whose types has a call signature) and skips the others: the same
  result (441 tests, fixture links identical), the early phase faster (337 k links at 25.7 s).
- The lab still does not converge in 150 s: between ~40 s and ~80 s the run is slow (~135 k items/s) while it makes
  nodes for named declared types produced per receiver (`PRODUCED` with a receiver in its environment) and protocol
  probes, and calls, index reads and coercions keep receiving sets of 100-400 objects (array elements merged per
  allocation site and heap context: e.g. `items[i]` in a method's run holding 307-438).
- Open, in the order the evidence gives: (1) where those large sets come from (the elements and dictionary parts of
  objects made at one site and reached by many callers); (2) declared types read as what a place admits (TypeScript's
  annotations as filters: an `items: Lot[]` holds no iterator) — a choice to weigh against the Task 6 ruling that an
  annotation is no value; (3) the solver's cycles.

### F28. Fields are initialized where their object is made (P1, soundness) — 2026-11-12 06:30
- Found by following F27's large sets back (`pushAll(into, items)` in doc.ts receives arrays whose elements merge):
  an instance field's initializer was evaluated once, in ROOT, for every instance of its class (all RoadDocs shared
  one `elements` array), `this` in an initializer was empty, and an object literal's property value was evaluated in
  ROOT, where the locals and parameters it reads are empty (`wrap(v) { return { value: v }; }` gave nothing: a loss of
  flow). Checked with a bounded probe (fields_probe) before any change.
- ECMA-262: InitializeInstanceElements runs each field initializer at every [[Construct]], `this` the new object;
  PropertyDefinitionEvaluation evaluates a literal's property values in the running context. Both cores now initialize
  a class instance's fields when `new` makes it (in its class's run, `this` the instance, one per allocation site and
  heap context) and a literal's properties where it is evaluated; a literal's property has the `this` around it.
- Tests (red on both cores before): test_each_instance_initializes_its_own_fields,
  test_a_literal_property_is_evaluated_where_the_literal_is; 445 tests; fixture links identical across the cores.
- The lab with contexts: 93 M items in 150 s, 371.7 k links (more: the flows the literals lost now arrive), not
  converged.

### F29. What is left is the graph's own size (P1) — 2026-11-12 07:00 — open
- The stores by keys of unknown text that write the most are now recursive traversals over every record
  (`SharedMembers.canon`, `shareRecord`, `intern`: `node[key] = this.canon(item)` with `key` from `Object.keys(node)`):
  ~600 records' dictionary parts get the same ~205 values. The key comes from a declared function returning `string[]`
  (nothing in the declaration ties it to its argument, and the tracer has no rule for a library's member), and the
  recursion runs through one call site, so neither the language's rules nor call-site contexts tell those records
  apart: this merge belongs to the analysis, not to a mistake in it.
- The engine's profile on the lab (90 s, RelWithDebInfo build): inserting identity edges into the table of edges seen
  21% (one random access into a table of tens of millions of entries per edge), making key terms 15%, adding objects to
  sets 13%. The work left is the size of the constraint graph the analysis builds.
- Two ways on, each a change of design to weigh with the owner's order (the costliest solution at the cause):
  (a) the solver's representation: edges kept per source in the source's own adjacency (no global table), cycles of
      identity edges collapsed (Hardekopf & Lin's lazy and hybrid cycle detection), sets as sorted chunks;
  (b) answering the tracer's question instead of solving the whole program (F7, F9: demand-driven points-to,
      Sridharan & Bodik; Boomerang): a feature trace needs the flows from its seed, not every flow of the lab.

### F30. The constraint graph's cycles, collapsed (P1) — 2026-11-12 08:30
- Measured (the engine's cycle report, Tarjan over the identity edges): a cycle that grows to 74,421 nodes, the nodes of
  the cycles holding 65.6 M object memberships (~880 each, the same set in every member): every object crossed the
  whole cycle member by member.
- Built (engine/src/cycles.cpp; Hardekopf & Lin, PLDI 2007): every `--collapse-every` items (2^22 by default) the cycles
  of identity edges are found by Tarjan's algorithm over the representatives and each is collapsed into one node, which
  takes its members' objects, edges and subscriptions; each member's edges and subscriptions are handed only what the
  union holds that they had not been handed, and a member's first applications not yet run become such deliveries with
  exactly the objects they were made for. The least fixpoint is the same.
- Tests: solver_test's cycles_collapse_to_the_same_fixpoint (two solvers, one collapsing midway with a first
  application pending: red before the pending firsts were carried over); every heap test runs the engine collapsing
  every 4 items (support.COLLAPSE_OFTEN): 445 tests green.
- The lab with contexts: 422 k links at 96 s (the best before: ~372 k at 150 s); the analysis reaches more of the lab
  faster (11.4 M nodes, 888 k objects at 108 s) and still does not converge in 150 s; a representative that took tens
  of thousands of subscriptions makes items long enough to hold the progress lines.
