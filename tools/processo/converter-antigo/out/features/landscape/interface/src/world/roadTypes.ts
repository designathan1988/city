// What the feature uses of src/world/roadTypes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roadTypes.ts:7-7 type SegmentDirection
/** Legal driving directions, relative to the stored `a -> b` orientation (`RoadSegment.direction`; `doc.ts` re-exports it). */
export type SegmentDirection = 'both' | 'aToB' | 'bToA';

// src/world/roadTypes.ts:10-15 enum Level
/** Surface levels, in painting order. */
export enum Level {
  Casing = 1,
  Sidewalk = 2,
  Curb = 3,
  Asphalt = 4,
}

// src/world/roadTypes.ts:18-23 variable SURFACE_LEVELS
/** Levels that carry a filled surface built from roads and junctions alike. */
export const SURFACE_LEVELS = [
  Level.Casing,
  Level.Sidewalk,
  Level.Curb,
  Level.Asphalt,
] as const;

// src/world/roadTypes.ts:25-25 type SurfaceLevel
export type SurfaceLevel = (typeof SURFACE_LEVELS)[number];

// src/world/roadTypes.ts:27-27 type MarkingStyle
type MarkingStyle = 'none' | 'center' | 'lanes';

// src/world/roadTypes.ts:29-80 interface RoadType
export interface RoadType {
  readonly id: string;
  /** Translation key for the class name, resolved by `ui/i18n`. */
  readonly nameKey: string;
  /** Translation key for the one-line description under the name. */
  readonly subKey: string;
  /** Lane count to substitute into `subKey` when the class has been overridden. */
  readonly subLanes: number | null;
  /** Whether an overridden description is for a one-way road. */
  readonly subOneWay: boolean;
  /** Carriageway width, kerb face to kerb face, in world units: the travel lanes, the median and any parking lanes. */
  readonly width: number;
  /** Total lane count, both directions. */
  readonly lanes: number;
  /** Footway width, in world units: on each side, or the wider one when the sides differ */
  readonly sidewalk: number;
  /** The footways left and right of a -> b when they differ; absent: `sidewalk` both. */
  readonly sidewalkLeft?: number;
  readonly sidewalkRight?: number;
  /** A footway level with the carriageway, without a kerb (a shared surface). */
  readonly flushLeft?: boolean;
  readonly flushRight?: boolean;
  /** A central reservation painted on the carriageway, not kerbed. */
  readonly medianFlush?: boolean;
  /** What its elements are paved with; the class's own look when absent. */
  readonly materials?: SectionMaterials;
  /** Central reservation width, in world units. Zero when absent. */
  readonly median: number;
  /** Parking lanes inside the kerbs, left and right of the a -> b direction: their depth from the kerb face */
  readonly parkingLeft: number;
  readonly parkingRight: number;
  readonly parkingLeftKind: ParkingKind;
  readonly parkingRightKind: ParkingKind;
  /** Free-flow speed, in world units per second. */
  readonly speedLimit: number;
  /** Higher wins right of way at unsignalised junctions. */
  readonly priorityRank: number;
  readonly turnsForward?: readonly LaneTurnRule[];
  readonly turnsBackward?: readonly LaneTurnRule[];
  /** Lane uses and lines from the section (`RoadSection.useForward`). */
  readonly useForward?: readonly LaneUse[];
  readonly useBackward?: readonly LaneUse[];
  readonly linesForward?: readonly LaneLine[];
  readonly linesBackward?: readonly LaneLine[];
  /** The line between the directions (V6, `RoadSection.centreLine`). */
  readonly centreLine?: CentreLine;
  readonly markings: MarkingStyle;
  readonly color: string;
  readonly edge: string;
  readonly curb: string;
  readonly line: string;
}

// src/world/roadTypes.ts:86-86 variable LANE_LINE
/** The line between two lanes running the SAME way: always white. */
export const LANE_LINE = '#eee8d7';

// src/world/roadTypes.ts:90-90 variable CURB_BAND
/** The kerb stone's width on top, beyond the carriageway edge: 15 cm, as a precast kerb. */
export const CURB_BAND = m(0.15);

// src/world/roadTypes.ts:93-93 variable FOOTWAY_RISE
/** Height of the footway above the carriageway, world units. */
export const FOOTWAY_RISE = m(0.15);

// src/world/roadTypes.ts:95-95 variable CASING_BAND
/** Extra half-width of the casing beyond the footway edge: the grass verge that carries the footway's edge down */
export const CASING_BAND = m(1);

// src/world/roadTypes.ts:248-260 function roadProfile
/** Resolves the physical road profile for a segment: DO NOT build it again for every leg of every pass of a junction (`junction/build.ts`, `derive.ts`, `bulb.ts`, a pair of lanes in `junctionBuild.ts`): worked out once for its section, parking, class, lanes and direction. */
export function roadProfile(
  typeIndex: number,
  configuredLanes?: number | null,
  direction: SegmentDirection = 'both',
  section?: RoadSection,
  parking?: SegmentParking,
): RoadType;

// src/world/roadTypes.ts:339-350 function halfWidth
/** Half-width of a road type at a given surface level. */
export function halfWidth(rt: Pick<RoadType, 'width' | 'sidewalk'>, level: SurfaceLevel): number;

// src/world/roadTypes.ts:353-353 type RoadSide
/** One side of a road, left or right of a -> b. */
export type RoadSide = 'left' | 'right';

// src/world/roadTypes.ts:356-357 function footwayOn
/** The footway on one side of a -> b, kerb included. */
export const footwayOn = (rt: RoadType, side: RoadSide): number => …;

// src/world/roadTypes.ts:383-383 function sidewalkHalf
export const sidewalkHalf = (rt: RoadType): number => …;

// src/world/roadTypes.ts:384-385 function casingHalf
export const casingHalf = (rt: RoadType): number => …;

// src/world/roadTypes.ts:403-404 function travelShift
/** Lateral offset of the travel way's centre from the road centreline, in the a -> b frame (positive: left). */
export const travelShift = (rt: RoadType): number => …;
