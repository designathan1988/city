// What the feature uses of src/world/doc.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/doc.ts:64-70 function fitRoadCurve
/** `shape` flattened until no band of a road of this profile folds over (see `RoadDoc.fitCurve`). */
export function fitRoadCurve(
  a: Vec2, b: Vec2, shape: CurveShape | null, type: number,
  lanes: number | null = null, direction: SegmentDirection = 'both', section?: RoadSection,
  parking?: SegmentParking,
): CurveShape | null;

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

// src/world/doc.ts:155-155 field RoadDoc.segments
readonly segments: Map<SegmentId, RoadSegment> = new RecordMap();

// src/world/doc.ts:187-187 field RoadDoc.transit
/** Public transport the player laid out: bus stops and terminals, train and metro tracks and stations, lines */
  transit: TransitData = emptyTransit();

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:289-289 field RoadDoc.buildings
/** Modular buildings. */
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:297-297 field RoadDoc.lotStore
/** The land's lots (`world/lots.ts`): the cadastre the player zones and edits, with its owner (`lotStore.ts`) */
  readonly lotStore = new LotStore(this.changes);

// src/world/doc.ts:321-323 method RoadDoc.lotsIn
/** The lots whose box meets a rectangle (a superset: test each), in increasing id order. */
  lotsIn(rect: ChangeRect): Lot[];

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:424-428 method RoadDoc.requireNode
requireNode(id: NodeId): RoadNode;

// src/world/doc.ts:430-434 method RoadDoc.requireSegment
requireSegment(id: SegmentId): RoadSegment;

// src/world/doc.ts:686-688 method RoadDoc.degree
degree(id: NodeId): number;

// src/world/doc.ts:703-706 method RoadDoc.mapEdge
/** Whether a node is the map's edge: the end of one road, where traffic and people come in from outside and leave */
  mapEdge(id: NodeId): boolean;

// src/world/doc.ts:732-765 method RoadDoc.addSegment
addSegment(
    a: NodeId,
    b: NodeId,
    type: number,
    curve: CurveShape | null = null,
    dashOrigin = 0,
    direction: SegmentDirection = 'both',
    lanes: number | null = null,
    structure: RoadStructure = 'ground',
    section?: RoadSection,
    parking?: SegmentParking,
  ): RoadSegment | null;

// src/world/doc.ts:964-974 method RoadDoc.setSegmentParking
/** Parking on the two sides of a segment; none on either side removes it. */
  setSegmentParking(id: SegmentId, parking?: SegmentParking): void;

// src/world/doc.ts:1027-1036 method RoadDoc.setNodeApproachRules
/** Each leg's rule at a junction (`RoadNode.approachRules`); undefined: all derived. */
  setNodeApproachRules(id: NodeId, rules: readonly ApproachRuleEntry[] | undefined): void;

// src/world/doc.ts:1039-1048 method RoadDoc.setNodeSignal
/** A signal's settings (`RoadNode.signal`); undefined: adaptive with the game's timings. */
  setNodeSignal(id: NodeId, signal: SignalSettings | undefined): void;

// src/world/doc.ts:2053-2057 method RoadDoc.replaceFromJSON
/** Replaces this instance in place while restoring allocator invariants. */
  /** `preparedBuildings`: its buildings prepared by a worker (`editor/preparedCity.ts`), filed as they are; `buildingTable`: its buildings' table (`prepareCity`), the store's source of truth as it is; `whole`: another map opened, replacing this one whole (`replaceWhole`), never compared with it record by record. */
  replaceFromJSON(data: SerializedDoc, options: { readonly repair?: boolean; readonly preparedBuildings?: boolean; readonly buildingTable?: ArrayBufferLike; readonly whole?: boolean } = {}): void;

// src/world/doc.ts:2941-2941 type RecordPart
/** The document's keyed collections, its ordered lists and its single values, as undo restores them (`RoadDoc.restore`). */
export type RecordPart = 'nodes' | 'segments' | 'poles' | 'poleSpans' | 'landscape' | 'barriers' | 'buildings' | 'lots';

// src/world/doc.ts:2942-2943 type ListPart
export type ListPart = 'terrainStamps' | 'terrainPaint' | 'fogDabs' | 'gullyDabs' | 'clouds' | 'trees' | 'treeClearings' | 'elements'
  | 'zones' | 'zoneMarks' | 'lotKeys' | 'people';

// src/world/doc.ts:2944-2945 type ValuePart
export type ValuePart = 'terrainRelief' | 'markingStyle' | 'nature' | 'fogSettings' | 'gullyAuto' | 'weather' | 'economy' | 'transit'
  | 'nextZoneId' | 'nextLotId' | 'allocators';

// src/world/doc.ts:3029-3047 function segmentRecord
/** A stored road between nodes `a` and `b` as a document holds it. */
function segmentRecord(s: SerializedDoc['segments'][number], a: NodeId, b: NodeId): RoadSegment;

// src/world/doc.ts:3050-3064 function finishNode
/** What of a stored node names its roads, put on it once its roads are in (`incident`): a crossing only on a two-road node that still holds its road, the player's lane connections (kept as stored: a link to a lane that is gone is left out of the build, `lanelets.ts`), rules, signal, end, U-turn. */
function finishNode(node: RoadNode, n: SerializedDoc['nodes'][number]): void;

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

// src/world/doc.ts:3271-3277 function sameNode
/** Whether two versions of a node are the same, field for field. */
function sameNode(p: RoadNode, q: RoadNode): boolean;

// src/world/doc.ts:3280-3285 function sameSegment
/** Whether two versions of a segment are the same, field for field (its ends' positions aside). */
function sameSegment(p: RoadSegment, q: RoadSegment): boolean;

// src/world/doc.ts:3289-3301 function copyNode
/** Whether two documents hold the same nodes and segments, field for field. */
/** A node's record copied whole, nothing aliased (`clone`, `replaceContents`). */
function copyNode(node: RoadNode): RoadNode;

// src/world/doc.ts:3522-3524 function sameList
function sameList<T>(a: readonly T[], b: readonly T[]): boolean;

// src/world/doc.ts:3650-3661 function savedNode
/** A node as the file holds it (`toJSON`, a map saved by tiles): copied, never aliased. */
export function savedNode(n: RoadNode): SerializedDoc['nodes'][number];

// src/world/doc.ts:3672-3687 function savedSegment
/** A segment as the file holds it. */
export function savedSegment(s: RoadSegment): SerializedDoc['segments'][number];
