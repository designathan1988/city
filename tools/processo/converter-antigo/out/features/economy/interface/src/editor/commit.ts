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

// src/editor/commit.ts:60-60 variable STATION_STEP
/** Spacing of a dry run's stations (`DraftResult.stations`). */
export const STATION_STEP = m(2);

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

// src/editor/commit.ts:254-277 function draftStations
/** The stations of the road a gesture laid, on the copy it was laid on: the new segments that lie on the drawn pieces */
function draftStations(
  before: RoadDoc, work: RoadDoc, workNet: Network, pieces: readonly RoadPathPiece[], type: number, startAt: Vec2,
  sampled: ((x: number, y: number) => number) | undefined,
  bore: { bored: boolean; elevation: RoadElevation | null }, previous: RoadElevation | null,
): RoadStation[];

// src/editor/commit.ts:747-765 function moveNodeChecked
/** A node dropped by the Move tool at `to`, reconciled with the network */
export function moveNodeChecked(doc: RoadDoc, net: Network, id: NodeId, to: Vec2): DraftResult;
