// What the feature uses of src/world/poleLines.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/world/poleLines.ts:230-257 function poleLinesOf
/** The pole lines inside `rect`, made now and not kept (`poleLinesIn` keeps them) */
export function poleLinesOf(net: Network, rect: ChangeRect): PoleLines;
