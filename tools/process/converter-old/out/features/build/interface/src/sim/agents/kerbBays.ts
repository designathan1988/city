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

// src/sim/agents/kerbBays.ts:48-48 variable NO_LOT
/** No lot: the bay stands on the street. */
const NO_LOT = -1 as BuildingId;

// src/sim/agents/kerbBays.ts:50-50 type Entrance
type Entrance = [Vec2, Vec2];

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

// src/sim/agents/kerbBays.ts:97-97 interface Box
interface Box { x0: number; y0: number; x1: number; y1: number }

// src/sim/agents/kerbBays.ts:100-178 class EntranceBook
/** THE BUILDINGS' ENTRANCES AS NUMBERS IN COLUMNS (`core/idSlots.ts`), NOT THEIR RECORDS' ARRAYS: each building's run of entrances (ends x, y, x, y) in one typed array, and the ids by cell. DO NOT hold each building's `entrances` list in a `Map` with a `Set` entry a cell: ~1.2 KB of the simulation's heap a building of the city (2026-10-21). */
export class EntranceBook { … }

// src/sim/agents/kerbBays.ts:114-124 method EntranceBook.same
/** Whether a building's entrances kept are these (none kept: `list` is empty). */
  same(id: BuildingId, list: readonly Entrance[]): boolean;

// src/sim/agents/kerbBays.ts:127-137 method EntranceBook.box
/** The box of a building's entrances kept; null for none. */
  box(id: BuildingId): Box | null;

// src/sim/agents/kerbBays.ts:140-151 method EntranceBook.put
/** A building's entrances now (none: let go), filed by the cells their box reaches. */
  put(id: BuildingId, list: readonly Entrance[]): void;

// src/sim/agents/kerbBays.ts:154-159 method EntranceBook.each
/** Each of a building's entrances kept, ends (x, y) to (x, y). */
  each(id: BuildingId, visit: (px: number, py: number, qx: number, qy: number) => void): void;

// src/sim/agents/kerbBays.ts:162-177 method EntranceBook.file
/** Files (or unfiles) a building by the cells a box reaches. */
  private file(id: BuildingId, box: Box, add: boolean): void;

// src/sim/agents/kerbBays.ts:180-187 function entranceBox
function entranceBox(list: readonly Entrance[]): Box;

// src/sim/agents/kerbBays.ts:340-356 function entrancesFacing
/** The entrances that may face a street (`frontagesOn`'s box test), in the store's order. */
function entrancesFacing(book: KerbBook, r: { x0: number; y0: number; x1: number; y1: number }, rank: (id: BuildingId) => number): Entrance[];
