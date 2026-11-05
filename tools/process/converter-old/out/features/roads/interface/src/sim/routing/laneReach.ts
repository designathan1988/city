// What the feature uses of src/sim/routing/laneReach.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/laneReach.ts:7-13 interface ReachWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface ReachWorld {
  readonly doc: RoadDoc;
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  lanelet(id: LaneletId): Lanelet | undefined;
  connector(id: string): Connector | undefined;
}

// src/sim/routing/laneReach.ts:17-17 field BodyReach.comp
private readonly comp = new Map<LaneletId, number>();

// src/sim/routing/laneReach.ts:18-18 field BodyReach.members
private readonly members = new Map<number, Set<LaneletId>>();

// src/sim/routing/laneReach.ts:20-20 field BodyReach.out
/** Each link's ways out (with repeats, as `next` lists them), and the ways in to each, counted. */
  private readonly out = new Map<LaneletId, LaneletId[]>();

// src/sim/routing/laneReach.ts:21-21 field BodyReach.into
private readonly into = new Map<LaneletId, Map<LaneletId, number>>();

// src/sim/routing/laneReach.ts:26-26 field BodyReach.exitsMemo
/** Each component's road ends, sorted, summed when asked (`exitsOf`). */
  private readonly exitsMemo = new Map<number, readonly LaneletId[]>();

// src/sim/routing/laneReach.ts:34-37 method BodyReach.exitsFrom
/** The road ends reachable from a link lane (its own included when it is one). */
  exitsFrom(w: ReachWorld, start: LaneletId): readonly LaneletId[];

// src/sim/routing/laneReach.ts:43-50 method BodyReach.reachesFrom
/** Whether a link lane can lead to another for this body; null when one of them is no link lane known here. */
  reachesFrom(start: LaneletId, goal: LaneletId): boolean | null;

// src/sim/routing/laneReach.ts:53-63 method BodyReach.buildAll
/** @rebuild Every link lane: a body class asked for the first time, or a lane graph built whole. */
  buildAll(w: ReachWorld): void;

// src/sim/routing/laneReach.ts:66-158 method BodyReach.follow
/** Brought up to the lane graph's changes. */
  follow(w: ReachWorld, changes: readonly GraphChange[]): void;

// src/sim/routing/laneReach.ts:161-169 method BodyReach.next
/** A link's ways out this body takes: the movements it fits, and the lane changes beside it. */
  private next(w: ReachWorld, lane: LaneletId): LaneletId[];

// src/sim/routing/laneReach.ts:172-194 method BodyReach.reaches
/** Whether `u` reaches `v` now: a search from each end at once, the smaller side giving out first when it does not. */
  private reaches(u: LaneletId, v: LaneletId): boolean;

// src/sim/routing/laneReach.ts:197-235 method BodyReach.tarjan
/** Tarjan's components of `lanes` (iterative), over the ways out that stay within `inSet`. */
  private tarjan(lanes: readonly LaneletId[], inSet: (id: LaneletId) => boolean): LaneletId[][];

// src/sim/routing/laneReach.ts:238-243 method BodyReach.label
/** A new component of these lanes; its id. */
  private label(lanes: readonly LaneletId[]): number;

// src/sim/routing/laneReach.ts:246-256 method BodyReach.retag
/** Lanes given new components by `relabel`, the condensation's counts of their ways taken out before and put back after. */
  private retag(lanes: readonly LaneletId[], relabel: () => void): void;

// src/sim/routing/laneReach.ts:259-263 method BodyReach.edge
/** A way out added (+1) or taken (-1): the ways in, and the condensation between two components. */
  private edge(u: LaneletId, v: LaneletId, n: 1 | -1): void;

// src/sim/routing/laneReach.ts:265-270 method BodyReach.countIn
private countIn(v: LaneletId, u: LaneletId, n: number): void;

// src/sim/routing/laneReach.ts:300-321 method BodyReach.exitsOf
/** A component's road ends: those of its lanes, and those of the components it leads to (summed first). */
  private exitsOf(w: ReachWorld, root: number): readonly LaneletId[];

// src/sim/routing/laneReach.ts:333-337 function boundaryExit
/** A lane that leaves the map: a link into a road end at the edge, with no way on. */
export function boundaryExit(w: ReachWorld, id: LaneletId): boolean;

// src/sim/routing/laneReach.ts:345-350 method LaneReaches.exitsFrom
/** The road ends a link lane can reach, for a body (its own included when it is one). */
  exitsFrom(w: ReachWorld, start: LaneletId, body: BodyClass): readonly LaneletId[];

// src/sim/routing/laneReach.ts:353-358 method LaneReaches.mayReach
/** Whether a link lane can lead to another for a body (false: no route, DO NOT search the graph for one); true when it may, or when it cannot be said. */
  mayReach(w: ReachWorld, start: LaneletId, goal: LaneletId, body: BodyClass): boolean;
