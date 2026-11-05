// What the feature uses of src/world/poleLines.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/poleLines.ts:26-26 variable POLE_KERB_INSET
/** The line wire poles stand on, and the kerb they stand behind. */

/** How far behind the kerb stone the pole line runs: the middle of the lamp zone. */
export const POLE_KERB_INSET = LAMP_ZONE / 2;

// src/world/poleLines.ts:28-37 interface PoleLines
export interface PoleLines {
  /** Closed contours, first point repeated at the end. */
  readonly contours: readonly Polyline[];
  /** The ground footways: paving out to the outer edge, and the kerbed carriageway inside it. */
  readonly paving: MultiPoly;
  readonly kerbed: MultiPoly;
  readonly accesses: readonly CrossingAccess[];
  /** The contours' edges filed by place (`nearestOnContour`). */
  readonly edges: ContourEdges;
}

// src/world/poleLines.ts:40-40 interface ContourEdges
/** The edges of a set of contours filed by place: each edge (contour, index) in every bucket its bounds reach. */
interface ContourEdges { readonly buckets: ReadonlyMap<number, readonly number[]>; readonly scratch: ContourScratch }

// src/world/poleLines.ts:45-62 function contourEdges
function contourEdges(contours: readonly Polyline[]): ContourEdges;

// src/world/poleLines.ts:65-70 interface OnContour
/** A place on a contour. */
export interface OnContour {
  readonly contour: number;
  readonly s: number;
  readonly at: Vec2;
  readonly distance: number;
}

// src/world/poleLines.ts:73-80 interface ContourScratch
/** What `nearestOnContour` keeps of each contour while it answers, by contour, made with the lines' edges (`contourEdges`): written over at each question (`stamp` tells this question's from an older one's), DO NOT make a map and an object for every contour met at every question - asked for every station of every street a city's power lines are routed over. */
interface ContourScratch {
  readonly distSq: Float64Array;
  readonly edge: Int32Array;
  readonly t: Float64Array;
  readonly stamp: Uint32Array;
  readonly met: number[];
  round: number;
}

// src/world/poleLines.ts:90-146 function nearestOnContour
/** DO NOT go back to scanning the contours: the question is local. */
export function nearestOnContour(lines: PoleLines, at: Vec2, reach: number): OnContour | null;

// src/world/poleLines.ts:150-156 function polesInclude
/** The streets whose kerb lines carry poles: ground streets with footways (the two levels read; `surfaces` unions all four). */
function polesInclude(net: Network): (id: SegmentId) => boolean;

// src/world/poleLines.ts:159-168 function contoursOf
/** The contours of the kerb line round the kerbed polygons. */
function contoursOf(kerbed: MultiPoly): Polyline[];

// src/world/poleLines.ts:180-180 variable LINES_HALO
/** How far round a place its pole lines are made from: the farthest any question about a point reaches */
const LINES_HALO = m(32);

// src/world/poleLines.ts:184-184 variable poleLinesWork
/** What the regional pole lines did: made, kept, and the streets made from. A count for the budgets (`core/perfBudget.ts`). */
export const poleLinesWork = { made: 0, kept: 0, segments: 0 };

// src/world/poleLines.ts:187-190 function regionStamp
/** The versions of the cells a region's lines are made from (`LINES_HALO` round it): equal, nothing they read moved. */
function regionStamp(net: Network, rect: ChangeRect): string;

// src/world/poleLines.ts:193-218 class PoleLineState
/** DO NOT go back to a ceiling that lets the least recently asked go (`REGIONAL_KEPT`, 64) */
export class PoleLineState { … }

// src/world/poleLines.ts:201-209 method PoleLineState.linesIn
/** The lines of `rect`, kept or made now. */
  linesIn(net: Network, rect: ChangeRect): PoleLines;

// src/world/poleLines.ts:212-217 method PoleLineState.letGoStale
/** @rebuild The regions some cell of whose moved since they were made, let go: once per edit of the network */
  private letGoStale(net: Network): void;

// src/world/poleLines.ts:221-221 function stateOf
/** The network's pole line state, made when first asked. */
const stateOf = (net: Network): PoleLineState => …;

// src/world/poleLines.ts:224-226 function poleLinesIn
/** The pole lines inside `rect`: the same lines `poleLines` has there */
export function poleLinesIn(net: Network, rect: ChangeRect): PoleLines;

// src/world/poleLines.ts:230-257 function poleLinesOf
/** The pole lines inside `rect`, made now and not kept (`poleLinesIn` keeps them) */
export function poleLinesOf(net: Network, rect: ChangeRect): PoleLines;

// src/world/poleLines.ts:260-262 function poleLinesAt
/** The pole lines of the cell a point is in (`poleLinesIn`): right for any question reaching under `LINES_HALO - POLE_KERB_INSET` from it. */
export function poleLinesAt(net: Network, p: Vec2): PoleLines;

// src/world/poleLines.ts:265-265 interface WindingRing
/** A ring as Clipper takes it in (`core/clipper.ts` `input`): its vertices on Clipper's grid */
interface WindingRing { readonly xy: Float64Array; readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number }

// src/world/poleLines.ts:267-299 function windingRings
function windingRings(polys: MultiPoly): WindingRing[];

// src/world/poleLines.ts:302-312 function windingOf
/** The winding number of a ring round a point, both in grid steps (Sunday's crossing rule: upward edges with the point on their left count +1, downward ones with it on their right -1). */
function windingOf(ring: WindingRing, x: number, y: number): number;

// src/world/poleLines.ts:315-315 interface FootRings
/** The footway and kerb rings of a street or a junction (`NetworkLines.rings`): made again with the object the network builds for it. */
interface FootRings { readonly paving: WindingRing[]; readonly kerbed: WindingRing[] }

// src/world/poleLines.ts:318-338 function insideLevels
/** DO NOT answer a point from a region's union again: the answer is local. */
function insideLevels(net: Network, p: Vec2): { readonly paving: boolean; readonly kerbed: boolean };

// src/world/poleLines.ts:341-352 function ribbonRings
/** The footway and kerb rings of a street, made once per ribbon the network built. */
function ribbonRings(net: Network, ribbon: SegmentRibbon, include: (id: SegmentId) => boolean): FootRings;

// src/world/poleLines.ts:355-366 function junctionRings
/** The footway and kerb rings of a junction, made once per junction the network solved. */
function junctionRings(net: Network, id: NodeId, byLevel: ReadonlyMap<SurfaceLevel, Junction>, include: (id: SegmentId) => boolean): FootRings;

// src/world/poleLines.ts:369-381 function localRings
/** The footway and kerb rings of the streets with footways and their junctions filed over `rect`, each made once per object the network built. */
function localRings(net: Network, rect: ChangeRect): FootRings[];

// src/world/poleLines.ts:384-472 function markPlates
/** DO NOT go back to asking each cell: that is the whole raster times the rings round each cell. */
export function* markPlates(net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void>;

// src/world/poleLines.ts:475-478 function onFootway
/** Whether a point is on a footway: on the paving, outside every kerbed carriageway. */
export function onFootway(net: Network, p: Vec2): boolean;

// src/world/poleLines.ts:481-484 function onPaving
/** DO NOT ask the two in turn: each walked the rings round the point again. */
export function onPaving(net: Network, p: Vec2): boolean;

// src/world/poleLines.ts:487-487 variable kerbWork
/** The work `kerbward` did: kerb edges walked. A count for the performance budgets (`core/perfBudget.ts`). */
export const kerbWork = { edges: 0 };

// src/world/poleLines.ts:491-502 function ringBoxes
function ringBoxes(polys: MultiPoly): readonly (readonly [number, number, number, number])[];

// src/world/poleLines.ts:505-540 function kerbward
/** The unit direction from a point to the nearest kerb edge */
export function kerbward(net: Network, p: Vec2, reach: number): Vec2 | null;

// src/world/poleLines.ts:543-573 function kerbwardOfUnion
/** `kerbward` over the cell's union (`poleLinesAt`): for a point inside a kerb, where the union and the rings differ. */
function kerbwardOfUnion(net: Network, p: Vec2, reach: number): Vec2 | null;
