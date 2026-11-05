// What the feature uses of src/editor/growthFeed.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/growthFeed.ts:25-33 interface GrowthHost
/** THE GROWTH, THE PAGE'S SIDE: the lot picked (`zoning.ts` `pickLot`), sent to the growth's worker with what it reads there (`growthReplica.ts`), and the building it sends back written in the document (`commitGrown`) - the same steps as `growOnLot`, the heavy one (`designOnLot`) off the page. One lot at a time: the next lot and its stream depend on the building grown before (the street fills along; the stream is seeded by the next id), so the city grows exactly as it grew on the page. What the worker keeps is kept in step here, from the document's diary and the network's changes, for the places it is sent: the buildings filed round each lot (`BuildingStore.near`) sent once, as where the worker reads them (`recordTable.ts` `refOf`: their tile's shared table and row, the one the cell jobs read; never the record copied), then again only when the diary names them (`changedSince`); the roads and junctions filed round each lot (`Network.segmentCells`, `junctionCells`) the same way (`changesSince`); the lot's own ground and paving read for it. DO NOT send the document or the network whole for a lot: the copy of a town's records for every building grown is the work this took off the page, made again. Written while the worker made it (a building put up, a road drawn, the ground sculpted), the building is checked again on the page (`validateBuilding`, the neighbours' rings already worked out) and asked again if it no longer stands - never added unchecked against the document as it is. */

/** Where the growth reads the document: the page's document, network and ground. */
export interface GrowthHost {
  readonly doc: RoadDoc;
  readonly net: Network | null;
  readonly groundAt: ((x: number, y: number) => number) | null;
  /** The paving's height; with `pack`, handed to the worker as the roads it reads from (`world/paving.ts`). */
  readonly pavedAt?: PavingSource;
  /** The side of the terrain's cells: the grid the lot's ground is read on (`world/groundPatch.ts`). */
  readonly cell: number;
}

// src/editor/growthFeed.ts:36-39 interface GrowthLane
/** Where the growth's worker answers (`growthPool.ts`; a spec's stand-in). */
export interface GrowthLane {
  /** `transfer`: buffers made for this ask alone, handed over rather than copied (MDN, "Transferable objects"). */
  post(ask: GrowAsk, transfer: readonly ArrayBuffer[]): void;
}

// src/editor/growthFeed.ts:42-44 type GrowthOutcome
/** What an answer came to: the buildings written (their ids, in order), the lots set aside, and whether the rest was let go to be asked for again (the document moved under it, `ask`); or nothing (`stale`: not the ask waited for). */
export type GrowthOutcome =
  | { readonly kind: 'answered'; readonly grown: readonly number[]; readonly refused: number; readonly again: boolean }
  | { readonly kind: 'stale' };

// src/editor/growthFeed.ts:54-54 variable MISS_MARGIN
/** More read round a lot each time the worker asked outside what it was sent (`GrowthMiss`), world units. */
const MISS_MARGIN = 32;

// src/editor/growthFeed.ts:56-56 variable MOST_MISSES
/** Misses before a lot is set aside: a lot that reads farther than this is not one the growth can build. */
const MOST_MISSES = 4;

// src/editor/growthFeed.ts:58-58 variable MOST_REFUSALS
/** Times a lot's building may be refused on the page, the document having moved, before the lot is set aside. */
const MOST_REFUSALS = 3;

// src/editor/growthFeed.ts:72-333 class GrowthSession
export class GrowthSession { … }

// src/editor/growthFeed.ts:89-89 field GrowthSession.roadsOf
private roadsOf: Network | null = null;

// src/editor/growthFeed.ts:99-101 getter GrowthSession.busy
/** Whether a building is being made. */
  get busy(): boolean;

// src/editor/growthFeed.ts:104-115 method GrowthSession.ask
/** Asks the worker for the next building (`pickLot`); false when no lot is open (`refused` set aside) - or true, already asking, while one is made. */
  ask(refused: Set<number>, seed: number, most = 1): boolean;

// src/editor/growthFeed.ts:118-126 method GrowthSession.restart
/** The worker started again with nothing (it failed, `growthPool.ts`): all is sent again as it is asked for, and the building being made asked for again. */
  restart(): void;

// src/editor/growthFeed.ts:129-191 method GrowthSession.receive
/** The worker's answer, its buildings written in order (`onGrown` told of each as it is, before the next) while each still stands where it was made - checked again first when the document moved meanwhile; the rest let go at the first that does not, to be asked for again. */
  receive(answer: GrowAnswer, onGrown: (id: number) => void = () => {}): GrowthOutcome;

// src/editor/growthFeed.ts:200-216 method GrowthSession.readChanges
/** What changed of what the worker holds, from the buildings' diary and the network's changes: those it holds marked to be sent again, everything when they cannot say (another map, entries forgotten, a load). */
  private readChanges(doc: RoadDoc, net: Network | null): void;

// src/editor/growthFeed.ts:227-274 method GrowthSession.send
/** The ask for `lot` (picked here, `pickLot`, its stream `rng`) and the lots the growth may go on to in the worker: the open lots round it, with what they all read - the buildings, roads and ground round them, the paving there - sent once and kept in step (`syncBuildings`, `syncRoads`), the ground and the paving of the region read for this ask. */
  private send(lot: Lot, rng: RngState, refused: Set<number>, extra: number, misses: number): void;

// src/editor/growthFeed.ts:300-332 method GrowthSession.syncRoads
/** The roads and junctions the worker holds that changed (`readChanges`), and those filed round this lot it does not hold yet. */
  private syncRoads(net: Network, rect: ChangeRect): NonNullable<GrowthSync['roads']>;

// src/editor/growthFeed.ts:336-341 function roadPiece
/** A road as the worker keeps it (`RoadPiece`), or null: no ribbon filed for it. */
function roadPiece(net: Network, id: SegmentId): RoadPiece | null;

// src/editor/growthFeed.ts:344-353 function junctionPiece
/** A junction's kerb ring as the worker keeps it (`JunctionPiece`), or null: none filed. */
function junctionPiece(net: Network, node: NodeId): JunctionPiece | null;
