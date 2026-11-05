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

// src/editor/growthReplica.ts:155-159 method NeighbourReplica.near
near(minX: number, minY: number, maxX: number, maxY: number): readonly Neighbour[];

// src/editor/growthReplica.ts:163-163 interface Structured
/** A road's structure as `RoadReach.doc` answers it. */
interface Structured { readonly structure: RoadStructure }
