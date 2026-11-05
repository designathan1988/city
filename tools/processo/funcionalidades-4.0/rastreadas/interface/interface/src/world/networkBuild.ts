// What the feature uses of src/world/networkBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/networkBuild.ts:48-54 interface MemoEntry
/**
 * A junction solved for one pass, with what the pass handed it (`junctionKey`), the plate it reaches, how many levels it has and what
 * the reconciling of its roads reads of it (`trimSummary`). A pass's junction a later pass solved again is let go once the rebuild
 * is over (`byLevel` null: DO NOT keep two or three whole junctions a pressured node, ~two thirds of the junctions' memory,
 * 2026-10-22): its summary and plate are all the next rebuilds read of it, and it is solved again should a node fall back to it (`levels3`).
 */
interface MemoEntry {
  readonly key: number;
  readonly byLevel: JunctionLevels | null;
  readonly size: number;
  readonly plate: readonly (readonly [SegmentId, number])[];
  readonly summary: readonly number[];
}

// src/world/networkBuild.ts:69-72 interface NetworkBake
/** A TILE'S JUNCTIONS AND RIBBONS SOLVED OFF THE PAGE (`editor/tileStore.ts` prepareTile, the save worker or the generator's): each node's passes with the key of what they were solved from (`nodeBakeKey`), each road's ribbon with what it was made from; the page's build takes them where its inputs are the same (`takeBakes`) and solves only the nodes with a road of a tile not here. Plain data and typed arrays, cloned by the message. */
export interface NetworkBake {
  readonly nodes: BakedNode[];
  readonly segments: BakedSegment[];
}

// src/world/networkBuild.ts:73-79 interface BakedNode
export interface BakedNode {
  readonly id: NodeId;
  readonly key: number;
  readonly j1: BakedMemo | null;
  readonly j2: BakedMemo | null;
  readonly j3: BakedMemo | null;
}

// src/world/networkBuild.ts:81-81 interface BakedMemo
/** A pass's junctions as their data (`JunctionLevels.data`: the carriageway, the frames its outer levels are made from, what is kept whole; null: let go, `MemoEntry`). */
interface BakedMemo { readonly key: number; readonly levels: JunctionLevelsData | null; readonly size: number; readonly plate: MemoEntry['plate']; readonly summary: readonly number[] }

// src/world/networkBuild.ts:83-85 type BakedRibbon
/** A ribbon as what it is made from (`ribbons.ts` LeanRibbon): its road, its whole centreline as its flat array (`Polyline.xy`) and its box; its levels made on the thread that reads them (`ribbonFromBake`). */
export type BakedRibbon = Pick<SegmentRibbon, 'id' | 'typeIndex' | 'direction' | 'road' | 'dashOrigin'> & {
  readonly full: Float64Array; readonly bounds: ChangeRect;
};

// src/world/networkBuild.ts:95-103 interface BakedSegment
export interface BakedSegment {
  readonly id: SegmentId;
  /** `segmentSourceKey` of the road the ribbon was made from. */
  readonly source: number;
  readonly trims: SegmentTrims;
  readonly bendA: number;
  readonly bendB: number;
  readonly ribbon: BakedRibbon;
}
