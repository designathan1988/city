// What the feature uses of src/render/roadSurfaceReads.ts (shared with the program): declarations as the source writes them, bodies left out.

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
