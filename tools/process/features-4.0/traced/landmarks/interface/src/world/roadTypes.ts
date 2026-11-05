// What the feature uses of src/world/roadTypes.ts (shared with the program): declarations as the source writes them, bodies left out.

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
