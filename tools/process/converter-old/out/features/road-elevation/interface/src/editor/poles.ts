// What the feature uses of src/editor/poles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/poles.ts:40-40 type PoleSnapKind
type PoleSnapKind = 'pole' | 'footway' | 'free';

// src/editor/poles.ts:42-47 interface PoleSnap
export interface PoleSnap {
  readonly at: Vec2;
  readonly kind: PoleSnapKind;
  /** The existing pole this snapped onto, when `kind` is `pole`. */
  readonly pole?: PoleId;
}

// src/editor/poles.ts:65-70 interface PlannedPole
interface PlannedPole {
  readonly at: Vec2;
  /** The pole already standing here, which the run will reuse. */
  readonly existing: PoleId | null;
  readonly lamp: boolean;
}

// src/editor/poles.ts:72-72 type PoleRunRefusal
type PoleRunRefusal = 'offFootway' | 'noPath';

// src/editor/poles.ts:74-80 interface PoleRunPlan
export interface PoleRunPlan {
  readonly from: PoleSnap;
  readonly to: PoleSnap;
  readonly poles: readonly PlannedPole[];
  /** Set when nothing would be built, and why. */
  readonly refused?: PoleRunRefusal;
}

// src/editor/poles.ts:83-85 function placeOf
/** Where a snapped end lies on the contours, or null when no contour is near it. */
function placeOf(lines: PoleLines, snap: PoleSnap): OnContour | null;

// src/editor/poles.ts:89-89 function endOff
/** THE ROUTE GRAPH of a network's pole lines */
/** A run's end off its contour: the pole it starts or ends at, put before or after the route. */
const endOff = (snap: PoleSnap, on: OnContour): Vec2 | null => …;

// src/editor/poles.ts:104-104 variable ROUTE_MARGIN
/** A route searched first this far round its two ends, then twice as far, and so on. */
const ROUTE_MARGIN = m(96);

// src/editor/poles.ts:113-137 function routeRun
/** The run's route, read from the lines of a region round its ends only (`poleLinesIn`), as exact as the whole map's */
function routeRun(net: Network, from: PoleSnap, to: PoleSnap, first = ROUTE_MARGIN):
  { readonly path: Vec2[]; readonly lines: PoleLines } | 'noPlace' | 'noPath';

// src/editor/poles.ts:140-143 interface Routing
/** How a run is planned: the lines its ends snap onto, and its route. */
interface Routing {
  linesAt(at: Vec2): PoleLines;
  route(from: PoleSnap, to: PoleSnap): { readonly path: Vec2[]; readonly lines: PoleLines } | 'noPlace' | 'noPath';
}

// src/editor/poles.ts:146-149 function regional
/** The pole tool's planning: each end on its cell's lines, the route over a region round the run (`routeRun`). */
const regional = (net: Network, first: number): Routing => …;

// src/editor/poles.ts:240-258 function planPoleRun
/** Everything the run WOULD build, from the two raw ends of the gesture. */
export function planPoleRun(
  doc: RoadDoc,
  net: Network,
  rawFrom: Vec2,
  rawTo: Vec2,
  reach: number,
  spacing = DEFAULT_POLE_SPACING,
  lamps: PoleLampMode = 'alternate',
): PoleRunPlan;

// src/editor/poles.ts:261-265 variable lastPlan
/** The last plan made, and what it was made from (`planPoleRun`). */
let lastPlan: {
  readonly doc: RoadDoc; readonly net: Network; readonly revision: number; readonly poles: number;
  readonly fx: number; readonly fy: number; readonly tx: number; readonly ty: number;
  readonly reach: number; readonly spacing: number; readonly lamps: PoleLampMode; readonly plan: PoleRunPlan;
} | null = null;

// src/editor/poles.ts:275-365 function computePoleRun
function computePoleRun(
  doc: RoadDoc,
  net: Network,
  rawFrom: Vec2,
  rawTo: Vec2,
  reach: number,
  spacing: number,
  lamps: PoleLampMode,
  routing: Routing,
): PoleRunPlan;
