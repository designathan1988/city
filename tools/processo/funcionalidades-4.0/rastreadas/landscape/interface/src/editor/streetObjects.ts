// What the feature uses of src/editor/streetObjects.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/streetObjects.ts:67-69 type UturnResult
export type UturnResult =
  | { readonly committed: true; readonly node: NodeId }
  | { readonly committed: false; readonly reason: 'noMedian' | 'tooShort' | 'narrow' | 'geometry' };

// src/editor/streetObjects.ts:72-105 function commitUturn
/** Puts a U-turn through the median at the selected arc distance (the middle by default, retorno) */
export function commitUturn(doc: RoadDoc, net: Network, segmentId: SegmentId, toward: 'a' | 'b', s?: number): UturnResult;
