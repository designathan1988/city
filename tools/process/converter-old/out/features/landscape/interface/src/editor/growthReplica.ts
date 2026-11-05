// What the feature uses of src/editor/growthReplica.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/growthReplica.ts:23-32 interface RoadPiece
/** THE GROWTH'S WORKER: a building grown on a lot - its plan, its body made to measure, its yards furnished, each try checked against its neighbours, its roads and its slope (`zoning.ts` `designOnLot`) - worked out off the page's thread (`growth.worker.ts`), the page only writing the record it gets back (`growthFeed.ts`). A city of 2,560 m grew 28.6 s of its 120 s of work on the page's thread, in the frame, and the town stuttered as it grew (profile of 2026-10-15). What the worker reads is kept here, sent by the page with each lot and kept in step by the document's diary (`growthFeed.ts`), never the document copied whole for a lot: - the buildings round the lots grown, as their neighbours read them (`neighbours.ts` `settledNeighbour`: their boxes, what they put on the ground, their looks and height), each read from its tile's shared table (`recordTable.ts`: the page posts the table's handle and the row, never the record copied) and let go; - the roads and junctions filed round the lots grown, as the checks read them (`validate.ts` `RoadReach`): each road's widths, structure and centreline, each junction's kerb ring, filed over the same rectangles as the page's network files them, so a question about a place finds the same; - the lot's ground at the terrain's corners (`world/groundPatch.ts`) and its street's paving at the points the growth reads it (`streetPointsOf`). Each request says where its lot reads (`growthReachOf`): a question asked outside it is a miss (`GrowthMiss`), answered so, and the page sends the lot again with more round it - never a building made on what the worker was not sent. Pure: no three, no DOM, no page. */

/** A road as the page's network has it (`validate.ts` `RoadReachRibbon`), filed over `rect` (`Network.segmentCells`). */
export interface RoadPiece {
  readonly id: number;
  readonly rect: ChangeRect;
  readonly width: number;
  readonly sidewalk: number;
  /** Its record's structure; null: the document has no record of it. */
  readonly structure: RoadStructure | null;
  /** Its whole centreline (`SegmentRibbon.full.xy`). */
  readonly xy: Float64Array;
}

// src/editor/growthReplica.ts:35-42 interface JunctionPiece
/** A junction's kerb ring as the page's network has it, filed over `rect` (`Network.junctionCells`). */
export interface JunctionPiece {
  readonly node: number;
  readonly rect: ChangeRect;
  /** The ring flattened (`Ring.flatten`), x and y in turn. */
  readonly ring: Float64Array;
  readonly bbox: Aabb;
  readonly empty: boolean;
}

// src/editor/growthReplica.ts:45-60 interface GrowthSync
/** What changed of what the worker keeps since the last request (`growthFeed.ts`). */
export interface GrowthSync {
  readonly buildings: {
    /** Everything kept forgotten first (another map, the diary forgot). */
    readonly reset: boolean;
    /** Where each building to keep is read (`recordTable.ts` `refOf`): its tile's shared table and row, a SharedArrayBuffer posted as a handle. */
    readonly put: readonly RecordRef[];
    readonly drop: readonly number[];
  };
  readonly roads: {
    readonly reset: boolean;
    readonly segments: readonly RoadPiece[];
    readonly dropSegments: readonly number[];
    readonly junctions: readonly JunctionPiece[];
    readonly dropJunctions: readonly number[];
  } | null;
}

// src/editor/growthReplica.ts:63-92 interface GrowAsk
/** Buildings asked for: the first lot (picked by the page, `pickLot`) and the stream it is made with, then the lots the growth goes on to (`nearestLot`) while they are in `region` - the street filling along - and what they read. */
export interface GrowAsk {
  readonly id: number;
  readonly sync: GrowthSync;
  readonly lot: Lot;
  readonly rng: RngState;
  /** The seed of the lots' streams (`lotStream`). */
  readonly seed: number;
  /** Most lots grown for this ask (`BATCH_MOST` at most): 1, a lot at a time (the zones' growth, paced by the city's clock). */
  readonly most: number;
  /** The id the first building will take (its draft's, for the checks); the next ones take the ids after it. */
  readonly nextId: number;
  /** How many lots are open, `lot` among them (`OpenLots.count`). */
  readonly count: number;
  /** Where the building grown last stands (`OpenLots.lastGrown`), or null. */
  readonly last: { readonly x: number; readonly y: number } | null;
  /** The open lots whose centre is in `rect`, in the lots' order, `lot` not among them. */
  readonly region: { readonly rect: ChangeRect; readonly lots: readonly Lot[] };
  /** Where the lots read the document: `lot`, the ground, the roads, the paving and the buildings they meet; `around`, the buildings they look at. */
  readonly reach: { readonly lot: ChangeRect; readonly around: ChangeRect };
  /** The ground at the terrain's corners over `reach.lot`; null: no ground (the slope is not checked). */
  readonly ground: GroundPatch | null;
  /** The paving over `reach.lot` (`world/paving.ts` `PavingSource.pack`), read here as the page reads it; null: none handed over. */
  readonly paving: PavingData | null;
  /** The paving's height at each of `streetPointsOf(lot)` (the first lot's), x, y, height in turn, read by the page: where there is no `paving` to hand over, or a point of it the page must answer (asked again after a miss); null: none. */
  readonly paved: Float64Array | null;
  /** Whether there is paving to read at all (a grown building's street floor). */
  readonly pavedRead: boolean;
  /** Whether the roads are checked (a network on the page). */
  readonly roads: boolean;
}

// src/editor/growthReplica.ts:95-98 interface Grown
/** A lot grown: the building made for it, null when nothing fits it. */
export interface Grown {
  readonly lot: number;
  readonly record: Omit<Building, 'id'> | null;
}

// src/editor/growthReplica.ts:100-103 type GrowAnswer
export type GrowAnswer =
  /** The lots grown, in order (the first the one asked for); `miss`: the next one read outside what was sent. */
  | { readonly id: number; readonly grown: readonly Grown[]; readonly miss?: string; readonly ms: number }
  | { readonly id: number; readonly error: string };

// src/editor/growthReplica.ts:109-109 class GrowthMiss
/** A question asked outside where the request said its lot reads: the page sends it again with more round it. */
export class GrowthMiss extends Error { … }

// src/editor/growthReplica.ts:251-289 method GrowthReplica.handle
/** One ask: what changed taken in; the first lot's building made, then the next lot's - the open lot nearest the building grown last, as `pickLot` finds it - and so on while that lot is one the ask sent (`region`), up to `BATCH_MOST`; each building grown read by the next lots as a neighbour. The growth stops where only the page can go on: no lot open, the last building too near the region's edge to know its nearest lot, none near (the next is drawn at random over the town), or a lot reading outside what was sent (`miss`). */
  handle(ask: GrowAsk): GrowAnswer;
