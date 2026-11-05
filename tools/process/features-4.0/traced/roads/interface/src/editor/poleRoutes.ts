// What the feature uses of src/editor/poleRoutes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/poleRoutes.ts:12-28 interface RouteGraph
/** THE ROUTE GRAPH of a set of pole lines and the shortest way through it (`editor/poles.ts` plans the runs on it) */

export interface RouteGraph {
  /** The stations: every vertex of every contour */
  readonly x: Float64Array;
  readonly y: Float64Array;
  /** Each station's arc length along its contour, and the contour. */
  readonly s: Float64Array;
  readonly contour: Int32Array;
  /** Each contour's first station and count; its stations run in order of `s`. */
  readonly runFirst: Int32Array;
  readonly runCount: Int32Array;
  /** Spans over a street, by station: those of station i are `spanTo`/`spanLength` from `spanStart[i]` to `spanStart[i + 1]`. */
  readonly spanStart: Int32Array;
  readonly spanTo: Int32Array;
  readonly spanLength: Float64Array;
  /** DO NOT allocate per run: the work of a run must be the stations it reaches. */
  readonly scratch: { readonly distTo: Float64Array; readonly prev: Int32Array; readonly done: Uint8Array; readonly touched: number[] };
}

// src/editor/poleRoutes.ts:31-31 variable STATION_STEP
/** Station spacing along a contour. */
const STATION_STEP = m(2);

// src/editor/poleRoutes.ts:35-35 variable SPAN_LANDING
/** A cast across the street must land this close to a contour to make a span there. */
const SPAN_LANDING = m(1);

// src/editor/poleRoutes.ts:50-122 function buildRouteGraph
/** The route graph of a set of lines: their contours' stations, and the spans over the streets that reach `region`. */
export function buildRouteGraph(net: Network, lines: PoleLines, region: ChangeRect): RouteGraph;

// src/editor/poleRoutes.ts:125-197 function route
/** The route of a run, as a list of points */
export function route(g: RouteGraph, a: OnContour, b: OnContour, head: Vec2 | null, tail: Vec2 | null,
  /** Filled with the box of every station reached when no route is found (`poles.ts` routeRun: a part of the lines closed in itself needs no larger region). */
  reached?: { x0: number; y0: number; x1: number; y1: number }): { readonly path: Vec2[]; readonly cost: number } | null;
