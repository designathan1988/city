// What the feature uses of src/world/roads/buildMode.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/buildMode.ts:10-10 type BuildMode
/** HOW A ROAD IS BUILT at each point along it */
export type BuildMode = 'ground' | 'embankment' | 'cutting' | 'bridge' | 'tunnel';

// src/world/roads/buildMode.ts:13-21 function buildModeAt
/** The way of building at one point: `lift` is the deck over the natural ground (world units). */
export function buildModeAt(structure: RoadStructure, lift: number): BuildMode;

// src/world/roads/buildMode.ts:24-34 interface RoadStation
/** A point of a road being built: where, how far along, its deck, the ground under it, and how. */
export interface RoadStation {
  /** Distance along the road from its start (world units). */
  readonly s: number;
  readonly x: number;
  readonly y: number;
  /** The solved deck height (world height). */
  readonly deck: number;
  /** The natural ground under it. */
  readonly ground: number;
  readonly mode: BuildMode;
}

// src/world/roads/buildMode.ts:44-79 function orderAlong
/** The segments of a road laid from `startAt`, in order and each with its direction */
export function orderAlong(doc: RoadDoc, ids: ReadonlySet<SegmentId>, startAt: Vec2): { id: SegmentId; reversed: boolean }[];

// src/world/roads/buildMode.ts:82-107 function stationsAlong
/** The stations of a road every `step` units along it */
export function stationsAlong(
  net: Network,
  elevation: RoadElevation,
  ground: (x: number, y: number) => number,
  path: readonly { id: SegmentId; reversed: boolean }[],
  step: number,
): RoadStation[];
