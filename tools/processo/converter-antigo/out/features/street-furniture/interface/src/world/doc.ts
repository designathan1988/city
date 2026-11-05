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

// src/world/doc.ts:148-151 function rectsAlong
/** The authoring document: what the user drew, before any derived geometry. */
/** The place a run of points (a wall, a span of wire) covers, for the diary of changes: one box round them all. */
function rectsAlong(points: readonly { readonly x: number; readonly y: number }[], pad: number): ChangeRect[];

// src/world/doc.ts:153-2888 class RoadDoc
export class RoadDoc { … }

// src/world/doc.ts:154-154 field RoadDoc.nodes
readonly nodes: Map<NodeId, RoadNode> = new RecordMap();

// src/world/doc.ts:161-161 field RoadDoc.poleMap
/** The overhead utility network: poles and the wire runs between them. */
  // DO NOT write these maps directly.
  private poleMap: Map<PoleId, UtilityPole> = new RecordMap();

// src/world/doc.ts:162-162 field RoadDoc.spanMap
private spanMap: Map<SpanId, UtilitySpan> = new RecordMap();

// src/world/doc.ts:163-163 field RoadDoc.poleIndex
private poleIndex = new PoleIndex();

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

// src/world/doc.ts:459-463 method RoadDoc.putPoleRecord
private putPoleRecord(pole: UtilityPole): void;

// src/world/doc.ts:465-469 method RoadDoc.deletePoleRecord
private deletePoleRecord(id: PoleId): void;

// src/world/doc.ts:471-479 method RoadDoc.putSpanRecord
private putSpanRecord(span: UtilitySpan): void;

// src/world/doc.ts:481-487 method RoadDoc.deleteSpanRecord
private deleteSpanRecord(id: SpanId): void;

// src/world/doc.ts:489-495 method RoadDoc.clearPoleRecords
private clearPoleRecords(): void;

// src/world/doc.ts:498-509 method RoadDoc.addPoleSpan
/** Strings wire between two existing poles. Returns null for a degenerate run. */
  addPoleSpan(a: PoleId, b: PoleId): UtilitySpan | null;

// src/world/doc.ts:512-519 method RoadDoc.addBarrier
/** Draws a barrier along `points` (two or more, on the map). */
  addBarrier(kind: BarrierKind, points: readonly { x: number; y: number }[]): Barrier | null;

// src/world/doc.ts:521-527 method RoadDoc.removeBarrier
removeBarrier(id: number): boolean;

// src/world/doc.ts:535-537 method RoadDoc.barriersIn
barriersIn(rect: ChangeRect): Barrier[];

// src/world/doc.ts:540-544 method RoadDoc.putBarrierRecord
/** A barrier put (new, or replacing the one of its id, which keeps its place in the map's order). */
  private putBarrierRecord(barrier: Barrier): void;

// src/world/doc.ts:546-550 method RoadDoc.deleteBarrierRecord
private deleteBarrierRecord(id: number): void;

// src/world/doc.ts:552-556 method RoadDoc.clearBarrierRecords
private clearBarrierRecords(): void;

// src/world/doc.ts:559-573 method RoadDoc.barrierNear
/** The barrier whose path passes nearest a point, within `radius`, or null: those round it only (`barriersIn`). */
  barrierNear(at: { x: number; y: number }, radius: number): Barrier | null;

// src/world/doc.ts:576-578 method RoadDoc.addLandscape
/** Places one landscaping item where `snapLandscape` put it. */
  addLandscape(kind: LandscapeKind, at: { x: number; y: number }, extra: { signType?: SignType; text?: string; planted?: number } = {}): LandscapeItem;

// src/world/doc.ts:596-602 method RoadDoc.removeLandscape
removeLandscape(id: number): boolean;

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

// src/world/doc.ts:1027-1036 method RoadDoc.setNodeApproachRules
/** Each leg's rule at a junction (`RoadNode.approachRules`); undefined: all derived. */
  setNodeApproachRules(id: NodeId, rules: readonly ApproachRuleEntry[] | undefined): void;

// src/world/doc.ts:1347-1352 method RoadDoc.setNodeControl
setNodeControl(id: NodeId, control: JunctionControl): void;

// src/world/doc.ts:1407-1412 method RoadDoc.markNode
/** DO NOT bring them back: what changed is the diary's to say. */
  markNode(id: NodeId, traffic = true, also: readonly ChangeRect[] = []): void;

// src/world/doc.ts:1827-1831 method RoadDoc.fork
/** DO NOT go back to copying the document for an edit (CLAUDE.md, "Rebuilding everything when one part changes"). */
  fork(): RoadDoc;

// src/world/doc.ts:1847-1850 method RoadDoc.takeBack
/** Takes back an edit made on a working copy of this document (`fork`): what it wrote and its diary entries (`takeFork`). */
  takeBack(copy: RoadDoc): void;

// src/world/doc.ts:1853-1899 method RoadDoc.layOver
/** Makes `copy` a working copy over this document as it stands (`fork`); also how a copy taken back starts again. */
  private layOver(copy: RoadDoc): void;

// src/world/doc.ts:1902-1950 method RoadDoc.takeFork
/** A working copy (`fork`) of this document taken back */
  private takeFork(copy: RoadDoc): void;

// src/world/doc.ts:1953-1958 method RoadDoc.nodeToWrite
/** A node's record to change in place: a working copy's own (copied from the original's the first time), or the record itself. */
  private nodeToWrite(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:1987-2000 method RoadDoc.ownIndex
/** An index by place about to be written: a working copy's own, copied from the original's the first time (`fork`). */
  private ownIndex(kind: IndexKind): void;

// src/world/doc.ts:2233-2337 method RoadDoc.restore
restore(r: Restoration): void;

// src/world/doc.ts:2404-2552 method RoadDoc.replaceContents
/** Takes `source`'s contents and says what differed: the roads, the land, the poles and the barriers */
  /** `whole`: another map (`replaceWhole`): everything taken, nothing compared, the changes written by the caller. */
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
    roads: boolean; land: boolean; barriers: boolean; others: DocChangeKind[];
  };

// src/world/doc.ts:2636-2670 method RoadDoc.serialized
/** `toJSON`, the buildings left out when `withBuildings` is false (`clone` shares them); 'apart': their list written empty, in its place (their table goes apart, `toJSONBuildingsApart`). */
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc;

// src/world/doc.ts:2701-2887 method RoadDoc.fromJSON
/** Builds a document from its serialized form. */
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc;

// src/world/doc.ts:2941-2941 type RecordPart
/** The document's keyed collections, its ordered lists and its single values, as undo restores them (`RoadDoc.restore`). */
export type RecordPart = 'nodes' | 'segments' | 'poles' | 'poleSpans' | 'landscape' | 'barriers' | 'buildings' | 'lots';

// src/world/doc.ts:2942-2943 type ListPart
export type ListPart = 'terrainStamps' | 'terrainPaint' | 'fogDabs' | 'gullyDabs' | 'clouds' | 'trees' | 'treeClearings' | 'elements'
  | 'zones' | 'zoneMarks' | 'lotKeys' | 'people';

// src/world/doc.ts:2944-2945 type ValuePart
export type ValuePart = 'terrainRelief' | 'markingStyle' | 'nature' | 'fogSettings' | 'gullyAuto' | 'weather' | 'economy' | 'transit'
  | 'nextZoneId' | 'nextLotId' | 'allocators';

// src/world/doc.ts:2957-2957 type IndexKind
/** The indices by place a working copy reads from the original until it writes their kind (`RoadDoc.ownIndex`). */
type IndexKind = 'poles' | 'barriers' | 'landscape';

// src/world/doc.ts:3080-3085 function barrierRecord
/** A stored wall, fence or hedge, or null when it is not one (fewer than two points). */
function barrierRecord(raw: NonNullable<SerializedDoc['barriers']>[number], repair: boolean): Barrier | null;

// src/world/doc.ts:3169-3175 interface Restoration
export interface Restoration {
  readonly records: readonly { readonly part: RecordPart; readonly id: number; readonly value: unknown }[];
  /** The whole map as its image, when the edit replaced it whole (`RoadDoc.replaceWith`): put back as it is, nothing else. */
  readonly whole?: DocImage;
  readonly lists: readonly { readonly part: ListPart; readonly start: number; readonly remove: number; readonly items: readonly unknown[] }[];
  readonly values: readonly { readonly part: ValuePart; readonly value: unknown }[];
}

// src/world/doc.ts:3181-3255 interface SerializedDoc
export interface SerializedDoc {
  readonly version: 1;
  /** Metres per world unit the map was saved in (`world/units.ts`); absent: 0.4, every map before 2026-10-10 (`rescale.ts`). */
  readonly unit?: number;
  readonly nodes: readonly {
    id: number; x: number; y: number; heightOffset?: number; smooth?: boolean;
    control?: JunctionControl; blockedMovements?: readonly string[];
    crossing?: { kind: NodeCrossingKind; segment: number };
    /** The player's lane connections; absent on every older map. */
    laneLinks?: readonly { from: number; fromLane: number; to: number; toLane: number }[];
    /** Each leg's rule; absent on every older map. */
    approachRules?: readonly { segment: number; rule: string }[];
    /** A signal's settings (V5); absent on every older map. */
    signal?: unknown;
    /** A road's end that is a turning circle (V8); absent: the map's edge. */
    end?: string;
    /** A U-turn through the median (V8): the road its traffic arrives from; absent on every older map. */
    uturn?: number;
  }[];
  readonly segments: readonly {
    id: number;
    a: number;
    b: number;
    type: number;
    curve: CurveShape | null;
    dashOrigin?: number;
    direction?: SegmentDirection;
    lanes?: number | null;
    section?: RoadSection;
    parking?: SegmentParking;
    /** A current structure id, or a legacy one `migrateStructure` maps. */
    structure?: RoadStructure | 'viaduct';
    /** Retaining walls in its cutting; absent on every older map. */
    cutWalls?: boolean;
  }[];
  readonly terrain?: readonly TerrainStamp[];
  /** `ReliefVersion`; absent on maps made before the natural landform. */
  readonly relief?: number;
  /** The regional standard of the road paint; absent: 'classic'. */
  readonly markingStyle?: string;
  /** The ecosystem (`NatureSettings`); absent on maps made before it. */
  readonly nature?: { readonly region: string; readonly seed: number };
  /** The utility network. */
  readonly poles?: readonly { id: number; x: number; y: number; lamp?: boolean }[];
  readonly poleSpans?: readonly { id: number; a: number; b: number }[];
  /** Ground painted over the terrain (`terrainPaint.ts`); OPTIONAL. */
  readonly paint?: readonly { kind: string; x: number; y: number; radius: number; strength: number }[];
  readonly fog?: { readonly dabs?: readonly unknown[]; readonly settings?: unknown };
  readonly gullies?: { readonly dabs?: readonly unknown[]; readonly auto?: unknown };
  readonly clouds?: readonly unknown[];
  readonly elements?: readonly unknown[];
  readonly trees?: readonly unknown[];
  readonly weather?: unknown;
  readonly treeClearings?: readonly unknown[];
  /** The player's landscaping (`landscape.ts`); OPTIONAL like the poles. */
  readonly landscape?: readonly { id: number; kind: string; x: number; y: number; signType?: string; text?: string; planted?: number }[];
  /** Public transport (`transit.ts`); OPTIONAL like the poles. */
  readonly transit?: unknown;
  /** Walls, fences and hedges (`barriers.ts`); OPTIONAL like the poles. */
  readonly barriers?: readonly { id: number; kind: string; points: readonly { x: number; y: number }[] }[];
  /** Modular buildings. */
  readonly buildings?: readonly SerializedBuilding[];
  /** Roadside land-use strokes; absent in maps saved before zoning. */
  readonly zones?: readonly Zone[];
  readonly zoneMarks?: readonly ZoneMark[];
  /** The lots (`lots.ts`); absent in maps saved before them. */
  readonly lots?: readonly Lot[];
  readonly lotKeys?: readonly string[];
  /** People from the Person Creator; OPTIONAL like the buildings. Normalised on load. */
  readonly people?: readonly unknown[];
  /** The balance (`economy.ts`); absent at the starting balance. */
  readonly economy?: { readonly balance: number };
  /** The map's own allocators' next ids (`allocatorMarks`): written with the streamed world's values only (`worldValues`). */
  readonly allocators?: readonly number[];
}

// src/world/doc.ts:3320-3320 variable LANDSCAPE_CELL
/** Side of the cells the street furniture is filed in (`RoadDoc.landscapeIn`), world units. */
const LANDSCAPE_CELL = m(32);

// src/world/doc.ts:3322-3322 variable ITEM_PAD
/** Around a pole, a bench, a tree, a fence: what stands round it. */
const ITEM_PAD = 6;

// src/world/doc.ts:3378-3378 type MovedKinds
/** Whether a source document changed any of these kinds since it was copied (`RoadDoc.movedIn`). */
type MovedKinds = (...kinds: readonly DocChangeKind[]) => boolean;

// src/world/doc.ts:3439-3471 function itemDifferences
/** Where two documents' poles and wires, landscaping and barriers differ. */
function itemDifferences(a: RoadDoc, b: RoadDoc, moved: MovedKinds): { utilities: ChangeRect[]; landscape: ChangeRect[]; barriers: ChangeRect[] };

// src/world/doc.ts:3474-3484 function sameBarriers
/** Whether two documents hold the same barriers, point for point. */
function sameBarriers(a: RoadDoc, b: RoadDoc): boolean;

// src/world/doc.ts:3650-3661 function savedNode
/** A node as the file holds it (`toJSON`, a map saved by tiles): copied, never aliased. */
export function savedNode(n: RoadNode): SerializedDoc['nodes'][number];

// src/world/doc.ts:3664-3669 function inIdOrder
/** DO NOT write records in the map's insertion order: an undo puts a removed record back at the end, and the map saved would not be the map it undid to. The file holds them by id (a load's order). */
function inIdOrder<T extends { readonly id: number }>(records: Iterable<T>): T[];

// src/world/doc.ts:3672-3687 function savedSegment
/** A segment as the file holds it. */
export function savedSegment(s: RoadSegment): SerializedDoc['segments'][number];

// src/world/doc.ts:3690-3690 function savedPole
/** A pole, a span, a landscaping item and a barrier as the file holds them. */
export const savedPole = (p: UtilityPole): NonNullable<SerializedDoc['poles']>[number] => …;

// src/world/doc.ts:3691-3691 function savedSpan
export const savedSpan = (s: UtilitySpan): NonNullable<SerializedDoc['poleSpans']>[number] => …;

// src/world/doc.ts:3692-3694 function savedLandscape
export const savedLandscape = (item: LandscapeItem): NonNullable<SerializedDoc['landscape']>[number] => …;

// src/world/doc.ts:3695-3695 function savedBarrier
export const savedBarrier = (b: Barrier): NonNullable<SerializedDoc['barriers']>[number] => …;
