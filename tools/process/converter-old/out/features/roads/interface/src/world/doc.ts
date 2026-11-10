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

// src/world/doc.ts:83-87 function movementMentions
/** Whether a movement key names segment `id` as its entry or its exit. */
function movementMentions(key: string, id: SegmentId): boolean;

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

// src/world/doc.ts:164-164 getter RoadDoc.poles
get poles(): ReadonlyMap<PoleId, UtilityPole>;

// src/world/doc.ts:178-178 getter RoadDoc.landscape
get landscape(): ReadonlyMap<number, LandscapeItem>;

// src/world/doc.ts:182-182 field RoadDoc.streetNameCount
/** How many street name plates carry each name (`roads/streetNames.ts` gives a new street a name none has). */
  private streetNameCount = new Map<string, number>();

// src/world/doc.ts:200-200 field RoadDoc.terrainStamps
readonly terrainStamps: TerrainStamp[] = [];

// src/world/doc.ts:202-202 field RoadDoc.terrainRelief
/** The land this map was made on (`world/terrain.ts` `ReliefVersion`). */
  terrainRelief: ReliefVersion = RELIEF_LEGACY;

// src/world/doc.ts:204-204 field RoadDoc.markingStyle
/** The regional standard of the map's road paint */
  markingStyle: MarkingStyleId = 'classic';

// src/world/doc.ts:213-213 field RoadDoc.gullyDabs
/** Gullies cut or wiped with the brush (`gullies.ts`), oldest first, and how much of the steep land carries them of itself (0..1). */
  readonly gullyDabs: GullyDab[] = [];

// src/world/doc.ts:214-214 field RoadDoc.gullyAuto
gullyAuto = DEFAULT_GULLY_AUTO;

// src/world/doc.ts:218-218 field RoadDoc.weather
/** The map's weather (`weather.ts`). */
  weather: Weather = DEFAULT_WEATHER;

// src/world/doc.ts:220-220 field RoadDoc.economy
/** The money the roads are paid from (`economy.ts`). */
  economy: Economy = DEFAULT_ECONOMY;

// src/world/doc.ts:243-243 field RoadDoc.elements
/** DO NOT push or splice it from outside. */
  readonly elements: ElementItem[] = [];

// src/world/doc.ts:245-245 field RoadDoc.elementGrid
/** The elements by place (`elementsIn`): made when first asked, null after the whole list is replaced (as `treeGrid`). */
  private elementGrid: ItemGrid<ElementItem> | null = null;

// src/world/doc.ts:256-259 method RoadDoc.elementsIn
/** The elements standing in the cells a rectangle reaches (a superset: test each). */
  elementsIn(rect: ChangeRect): ElementItem[];

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:264-264 field RoadDoc.forkedFrom
/** The document this one was copied from (`clone`) and that document's diary serial then */
  private forkedFrom: { readonly doc: RoadDoc; readonly version: number } | null = null;

// src/world/doc.ts:266-266 field RoadDoc.below
/** The document this one is a working copy OVER (`fork`) */
  private below: RoadDoc | null = null;

// src/world/doc.ts:289-289 field RoadDoc.buildings
/** Modular buildings. */
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:371-371 getter RoadDoc.revision
/** The roads' geometry: every structural change; consumers use it to invalidate caches. */
  get revision(): number;

// src/world/doc.ts:396-402 method RoadDoc.segmentRect
/** A road segment's rectangle as drawn: its ends, its bulge and its width. */
  segmentRect(id: SegmentId): ChangeRect | null;

// src/world/doc.ts:405-414 method RoadDoc.nodeRect
/** A node's rectangle: the junction round it, out to its roads' widths. */
  nodeRect(id: NodeId): ChangeRect | null;

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:424-428 method RoadDoc.requireNode
requireNode(id: NodeId): RoadNode;

// src/world/doc.ts:430-434 method RoadDoc.requireSegment
requireSegment(id: SegmentId): RoadSegment;

// src/world/doc.ts:450-452 method RoadDoc.polesIn
/** The poles standing in the cells a rectangle reaches (`PoleIndex.near`): a superset, test each. */
  polesIn(rect: ChangeRect): UtilityPole[];

// src/world/doc.ts:498-509 method RoadDoc.addPoleSpan
/** Strings wire between two existing poles. Returns null for a degenerate run. */
  addPoleSpan(a: PoleId, b: PoleId): UtilitySpan | null;

// src/world/doc.ts:576-578 method RoadDoc.addLandscape
/** Places one landscaping item where `snapLandscape` put it. */
  addLandscape(kind: LandscapeKind, at: { x: number; y: number }, extra: { signType?: SignType; text?: string; planted?: number } = {}): LandscapeItem;

// src/world/doc.ts:596-602 method RoadDoc.removeLandscape
removeLandscape(id: number): boolean;

// src/world/doc.ts:605-607 method RoadDoc.landscapeIn
/** The street furniture standing in the cells a rectangle reaches (a superset: test each), in increasing id order. */
  landscapeIn(rect: ChangeRect): LandscapeItem[];

// src/world/doc.ts:610-612 method RoadDoc.landscapeInAny
/** The street furniture whose box reaches any of `rects`, each once, in increasing id order (`CellFiling.idsMeetingAny`). */
  landscapeInAny(rects: readonly ChangeRect[]): LandscapeItem[];

// src/world/doc.ts:654-668 method RoadDoc.removePole
/** Removes a pole and every wire that reached it. */
  removePole(id: PoleId): void;

// src/world/doc.ts:686-688 method RoadDoc.degree
degree(id: NodeId): number;

// src/world/doc.ts:691-695 method RoadDoc.editableNode
/** A node an edit may change, or undefined: DO NOT edit a copy of a node whose own tile is not loaded (a tile's edge node borrowed by a neighbour): the owner's record takes its place when that tile comes (`loadTile`), and the edit would be lost. */
  private editableNode(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:703-706 method RoadDoc.mapEdge
/** Whether a node is the map's edge: the end of one road, where traffic and people come in from outside and leave */
  mapEdge(id: NodeId): boolean;

// src/world/doc.ts:711-720 method RoadDoc.addNode
/** Adds a node, ON THE MAP. */
  addNode(p: Vec2, heightOffset = 0, smooth = false): RoadNode;

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

// src/world/doc.ts:767-785 method RoadDoc.removeSegment
removeSegment(id: SegmentId): void;

// src/world/doc.ts:810-817 method RoadDoc.removeNode
removeNode(id: NodeId): void;

// src/world/doc.ts:820-859 method RoadDoc.mergeNodes
/** Rewires every segment from `source` into `target`, then removes `source`. */
  mergeNodes(target: NodeId, source: NodeId): boolean;

// src/world/doc.ts:895-906 method RoadDoc.fitCurve
/** A curve is never tighter than its road is wide. */
  private fitCurve(s: RoadSegment): void;

// src/world/doc.ts:909-917 method RoadDoc.setSegmentType
setSegmentType(id: SegmentId, type: number): void;

// src/world/doc.ts:919-926 method RoadDoc.setSegmentCurve
setSegmentCurve(id: SegmentId, curve: CurveShape | null): void;

// src/world/doc.ts:928-937 method RoadDoc.setSegmentDirection
setSegmentDirection(id: SegmentId, direction: SegmentDirection): void;

// src/world/doc.ts:939-948 method RoadDoc.setSegmentLanes
setSegmentLanes(id: SegmentId, lanes: number | null): void;

// src/world/doc.ts:951-961 method RoadDoc.setSegmentSection
/** Changes every section consumer through the existing geometry/topology revision gates. */
  setSegmentSection(id: SegmentId, section?: RoadSection): void;

// src/world/doc.ts:964-974 method RoadDoc.setSegmentParking
/** Parking on the two sides of a segment; none on either side removes it. */
  setSegmentParking(id: SegmentId, parking?: SegmentParking): void;

// src/world/doc.ts:987-996 method RoadDoc.setNodeLaneLinks
/** The player's lane connections at a node; undefined or empty: all derived (`RoadNode.laneLinks`). */
  setNodeLaneLinks(id: NodeId, links: readonly LaneLink[] | undefined): void;

// src/world/doc.ts:999-1004 method RoadDoc.setMarkingStyle
/** The regional standard of the road paint: every road painted again. */
  setMarkingStyle(style: MarkingStyleId): void;

// src/world/doc.ts:1027-1036 method RoadDoc.setNodeApproachRules
/** Each leg's rule at a junction (`RoadNode.approachRules`); undefined: all derived. */
  setNodeApproachRules(id: NodeId, rules: readonly ApproachRuleEntry[] | undefined): void;

// src/world/doc.ts:1039-1048 method RoadDoc.setNodeSignal
/** A signal's settings (`RoadNode.signal`); undefined: adaptive with the game's timings. */
  setNodeSignal(id: NodeId, signal: SignalSettings | undefined): void;

// src/world/doc.ts:1087-1098 method RoadDoc.withRoom
/** The items of `items` their tiles have room for (`TILE_CAPS`), in order: DO NOT take an old item away to make room - the player's work is never deleted, the new item is refused (the caller says so). */
  private withRoom<T extends { readonly x: number; readonly y: number }>(part: CappedPart, items: readonly T[]): { readonly taken: T[]; readonly counts: Map<number, number> };

// src/world/doc.ts:1129-1137 method RoadDoc.addGullyDab
/** False: its tile is full, nothing laid. */
  addGullyDab(dab: GullyDab): boolean;

// src/world/doc.ts:1148-1152 method RoadDoc.setGullyAuto
setGullyAuto(amount: number): void;

// src/world/doc.ts:1347-1352 method RoadDoc.setNodeControl
setNodeControl(id: NodeId, control: JunctionControl): void;

// src/world/doc.ts:1374-1383 method RoadDoc.setMovementBlocked
setMovementBlocked(id: NodeId, from: SegmentId, to: SegmentId, blocked: boolean): void;

// src/world/doc.ts:1386-1402 method RoadDoc.pruneOrphanNodes
/** DO NOT walk every node of the map for it: every road taken away ends here. */
  pruneOrphanNodes(): number;

// src/world/doc.ts:1407-1412 method RoadDoc.markNode
/** DO NOT bring them back: what changed is the diary's to say. */
  markNode(id: NodeId, traffic = true, also: readonly ChangeRect[] = []): void;

// src/world/doc.ts:1415-1424 method RoadDoc.markSegment
/** DO NOT write only the segment's rectangle: what is derived round a junction would keep the old mouths. */
  markSegment(id: SegmentId, before: ChangeRect | null = null): void;

// src/world/doc.ts:1427-1429 method RoadDoc.rectsOrMap
/** A rectangle as the diary takes it: itself, or the whole map when there is none. */
  private rectsOrMap(rect: ChangeRect | null): readonly ChangeRect[] | null;

// src/world/doc.ts:1853-1899 method RoadDoc.layOver
/** Makes `copy` a working copy over this document as it stands (`fork`); also how a copy taken back starts again. */
  private layOver(copy: RoadDoc): void;

// src/world/doc.ts:1902-1950 method RoadDoc.takeFork
/** A working copy (`fork`) of this document taken back */
  private takeFork(copy: RoadDoc): void;

// src/world/doc.ts:1953-1958 method RoadDoc.nodeToWrite
/** A node's record to change in place: a working copy's own (copied from the original's the first time), or the record itself. */
  private nodeToWrite(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:1961-1966 method RoadDoc.segmentToWrite
/** A segment's record to change in place (as `nodeToWrite`). */
  private segmentToWrite(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:1969-1978 method RoadDoc.ownList
/** A list about to be written in place: a working copy's own, copied from the original's the first time (`fork`). */
  private ownList(part: DocListPart): void;

// src/world/doc.ts:2358-2372 method RoadDoc.valueOf
/** A value of the document by its part's name (`world/docImage.ts`). */
  valueOf(part: ValuePart): unknown;

// src/world/doc.ts:2374-2400 method RoadDoc.restoreValue
private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void;

// src/world/doc.ts:2404-2552 method RoadDoc.replaceContents
/** Takes `source`'s contents and says what differed: the roads, the land, the poles and the barriers */
  /** `whole`: another map (`replaceWhole`): everything taken, nothing compared, the changes written by the caller. */
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
    roads: boolean; land: boolean; barriers: boolean; others: DocChangeKind[];
  };

// src/world/doc.ts:2599-2616 method RoadDoc.savedGlobals
/** One part of what a map saved by tiles keeps outside its tiles (`editor/chunkedSave.ts`): only that part copied. */
  savedGlobals(part: GlobalPart): Partial<SavedGlobals>;

// src/world/doc.ts:2673-2698 method RoadDoc.worldPart
/** One of the map's parts no tile holds, as `toJSON` writes it (`WorldPart`): the streamed world writes each apart, only when its kind changed (`worldStreamer.ts`). */
  worldPart(part: WorldPart): Partial<SerializedDoc>;

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

// src/world/doc.ts:2947-2947 type DocListPart
/** The lists the document holds itself (the lots' keys are the lot store's): what a working copy shares until it writes (`RoadDoc.fork`). */
type DocListPart = Exclude<ListPart, 'lotKeys'>;

// src/world/doc.ts:2949-2949 type CappedPart
/** The lists with a cap, each item placed (`OWNER.at`). */
type CappedPart = 'terrainStamps' | 'terrainPaint' | 'fogDabs' | 'gullyDabs' | 'trees' | 'treeClearings' | 'elements';

// src/world/doc.ts:2951-2954 variable TILE_CAPS
/** DECISION 5.1 (the player's, 2026-10-17): each list's cap is a TILE's, and past it the new item is refused with a word to the player; nothing is ever taken away for it. */
const TILE_CAPS: Readonly<Record<CappedPart, number>> = {
  terrainStamps: MAX_TERRAIN_STAMPS, terrainPaint: MAX_PAINT_DABS, fogDabs: MAX_FOG_DABS, gullyDabs: MAX_GULLY_DABS,
  trees: MAX_PLANTED_TREES, treeClearings: MAX_TREE_CLEARINGS, elements: MAX_ELEMENTS,
};

// src/world/doc.ts:2960-2960 variable ROAD_KINDS
/** The kinds the roads' changes are written under (`roadsChanged`). */
const ROAD_KINDS: readonly DocChangeKind[] = ['roads', 'traffic'];

// src/world/doc.ts:3015-3015 function onWorld
/** A place on the world, copied and held within the world's reach (`bounds.ts` WORLD_HALF, thousands of km: no map edge, only numbers kept finite). */
const onWorld = (p: { readonly x: number; readonly y: number }): { x: number; y: number } => …;

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

// src/world/doc.ts:3178-3178 type WorldPart
/** The map's parts no tile holds (`RoadDoc.worldPart`), each named as the diary's kind that changes it (`allocators`: the map's own id allocators). */
export type WorldPart = 'terrain' | 'markingStyle' | 'nature' | 'fog' | 'gullies' | 'clouds' | 'weather' | 'clearings' | 'zones' | 'people' | 'transit' | 'economy' | 'allocators';

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

// src/world/doc.ts:3263-3268 function normaliseLaneCount
function normaliseLaneCount(lanes: number | null, direction: SegmentDirection): number | null;

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

// src/world/doc.ts:3304-3311 function copySegment
/** A segment's record copied whole (`clone`, `replaceContents`). */
function copySegment(segment: RoadSegment): RoadSegment;

// src/world/doc.ts:3324-3325 function circleRect
const circleRect = (c: { readonly x: number; readonly y: number; readonly radius: number }): ChangeRect => …;

// src/world/doc.ts:3338-3342 function elementsBox
/** The box round elements, grown by the largest one's size; null for none. */
function elementsBox(items: readonly ElementItem[]): ChangeRect | null;

// src/world/doc.ts:3378-3378 type MovedKinds
/** Whether a source document changed any of these kinds since it was copied (`RoadDoc.movedIn`). */
type MovedKinds = (...kinds: readonly DocChangeKind[]) => boolean;

// src/world/doc.ts:3487-3513 class ItemGrid
/** Things standing at a point (an element of the brush), by the cell of their point */
class ItemGrid<T extends { … }

// src/world/doc.ts:3496-3502 method ItemGrid.remove
remove(item: T): void;

// src/world/doc.ts:3516-3520 function keepWhere
/** Keeps in `list`, in order and in place, the items `keep` says; one pass. */
function keepWhere<T>(list: T[], keep: (item: T) => boolean): void;

// src/world/doc.ts:3522-3524 function sameList
function sameList<T>(a: readonly T[], b: readonly T[]): boolean;

// src/world/doc.ts:3530-3540 function sameStamps
/** Two stamp lists that shape the same land (stamp by stamp, field by field). */
export function sameStamps(a: readonly TerrainStamp[], b: readonly TerrainStamp[]): boolean;

// src/world/doc.ts:3543-3550 function dropStaleCrossing
/** A crossing stays only on a two-road node that still holds its segment. */
function dropStaleCrossing(node: RoadNode | undefined): void;

// src/world/doc.ts:3630-3630 variable GLOBAL_PARTS
/** The parts of what a map saved by tiles keeps outside its tiles, each written again only when its kinds change: the single values, the land's keys, the people, the transport. */
export const GLOBAL_PARTS = ['values', 'lotKeys', 'people', 'transit'] as const;

// src/world/doc.ts:3631-3631 type GlobalPart
export type GlobalPart = (typeof GLOBAL_PARTS)[number];

// src/world/doc.ts:3634-3647 interface SavedGlobals
/** What a map saved by tiles keeps outside its tiles (`RoadDoc.savedGlobals`): what has no place on the map. */
export interface SavedGlobals {
  readonly version: 1;
  readonly unit?: number;
  readonly relief?: number;
  readonly markingStyle?: string;
  readonly nature?: { readonly region: string; readonly seed: number };
  readonly weather?: unknown;
  readonly fogSettings?: unknown;
  readonly gullyAuto?: number;
  readonly lotKeys?: readonly string[];
  readonly people?: readonly unknown[];
  readonly transit?: unknown;
  readonly economy?: { readonly balance: number };
}

// src/world/doc.ts:3650-3661 function savedNode
/** A node as the file holds it (`toJSON`, a map saved by tiles): copied, never aliased. */
export function savedNode(n: RoadNode): SerializedDoc['nodes'][number];

// src/world/doc.ts:3672-3687 function savedSegment
/** A segment as the file holds it. */
export function savedSegment(s: RoadSegment): SerializedDoc['segments'][number];

// src/world/doc.ts:3698-3700 function pushAll
/** Each of `items` pushed onto `into`, in order. DO NOT spread a whole map's list into `push` (EDT-14): its arguments past the engine's stack (~100 000) threw RangeError. */
function pushAll<T>(into: T[], items: readonly T[]): void;
