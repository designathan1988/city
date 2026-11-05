// What the feature uses of src/world/networkBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/networkBuild.ts:24-37 interface Network
/** What the build writes: the network's own derived maps */
interface Network {
  readonly doc: RoadDoc;
  readonly polylines: PolylineCache;
  readonly junctions: Map<NodeId, JunctionLevels>;
  readonly ribbons: Map<SegmentId, SegmentRibbon>;
  readonly trims: Map<SegmentId, SegmentTrims>;
  readonly plateReach: Map<string, number>;
  readonly transitions: Set<NodeId>;
  readonly impossible: Map<NodeId, number>;
  readonly squeezed: Map<NodeId, number>;
  readonly segmentCells: CellFiling<SegmentId, SegmentRibbon>;
  readonly junctionCells: CellFiling<NodeId, JunctionLevels>;
  readonly flushCells: CellFiling<SegmentId, SegmentRibbon>;
}

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

// src/world/networkBuild.ts:56-58 function memoEntry
function memoEntry(key: number, byLevel: JunctionLevels, plate: MemoEntry['plate']): MemoEntry;

// src/world/networkBuild.ts:81-81 interface BakedMemo
/** A pass's junctions as their data (`JunctionLevels.data`: the carriageway, the frames its outer levels are made from, what is kept whole; null: let go, `MemoEntry`). */
interface BakedMemo { readonly key: number; readonly levels: JunctionLevelsData | null; readonly size: number; readonly plate: MemoEntry['plate']; readonly summary: readonly number[] }

// src/world/networkBuild.ts:87-90 function memoData
function memoData(entry: MemoEntry | undefined): BakedMemo | null;

// src/world/networkBuild.ts:92-94 function memoOf
function memoOf(baked: BakedMemo): MemoEntry;

// src/world/networkBuild.ts:287-487 method NetworkBuild.rebuild
/** @rebuild Brings the network up to the document; returns what changed (`null`: everything, a build from nothing). */
  rebuild(): NetworkDelta | null;

// src/world/networkBuild.ts:595-607 method NetworkBuild.levels3
/** The junction a node has after the last pass, whole: a pass let go and fallen back to solved again, the same (its inputs are the ones it was solved from: a change of them re-marks the node). */
  private levels3(node: NodeId): JunctionLevels | null;

// src/world/networkBuild.ts:617-645 method NetworkBuild.solveNode
/** One node's junction for one pass: the entry made for the same key before when there is one */
  private solveNode(node: NodeId, scales: ReadonlyMap<SegmentId, number>, capped: boolean, known: MemoEntry | undefined, letGo = false): MemoEntry;

// src/world/networkBuild.ts:727-740 method NetworkBuild.finalTrims
/** The reconciled trims lowered to what the final junctions actually used */
  private finalTrims(id: SegmentId, seg: RoadSegment): SegmentTrims;

// src/world/networkBuild.ts:758-777 method NetworkBuild.squeeze
/** How far the worst mouth of a junction is cut short of where two of its legs' carriageways stop overlapping */
  private squeeze(node: NodeId): number;
