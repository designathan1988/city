// What the feature uses of src/sim/agents/parking.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/parking.ts:26-29 interface BayWorld
/** DO NOT import `SimWorld`: what the bays read of a world (the simulation's, or a tile's own roads and buildings where it is baked, `sim/tileBays.ts`). */
export interface BayWorld extends KerbWorld, LaneGridWorld {
  /** A lane's runtime now (`SimWorld.rt`): whether it is a ghost. */
  rt(id: LaneletId): { readonly ghost: boolean };
}

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

// src/sim/agents/parking.ts:68-70 function useBayWorkers
/** The workers a world's lots are worked out by (the game's, `main.ts`). */
export function useBayWorkers(w: BayWorld, workers: BayWorkers): void;

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

// src/sim/agents/parking.ts:204-204 function byText
const byText = (a: string, b: string): number => …;

// src/sim/agents/parking.ts:230-230 interface Asked
/** A lot asked of the workers, with what it is worked out from. */
interface Asked { readonly id: BuildingId; readonly sources: LotSources; readonly ask: LotAsk }

// src/sim/agents/parking.ts:374-416 function lookAtLot
/** A lot looked at: kept as it is (or as it is being worked out) when its record and its reach are untouched */
function lookAtLot(w: BayWorld, book: BayBook, b: TrafficBuilding, rects: readonly ChangeRect[] | null, walls: Walls, asked: Asked[] | null): void;

// src/sim/agents/parking.ts:419-420 function bothWays
/** Whether a street runs both ways (a gate onto a one-way street may face either kerb, `lotBays.ts` gateLane). */
const bothWays = (w: BayWorld, segment: SegmentId | undefined): boolean => …;

// src/sim/agents/parking.ts:458-474 class Walls
/** DO NOT go back to a list of every building's walls: one change of one building made it again. */
export class Walls { … }
