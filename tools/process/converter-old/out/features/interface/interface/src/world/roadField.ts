// What the feature uses of src/world/roadField.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roadField.ts:12-12 variable BLEND_TAU
/** THE READS OF THE ROADS' HEIGHT FIELD (`elevation.ts`) */

/** Softness of the blend between neighbouring profiles, in world units. */
export const BLEND_TAU = m(1);

// src/world/roadField.ts:16-16 variable PROFILE_REACH
/** How far from a road its own profile is the whole answer. */
export const PROFILE_REACH = m(24);

// src/world/roadField.ts:57-75 interface RoadSample
interface RoadSample {
  /** Distance along the nearest road, in world units. */
  readonly along: number;
  /** Signed offset across it. */
  readonly across: number;
  /** Class index of that road, or -1 when there is no road near the point. */
  readonly type: number;
  /** Half the casing width of that road. */
  readonly half: number;
  /** Its central reservation width. */
  readonly median: number;
  /** Its actual footway width, including the kerb. Optional for older query adapters. */
  readonly sidewalk?: number;
  /** Each side's footway, left and right of a -> b, when they differ. */
  readonly sidewalkLeft?: number;
  readonly sidewalkRight?: number;
  /** The segment the point was read off. */
  readonly segment?: SegmentId;
}

// src/world/roadField.ts:78-88 interface RoadSampleOut
/** `RoadSample` as `roadAtInto` writes it, one kept by its reader: every field written at each read. */
export interface RoadSampleOut {
  along: number;
  across: number;
  type: number;
  half: number;
  median: number;
  sidewalk: number | undefined;
  sidewalkLeft: number | undefined;
  sidewalkRight: number | undefined;
  segment: SegmentId | undefined;
}

// src/world/roadField.ts:96-122 interface RoadReads
/** The reads of the field a road surface makes per vertex (`render/roadSurfaceReads.ts`). */
export interface RoadReads {
  /** Deck height of the road surface at a world point. */
  at(x: number, y: number, structures?: ReadonlySet<RoadStructure>, includeManual?: boolean): number;
  /** Deck height on one specific segment — what an agent riding it stands on. */
  onSegment(segment: SegmentId, x: number, y: number): number;
  /** Everything a surface needs to know about the road nearest a point. */
  roadAt(x: number, y: number, structures?: ReadonlySet<RoadStructure>, includeManual?: boolean): RoadSample;
  /** `roadAt` written into `out`, nothing made (the paving read under every building's footprint, `paving.ts`); `sidewalkLeft`/`Right` only when they differ. */
  roadAtInto?(x: number, y: number, structures: ReadonlySet<RoadStructure> | undefined, out: RoadSampleOut, includeManual?: boolean): RoadSampleOut;
  /** Texture coordinates of (x, y) in the frame of the road nearest to */
  surfaceFrameAt(
    x: number,
    y: number,
    structures: ReadonlySet<RoadStructure> | undefined,
    pickX: number,
    pickY: number,
    includeManual?: boolean,
    segment?: SegmentId,
    /** Lay `along` at the true length of each line parallel to the road */
    panel?: number,
  ): { along: number; across: number };
  /** `surfaceFrameAt` written into `out`, nothing made (asked for every vertex of a road surface, `roadSurfaceReads.ts`). */
  surfaceFrameInto?(x: number, y: number, structures: ReadonlySet<RoadStructure> | undefined, pickX: number, pickY: number,
    includeManual: boolean | undefined, segment: SegmentId | undefined, panel: number | undefined, out: { along: number; across: number }): { along: number; across: number };
  /** Runs `work` with every point it reads remembered, so a point asked again */
  remembering?<T>(work: () => T): T;
}

// src/world/roadField.ts:657-669 interface FieldData
/** The profiles a batch of road tiles reads, as typed arrays handed to a worker (MDN, "Transferable objects") */
export interface FieldData {
  readonly ids: Float64Array;
  readonly structures: readonly RoadStructure[];
  /** Per profile: manual, type, half, median, sidewalk, left, right, length, step, box (4). */
  readonly numbers: Float64Array;
  /** Where each profile's line ends in `lines`, and its heights in `heights`. */
  readonly lineEnds: Uint32Array;
  readonly lines: Float64Array;
  readonly heightEnds: Uint32Array;
  readonly heights: Float64Array;
  /** The index cells the profiles answer for (`SpatialIndex.meeting`). */
  readonly cells: Float64Array;
}
