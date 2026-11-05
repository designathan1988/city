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

// src/world/doc.ts:155-155 field RoadDoc.segments
readonly segments: Map<SegmentId, RoadSegment> = new RecordMap();

// src/world/doc.ts:161-161 field RoadDoc.poleMap
/** The overhead utility network: poles and the wire runs between them. */
  // DO NOT write these maps directly.
  private poleMap: Map<PoleId, UtilityPole> = new RecordMap();

// src/world/doc.ts:162-162 field RoadDoc.spanMap
private spanMap: Map<SpanId, UtilitySpan> = new RecordMap();

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

// src/world/doc.ts:177-177 field RoadDoc.landscapeMap
/** Street landscaping the player placed on the footways: trees, shrubs, benches, bins, street lights, hydrants, post boxes */
  // DO NOT write this map directly.
  private landscapeMap: Map<number, LandscapeItem> = new RecordMap();

// src/world/doc.ts:178-178 getter RoadDoc.landscape
get landscape(): ReadonlyMap<number, LandscapeItem>;

// src/world/doc.ts:187-187 field RoadDoc.transit
/** Public transport the player laid out: bus stops and terminals, train and metro tracks and stations, lines */
  transit: TransitData = emptyTransit();

// src/world/doc.ts:200-200 field RoadDoc.terrainStamps
readonly terrainStamps: TerrainStamp[] = [];

// src/world/doc.ts:202-202 field RoadDoc.terrainRelief
/** The land this map was made on (`world/terrain.ts` `ReliefVersion`). */
  terrainRelief: ReliefVersion = RELIEF_LEGACY;

// src/world/doc.ts:204-204 field RoadDoc.markingStyle
/** The regional standard of the map's road paint */
  markingStyle: MarkingStyleId = 'classic';

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

// src/world/doc.ts:220-220 field RoadDoc.economy
/** The money the roads are paid from (`economy.ts`). */
  economy: Economy = DEFAULT_ECONOMY;

// src/world/doc.ts:222-222 field RoadDoc.trees
/** Trees the player planted (`trees.ts`), oldest first. */
  readonly trees: PlantedTree[] = [];

// src/world/doc.ts:241-241 field RoadDoc.treeClearings
/** Where trees were cut away, the woods' own too (`trees.ts`), oldest first. */
  readonly treeClearings: TreeClearing[] = [];

// src/world/doc.ts:243-243 field RoadDoc.elements
/** DO NOT push or splice it from outside. */
  readonly elements: ElementItem[] = [];

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:289-289 field RoadDoc.buildings
/** Modular buildings. */
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:292-292 field RoadDoc.zones
/** Authored land-use strokes, separate from the buildings they generated. */
  readonly zones: Zone[] = [];

// src/world/doc.ts:295-295 field RoadDoc.zoneMarks
/** The zoned cells of the street grid (`world/zoneGrid.ts`), stored by place. */
  readonly zoneMarks: ZoneMark[] = [];

// src/world/doc.ts:297-297 field RoadDoc.lotStore
/** The land's lots (`world/lots.ts`): the cadastre the player zones and edits, with its owner (`lotStore.ts`) */
  readonly lotStore = new LotStore(this.changes);

// src/world/doc.ts:346-346 field RoadDoc.people
/** The people made in the Person Creator, saved with the city. */
  readonly people: PersonSpec[] = [];

// src/world/doc.ts:371-371 getter RoadDoc.revision
/** The roads' geometry: every structural change; consumers use it to invalidate caches. */
  get revision(): number;

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:455-457 method RoadDoc.spansAt
/** The spans reaching a pole, in the order they were strung. */
  spansAt(id: PoleId): UtilitySpan[];

// src/world/doc.ts:686-688 method RoadDoc.degree
degree(id: NodeId): number;

// src/world/doc.ts:820-859 method RoadDoc.mergeNodes
/** Rewires every segment from `source` into `target`, then removes `source`. */
  mergeNodes(target: NodeId, source: NodeId): boolean;

// src/world/doc.ts:1953-1958 method RoadDoc.nodeToWrite
/** A node's record to change in place: a working copy's own (copied from the original's the first time), or the record itself. */
  private nodeToWrite(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:1961-1966 method RoadDoc.segmentToWrite
/** A segment's record to change in place (as `nodeToWrite`). */
  private segmentToWrite(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:2053-2057 method RoadDoc.replaceFromJSON
/** Replaces this instance in place while restoring allocator invariants. */
  /** `preparedBuildings`: its buildings prepared by a worker (`editor/preparedCity.ts`), filed as they are; `buildingTable`: its buildings' table (`prepareCity`), the store's source of truth as it is; `whole`: another map opened, replacing this one whole (`replaceWhole`), never compared with it record by record. */
  replaceFromJSON(data: SerializedDoc, options: { readonly repair?: boolean; readonly preparedBuildings?: boolean; readonly buildingTable?: ArrayBufferLike; readonly whole?: boolean } = {}): void;

// src/world/doc.ts:2060-2070 method RoadDoc.replaceWhole
/** ANOTHER MAP IN PLACE OF THIS ONE (a map opened, a city generated, a whole map put back by undo): its contents taken as they are and one change of the whole map written for every kind - DO NOT compare the two maps record by record (every road, item and building of both walked, decoded and compared to find that all of it changed). */
  replaceWhole(source: RoadDoc): void;

// src/world/doc.ts:2073-2104 method RoadDoc.replaceWith
/** Replaces this instance from another valid document - every road drawn (an edited clone, `commitDraft`) and every undo */
  replaceWith(source: RoadDoc): void;

// src/world/doc.ts:2130-2130 field RoadDoc.recorder
/** DO NOT keep an image of the whole map for undo (`structuredClone` of every record at the first edit): the edit records */
  private recorder: EditRecorder | null = null;

// src/world/doc.ts:2184-2191 method RoadDoc.note
/** A record's state before its first write in the edit, kept once. */
  private note(part: RecordPart, id: number, was: unknown): void;

// src/world/doc.ts:2194-2197 method RoadDoc.noteList
/** A list's state before its first write in the edit, kept once (only the lists the edit writes are copied). */
  private noteList(part: ListPart): void;

// src/world/doc.ts:2200-2212 method RoadDoc.listen
/** The record maps and the stores told of their writes while an edit is recorded (`RecordMap.before`). */
  private listen(rec: EditRecorder | null): void;

// src/world/doc.ts:2233-2337 method RoadDoc.restore
restore(r: Restoration): void;

// src/world/doc.ts:2340-2355 method RoadDoc.listOf
/** A list of the document by its part's name (`restore`, `world/docImage.ts`); read-only to the image (written through `restore`). */
  listOf(part: ListPart): readonly unknown[];

// src/world/doc.ts:2358-2372 method RoadDoc.valueOf
/** A value of the document by its part's name (`world/docImage.ts`). */
  valueOf(part: ValuePart): unknown;

// src/world/doc.ts:2556-2558 method RoadDoc.toJSON
toJSON(): SerializedDoc;

// src/world/doc.ts:2566-2568 method RoadDoc.image
/** The map as an image (`docBinary.ts`): each part's JSON on its own (a long list in runs), the buildings as their table and what was written over it - no text of the whole map, the table shared, never copied. */
  image(): DocImage;

// src/world/doc.ts:2571-2573 method RoadDoc.fromImage
/** A map read back from its image (`image`), exactly as it was: its buildings read from their table when asked. */
  static fromImage(image: DocImage): RoadDoc;

// src/world/doc.ts:2636-2670 method RoadDoc.serialized
/** `toJSON`, the buildings left out when `withBuildings` is false (`clone` shares them); 'apart': their list written empty, in its place (their table goes apart, `toJSONBuildingsApart`). */
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc;

// src/world/doc.ts:2941-2941 type RecordPart
/** The document's keyed collections, its ordered lists and its single values, as undo restores them (`RoadDoc.restore`). */
export type RecordPart = 'nodes' | 'segments' | 'poles' | 'poleSpans' | 'landscape' | 'barriers' | 'buildings' | 'lots';

// src/world/doc.ts:2942-2943 type ListPart
export type ListPart = 'terrainStamps' | 'terrainPaint' | 'fogDabs' | 'gullyDabs' | 'clouds' | 'trees' | 'treeClearings' | 'elements'
  | 'zones' | 'zoneMarks' | 'lotKeys' | 'people';

// src/world/doc.ts:2944-2945 type ValuePart
export type ValuePart = 'terrainRelief' | 'markingStyle' | 'nature' | 'fogSettings' | 'gullyAuto' | 'weather' | 'economy' | 'transit'
  | 'nextZoneId' | 'nextLotId' | 'allocators';

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

// src/world/doc.ts:3527-3527 variable AFTER_BUILDINGS
/** `toJSON`'s keys written after the buildings (`RoadDoc.toText`). */
const AFTER_BUILDINGS = new Set(['zones', 'zoneMarks', 'lots', 'lotKeys', 'people', 'transit', 'economy']);

// src/world/doc.ts:3562-3568 interface EditRecorder
/** What an edit keeps while it is recorded (`RoadDoc.beginEdit`): each thing's state before its first write. */
interface EditRecorder {
  readonly records: Map<RecordPart, Map<number, unknown>>;
  readonly lists: Map<ListPart, unknown[]>;
  readonly values: Map<ValuePart, unknown>;
  /** The map before, as its image (`image`). */
  whole: DocImage | null;
}

// src/world/doc.ts:3570-3570 variable VALUE_PARTS
const VALUE_PARTS: readonly ValuePart[] = ['terrainRelief', 'markingStyle', 'nature', 'fogSettings', 'gullyAuto', 'weather', 'economy', 'transit', 'nextZoneId', 'nextLotId', 'allocators'];

// src/world/doc.ts:3573-3602 class MemberDiff
/** ONE SIDE OF A BUILDING'S CHANGE AS ITS MEMBERS THAT CHANGED (tldraw's history of record diffs; report "Arquitetura para cidades grandes", "A conta de memória": undo memory proportional to the change, not to the records): the record's keys in their order, and the values of those that differ from the other side. Put back over the record as it stands on the other side (`over`), its keys in their order: the record undone is the record it was, text for text. */
export class MemberDiff { … }

// src/world/doc.ts:3577-3577 field MemberDiff.orders
/** The key orders met, each one list for every record of that shape: a building's ~25 keys kept once, not twice an edit. At most `ORDERS_KEPT`: past it, a record's own list (a map of odd records cannot grow it without bound). */
  private static readonly orders = new Map<string, readonly string[]>();

// src/world/doc.ts:3578-3578 field MemberDiff.ORDERS_KEPT
private static readonly ORDERS_KEPT = 4096;

// src/world/doc.ts:3617-3625 function copyRecord
/** A record kept apart from the one that goes on being written: the roads' copied as the fork copies them, the stores' by reference */
function copyRecord(part: RecordPart, value: unknown): unknown;

// src/world/doc.ts:3630-3630 variable GLOBAL_PARTS
/** The parts of what a map saved by tiles keeps outside its tiles, each written again only when its kinds change: the single values, the land's keys, the people, the transport. */
export const GLOBAL_PARTS = ['values', 'lotKeys', 'people', 'transit'] as const;

// src/world/doc.ts:3650-3661 function savedNode
/** A node as the file holds it (`toJSON`, a map saved by tiles): copied, never aliased. */
export function savedNode(n: RoadNode): SerializedDoc['nodes'][number];

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
