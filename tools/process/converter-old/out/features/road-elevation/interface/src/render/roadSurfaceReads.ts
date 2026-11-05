// What the feature uses of src/render/roadSurfaceReads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaceReads.ts:16-16 variable KERB_RISE
/** WHAT EACH SURFACE OF A ROAD PASS READS PER VERTEX (`roadSurfaces.ts`) */

/** Height of the kerb face above the carriageway. */
const KERB_RISE = FOOTWAY_RISE + m(0.01);

// src/render/roadSurfaceReads.ts:18-18 variable VERGE_DROP
/** Depth of the verge below the carriageway, where the grass starts. */
const VERGE_DROP = m(0.04);

// src/render/roadSurfaceReads.ts:20-20 variable VERGE_SKIRT
/** Drop of the verge skirt into the ground. */
const VERGE_SKIRT = 0.5;

// src/render/roadSurfaceReads.ts:24-24 variable TERRAIN_UV
/** World size of one UV unit on the ground verge when it wears the terrain's material (and a retaining wall's backfill, `structures.ts`). */
export const TERRAIN_UV = 64;

// src/render/roadSurfaceReads.ts:28-28 variable MEDIAN_KERB
/** Height of the median island's kerb above the carriageway: the footway's kerb (15 cm). */
const MEDIAN_KERB = FOOTWAY_RISE;

// src/render/roadSurfaceReads.ts:30-30 variable MEDIAN_PLANTING
/** Height of the planting inside it: the soil a hair over the kerb's top, so the two are never one plane. */
const MEDIAN_PLANTING = FOOTWAY_RISE + m(0.02);

// src/render/roadSurfaceReads.ts:34-34 variable FOOTWAY_EDGE_CELL
/** The grid the footways' outer edges are filed by for the verge's fall (`fromFootway`). */
export const FOOTWAY_EDGE_CELL = m(32);

// src/render/roadSurfaceReads.ts:40-61 interface PassDesc
/** A road pass as its surfaces are read (`roadSurfaceSteps`): plain data, handed to a worker as it is. */
export interface PassDesc {
  readonly id: string;
  readonly structure: RoadStructure;
  /** An alignment's root segment and its chain, for a pass of roads at authored heights. */
  readonly segment?: SegmentId;
  readonly chain?: readonly SegmentId[];
  /** The type of the root segment's road as the document has it (0 when it has none). */
  readonly rootType: number;
  readonly raised: boolean;
  readonly maxEdge: number;
  /** Whether the ground verge wears the terrain's own material (its UV in world x, y). */
  readonly groundMaterial: boolean;
  /** World size of one texture tile of each material. */
  readonly scale: { readonly asphalt: number; readonly footway: number; readonly kerb: number; readonly verge: number; readonly deck: number };
  /** The paint's colours, in the order the network paints them, and its height over the deck. */
  readonly paint: readonly string[];
  readonly paintLift: number;
  /** The carriageway's tints against the colour its texture was baked at, made on the page (three's colour management) */
  readonly classTint: readonly number[];
  readonly referenceTint: readonly [number, number, number];
  readonly pavingTint: Readonly<Record<string, readonly [number, number, number]>>;
}

// src/render/roadSurfaceReads.ts:64-74 interface RoadLook
/** What a surface reads of a road beside the field: its paving, widths and its carriageway's span between its mouths. */
export interface RoadLook {
  readonly width: number;
  /** One lane's width (`laneWidth`). */
  readonly lane: number;
  /** The carriageway between its junction mouths, arc length along the road. */
  readonly from: number;
  readonly to: number;
  readonly carriageway?: string | undefined;
  readonly footwayLeft?: FootwayMaterial | undefined;
  readonly footwayRight?: FootwayMaterial | undefined;
}

// src/render/roadSurfaceReads.ts:77-91 interface SurfaceReads
/** Everything a pass's surfaces read besides the point: on the page the network's, in a worker the roads handed to it. */
export interface SurfaceReads {
  readonly field: RoadReads;
  /** The ground as it is drawn. */
  terrainAt(x: number, y: number): number;
  /** A road's centreline (an alignment's chain, `segmentAt`). */
  lineOf(id: SegmentId): Polyline | undefined;
  /** A road's look; undefined for one without a ribbon. */
  look(id: SegmentId): RoadLook | undefined;
  /** The share of `FOOTWAY_RISE` the footway stands at (`footwayRiseShare`). */
  footwayShare(x: number, y: number): number;
  /** The walking surface's share of it over a dropped kerb (`walkingRise`, on a footway). */
  walkingRise(x: number, y: number, structure: string): number;
  /** The footways' outer edges of the pass, by cell (`footwayEdgeCell`): four numbers an edge. */
  footwayEdges(key: number): ArrayLike<number> | undefined;
}

// src/render/roadSurfaceReads.ts:94-99 interface SurfaceRead
/** One surface of a pass: what its shape is cut from and what each vertex reads. */
export interface SurfaceRead {
  readonly source: TileSource;
  readonly options: TileMeshOptions & { readonly name: string; readonly maxEdge: number };
  /** Its top or bottom reads the ground at its every vertex (`SurfaceReads.terrainAt`). */
  readonly ground?: boolean;
}

// src/render/roadSurfaceReads.ts:105-105 function offset
const offset = (base: HeightFn, amount: number): HeightFn => …;

// src/render/roadSurfaceReads.ts:108-359 function passSurfaces
/** The surfaces of a pass, outermost first, then its paint in the network's order. */
export function passSurfaces(pass: PassDesc, reads: SurfaceReads): SurfaceRead[];
