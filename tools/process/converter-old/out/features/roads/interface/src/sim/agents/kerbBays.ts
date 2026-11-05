// What the feature uses of src/sim/agents/kerbBays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/kerbBays.ts:21-27 interface KerbWorld
/** THE BAYS ALONG THE KERBS, KEPT BY STREET (`parking.ts` `collectBays` hands them out after the lots' stalls). */

/** DO NOT import `SimWorld`: the world reaches this file, and the type import closed an import cycle */
export interface KerbWorld {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly topologyLog: ChangeLog<GraphChange>;
  /** The buildings' entrances (`TrafficBuilding.entrances`). */
  readonly buildings: TrafficBuildings;
}

// src/sim/agents/kerbBays.ts:30-41 interface KerbReads
/** What a street's kerb bays read besides the network and the entrances, and what else is asked for them (`parking.ts`). */
export interface KerbReads {
  /** A building's place in the store's order: the entrances facing a street are read in it. */
  readonly rank: (id: BuildingId) => number;
  /** The lane beside a point of a street, on its kerb side (`parking.ts` `laneBeside`). */
  readonly laneAt: (x: number, y: number, segment: SegmentId) => BayLane | null;
  /** A street's link lanes as `laneAt` reads them, digested. */
  readonly linksKey: (segment: SegmentId) => number;
  /** The street's bays baked with its tile (`sim/tileBays.ts`) when they were worked out from the very same (`key`); null: none. */
  readonly baked?: (segment: SegmentId, key: number) => readonly Bay[] | null;
  /** Whether the street's tile's baked bays are on their way (`parking.ts` tileBaysWanted): it waits for them, as a lot waits for the workers. */
  readonly waits?: (segment: SegmentId) => boolean;
}

// src/sim/agents/kerbBays.ts:44-44 variable KERB_AHEAD
/** Ahead of a kerb bay, the place on the lane a car stops at to back in, and leaves for. */
const KERB_AHEAD = m(7);

// src/sim/agents/kerbBays.ts:46-46 variable KERB_MIN_AHEAD
/** The least a kerb bay's joining point may lie ahead of it, u. */
const KERB_MIN_AHEAD = m(4);

// src/sim/agents/kerbBays.ts:48-48 variable NO_LOT
/** No lot: the bay stands on the street. */
const NO_LOT = -1 as BuildingId;

// src/sim/agents/kerbBays.ts:50-50 type Entrance
type Entrance = [Vec2, Vec2];

// src/sim/agents/kerbBays.ts:52-52 variable FRONTAGE_REACH
/** Past this beyond a street's half width, a lot's entrance keeps no bay off its kerb (`parkingLayout.ts` `frontagesOn`). */
const FRONTAGE_REACH = m(8);

// src/sim/agents/kerbBays.ts:54-54 variable KERB_CELL
/** Cell of the kerbs' and entrances' grids, world units. */
const KERB_CELL = m(64);

// src/sim/agents/kerbBays.ts:55-55 function kerbCell
const kerbCell = (i: number, j: number): number => …;

// src/sim/agents/kerbBays.ts:72-88 interface KerbBook
/** The bays along the kerbs of the streets that park (`RoadSegment.parking`), kept by street. */
export interface KerbBook {
  /** The network, its state and the lane graph's the streets were read at (`Network.changesSince`, `SimWorld.topologyLog`). */
  net: Network | null;
  netSeen: object | null;
  lanesSeen: object | null;
  /** The streets that park: each one's reach (its line's box, grown), and the streets by cell. */
  readonly reach: Map<SegmentId, { x0: number; y0: number; x1: number; y1: number }>;
  readonly streets: Map<number, SegmentId[]>;
  /** Each street's bays (ids from 0), and what they were worked out from (`kerbKey`). */
  readonly bays: Map<SegmentId, readonly Bay[]>;
  readonly keys: Map<SegmentId, number>;
  /** Streets waiting for their tile's baked bays (`KerbReads.waits`); streets to look at again at the next follow (their tile's bays landed). */
  readonly held: Set<SegmentId>;
  readonly redo: Set<SegmentId>;
  /** The entrances of each building that has any, and the buildings by cell (`EntranceBook`). */
  readonly entrances: EntranceBook;
}

// src/sim/agents/kerbBays.ts:100-178 class EntranceBook
/** THE BUILDINGS' ENTRANCES AS NUMBERS IN COLUMNS (`core/idSlots.ts`), NOT THEIR RECORDS' ARRAYS: each building's run of entrances (ends x, y, x, y) in one typed array, and the ids by cell. DO NOT hold each building's `entrances` list in a `Map` with a `Set` entry a cell: ~1.2 KB of the simulation's heap a building of the city (2026-10-21). */
export class EntranceBook { … }

// src/sim/agents/kerbBays.ts:190-268 function followKerbs
/** DO NOT lay out every street of the town when the roads move: every road drawn laid out the town's kerbs again. */
export function followKerbs(w: KerbWorld, book: KerbBook, delta: BuildingDelta | null, reads: KerbReads): Set<SegmentId>;

// src/sim/agents/kerbBays.ts:271-285 function sameBays
/** Whether two lists of a street's bays are the same bays, field by field. */
function sameBays(a: readonly Bay[], b: readonly Bay[]): boolean;

// src/sim/agents/kerbBays.ts:295-302 function readStreets
/** @rebuild The streets that park, read whole: the first look, a map opened, or roads further behind than the logs remember. */
function readStreets(w: KerbWorld, book: KerbBook): void;

// src/sim/agents/kerbBays.ts:305-321 function fileStreet
/** A street that parks filed: its reach (its line's box, grown) and the cells it covers. */
function fileStreet(w: KerbWorld, book: KerbBook, id: SegmentId): void;

// src/sim/agents/kerbBays.ts:324-337 function unfileStreet
/** A street taken out of the cells it was filed in. */
function unfileStreet(book: KerbBook, id: SegmentId): void;

// src/sim/agents/kerbBays.ts:359-363 function kerbKey
/** DO NOT leave out anything `kerbBaysOf` reads: what one street's kerb bays are worked out from, digested (its lay out's own reads, the entrances facing it, its lanes). */
function kerbKey(w: KerbWorld, id: SegmentId, entrances: readonly Entrance[], reads: KerbReads): number;

// src/sim/agents/kerbBays.ts:366-387 function kerbBaysOf
/** One street's kerb bays: laid out alone (`parkingLayout` over a network of this one street), against the entrances in front of it. */
function kerbBaysOf(w: KerbWorld, id: SegmentId, entrances: readonly Entrance[], laneAt: KerbReads['laneAt']): Bay[];
