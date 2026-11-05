// What the feature uses of src/sim/agents/parking.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/parking.ts:26-29 interface BayWorld
/** DO NOT import `SimWorld`: what the bays read of a world (the simulation's, or a tile's own roads and buildings where it is baked, `sim/tileBays.ts`). */
export interface BayWorld extends KerbWorld, LaneGridWorld {
  /** A lane's runtime now (`SimWorld.rt`): whether it is a ghost. */
  rt(id: LaneletId): { readonly ghost: boolean };
}

// src/sim/agents/parking.ts:36-36 variable LOT_REACHED
/** How far round a change a lot's box grown by `LOT_READS` (`TrafficBuilding.reach`) may reach */
const LOT_REACHED = LOT_READS * 2;

// src/sim/agents/parking.ts:57-57 interface LotSources
/** What a lot's bays are worked out from: its record (`lotKey`), and the lanes and walls within reach, digested. */
interface LotSources { readonly lot: number; readonly key: number }

// src/sim/agents/parking.ts:59-59 interface LotBays
/** A lot's bays as last worked out, with what they were worked out from: its record */
interface LotBays extends LotSources { readonly bays: readonly Bay[] }

// src/sim/agents/parking.ts:62-66 interface BayWorkers
/** What works lots' bays out off the page (`bayPool.ts`): each lot asked, in order, as `bayRows` would on the page. */
export interface BayWorkers {
  work(asks: readonly LotAsk[]): Promise<readonly (readonly Bay[])[]>;
  /** Called while lots wait (`baysNow`): fails the workers if one with lots waiting has been silent too long. */
  check?(): void;
}

// src/sim/agents/parking.ts:73-81 interface BakedBays
/** A tile's bays as baked where it was written (`sim/tileBays.ts` TileBays): each lot's and street's, with what they were worked out from. */
export interface BakedBays {
  /** The lots and the streets it holds bays of. */
  lots(): Iterable<BuildingId>;
  kerbs(): Iterable<SegmentId>;
  /** A lot's bays, when baked from this very record (`lotKey`) and these lanes and walls (`lookAtLot`); null else. */
  lot(id: BuildingId, record: number, sources: number): readonly Bay[] | null;
  /** A street's bays, when baked from the very same (`kerbBays.ts` kerbKey); null else. */
  kerb(id: SegmentId, sources: number): readonly Bay[] | null;
}

// src/sim/agents/parking.ts:92-123 interface BayBook
/** The bays of a world as last worked out, and what they were worked out from. */
interface BayBook {
  /** The bays' sources (`L<building>` a lot, `K<segment>` a street's kerb) whose bays changed since `logBase`, oldest first */
  log: string[];
  /** Entries of `log` before it are let go; `logBase` is the serial of the entry at it. */
  logHead: number;
  logBase: number;
  serial: number;
  /** DO NOT forget the log for every reader when one lags: each reader's serial read up to (`bayChangesSince`), the log kept from the least. */
  readonly readers: Map<object, number>;
  /** `out` is to be put together again before it is handed out (`collectBays`). */
  outStale: boolean;
  /** The lots (buildings with a `parking` element), in the store's order. */
  readonly lots: BuildingsInOrder;
  readonly known: Map<BuildingId, LotBays>;
  /** The lots being worked out by the workers, with what from; until the answer lands a lot keeps its bays in `known` */
  readonly waiting: Map<BuildingId, LotSources>;
  readonly kerb: KerbBook;
  /** What works the lots out off the page (`useBayWorkers`); none: on the page. */
  workers: BayWorkers | null;
  /** The lanes' changes the lots were last worked out on (`LaneGrid.changes`). */
  lanesSeen: object | null;
  /** The town's list, ids in order. */
  out: readonly Bay[];
  /** The bays baked with the tiles held (`tileBaysRead`), by tile, and the tile's bake each lot and street is in: taken where worked out from the same. */
  readonly bakedTiles: Map<number, BakedBays>;
  readonly bakedLots: Map<BuildingId, BakedBays>;
  readonly bakedKerbs: Map<SegmentId, BakedBays>;
  /** The tiles whose baked bays are being read, with the read (`tileBaysWanted`): their streets wait for them. */
  readonly reading: Map<number, number>;
  /** What the kerbs read of this world (`kerbReads`), made once. */
  reads: KerbReads | null;
}

// src/sim/agents/parking.ts:230-230 interface Asked
/** A lot asked of the workers, with what it is worked out from. */
interface Asked { readonly id: BuildingId; readonly sources: LotSources; readonly ask: LotAsk }

// src/sim/agents/parking.ts:233-275 function baysNow
/** The bays brought up to date, from the diary: the lots a building change reaches worked out again, the others kept */
function baysNow(w: BayWorld): BayBook;

// src/sim/agents/parking.ts:335-341 function baySources
/** @rebuild Every source of bays, in the town's order (the lots in the store's, then the streets by id): read whole. */
export function baySources(w: BayWorld): string[];

// src/sim/agents/parking.ts:354-359 function baysOfSource
/** A source's bays now (`bayChangesSince`), as fresh copies (ids within the source); none when it has none. */
export function baysOfSource(w: BayWorld, source: string): Bay[];

// src/sim/agents/parking.ts:362-368 function lotsReached
/** The lots whose reach (`LOT_READS`) meets one of the rectangles, and the lots named; found by place. */
function lotsReached(w: BayWorld, lots: BuildingsInOrder, rects: readonly ChangeRect[], named: readonly BuildingId[]): Set<BuildingId>;

// src/sim/agents/parking.ts:370-371 function meets
const meets = (r: ChangeRect, box: { minX: number; minY: number; maxX: number; maxY: number }): boolean => …;

// src/sim/agents/parking.ts:374-416 function lookAtLot
/** A lot looked at: kept as it is (or as it is being worked out) when its record and its reach are untouched */
function lookAtLot(w: BayWorld, book: BayBook, b: TrafficBuilding, rects: readonly ChangeRect[] | null, walls: Walls, asked: Asked[] | null): void;

// src/sim/agents/parking.ts:437-440 interface Wall
export interface Wall extends WallRing {
  /** Its building and its ring, digested: its part of a lot's key (`lookAtLot`). */
  readonly digest: number;
}

// src/sim/agents/parking.ts:443-455 function buildingWalls
/** A building's solid footprints, with their bounds and digests: read by id alone (`TrafficBuildings.footprints`), no traffic record worked out for them, kept for the life of this look (`Walls`). */
function buildingWalls(store: TrafficBuildings, id: BuildingId): readonly Wall[];

// src/sim/agents/parking.ts:458-474 class Walls
/** DO NOT go back to a list of every building's walls: one change of one building made it again. */
export class Walls { … }

// src/sim/agents/parking.ts:460-460 field Walls.read
/** Each building's walls read in this look (a look's lots share their neighbours). */
  private readonly read = new Map<BuildingId, readonly Wall[]>();
