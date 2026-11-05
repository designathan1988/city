// What the feature uses of src/editor/streetObjects.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/streetObjects.ts:11-11 type PedestrianCrossingKind
export type PedestrianCrossingKind = 'zebra' | 'signal';

// src/editor/streetObjects.ts:12-14 type PedestrianCrossingResult
export type PedestrianCrossingResult =
  | { readonly committed: true; readonly node: NodeId; readonly segments: readonly SegmentId[] }
  | { readonly committed: false; readonly reason: 'unsupported' | 'tooShort' | 'geometry' };

// src/editor/streetObjects.ts:17-65 function commitPedestrianCrossing
/** Inserts one crossing whose centre is at the selected arc distance (midpoint by default). */
export function commitPedestrianCrossing(
  doc: RoadDoc,
  net: Network,
  segmentId: SegmentId,
  kind: PedestrianCrossingKind,
  s?: number,
): PedestrianCrossingResult;

// src/editor/streetObjects.ts:67-69 type UturnResult
export type UturnResult =
  | { readonly committed: true; readonly node: NodeId }
  | { readonly committed: false; readonly reason: 'noMedian' | 'tooShort' | 'narrow' | 'geometry' };

// src/editor/streetObjects.ts:72-105 function commitUturn
/** Puts a U-turn through the median at the selected arc distance (the middle by default, retorno) */
export function commitUturn(doc: RoadDoc, net: Network, segmentId: SegmentId, toward: 'a' | 'b', s?: number): UturnResult;
