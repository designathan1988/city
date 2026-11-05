// What the feature uses of src/editor/commit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/commit.ts:44-57 interface DraftResult
export interface DraftResult {
  readonly committed: boolean;
  readonly reason?: 'tooShort' | 'duplicate' | 'degenerate' | 'clearance' | RoadEditRefusal;
  readonly heightLimited?: boolean;
  readonly finalHeightOffset?: number;
  /** The roads' heights solved for the tunnel test on the ground the renderer reads, when no tunnel was bored */
  readonly elevation?: RoadElevation;
  /** What the edit costs (`world/economy.ts`): money taken, negative when money comes back. */
  readonly cost?: number;
  /** A dry run's road as it would be built */
  readonly stations?: readonly RoadStation[];
  /** The segments the edit added (`RoadDoc.addedSegments`), read off its working copy before it was taken back */
  readonly laid?: readonly SegmentId[];
}

// src/editor/commit.ts:63-159 function commitRoadPath
/** Commits one freehand gesture as one undoable, atomic road. */
export function commitRoadPath(
  doc: RoadDoc,
  net: Network,
  start: Anchor,
  end: Anchor,
  type: number,
  pieces: readonly RoadPathPiece[],
  /** Lanes for every segment laid, or null for the class's own profile. */
  lanes: number | null = null,
  /** Parking left and right of the drawing direction, for every segment laid. */
  parking?: SegmentParking,
  /** The ground as already sampled for the roads' heights (the renderer's natural terrain) */
  ground?: (x: number, y: number) => number,
  /** `dryRun`: everything is worked out and judged on the copy, and the live map is left alone */
  options: {
    readonly dryRun?: boolean;
    /** The roads' heights the game last solved on this same natural ground (`ground`) */
    readonly groundSolve?: RoadElevation | null;
  } = {},
): DraftResult;

// src/editor/commit.ts:162-172 function layCopy
/** The road worked out on the copy, paid for on it (so the live map takes the balance with the roads and undo gives it back) and taken back. */
function layCopy(doc: RoadDoc, net: Network, work: RoadDoc, workNet: Network, bore: { bored: boolean; elevation: RoadElevation | null },
  heightLimited: boolean, finalHeightOffset: number, ground: ((x: number, y: number) => number) | undefined, timed: (what: string) => void): DraftResult;

// src/editor/commit.ts:177-182 variable lastDryRun
/** The last dry run that would lay its road, with the map it was worked out on: the click on the same road lays its copy (`commitRoadPath`).
 *  Kept only while nothing of the document was written since (its whole diary's `version`: roads, money, land, buildings), the network
 *  is the one it read, the roads' last solve on the ground is the same object, and the road asked is the same, piece by piece. */
let lastDryRun: {
  readonly doc: RoadDoc; readonly net: Network; readonly version: number; readonly netRevision: number;
  readonly groundSolve: RoadElevation | null; readonly key: string;
  readonly work: RoadDoc; readonly workNet: Network; readonly bore: { bored: boolean; elevation: RoadElevation | null };
  readonly heightLimited: boolean; readonly finalHeightOffset: number;
} | null = null;

// src/editor/commit.ts:185-185 variable dryRunsReused
/** Roads laid from their dry run's copy, for the tests. */
export let dryRunsReused = 0;

// src/editor/commit.ts:188-191 function dryRunKey
/** The road asked, as a key: its ends, class, pieces (joined), lanes, parking, and whether the ground was handed in. */
function dryRunKey(start: Anchor, end: Anchor, type: number, pieces: readonly RoadPathPiece[], lanes: number | null,
  parking: SegmentParking | undefined, sampled: boolean): string;

// src/editor/commit.ts:369-515 function commitDraftInPlace
function commitDraftInPlace(
  doc: RoadDoc,
  net: Network,
  start: Anchor,
  end: Anchor,
  type: number,
  curve: CurveShape | null,
  structure: RoadStructure,
  heights?: { readonly start: number; readonly end: number; readonly smoothEnd?: boolean },
  lanes: number | null = null,
  parking?: SegmentParking,
): DraftResult;

// src/editor/commit.ts:768-814 function joinSegments
/** Joins two compatible straight segments meeting at an otherwise unused node. */
export function joinSegments(doc: RoadDoc, nodeId: NodeId): boolean;
