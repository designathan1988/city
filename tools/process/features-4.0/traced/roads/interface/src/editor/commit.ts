// What the feature uses of src/editor/commit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/commit.ts:34-34 variable MERGE_EPS
/** Two nodes closer than this are the same node. */
const MERGE_EPS = m(1.04);

// src/editor/commit.ts:36-36 variable CROSS_PAD
/** Slack round a piece's box when looking for crossings in it: only ever lets more pairs through to `segSeg`, which decides. */
const CROSS_PAD = m(1e-3);

// src/editor/commit.ts:38-38 variable HEIGHT_JOIN_EPS
/** Distinct decks at the same map point remain separate networks. */
const HEIGHT_JOIN_EPS = m(0.3);

// src/editor/commit.ts:40-40 variable CROSSING_CLEARANCE
/** Vertical room required before two crossing carriageways can pass independently. */
const CROSSING_CLEARANCE = ROAD_TUNING.clearance.elevated;

// src/editor/commit.ts:42-42 variable AUTO_TUNNEL_COVER
/** Cover over a road at grade past which it is bored as a tunnel instead of cut */
const AUTO_TUNNEL_COVER = ROAD_TUNING.tunnel.autoCover;

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

// src/editor/commit.ts:195-195 variable onTerrain
/** Turns the roads just laid at grade into tunnels wherever their profile would run deeper than `AUTO_TUNNEL_COVER` under */
/** The terrain's index as last built, and the last solve on it (no ground handed in): a road edit does not move the land, so the next reuses both. */
let onTerrain: { readonly index: TerrainIndex; solve: RoadElevation | null } | null = null;

// src/editor/commit.ts:196-201 function terrainIndexOf
function terrainIndexOf(doc: RoadDoc): TerrainIndex;

// src/editor/commit.ts:204-212 function solveOver
/** A solve of `net` over `ground`: from the game's last on the same natural ground, or from the last on the same index */
function solveOver(net: Network, ground: (x: number, y: number) => number, sampled: boolean, index: TerrainIndex | null,
  previous: RoadElevation | null): RoadElevation;

// src/editor/commit.ts:214-251 function boreDeepCuts
function boreDeepCuts(before: RoadDoc, work: RoadDoc, workNet: Network, sampled?: (x: number, y: number) => number,
  previous: RoadElevation | null = null): { bored: boolean; elevation: RoadElevation | null };

// src/editor/commit.ts:254-277 function draftStations
/** The stations of the road a gesture laid, on the copy it was laid on: the new segments that lie on the drawn pieces */
function draftStations(
  before: RoadDoc, work: RoadDoc, workNet: Network, pieces: readonly RoadPathPiece[], type: number, startAt: Vec2,
  sampled: ((x: number, y: number) => number) | undefined,
  bore: { bored: boolean; elevation: RoadElevation | null }, previous: RoadElevation | null,
): RoadStation[];

// src/editor/commit.ts:280-298 function joinShortPieces
/** A piece too short to be a road is folded into the next one (the last into the one before) */
function joinShortPieces(pieces: readonly RoadPathPiece[]): readonly RoadPathPiece[];

// src/editor/commit.ts:300-306 interface DraftStop
interface DraftStop {
  readonly node: NodeId;
  /** Quadratic parameter on the drafted road. */
  readonly q: number;
  /** Flattened arc length on the drafted road, for dash continuity. */
  readonly s: number;
}

// src/editor/commit.ts:315-319 interface TaggedCut
interface TaggedCut<Tag> {
  readonly at: Vec2;
  readonly s: number;
  readonly tag: Tag;
}

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

// src/editor/commit.ts:543-554 function normalizeDraftStops
function normalizeDraftStops(stops: readonly DraftStop[]): DraftStop[];

// src/editor/commit.ts:556-581 function alreadyJoined
function alreadyJoined(
  doc: RoadDoc,
  a: NodeId,
  b: NodeId,
  candidate: CurveShape | null,
  structure: RoadStructure = 'ground',
): boolean;

// src/editor/commit.ts:584-634 function materializeEndpoints
/** Materializes both anchors together so two cuts on one segment stay valid. */
function materializeEndpoints(
  doc: RoadDoc,
  net: Network,
  start: Anchor,
  end: Anchor,
  heights?: { readonly start: number; readonly end: number; readonly smoothEnd?: boolean },
): [NodeId, NodeId] | null;

// src/editor/commit.ts:637-652 function materializeFree
/** The node a free anchor lands on. */
function materializeFree(doc: RoadDoc, net: Network, at: Vec2, heightOffset = 0, smooth = false,
  /** Nodes this draft has already put down, with no road yet (the other end). */
  fresh: readonly NodeId[] = []): NodeId;

// src/editor/commit.ts:654-659 function segmentOffsetAt
function segmentOffsetAt(doc: RoadDoc, segment: NonNullable<ReturnType<RoadDoc['segment']>>, s: number, length: number): number;

// src/editor/commit.ts:662-671 function splitSegment
/** Splits a segment at an arc position, returning the new or endpoint node. */
export function splitSegment(
  doc: RoadDoc,
  net: Network,
  id: SegmentId,
  s: number,
  at: Vec2,
): NodeId | null;

// src/editor/commit.ts:768-814 function joinSegments
/** Joins two compatible straight segments meeting at an otherwise unused node. */
export function joinSegments(doc: RoadDoc, nodeId: NodeId): boolean;

// src/editor/commit.ts:817-853 function duplicateSegment
/** Creates a disconnected parallel copy of a road for rapid layout iteration. */
export function duplicateSegment(doc: RoadDoc, net: Network, id: SegmentId): SegmentId | null;

// src/editor/commit.ts:856-966 function splitSegmentAtCuts
/** Splits one original segment at any number of positions. */
function splitSegmentAtCuts<Tag>(
  doc: RoadDoc,
  net: Network,
  id: SegmentId,
  requests: readonly TaggedCut<Tag>[],
): Map<Tag, NodeId>;

// src/editor/commit.ts:968-971 function parameterAtArc
function parameterAtArc(polyline: Polyline, s: number): number;

// src/editor/commit.ts:973-987 function curveShapeForRange
function curveShapeForRange(
  originalA: Vec2,
  originalB: Vec2,
  curve: CurveShape | null,
  actualA: Vec2,
  actualB: Vec2,
  t0: number,
  t1: number,
): CurveShape | null;

// src/editor/commit.ts:1006-1008 function validPoint
function validPoint(point: Vec2): boolean;
