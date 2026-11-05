// What the feature uses of src/editor/poles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/poles.ts:17-17 variable POLE_PICK_PIXELS
/** Where a pole line goes: on the footway, just behind the kerb, and nowhere else */

/** Pick radius for an existing pole, in screen pixels. */
export const POLE_PICK_PIXELS = 34;

// src/editor/poles.ts:20-20 variable LINE_CATCH
/** A pointer this far from the line, or anywhere on a footway, takes the line. */
const LINE_CATCH = m(2.5);

// src/editor/poles.ts:26-26 variable CORNER_TOLERANCE
/** Douglas-Peucker tolerance for a corner of the route: a turn that bows the line this far. */
const CORNER_TOLERANCE = m(1.5);

// src/editor/poles.ts:29-29 variable MIN_POLE_GAP
/** Two poles of one run never stand closer than this; corners closer merge into one. */
const MIN_POLE_GAP = m(10);

// src/editor/poles.ts:32-32 variable REUSE_RADIUS
/** A planned pole this close to a pole already standing is that pole. */
const REUSE_RADIUS = m(6);

// src/editor/poles.ts:36-38 function lampFor
function lampFor(index: number, mode: PoleLampMode): boolean;

// src/editor/poles.ts:40-40 type PoleSnapKind
type PoleSnapKind = 'pole' | 'footway' | 'free';

// src/editor/poles.ts:42-47 interface PoleSnap
export interface PoleSnap {
  readonly at: Vec2;
  readonly kind: PoleSnapKind;
  /** The existing pole this snapped onto, when `kind` is `pole`. */
  readonly pole?: PoleId;
}

// src/editor/poles.ts:50-52 function snapPole
/** Snaps one end of a pole run: onto a pole already standing */
export function snapPole(doc: RoadDoc, net: Network, at: Vec2, reach: number): PoleSnap;

// src/editor/poles.ts:55-63 function snapOn
/** `snapPole` onto the contours of `lines` (any set of lines holding the point's cell). */
function snapOn(doc: RoadDoc, net: Network, lines: PoleLines, at: Vec2, reach: number): PoleSnap;

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

// src/editor/poles.ts:92-92 variable graphs
/** The graph of one set of lines (`poleLinesIn`): made again only with them. */
const graphs = new WeakMap<PoleLines, RouteGraph>();

// src/editor/poles.ts:95-101 function routeGraph
/** The route graph of the lines of `region` (`poleLinesIn`), made once per set of lines. */
function routeGraph(net: Network, lines: PoleLines, region: ChangeRect): RouteGraph;

// src/editor/poles.ts:104-104 variable ROUTE_MARGIN
/** A route searched first this far round its two ends, then twice as far, and so on. */
const ROUTE_MARGIN = m(96);

// src/editor/poles.ts:106-106 variable CLOSED_PAD
/** How near the region's edge the lines a run's start reaches must come for a larger region to join them to its end (`routeRun`). */
const CLOSED_PAD = m(24);

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

// src/editor/poles.ts:155-157 interface PoleRunPlanner
/** Plans many runs over the same streets (`poleRunPlanner`). */
export interface PoleRunPlanner {
  plan(doc: RoadDoc, rawFrom: Vec2, rawTo: Vec2, reach: number, spacing?: number, lamps?: PoleLampMode): PoleRunPlan;
}

// src/editor/poles.ts:160-199 function poleRunPlanner
/** DO NOT plan a batch of runs one region each: the lines are the same. */
export function poleRunPlanner(net: Network): PoleRunPlanner;

// src/editor/poles.ts:177-197 method poleRunPlanner.plan
plan(doc, rawFrom, rawTo, reach, spacing = DEFAULT_POLE_SPACING, lamps = 'alternate');

// src/editor/poles.ts:202-226 function corners
/** Indices of the points Ramer-Douglas-Peucker keeps at `tolerance`. */
function corners(points: readonly Vec2[], tolerance: number): number[];

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

// src/editor/poles.ts:368-370 function commitPoleRun
/** Applies a plan to the document. */
export function commitPoleRun(doc: RoadDoc, plan: PoleRunPlan): boolean;

// src/editor/poles.ts:373-388 function commitPoleRunPoles
/** `commitPoleRun`, giving the run's poles in order (null when nothing was built). */
export function commitPoleRunPoles(doc: RoadDoc, plan: PoleRunPlan): PoleId[] | null;

// src/editor/poles.ts:391-404 interface PoleToolHost
/** What the pole tool reads of the game and does to it. */
export interface PoleToolHost {
  readonly doc: RoadDoc;
  readonly net: Network;
  zoom(): number;
  /** The tool's verb ('build' or 'remove', `ui/toolChoices.ts`) and its lamps. */
  mode(): string;
  lamps(): PoleLampMode;
  /** Whether the pole tool is the tool in hand. */
  inHand(): boolean;
  /** An edit of the document, one undo step; whether it changed it. */
  mutate(fn: () => boolean): boolean;
  hint(key: string): void;
  redraw(): void;
}

// src/editor/poles.ts:407-515 class PoleTool
/** THE POLE TOOL: a distribution line traced as straight stretches, each a drag */
export class PoleTool { … }
