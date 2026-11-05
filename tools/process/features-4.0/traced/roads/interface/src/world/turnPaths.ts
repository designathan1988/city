// What the feature uses of src/world/turnPaths.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/turnPaths.ts:20-20 variable FALLBACK_HANDLE
const FALLBACK_HANDLE = 0.65;

// src/world/turnPaths.ts:24-24 variable STEPS
/** Samples per turn path, cosine-spaced. */
const STEPS = 48;

// src/world/turnPaths.ts:26-26 variable SWEEP_STEP
/** Body-centre spacing of the containment sweep, world units. */
const SWEEP_STEP = m(0.6);

// src/world/turnPaths.ts:30-30 type Poly
type Poly = readonly Vec2[];

// src/world/turnPaths.ts:31-31 interface Box
interface Box { minX: number; minY: number; maxX: number; maxY: number }

// src/world/turnPaths.ts:32-40 interface Area
interface Area {
  readonly poly: Poly;
  readonly box: Box;
  readonly hashA: number;
  readonly hashB: number;
  /** Its edges by band (`core/polygonBands.ts`), made the first time a point is tested against it; its flat x, y list, the first time a worker needs it (`flatRings`). */
  bands: PolygonBands | null;
  flat: Float64Array | null;
}

// src/world/turnPaths.ts:76-141 class JunctionSurface
/** The drivable surface around one node: its junction plate and its legs. */
export class JunctionSurface { … }

// src/world/turnPaths.ts:77-77 field JunctionSurface.areas
private readonly areas: Area[] = [];

// src/world/turnPaths.ts:83-83 field JunctionSurface.centre
readonly centre: Vec2;

// src/world/turnPaths.ts:144-171 function bezierTurn
/** Cubic Bezier from the end of `inCentre` to the start of `outCentre`. */
export function bezierTurn(inCentre: Polyline, outCentre: Polyline, handleFraction: number): Polyline;

// src/world/turnPaths.ts:174-176 interface WaitingLane
/** Another inbound lane - of another approach, or the one beside this movement's own */
export interface WaitingLane {
  readonly centre: Polyline;
}

// src/world/turnPaths.ts:179-179 variable QUEUE_REACH
/** How far behind a stop line a waiting vehicle is looked for, world units. */
const QUEUE_REACH = BODY_ENVELOPE[HEAVY]!.length * 1.5;

// src/world/turnPaths.ts:197-197 interface FailHint
/** Where, in its sequence of positions, the last candidate swept for a body left the surface (`sweep`); -1 before any did. */
interface FailHint { at: number }

// src/world/turnPaths.ts:284-364 function sweep
/** DO NOT fold the three passes back into one loop */
function sweep(surface: JunctionSurface, inCentre: Polyline, path: Polyline, outCentre: Polyline,
  waiting: readonly Queue[], body: BodyClass = HEAVY, ends: RoadEnds = NO_ROAD_ENDS, hint?: FailHint): number | null;

// src/world/turnPaths.ts:374-377 interface RoadEnds
/** Whether the arriving lane begins, or the departing lane ends, at the end of a road */
export interface RoadEnds {
  readonly fromRoadEnd: boolean;
  readonly toRoadEnd: boolean;
}

// src/world/turnPaths.ts:378-378 variable NO_ROAD_ENDS
const NO_ROAD_ENDS: RoadEnds = { fromRoadEnd: false, toRoadEnd: false };

// src/world/turnPaths.ts:445-445 variable CENTRE_TURN_RADIUS
/** The tightest a body of each class turns, on its centreline: AASHTO's design vehicles */
const CENTRE_TURN_RADIUS: readonly number[] = [m(3), m(6.4), m(11.6)];

// src/world/turnPaths.ts:489-510 function twoPieceTurn
/** Two tangent-continuous cubics through a point inside the junction. */
function twoPieceTurn(inCentre: Polyline, outCentre: Polyline, middle: Vec2, share: number): Polyline;

// src/world/turnPaths.ts:512-512 variable LANE_REACH
/** Reach of the sweep along the approach and exit: half the largest body plus the heading chord. */
const LANE_REACH = BODY_ENVELOPE[HEAVY]!.length / 2 + HEADING_CHORD + SWEEP_STEP * 2;

// src/world/turnPaths.ts:514-514 variable KEY_STEP
/** Spacing of the samples a lane's digest is taken from, world units. */
const KEY_STEP = m(0.4);

// src/world/turnPaths.ts:515-515 variable laneKeys
const laneKeys = new WeakMap<Polyline, Map<string, string>>();

// src/world/turnPaths.ts:539-550 function chooseTurn
/** The handle `turnPath` settles on, by sweeping the candidates. */
export function chooseTurn(inCentre: Polyline, outCentre: Polyline, surface: JunctionSurface,
  waiting: readonly WaitingLane[], ends: RoadEnds): { choice: TurnChoice; path: Polyline; body: BodyClass | -1 };

// src/world/turnPaths.ts:553-558 variable WAYPOINTS
/** The interior waypoints `chooseTurnFor` tries, as offsets from the node: a 5 x 5 grid 2 m apart, nearest the node first. */
const WAYPOINTS: readonly Vec2[] = (() => {
  const offsets: Vec2[] = [];
  for (const dx of [-4, -2, 0, 2, 4].map(m)) for (const dy of [-4, -2, 0, 2, 4].map(m))
    offsets.push({ x: dx, y: dy });
  return offsets.sort((a, b) => a.x * a.x + a.y * a.y - b.x * b.x - b.y * b.y || a.x - b.x || a.y - b.y);
})();

// src/world/turnPaths.ts:562-580 class Candidates
/** DO NOT build them per body size again. */
class Candidates { … }

// src/world/turnPaths.ts:583-632 function chooseTurnFor
/** The handle for one body size; null when no candidate keeps that body on the surface. */
function chooseTurnFor(candidates: Candidates, surface: JunctionSurface,
  queues: readonly Queue[], body: BodyClass, ends: RoadEnds): { choice: TurnChoice; path: Polyline } | null;
