// What the feature uses of src/sim/agents/trafficBuildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/trafficBuildings.ts:24-24 variable LOT_READS
/** THE BUILDINGS AS TRAFFIC READS THEM: what the simulation's worker is sent of each building, never the whole record */

/** How far a lot's bays read lanes and walls (`LANE_REACH`, the exits' reach), with a margin (`parking.ts`). */
export const LOT_READS = LANE_REACH + m(12);

// src/sim/agents/trafficBuildings.ts:26-26 interface TrafficBox
interface TrafficBox { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number }

// src/sim/agents/trafficBuildings.ts:28-28 type TrafficBlock
/** What traffic reads of a block: its box in the building's frame, open (a lot) or built, its floor (`lotBays.ts` LotBlock). */
type TrafficBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/trafficBuildings.ts:30-30 type TrafficPart
/** What traffic reads of a part of a lot (`lotNav.ts` LotPart), and its id (a dock's element). */
export type TrafficPart = LotPart & Pick<BuildingElement, 'id'>;

// src/sim/agents/trafficBuildings.ts:33-55 interface TrafficBuilding
/** A building as the simulation reads it: its frame, use, blocks and parts, and what the page works out of the whole record */
export interface TrafficBuilding {
  readonly id: BuildingId;
  readonly x: number;
  readonly y: number;
  readonly rotation: number;
  readonly use: BuildingUse;
  readonly function?: BuildingFunction;
  readonly volumes: readonly TrafficBlock[];
  /** Absent when the record has no parts at all (as `Building.elements`). */
  readonly elements?: readonly TrafficPart[];
  /** `storedBounds(b)`: its blocks and its parts, world. */
  readonly bounds: TrafficBox;
  /** `storedBounds(b, LOT_READS)`: the box its lot's bays read within. */
  readonly reach: TrafficBox;
  /** `solidFootprints(b)`: the world rings of its built ground blocks, what nobody drives or walks through. */
  readonly footprints: readonly (readonly Vec2[])[];
  /** `streetDoors(b)`: its doors straight onto the footway. */
  readonly doors: readonly StreetDoor[];
  /** `lorryDocks(b)`: its loading docks. */
  readonly docks: readonly LorryDock[];
  /** `entrancesOf(b)`: its car gates and the edges of its parking, where a kerb keeps no bay. */
  readonly entrances: readonly [Vec2, Vec2][];
}

// src/sim/agents/trafficBuildings.ts:57-57 function boxOf
const boxOf = (b: TrafficBox): TrafficBox => …;

// src/sim/agents/trafficBuildings.ts:59-61 function partOf
function partOf(el: BuildingElement): TrafficPart;

// src/sim/agents/trafficBuildings.ts:63-76 function project
function project(b: Building): TrafficBuilding;

// src/sim/agents/trafficBuildings.ts:79-79 variable trafficBuilding
/** DO NOT keep it per record on the page (`identityMemo`): a copy of every building of the map held for as long as the map, ~6 KB each; worked out when it is sent, kept by the worker alone. */
export const trafficBuilding: (b: Building) => TrafficBuilding = project;

// src/sim/agents/trafficBuildings.ts:84-84 variable TRAFFIC_KINDS
/** The kinds of place a building is (`ambient/demand.ts` kindOf), in the order of the draws' classes and of `LotPlaces`' trees. */
export const TRAFFIC_KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

// src/sim/agents/trafficBuildings.ts:86-86 variable TRAIT_PARKING
/** A building with a `parking` element: a lot whose stalls are bays (`parking.ts`). */
export const TRAIT_PARKING = 1;

// src/sim/agents/trafficBuildings.ts:88-88 variable TRAIT_YARD
/** A gated lot with docks: a lorry's yard (`lorryTraffic.ts`). */
export const TRAIT_YARD = 2;

// src/sim/agents/trafficBuildings.ts:91-102 interface TrafficFacts
/** What the walks over every building read of each (the order of the lots, the scenery's indexes, the meso's demand): numbers kept in columns in the worker (`TrafficBuildingTable`), no traffic record worked out for them. */
export interface TrafficFacts {
  readonly id: BuildingId;
  readonly x: number;
  readonly y: number;
  readonly kind: keyof Mix;
  /** `TRAIT_PARKING`, `TRAIT_YARD`. */
  readonly traits: number;
  /** The most frontage its way on foot can have (`frontageBound`). */
  readonly frontage: number;
  readonly bounds: TrafficBox;
  readonly reach: TrafficBox;
}

// src/sim/agents/trafficBuildings.ts:105-130 function frontageBound
/** The most frontage a building's way on foot can have (`lotDoors.ts`): its blocks' width along its widest people's gate, or along its facade with none, 6 m at the least. */
export function frontageBound(b: Pick<TrafficBuilding, 'volumes' | 'elements'>): number;

// src/sim/agents/trafficBuildings.ts:132-133 function traitsOf
const traitsOf = (b: TrafficBuilding): number => …;

// src/sim/agents/trafficBuildings.ts:136-138 function factsOf
/** A traffic record's facts (`TrafficFacts`), worked out of it. */
export function factsOf(b: TrafficBuilding): TrafficFacts;

// src/sim/agents/trafficBuildings.ts:141-165 interface TrafficBuildings
/** The buildings as the simulation asks for them: by id, all in the store's order, by place in the store's order. */
export interface TrafficBuildings {
  /** A building's traffic record (the worker works it out again from its row when it is not held: DO NOT ask it of every building). */
  get(id: BuildingId): TrafficBuilding | undefined;
  /** @rebuild Every traffic record in the store's order: DO NOT walk it in the worker, where each is worked out again (`ids` and `facts`). */
  all(): Iterable<TrafficBuilding>;
  /** Every building's id, in the order of `all()`, none read. */
  ids(): Iterable<BuildingId>;
  /** What the walks over every building read of one, no traffic record worked out (`TrafficFacts`). */
  facts(id: BuildingId): TrafficFacts | undefined;
  /** A building's entrances (`TrafficBuilding.entrances`), none read but them; none for a building not there. */
  entrances(id: BuildingId): readonly [Vec2, Vec2][];
  /** A building's footprints (`TrafficBuilding.footprints`), none read but them; none for a building not there. */
  footprints(id: BuildingId): readonly (readonly Vec2[])[];
  /** @rebuild Every building's facts, entrances or footprints in the store's order: DO NOT ask them id by id of every building (the page's store holds each record asked by id for a while: `TableRows.recent`). */
  eachFacts(visit: (b: TrafficFacts) => void): void;
  eachEntrances(visit: (id: BuildingId, entrances: readonly [Vec2, Vec2][]) => void): void;
  eachFootprints(visit: (id: BuildingId, footprints: readonly (readonly Vec2[])[]) => void): void;
  /** The buildings whose box may reach the rectangle, in the store's order. */
  near(minX: number, minY: number, maxX: number, maxY: number): TrafficBuilding[];
  /** The ids of `near`, none read. */
  nearIds(minX: number, minY: number, maxX: number, maxY: number): BuildingId[];
  /** The buildings `meets` keeps for one of `rects`, asked of those within `pad` of it, each once, in the store's order: their ids. */
  nearAny(rects: readonly (readonly [number, number, number, number])[], pad: number,
    meets: (b: TrafficFacts, rect: readonly [number, number, number, number]) => boolean): BuildingId[];
}

// src/sim/agents/trafficBuildings.ts:168-171 interface BuildingSource
/** What a reader of the buildings by the diary reads: the document (its diary) and the buildings (`SimWorld`). */
export interface BuildingSource {
  readonly doc: RoadDoc;
  readonly buildings: TrafficBuildings;
}

// src/sim/agents/trafficBuildings.ts:174-220 function docBuildings
/** A document's own buildings as traffic reads them (a world on the page or in a spec): each record's traffic record. */
export function docBuildings(doc: RoadDoc): TrafficBuildings;

// src/sim/agents/trafficBuildings.ts:223-223 variable BATCH_DETAIL
/** A batch of the buildings' changes in the diary (`TrafficBuildingTable.apply`): its detail is this, then one letter an id - gone, kept in its place, or new (put last). */
export const BATCH_DETAIL = 'batch:';

// src/sim/agents/trafficBuildings.ts:224-224 variable BATCH_GONE
export const BATCH_GONE = 'g';

// src/sim/agents/trafficBuildings.ts:225-225 variable BATCH_KEPT
export const BATCH_KEPT = 'k';

// src/sim/agents/trafficBuildings.ts:229-233 interface BuildingChange
/** A building of the page changed, as the worker is told it: its traffic record now and where its record is read (`RecordRef`), null when it is gone. */
export interface BuildingChange {
  readonly id: BuildingId;
  readonly value: TrafficBuilding | null;
  readonly ref: RecordRef | null;
}

// src/sim/agents/trafficBuildings.ts:253-689 class TrafficBuildingTable
/**
 * THE BUILDINGS IN THE SIMULATION'S WORKER AS NUMBERS IN COLUMNS, NOT AS OBJECTS (report "Arquitetura para cidades grandes", "colunas
 * binárias"): each building at a slot (`IdSlots`) with its place, boxes, kind and traits, its entrances and footprints in runs of one
 * typed array each (`RunPool`), its row (where its record is read again), and its place in the store's order; the slots filed by the
 * world's 256 m cells in typed lists. A traffic record is worked out again from its row when asked (`get`), the last asked held
 * (`HELD_RECORDS`) and any still held elsewhere found again (`WeakRef`, MDN): a record asked twice is one object while anyone holds it.
 * DO NOT keep every building's traffic record: ~10.7 KB of the worker's heap a building (2.1 GB at 200,000, 2026-10-21).
 */
export class TrafficBuildingTable implements TrafficBuildings { … }

// src/sim/agents/trafficBuildings.ts:285-285 field TrafficBuildingTable.readers
private readers: (TableReader | null)[] = [];

// src/sim/agents/trafficBuildings.ts:306-309 method TrafficBuildingTable.get
get(id: BuildingId): TrafficBuilding | undefined;

// src/sim/agents/trafficBuildings.ts:315-320 method TrafficBuildingTable.ids
*ids(): IterableIterator<BuildingId>;

// src/sim/agents/trafficBuildings.ts:322-325 method TrafficBuildingTable.facts
facts(id: BuildingId): TrafficFacts | undefined;

// src/sim/agents/trafficBuildings.ts:327-333 method TrafficBuildingTable.entrances
entrances(id: BuildingId): readonly [Vec2, Vec2][];

// src/sim/agents/trafficBuildings.ts:339-341 method TrafficBuildingTable.eachEntrances
eachEntrances(visit: (id: BuildingId, entrances: readonly [Vec2, Vec2][]) => void): void;

// src/sim/agents/trafficBuildings.ts:343-345 method TrafficBuildingTable.eachFootprints
eachFootprints(visit: (id: BuildingId, footprints: readonly (readonly Vec2[])[]) => void): void;

// src/sim/agents/trafficBuildings.ts:347-360 method TrafficBuildingTable.footprints
footprints(id: BuildingId): readonly (readonly Vec2[])[];

// src/sim/agents/trafficBuildings.ts:363-388 method TrafficBuildingTable.load
/** @rebuild Another map: every record read from its row, the diary told the whole set changed. DO NOT hold a record read here: each one's columns alone. */
  load(sections: BuildingSections): void;

// src/sim/agents/trafficBuildings.ts:391-412 method TrafficBuildingTable.apply
/** The page's changes, written in the diary as ONE entry (DO NOT write one a building: a tile's ~1,500 pushed every reader past what the diary keeps): each building's id once, in order, and what became of it (`BATCH_DETAIL`, `buildingOrder.ts`). */
  apply(changes: readonly BuildingChange[]): void;

// src/sim/agents/trafficBuildings.ts:418-420 method TrafficBuildingTable.nearIds
nearIds(minX: number, minY: number, maxX: number, maxY: number): BuildingId[];

// src/sim/agents/trafficBuildings.ts:422-431 method TrafficBuildingTable.nearAny
nearAny(rects: readonly (readonly [number, number, number, number])[], pad: number,
    meets: (b: TrafficFacts, rect: readonly [number, number, number, number]) => boolean): BuildingId[];

// src/sim/agents/trafficBuildings.ts:448-456 method TrafficBuildingTable.factsAt
private factsAt(slot: number): TrafficFacts;

// src/sim/agents/trafficBuildings.ts:459-487 method TrafficBuildingTable.place
/** A building's columns written from its traffic record, its row noted, filed by place: its slot. */
  private place(b: TrafficBuilding, ref: RecordRef): number;

// src/sim/agents/trafficBuildings.ts:489-499 method TrafficBuildingTable.remove
private remove(id: BuildingId, slot: number): void;

// src/sim/agents/trafficBuildings.ts:550-560 method TrafficBuildingTable.keepRef
private keepRef(slot: number, ref: RecordRef): void;

// src/sim/agents/trafficBuildings.ts:573-587 method TrafficBuildingTable.record
/** A slot's traffic record: held, still alive elsewhere, or worked out again from its row. */
  private record(slot: number): TrafficBuilding;

// src/sim/agents/trafficBuildings.ts:692-695 interface BuildingSent
/** A building of the page changed, as it is posted to the worker: where its record is read (`refOf`: its tile's shared table and row, or a table of its own), null when it is gone; DO NOT project it on the page. */
export interface BuildingSent {
  readonly id: BuildingId;
  readonly ref: RecordRef | null;
}

// src/sim/agents/trafficBuildings.ts:698-703 interface TrafficMap
/** What the worker's copy of the map is made of: the roads and the transit, the lots and their keys, the buildings as the document's own buffers (`BuildingSections`: its table, shared, and the records written over it). */
export interface TrafficMap {
  readonly doc: SerializedDoc;
  readonly lots: readonly Lot[];
  readonly lotKeys: readonly string[];
  readonly buildings: BuildingSections;
}

// src/sim/agents/trafficBuildings.ts:706-719 function trafficMapOf
/** The page's map as the worker is sent it whole (a map opened, generated, new): no terrain, paint or scenery, no building's record - the buildings' table posted as a handle (shared memory), never copied record by record. */
export function trafficMapOf(doc: RoadDoc): TrafficMap;

// src/sim/agents/trafficBuildings.ts:722-729 function buildingChangesOf
/** The buildings the diary named, as the worker is sent them: each record's place (`refOf`, not kept on the page), never its traffic record worked out here. */
export function buildingChangesOf(doc: RoadDoc, ids: Iterable<BuildingId>): BuildingSent[];

// src/sim/agents/trafficBuildings.ts:732-738 function trafficChangesOf
/** In the worker: the buildings sent (`buildingChangesOf`) read from their rows and worked out as traffic reads them. */
export function trafficChangesOf(sent: readonly BuildingSent[]): BuildingChange[];

// src/sim/agents/trafficBuildings.ts:741-746 function takeTrafficMap
/** The worker's copy made from a map sent whole (`trafficMapOf`), each building read from its row and worked out here. */
export function takeTrafficMap(doc: RoadDoc, buildings: TrafficBuildingTable, map: TrafficMap): void;
