// What the feature uses of src/sim/routing/router.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/router.ts:12-12 variable HORIZON
/** Horizon for a vehicle with no reachable boundary destination. */
const HORIZON = 24;

// src/sim/routing/router.ts:14-14 variable LOOKAHEAD
/** Bounded network lookahead used when comparing alternate turns. */
const LOOKAHEAD = 5;

// src/sim/routing/router.ts:19-19 field TrapIndex.bodies
/** Per body class asked for: the seeds, and the traps peeled from them (null: to peel again). */
  private readonly bodies = new Map<number, { readonly seeds: Set<LaneletId>; traps: ReadonlySet<LaneletId> | null }>();

// src/sim/routing/router.ts:22-27 method TrapIndex.traps
traps(w: SimWorld, body: number): ReadonlySet<LaneletId>;

// src/sim/routing/router.ts:30-49 method TrapIndex.follow
/** The seeds brought up to the lane graph's changes since it was last. */
  private follow(w: SimWorld): void;

// src/sim/routing/router.ts:53-60 function seedsOf
/** @rebuild Every lane with exits none of which this body fits. */
function seedsOf(w: SimWorld, body: number): Set<LaneletId>;

// src/sim/routing/router.ts:63-70 function fittingExits
/** The movements out of a lane this body fits. */
function fittingExits(w: SimWorld, lane: LaneletId, body: number): number;

// src/sim/routing/router.ts:73-74 function isSeed
/** A lane with exits none of which this body fits. */
const isSeed = (w: SimWorld, lane: LaneletId, body: number): boolean => …;

// src/sim/routing/router.ts:77-97 function peel
/** The traps: the seeds, and every lane whose movements this body fits all lead into traps, peeled back from them. */
function peel(w: SimWorld, body: number, seeds: ReadonlySet<LaneletId>): ReadonlySet<LaneletId>;

// src/sim/routing/router.ts:100-104 function trapLanes
export function trapLanes(w: SimWorld, body: number): ReadonlySet<LaneletId>;

// src/sim/routing/router.ts:107-186 function planFrom
/** Route planning over the lanelet graph. */
export function planFrom(w: SimWorld, v: Vehicle): LaneletId | null;

// src/sim/routing/router.ts:189-235 function extend
/** Grows a route forward until it reaches the horizon or a dead end. */
export function extend(w: SimWorld, v: Vehicle): void;

// src/sim/routing/router.ts:237-267 function chooseExit
function chooseExit(
  w: SimWorld,
  exits: readonly string[],
  body: ReturnType<typeof bodyClassOfArchetype>,
  visited = new Set<LaneletId>(),
  /** The vehicle's shape: a bus lane is buses only. */
  shape = 'car',
): string | null;

// src/sim/routing/router.ts:270-309 function routeCost
/** A small bounded search keeps the fallback moving without inventing a goal it cannot reach. */
function routeCost(
  w: SimWorld,
  connectorId: LaneletId,
  body: ReturnType<typeof bodyClassOfArchetype>,
  visited: Set<LaneletId>,
  remaining: number,
): number;

// src/sim/routing/router.ts:312-327 function reconsiderRoute
/** Lets a driver who has been held up look for another way round. */
export function reconsiderRoute(w: SimWorld, v: Vehicle): void;

// src/sim/routing/router.ts:330-374 function repairRoute
/** Repairs a route whose lanelets no longer exist, after a live edit. */
export function repairRoute(w: SimWorld, v: Vehicle): void;

// src/sim/routing/router.ts:377-389 function cutAtBreak
/** A route cut where it stops standing on the graph (a lane gone, a movement that no longer joins them); whether it was cut. */
export function cutAtBreak(w: SimWorld, v: Vehicle): boolean;

// src/sim/routing/router.ts:391-402 function isLegalTransition
function isLegalTransition(w: SimWorld, fromId: LaneletId, toId: LaneletId,
  body: ReturnType<typeof bodyClassOfArchetype>): boolean;
