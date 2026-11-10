// What the feature uses of src/sim/agents/trafficBuildings.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/sim/agents/trafficBuildings.ts:84-84 variable TRAFFIC_KINDS
/** The kinds of place a building is (`ambient/demand.ts` kindOf), in the order of the draws' classes and of `LotPlaces`' trees. */
export const TRAFFIC_KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

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

// src/sim/agents/trafficBuildings.ts:174-220 function docBuildings
/** A document's own buildings as traffic reads them (a world on the page or in a spec): each record's traffic record. */
export function docBuildings(doc: RoadDoc): TrafficBuildings;

// src/sim/agents/trafficBuildings.ts:177-181 function docBuildings.trafficBuilding
const trafficBuilding = (b: Building): TrafficBuilding =>;

// src/sim/agents/trafficBuildings.ts:190-193 method docBuildings.get
get: (id) =>;

// src/sim/agents/trafficBuildings.ts:198-201 method docBuildings.facts
facts: (id) =>;

// src/sim/agents/trafficBuildings.ts:206-209 method docBuildings.footprints
footprints: (id) =>;

// src/sim/agents/trafficBuildings.ts:211-211 method docBuildings.eachFacts
/** @rebuild Every record walked once (`TrafficBuildings.eachFacts`). */
    eachFacts: (visit) =>;

// src/sim/agents/trafficBuildings.ts:217-217 method docBuildings.nearIds
nearIds: (minX, minY, maxX, maxY) => …;

// src/sim/agents/trafficBuildings.ts:306-309 method TrafficBuildingTable.get
get(id: BuildingId): TrafficBuilding | undefined;

// src/sim/agents/trafficBuildings.ts:315-320 method TrafficBuildingTable.ids
*ids(): IterableIterator<BuildingId>;

// src/sim/agents/trafficBuildings.ts:322-325 method TrafficBuildingTable.facts
facts(id: BuildingId): TrafficFacts | undefined;

// src/sim/agents/trafficBuildings.ts:335-337 method TrafficBuildingTable.eachFacts
eachFacts(visit: (b: TrafficFacts) => void): void;

// src/sim/agents/trafficBuildings.ts:347-360 method TrafficBuildingTable.footprints
footprints(id: BuildingId): readonly (readonly Vec2[])[];

// src/sim/agents/trafficBuildings.ts:418-420 method TrafficBuildingTable.nearIds
nearIds(minX: number, minY: number, maxX: number, maxY: number): BuildingId[];

// src/sim/agents/trafficBuildings.ts:459-487 method TrafficBuildingTable.place
/** A building's columns written from its traffic record, its row noted, filed by place: its slot. */
  private place(b: TrafficBuilding, ref: RecordRef): number;

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
