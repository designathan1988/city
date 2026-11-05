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

// src/world/doc.ts:155-155 field RoadDoc.segments
readonly segments: Map<SegmentId, RoadSegment> = new RecordMap();

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

// src/world/doc.ts:305-307 getter RoadDoc.lots
/** @deprecated The lots as an array, in their order: made again after each change, for the readers that still walk the whole list */
  get lots(): readonly Lot[];

// src/world/doc.ts:321-323 method RoadDoc.lotsIn
/** The lots whose box meets a rectangle (a superset: test each), in increasing id order. */
  lotsIn(rect: ChangeRect): Lot[];

// src/world/doc.ts:329-329 field RoadDoc.openLotsIndex
/** The open lots in order and by place, for the growth (`openLots.ts`): made when first asked, kept by the diary's entries by id. */
  private openLotsIndex: OpenLots | null = null;

// src/world/doc.ts:330-338 getter RoadDoc.openLots
get openLots(): OpenLots;

// src/world/doc.ts:416-418 method RoadDoc.node
node(id: NodeId): RoadNode | undefined;

// src/world/doc.ts:420-422 method RoadDoc.segment
segment(id: SegmentId): RoadSegment | undefined;

// src/world/doc.ts:430-434 method RoadDoc.requireSegment
requireSegment(id: SegmentId): RoadSegment;

// src/world/doc.ts:1446-1446 field RoadDoc.tileContents
/** The tiles in this document (the active region, CLAUDE.md "Streaming world"): the page holds the tiles near the camera, never the world. */
  private tileContents = new Set<number>();

// src/world/doc.ts:1462-1462 field RoadDoc.tileNext
/** Each loaded tile's next ids, by kind (`TileRecords.next`): a record made in a tile takes its id from the tile's range. */
  private tileNext = new Map<number, TileNext>();

// src/world/doc.ts:1464-1464 field RoadDoc.tileGrown
/** Each loaded tile's grown rows (`TileRecords.grown`, kept by `preparedGrown`), and its lots as they were put in: a row is written back only while its lot is that very record (an edit replaces it). */
  private tileGrown = new Map<number, { readonly grown: PreparedGrown; readonly lots: ReadonlyMap<number, Lot> }>();

// src/world/doc.ts:1853-1899 method RoadDoc.layOver
/** Makes `copy` a working copy over this document as it stands (`fork`); also how a copy taken back starts again. */
  private layOver(copy: RoadDoc): void;

// src/world/doc.ts:1902-1950 method RoadDoc.takeFork
/** A working copy (`fork`) of this document taken back */
  private takeFork(copy: RoadDoc): void;

// src/world/doc.ts:1969-1978 method RoadDoc.ownList
/** A list about to be written in place: a working copy's own, copied from the original's the first time (`fork`). */
  private ownList(part: DocListPart): void;

// src/world/doc.ts:2114-2118 method RoadDoc.buildingsMovedIn
/** The ids of the buildings a working copy of this document changed since it was copied (`movedIn`), from the copy's diary */
  private buildingsMovedIn(source: RoadDoc): readonly BuildingId[] | null;

// src/world/doc.ts:2121-2125 method RoadDoc.lotsMovedIn
/** The ids of the lots a working copy of this document changed since it was copied, as `buildingsMovedIn`; null: compare them all. */
  private lotsMovedIn(source: RoadDoc): readonly number[] | null;

// src/world/doc.ts:2200-2212 method RoadDoc.listen
/** The record maps and the stores told of their writes while an edit is recorded (`RecordMap.before`). */
  private listen(rec: EditRecorder | null): void;

// src/world/doc.ts:2201-2203 function RoadDoc.listen.tell
const tell = <K extends number, V>(map: Map<K, V>, part: RecordPart): void =>;

// src/world/doc.ts:2215-2226 method RoadDoc.recordOf
/** A record of the document as it stands, by its part. */
  private recordOf(part: RecordPart, id: number): unknown;

// src/world/doc.ts:2229-2231 method RoadDoc.allocatorMarks
/** The id allocators' next ids: nodes, segments, poles, spans, barriers, landscaping, terrain stamps, buildings. */
  allocatorMarks(): number[];

// src/world/doc.ts:2233-2337 method RoadDoc.restore
restore(r: Restoration): void;

// src/world/doc.ts:2340-2355 method RoadDoc.listOf
/** A list of the document by its part's name (`restore`, `world/docImage.ts`); read-only to the image (written through `restore`). */
  listOf(part: ListPart): readonly unknown[];

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

// src/world/doc.ts:2566-2568 method RoadDoc.image
/** The map as an image (`docBinary.ts`): each part's JSON on its own (a long list in runs), the buildings as their table and what was written over it - no text of the whole map, the table shared, never copied. */
  image(): DocImage;

// src/world/doc.ts:2581-2583 method RoadDoc.fromBinary
/** A map read from its binary sections (`toBinary`): the buildings' tables in shared memory where the page is isolated, as the workers read them. */
  static fromBinary(bytes: ArrayBufferLike): RoadDoc;

// src/world/doc.ts:2586-2596 method RoadDoc.toText
/** `JSON.stringify(this.toJSON())`, the same text, with the buildings' text kept per record */
  toText(): string;

// src/world/doc.ts:2599-2616 method RoadDoc.savedGlobals
/** One part of what a map saved by tiles keeps outside its tiles (`editor/chunkedSave.ts`): only that part copied. */
  savedGlobals(part: GlobalPart): Partial<SavedGlobals>;

// src/world/doc.ts:2626-2633 method RoadDoc.reserveAllocators
/** The allocators' next ids as saved (`allocatorMarks`, the streamed world's values): each kept above them, never below what this document already holds. */
  private reserveAllocators(marks: readonly number[]): void;

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

// src/world/doc.ts:2987-2987 type TileNext
/** A tile's next ids, by kind (`TileRecords.next`). */
type TileNext = { -readonly [K in keyof TileRecords['next']]: number };

// src/world/doc.ts:3015-3015 function onWorld
/** A place on the world, copied and held within the world's reach (`bounds.ts` WORLD_HALF, thousands of km: no map edge, only numbers kept finite). */
const onWorld = (p: { readonly x: number; readonly y: number }): { x: number; y: number } => …;

// src/world/doc.ts:3019-3026 function nodeRecord
/** Where a lot key says its land lies (`lots.ts` keys: `b:` a block's middle, `s:` a strip's, in steps of 4 m). */
/** A stored node as a document holds it, at `at`, before its roads: what names a road comes after them (`finishNode`). */
function nodeRecord(n: SerializedDoc['nodes'][number], at: { readonly x: number; readonly y: number }): RoadNode;

// src/world/doc.ts:3067-3077 function landscapeRecord
/** A stored landscape item, or null when it is not one; `repair`: kept on the map. */
function landscapeRecord(raw: NonNullable<SerializedDoc['landscape']>[number], repair: boolean): LandscapeItem | null;

// src/world/doc.ts:3080-3085 function barrierRecord
/** A stored wall, fence or hedge, or null when it is not one (fewer than two points). */
function barrierRecord(raw: NonNullable<SerializedDoc['barriers']>[number], repair: boolean): Barrier | null;

// src/world/doc.ts:3095-3099 function lotRecord
/** A stored lot as a store keeps it: its own corners (a stored record is never changed in place, and the data read may be), its building only if it stands. */
function lotRecord(lot: Lot, standing: (id: number) => boolean): Lot;

// src/world/doc.ts:3111-3114 interface PreparedGrown
/** A prepared tile's grown rows and the buildings made from them (`editor/tileStore.ts` preparedRecords), each the very record so made: a record is replaced, never edited (`BuildingStore`), so one still in `made` is exactly its row's, written back as the row (`tileRecords`). */
export interface PreparedGrown {
  readonly rows: GrownRows;
  readonly made: WeakSet<object>;
}

// src/world/doc.ts:3116-3141 interface TileRecords
export interface TileRecords {
  readonly key: number;
  /** Written again at each change of the tile (the store's), for what is baked from it (far levels, link tables) to know it is stale. */
  readonly version: number;
  readonly nodes: SerializedDoc['nodes'];
  /** The nodes of other tiles its roads end on, as they were when written: put in the document only while their own tile is not. */
  readonly borrowedNodes: SerializedDoc['nodes'];
  readonly segments: SerializedDoc['segments'];
  /** Every car gate and parking edge of its buildings (`parkingLayout.ts` gatesOf), four numbers each: what its streets' parking keeps clear of, for a bake of its roads that does not read its buildings (`render/nearRoadTiles.ts`); none in a tile written before it was kept. */
  readonly gates?: readonly number[];
  readonly poles: NonNullable<SerializedDoc['poles']>;
  readonly borrowedPoles: NonNullable<SerializedDoc['poles']>;
  readonly poleSpans: NonNullable<SerializedDoc['poleSpans']>;
  readonly buildings: readonly unknown[];
  /** The buildings its generator grew, as rows (`growth/rows.ts`): made records again by whoever reads them (`growth/grownRows.ts` `tileBuildingsOf`). */
  readonly grown?: GrownRows;
  readonly lots: readonly Lot[];
  readonly lotKeys: readonly string[];
  readonly landscape: NonNullable<SerializedDoc['landscape']>;
  readonly barriers: NonNullable<SerializedDoc['barriers']>;
  readonly paint: NonNullable<SerializedDoc['paint']>;
  readonly trees: NonNullable<SerializedDoc['trees']>;
  readonly elements: NonNullable<SerializedDoc['elements']>;
  /** The next id each kind counts out in the tile's range (`TILE_COUNTED`), from `tileIdBase`. */
  readonly next: Readonly<Record<'node' | 'segment' | 'pole' | 'span' | 'building' | 'lot' | 'landscape' | 'barrier', number>>;
}

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

// src/world/doc.ts:3345-3359 function listBox
/** The box round a stretch of one of the document's lists (`RoadDoc.restore`) */
function listBox(part: ListPart, items: readonly unknown[]): ChangeRect | null | undefined;

// src/world/doc.ts:3362-3368 function spliceIn
/** `list.splice(start, remove, ...items)` without spreading `items` into arguments: a stretch of tens of thousands */
function spliceIn<T>(list: T[], start: number, remove: number, items: readonly T[]): T[];

// src/world/doc.ts:3378-3378 type MovedKinds
/** Whether a source document changed any of these kinds since it was copied (`RoadDoc.movedIn`). */
type MovedKinds = (...kinds: readonly DocChangeKind[]) => boolean;

// src/world/doc.ts:3562-3568 interface EditRecorder
/** What an edit keeps while it is recorded (`RoadDoc.beginEdit`): each thing's state before its first write. */
interface EditRecorder {
  readonly records: Map<RecordPart, Map<number, unknown>>;
  readonly lists: Map<ListPart, unknown[]>;
  readonly values: Map<ValuePart, unknown>;
  /** The map before, as its image (`image`). */
  whole: DocImage | null;
}

// src/world/doc.ts:3573-3602 class MemberDiff
/** ONE SIDE OF A BUILDING'S CHANGE AS ITS MEMBERS THAT CHANGED (tldraw's history of record diffs; report "Arquitetura para cidades grandes", "A conta de memória": undo memory proportional to the change, not to the records): the record's keys in their order, and the values of those that differ from the other side. Put back over the record as it stands on the other side (`over`), its keys in their order: the record undone is the record it was, text for text. */
export class MemberDiff { … }

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
