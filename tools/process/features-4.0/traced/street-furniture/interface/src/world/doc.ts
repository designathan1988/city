// What the feature uses of src/world/doc.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/doc.ts:77-77 type JunctionControl
/** Explicit junction policy. `auto` retains the class-based default policy. */
/** 'mini': a mini-roundabout: every entry gives way to whoever is in it or comes from the left. */
export type JunctionControl = 'auto' | 'signal' | 'stop' | 'yield' | 'priority' | 'none' | 'mini';

// src/world/doc.ts:89-113 interface RoadNode
export interface RoadNode {
  readonly id: NodeId;
  x: number;
  y: number;
  /** Authored height above or below the designed ground, in world units. */
  heightOffset: number;
  /** An internal control point of one continuous road gesture. */
  smooth: boolean;
  /** Materialized incidence list. */
  readonly incident: SegmentId[];
  control: JunctionControl;
  blockedMovements: string[];
  /** A pedestrian crossing placed mid-block, on a node that joins exactly two roads. */
  crossing?: NodeCrossing;
  /** The player's own lane connections at this node (`roads/connectors.ts`) */
  laneLinks?: readonly LaneLink[];
  /** Each leg's rule under 'priority' control (`roads/rules.ts`); absent legs: derived. */
  approachRules?: readonly ApproachRuleEntry[];
  /** A signal's settings (V5): fixed time, greens, offset, bus priority; absent: adaptive. */
  signal?: SignalSettings;
  /** What a road's end is: absent, the map's edge, where traffic leaves and comes in */
  end?: RoadEnd;
  /** A U-turn through an opening in the median (retorno) */
  uturn?: SegmentId;
}

// src/world/doc.ts:116-116 type RoadEnd
/** A road's end that is not the map's edge: a turning circle. */
export type RoadEnd = 'bulb';

// src/world/doc.ts:118-118 type NodeCrossingKind
export type NodeCrossingKind = 'zebra' | 'signal';

// src/world/doc.ts:119-122 interface NodeCrossing
export interface NodeCrossing {
  readonly kind: NodeCrossingKind;
  readonly segment: SegmentId;
}

// src/world/doc.ts:124-144 interface RoadSegment
export interface RoadSegment {
  readonly id: SegmentId;
  a: NodeId;
  b: NodeId;
  /** Null means a straight segment; see `CurveShape` for why `h` is absolute. */
  curve: CurveShape | null;
  type: number;
  /** Arc-length offset of this segment's start within the road it was drawn as. */
  dashOrigin: number;
  direction: SegmentDirection;
  /** Optional total travel-lane count; omitted means the class default. */
  lanes: number | null;
  /** Authored cross-section; absent preserves the exact class profile. */
  section?: RoadSection;
  /** On-street parking left and right of a -> b (`parking.ts`); absent means none. */
  parking?: SegmentParking;
  /** Vertical construction mode. Ground is the legacy/default value. */
  structure: RoadStructure;
  /** The sides of its cutting held by retaining walls instead of a batter */
  cutWalls?: true;
}

// src/world/doc.ts:153-2888 class RoadDoc
export class RoadDoc { … }

// src/world/doc.ts:154-154 field RoadDoc.nodes
readonly nodes: Map<NodeId, RoadNode> = new RecordMap();

// src/world/doc.ts:164-164 getter RoadDoc.poles
get poles(): ReadonlyMap<PoleId, UtilityPole>;

// src/world/doc.ts:165-165 getter RoadDoc.poleSpans
get poleSpans(): ReadonlyMap<SpanId, UtilitySpan>;

// src/world/doc.ts:169-169 field RoadDoc.barrierMap
/** Walls, fences and hedges drawn along paths (`barriers.ts`). */
  // DO NOT write this map directly.
  private barrierMap: Map<number, Barrier> = new RecordMap();

// src/world/doc.ts:170-170 getter RoadDoc.barriers
get barriers(): ReadonlyMap<number, Barrier>;

// src/world/doc.ts:172-172 field RoadDoc.barrierCells
/** The barriers by place (`barriersIn`), filed over the box of their points. */
  private barrierCells = new CellFiling<number, Barrier>(LANDSCAPE_CELL);

// src/world/doc.ts:177-177 field RoadDoc.landscapeMap
/** Street landscaping the player placed on the footways: trees, shrubs, benches, bins, street lights, hydrants, post boxes */
  // DO NOT write this map directly.
  private landscapeMap: Map<number, LandscapeItem> = new RecordMap();

// src/world/doc.ts:178-178 getter RoadDoc.landscape
get landscape(): ReadonlyMap<number, LandscapeItem>;

// src/world/doc.ts:180-180 field RoadDoc.landscapeCells
/** The street furniture by place (`landscapeIn`), in cells a few items wide. */
  private landscapeCells = new PointFiling<number>(LANDSCAPE_CELL);

// src/world/doc.ts:204-204 field RoadDoc.markingStyle
/** The regional standard of the map's road paint */
  markingStyle: MarkingStyleId = 'classic';

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:424-428 method RoadDoc.requireNode
requireNode(id: NodeId): RoadNode;

// src/world/doc.ts:430-434 method RoadDoc.requireSegment
requireSegment(id: SegmentId): RoadSegment;

// src/world/doc.ts:440-447 method RoadDoc.addPole
addPole(at: { x: number; y: number }, lamp = false): UtilityPole;

// src/world/doc.ts:450-452 method RoadDoc.polesIn
/** The poles standing in the cells a rectangle reaches (`PoleIndex.near`): a superset, test each. */
  polesIn(rect: ChangeRect): UtilityPole[];

// src/world/doc.ts:455-457 method RoadDoc.spansAt
/** The spans reaching a pole, in the order they were strung. */
  spansAt(id: PoleId): UtilitySpan[];

// src/world/doc.ts:498-509 method RoadDoc.addPoleSpan
/** Strings wire between two existing poles. Returns null for a degenerate run. */
  addPoleSpan(a: PoleId, b: PoleId): UtilitySpan | null;

// src/world/doc.ts:605-607 method RoadDoc.landscapeIn
/** The street furniture standing in the cells a rectangle reaches (a superset: test each), in increasing id order. */
  landscapeIn(rect: ChangeRect): LandscapeItem[];

// src/world/doc.ts:615-615 field RoadDoc.landscapeAt
/** Where a piece of street furniture stands, read from its record (`PointFiling`: the filing keeps no place of its own). */
  private readonly landscapeAt = (id: number): LandscapeItem => this.landscapeMap.get(id)!;

// src/world/doc.ts:654-668 method RoadDoc.removePole
/** Removes a pole and every wire that reached it. */
  removePole(id: PoleId): void;

// src/world/doc.ts:671-684 method RoadDoc.poleNear
/** The pole nearest a point, within `radius`, or null. */
  poleNear(at: { x: number; y: number }, radius: number): UtilityPole | null;

// src/world/doc.ts:691-695 method RoadDoc.editableNode
/** A node an edit may change, or undefined: DO NOT edit a copy of a node whose own tile is not loaded (a tile's edge node borrowed by a neighbour): the owner's record takes its place when that tile comes (`loadTile`), and the edit would be lost. */
  private editableNode(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:1407-1412 method RoadDoc.markNode
/** DO NOT bring them back: what changed is the diary's to say. */
  markNode(id: NodeId, traffic = true, also: readonly ChangeRect[] = []): void;

// src/world/doc.ts:1827-1831 method RoadDoc.fork
/** DO NOT go back to copying the document for an edit (CLAUDE.md, "Rebuilding everything when one part changes"). */
  fork(): RoadDoc;

// src/world/doc.ts:1847-1850 method RoadDoc.takeBack
/** Takes back an edit made on a working copy of this document (`fork`): what it wrote and its diary entries (`takeFork`). */
  takeBack(copy: RoadDoc): void;

// src/world/doc.ts:1953-1958 method RoadDoc.nodeToWrite
/** A node's record to change in place: a working copy's own (copied from the original's the first time), or the record itself. */
  private nodeToWrite(id: NodeId): RoadNode | undefined;
