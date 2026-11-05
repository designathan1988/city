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

// src/world/doc.ts:177-177 field RoadDoc.landscapeMap
/** Street landscaping the player placed on the footways: trees, shrubs, benches, bins, street lights, hydrants, post boxes */
  // DO NOT write this map directly.
  private landscapeMap: Map<number, LandscapeItem> = new RecordMap();

// src/world/doc.ts:178-178 getter RoadDoc.landscape
get landscape(): ReadonlyMap<number, LandscapeItem>;

// src/world/doc.ts:200-200 field RoadDoc.terrainStamps
readonly terrainStamps: TerrainStamp[] = [];

// src/world/doc.ts:202-202 field RoadDoc.terrainRelief
/** The land this map was made on (`world/terrain.ts` `ReliefVersion`). */
  terrainRelief: ReliefVersion = RELIEF_LEGACY;

// src/world/doc.ts:206-206 field RoadDoc.nature
/** The map's ecosystem (`world/ecology.ts`): its biome and seed. */
  nature: NatureSettings | null = null;

// src/world/doc.ts:208-208 field RoadDoc.terrainPaint
/** Ground painted over the terrain (`terrainPaint.ts`), oldest first. */
  readonly terrainPaint: PaintDab[] = [];

// src/world/doc.ts:210-210 field RoadDoc.fogDabs
/** Fog painted over the land (`fogPaint.ts`), oldest first, and how it behaves. */
  readonly fogDabs: FogDab[] = [];

// src/world/doc.ts:211-211 field RoadDoc.fogSettings
fogSettings: FogSettings = DEFAULT_FOG;

// src/world/doc.ts:213-213 field RoadDoc.gullyDabs
/** Gullies cut or wiped with the brush (`gullies.ts`), oldest first, and how much of the steep land carries them of itself (0..1). */
  readonly gullyDabs: GullyDab[] = [];

// src/world/doc.ts:214-214 field RoadDoc.gullyAuto
gullyAuto = DEFAULT_GULLY_AUTO;

// src/world/doc.ts:216-216 field RoadDoc.clouds
/** Clouds the player placed in the sky (`clouds.ts`). */
  readonly clouds: PlacedCloud[] = [];

// src/world/doc.ts:218-218 field RoadDoc.weather
/** The map's weather (`weather.ts`). */
  weather: Weather = DEFAULT_WEATHER;

// src/world/doc.ts:222-222 field RoadDoc.trees
/** Trees the player planted (`trees.ts`), oldest first. */
  readonly trees: PlantedTree[] = [];

// src/world/doc.ts:224-224 field RoadDoc.treeGrid
/** The planted trees by place (`treesIn`), made when first asked and then kept by the writers */
  private treeGrid: ItemGrid<PlantedTree> | null = null;

// src/world/doc.ts:226-232 getter RoadDoc.treeCells
/** @rebuild The whole list filed only on the first ask after it was replaced whole; the writers keep it after. */
  private get treeCells(): ItemGrid<PlantedTree>;

// src/world/doc.ts:235-239 method RoadDoc.treesIn
/** The planted trees standing in the cells a rectangle reaches (a superset: test each). */
  treesIn(rect: ChangeRect): PlantedTree[];

// src/world/doc.ts:241-241 field RoadDoc.treeClearings
/** Where trees were cut away, the woods' own too (`trees.ts`), oldest first. */
  readonly treeClearings: TreeClearing[] = [];

// src/world/doc.ts:243-243 field RoadDoc.elements
/** DO NOT push or splice it from outside. */
  readonly elements: ElementItem[] = [];

// src/world/doc.ts:245-245 field RoadDoc.elementGrid
/** The elements by place (`elementsIn`): made when first asked, null after the whole list is replaced (as `treeGrid`). */
  private elementGrid: ItemGrid<ElementItem> | null = null;

// src/world/doc.ts:247-253 getter RoadDoc.elementCells
/** @rebuild The whole list filed only on the first ask after it was replaced whole; the writers keep it after. */
  private get elementCells(): ItemGrid<ElementItem>;

// src/world/doc.ts:256-259 method RoadDoc.elementsIn
/** The elements standing in the cells a rectangle reaches (a superset: test each). */
  elementsIn(rect: ChangeRect): ElementItem[];

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:374-374 getter RoadDoc.terrainRevision
get terrainRevision(): number;

// src/world/doc.ts:381-381 getter RoadDoc.natureRevision
get natureRevision(): number;

// src/world/doc.ts:382-382 getter RoadDoc.paintRevision
get paintRevision(): number;

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:430-434 method RoadDoc.requireSegment
requireSegment(id: SegmentId): RoadSegment;

// src/world/doc.ts:576-578 method RoadDoc.addLandscape
/** Places one landscaping item where `snapLandscape` put it. */
  addLandscape(kind: LandscapeKind, at: { x: number; y: number }, extra: { signType?: SignType; text?: string; planted?: number } = {}): LandscapeItem;

// src/world/doc.ts:581-594 method RoadDoc.addLandscapeMany
/** DO NOT place a batch item by item with `addLandscape` */
  addLandscapeMany(list: readonly { readonly kind: LandscapeKind; readonly at: { x: number; y: number };
    readonly extra?: { signType?: SignType; text?: string; planted?: number } }[]): LandscapeItem[];

// src/world/doc.ts:596-602 method RoadDoc.removeLandscape
removeLandscape(id: number): boolean;

// src/world/doc.ts:605-607 method RoadDoc.landscapeIn
/** The street furniture standing in the cells a rectangle reaches (a superset: test each), in increasing id order. */
  landscapeIn(rect: ChangeRect): LandscapeItem[];

// src/world/doc.ts:610-612 method RoadDoc.landscapeInAny
/** The street furniture whose box reaches any of `rects`, each once, in increasing id order (`CellFiling.idsMeetingAny`). */
  landscapeInAny(rects: readonly ChangeRect[]): LandscapeItem[];

// src/world/doc.ts:615-615 field RoadDoc.landscapeAt
/** Where a piece of street furniture stands, read from its record (`PointFiling`: the filing keeps no place of its own). */
  private readonly landscapeAt = (id: number): LandscapeItem => this.landscapeMap.get(id)!;

// src/world/doc.ts:618-625 method RoadDoc.putLandscapeRecord
/** A record put (new, or replacing the one of its id, which keeps its place in the map's order). */
  private putLandscapeRecord(item: LandscapeItem): void;

// src/world/doc.ts:627-634 method RoadDoc.deleteLandscapeRecord
private deleteLandscapeRecord(id: number): void;

// src/world/doc.ts:636-640 method RoadDoc.countStreetName
private countStreetName(item: LandscapeItem, by: 1 | -1): void;

// src/world/doc.ts:1027-1036 method RoadDoc.setNodeApproachRules
/** Each leg's rule at a junction (`RoadNode.approachRules`); undefined: all derived. */
  setNodeApproachRules(id: NodeId, rules: readonly ApproachRuleEntry[] | undefined): void;

// src/world/doc.ts:1067-1070 method RoadDoc.setNature
/** Gives the map an ecosystem (or takes it away: null). */
  setNature(next: NatureSettings | null): void;

// src/world/doc.ts:1087-1098 method RoadDoc.withRoom
/** The items of `items` their tiles have room for (`TILE_CAPS`), in order: DO NOT take an old item away to make room - the player's work is never deleted, the new item is refused (the caller says so). */
  private withRoom<T extends { readonly x: number; readonly y: number }>(part: CappedPart, items: readonly T[]): { readonly taken: T[]; readonly counts: Map<number, number> };

// src/world/doc.ts:1101-1103 method RoadDoc.keepCounts
/** The counts kept through the list's own write (`ownList` lets them go): the add counted its items already. */
  private keepCounts(part: CappedPart, counts: Map<number, number>): void;

// src/world/doc.ts:1107-1115 method RoadDoc.addPaintDab
/** False: its tile holds as many dabs as it may (`TILE_CAPS`), nothing laid. */
  addPaintDab(dab: PaintDab): boolean;

// src/world/doc.ts:1148-1152 method RoadDoc.setGullyAuto
setGullyAuto(amount: number): void;

// src/world/doc.ts:1161-1165 method RoadDoc.setWeather
setWeather(change: Partial<Weather>): void;

// src/world/doc.ts:1168-1177 method RoadDoc.plantTrees
/** How many were planted: those of a full tile (`TILE_CAPS`) are refused, none taken away for them. */
  plantTrees(trees: readonly PlantedTree[]): number;

// src/world/doc.ts:1273-1286 method RoadDoc.addClouds
/** Several clouds at once, as many as the sky has room for: how many were added. */
  addClouds(values: readonly Omit<PlacedCloud, 'id'>[]): number;

// src/world/doc.ts:1297-1302 method RoadDoc.removeCloud
removeCloud(id: number): void;

// src/world/doc.ts:1312-1316 method RoadDoc.setFogSettings
setFogSettings(next: Partial<FogSettings>): void;

// src/world/doc.ts:1327-1336 method RoadDoc.addTerrainStamp
/** The stamp laid; null: its tile holds as many as it may (`TILE_CAPS`), the land left as it is (DO NOT drop the oldest: the relief sculpted first went away). */
  addTerrainStamp(value: Omit<TerrainStamp, 'id'>): TerrainStamp | null;

// src/world/doc.ts:1338-1345 method RoadDoc.clearTerrain
clearTerrain(): void;

// src/world/doc.ts:1347-1352 method RoadDoc.setNodeControl
setNodeControl(id: NodeId, control: JunctionControl): void;

// src/world/doc.ts:1853-1899 method RoadDoc.layOver
/** Makes `copy` a working copy over this document as it stands (`fork`); also how a copy taken back starts again. */
  private layOver(copy: RoadDoc): void;

// src/world/doc.ts:1902-1950 method RoadDoc.takeFork
/** A working copy (`fork`) of this document taken back */
  private takeFork(copy: RoadDoc): void;

// src/world/doc.ts:1969-1978 method RoadDoc.ownList
/** A list about to be written in place: a working copy's own, copied from the original's the first time (`fork`). */
  private ownList(part: DocListPart): void;

// src/world/doc.ts:2073-2104 method RoadDoc.replaceWith
/** Replaces this instance from another valid document - every road drawn (an edited clone, `commitDraft`) and every undo */
  replaceWith(source: RoadDoc): void;

// src/world/doc.ts:2229-2231 method RoadDoc.allocatorMarks
/** The id allocators' next ids: nodes, segments, poles, spans, barriers, landscaping, terrain stamps, buildings. */
  allocatorMarks(): number[];

// src/world/doc.ts:2233-2337 method RoadDoc.restore
restore(r: Restoration): void;

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

// src/world/doc.ts:2636-2670 method RoadDoc.serialized
/** `toJSON`, the buildings left out when `withBuildings` is false (`clone` shares them); 'apart': their list written empty, in its place (their table goes apart, `toJSONBuildingsApart`). */
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc;

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

// src/world/doc.ts:2955-2955 variable DOC_LISTS
const DOC_LISTS: readonly DocListPart[] = ['terrainStamps', 'terrainPaint', 'fogDabs', 'gullyDabs', 'clouds', 'trees', 'treeClearings', 'elements', 'zones', 'zoneMarks', 'people'];

// src/world/doc.ts:3015-3015 function onWorld
/** A place on the world, copied and held within the world's reach (`bounds.ts` WORLD_HALF, thousands of km: no map edge, only numbers kept finite). */
const onWorld = (p: { readonly x: number; readonly y: number }): { x: number; y: number } => …;

// src/world/doc.ts:3067-3077 function landscapeRecord
/** A stored landscape item, or null when it is not one; `repair`: kept on the map. */
function landscapeRecord(raw: NonNullable<SerializedDoc['landscape']>[number], repair: boolean): LandscapeItem | null;

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

// src/world/doc.ts:3314-3315 function placedCloud
/** A cloud placed, its keys in the order a map read back has them (`readCloud`): DO NOT let an undo of the whole map write them in another. */
const placedCloud = (v: Omit<PlacedCloud, 'id'>, id: number): PlacedCloud => …;

// src/world/doc.ts:3322-3322 variable ITEM_PAD
/** Around a pole, a bench, a tree, a fence: what stands round it. */
const ITEM_PAD = 6;

// src/world/doc.ts:3324-3325 function circleRect
const circleRect = (c: { readonly x: number; readonly y: number; readonly radius: number }): ChangeRect => …;

// src/world/doc.ts:3338-3342 function elementsBox
/** The box round elements, grown by the largest one's size; null for none. */
function elementsBox(items: readonly ElementItem[]): ChangeRect | null;

// src/world/doc.ts:3345-3359 function listBox
/** The box round a stretch of one of the document's lists (`RoadDoc.restore`) */
function listBox(part: ListPart, items: readonly unknown[]): ChangeRect | null | undefined;

// src/world/doc.ts:3378-3378 type MovedKinds
/** Whether a source document changed any of these kinds since it was copied (`RoadDoc.movedIn`). */
type MovedKinds = (...kinds: readonly DocChangeKind[]) => boolean;

// src/world/doc.ts:3422-3436 function stampDifferences
/** The land's stamps added, gone or changed between two lists, by id. */
function stampDifferences(a: readonly TerrainStamp[], b: readonly TerrainStamp[]): { rects: ChangeRect[]; ids: number[] };

// src/world/doc.ts:3439-3471 function itemDifferences
/** Where two documents' poles and wires, landscaping and barriers differ. */
function itemDifferences(a: RoadDoc, b: RoadDoc, moved: MovedKinds): { utilities: ChangeRect[]; landscape: ChangeRect[]; barriers: ChangeRect[] };

// src/world/doc.ts:3487-3513 class ItemGrid
/** Things standing at a point (an element of the brush), by the cell of their point */
class ItemGrid<T extends { … }

// src/world/doc.ts:3491-3495 method ItemGrid.add
add(item: T): void;

// src/world/doc.ts:3496-3502 method ItemGrid.remove
remove(item: T): void;

// src/world/doc.ts:3504-3512 method ItemGrid.near
/** The items in the cells a rectangle reaches. */
  near(rect: ChangeRect): T[];

// src/world/doc.ts:3516-3520 function keepWhere
/** Keeps in `list`, in order and in place, the items `keep` says; one pass. */
function keepWhere<T>(list: T[], keep: (item: T) => boolean): void;

// src/world/doc.ts:3530-3540 function sameStamps
/** Two stamp lists that shape the same land (stamp by stamp, field by field). */
export function sameStamps(a: readonly TerrainStamp[], b: readonly TerrainStamp[]): boolean;

// src/world/doc.ts:3552-3559 function samePaint
function samePaint(a: readonly PaintDab[], b: readonly PaintDab[]): boolean;

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

// src/world/doc.ts:3698-3700 function pushAll
/** Each of `items` pushed onto `into`, in order. DO NOT spread a whole map's list into `push` (EDT-14): its arguments past the engine's stack (~100 000) threw RangeError. */
function pushAll<T>(into: T[], items: readonly T[]): void;
