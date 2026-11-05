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

// src/world/doc.ts:187-187 field RoadDoc.transit
/** Public transport the player laid out: bus stops and terminals, train and metro tracks and stations, lines */
  transit: TransitData = emptyTransit();

// src/world/doc.ts:190-192 method RoadDoc.setTransit
/** The public transport replaced by an edit of it. */
  setTransit(next: TransitData): void;

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:289-289 field RoadDoc.buildings
/** Modular buildings. */
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:1853-1899 method RoadDoc.layOver
/** Makes `copy` a working copy over this document as it stands (`fork`); also how a copy taken back starts again. */
  private layOver(copy: RoadDoc): void;

// src/world/doc.ts:1902-1950 method RoadDoc.takeFork
/** A working copy (`fork`) of this document taken back */
  private takeFork(copy: RoadDoc): void;

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

// src/world/doc.ts:2944-2945 type ValuePart
export type ValuePart = 'terrainRelief' | 'markingStyle' | 'nature' | 'fogSettings' | 'gullyAuto' | 'weather' | 'economy' | 'transit'
  | 'nextZoneId' | 'nextLotId' | 'allocators';

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

// src/world/doc.ts:3378-3378 type MovedKinds
/** Whether a source document changed any of these kinds since it was copied (`RoadDoc.movedIn`). */
type MovedKinds = (...kinds: readonly DocChangeKind[]) => boolean;

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
