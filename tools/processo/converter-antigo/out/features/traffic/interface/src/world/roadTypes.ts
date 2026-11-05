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

// src/world/roadTypes.ts:97-230 variable ROAD_TYPES
export const ROAD_TYPES: readonly RoadType[] = [
  {
    id: 'local',
    nameKey: 'road.local',
    subKey: 'road.sub.local',
    subLanes: null,
    subOneWay: false,
    width: m(6),
    lanes: 2,
    sidewalk: m(2),
    median: 0,
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(30),
    priorityRank: 0,
    markings: 'none',
    color: '#5f6365',
    edge: '#aaa9a5',
    curb: '#d5d2cb',
    line: '#e1c45a',
  },
  {
    id: 'urban',
    nameKey: 'road.urban',
    subKey: 'road.sub.urban',
    subLanes: null,
    subOneWay: false,
    width: m(8),
    lanes: 2,
    sidewalk: m(2),
    median: 0,
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(50),
    priorityRank: 1,
    markings: 'center',
    color: '#3a3d3f',
    edge: '#aaa7a2',
    curb: '#d2cec7',
    line: '#e1c45a',
  },
  {
    id: 'avenue',
    nameKey: 'road.avenue',
    subKey: 'road.sub.avenue',
    subLanes: null,
    subOneWay: false,
    width: m(14),
    lanes: 4,
    sidewalk: m(2),
    median: 0,
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(60),
    priorityRank: 2,
    markings: 'lanes',
    color: '#35383a',
    edge: '#aaa7a1',
    curb: '#d3cfc7',
    line: '#eee8d7',
  },
  {
    id: 'boulevard',
    nameKey: 'road.boulevard',
    subKey: 'road.sub.boulevard',
    subLanes: null,
    subOneWay: false,
    width: m(18),
    lanes: 4,
    sidewalk: m(3),
    median: m(2),
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(60),
    priorityRank: 3,
    markings: 'lanes',
    color: '#333638',
    edge: '#aaa69f',
    curb: '#d4cfc7',
    line: '#eee8d8',
  },
  {
    id: 'highway',
    nameKey: 'road.highway',
    subKey: 'road.sub.highway',
    subLanes: null,
    subOneWay: false,
    width: m(18),
    lanes: 4,
    sidewalk: m(1),
    median: m(2),
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(100),
    priorityRank: 4,
    markings: 'lanes',
    color: '#343638',
    edge: '#aaa7a0',
    curb: '#c9c7c2',
    line: '#eee8d8',
  },
  {
    id: 'ramp',
    nameKey: 'road.ramp',
    subKey: 'road.sub.ramp',
    subLanes: null,
    subOneWay: true,
    width: m(4),
    lanes: 1,
    sidewalk: m(1),
    median: 0,
    parkingLeft: 0,
    parkingRight: 0,
    parkingLeftKind: 'none',
    parkingRightKind: 'none',
    speedLimit: kmh(60),
    priorityRank: 3,
    markings: 'none',
    color: '#3b3d3f',
    edge: '#aaa7a0',
    curb: '#c9c7c2',
    line: '#eee8d8',
  },
];

// src/world/roadTypes.ts:233-233 variable LAST_UPGRADE_CLASS
/** Street classes form the upgrade ladder; highway and access ramps are separate choices. */
export const LAST_UPGRADE_CLASS = 3;

// src/world/roadTypes.ts:235-236 function roadType
export const roadType = (i: number): RoadType => …;

// src/world/roadTypes.ts:243-243 variable NO_PART
/** A segment with no section, or no parking: one key object for all of them (`roadProfile`'s memo). */
const NO_PART = Object.freeze({});

// src/world/roadTypes.ts:245-245 variable profilesOf
/** Each segment's profile, by its section and parking (records replaced on an edit, never changed) and its class, lanes and direction. */
const profilesOf = identityMemo((_section: object) => identityMemo((_parking: object) => new Map<string, RoadType>()));

// src/world/roadTypes.ts:248-260 function roadProfile
/** Resolves the physical road profile for a segment: DO NOT build it again for every leg of every pass of a junction (`junction/build.ts`, `derive.ts`, `bulb.ts`, a pair of lanes in `junctionBuild.ts`): worked out once for its section, parking, class, lanes and direction. */
export function roadProfile(
  typeIndex: number,
  configuredLanes?: number | null,
  direction: SegmentDirection = 'both',
  section?: RoadSection,
  parking?: SegmentParking,
): RoadType;

// src/world/roadTypes.ts:262-285 function resolveProfile
function resolveProfile(
  typeIndex: number,
  configuredLanes: number | null | undefined,
  direction: SegmentDirection,
  section: RoadSection | undefined,
  parking: SegmentParking | undefined,
): RoadType;

// src/world/roadTypes.ts:339-350 function halfWidth
/** Half-width of a road type at a given surface level. */
export function halfWidth(rt: Pick<RoadType, 'width' | 'sidewalk'>, level: SurfaceLevel): number;

// src/world/roadTypes.ts:353-353 type RoadSide
/** One side of a road, left or right of a -> b. */
export type RoadSide = 'left' | 'right';

// src/world/roadTypes.ts:384-385 function casingHalf
export const casingHalf = (rt: RoadType): number => …;

// src/world/roadTypes.ts:391-392 function travelLanes
/** Number of lanes a vehicle may use in one travelling direction. */
export const travelLanes = (rt: RoadType, direction: SegmentDirection): number => …;

// src/world/roadTypes.ts:395-396 function laneWidth
/** Width of one travel lane: the carriageway less its median and parking lanes. */
export const laneWidth = (rt: RoadType): number => …;

// src/world/roadTypes.ts:420-427 function laneOffset
/** Signed lateral offset of a lane's centreline from the road centreline. */
export function laneOffset(rt: RoadType, lane: number, direction: SegmentDirection = 'both', forward = true): number;
