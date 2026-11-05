// What the feature uses of src/world/section.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/section.ts:8-11 interface Band
/** THE CROSS-SECTION OF A ROAD: what lies where across it, from the centreline out, as named bands. */

/** A band across one side of the road: from `inner` to `outer`, measured from the centreline. */
export interface Band {
  readonly inner: number;
  readonly outer: number;
}

// src/world/section.ts:14-23 interface SideZones
/** One side of the road beyond the carriageway edge. */
export interface SideZones {
  /** The kerb stone. */
  readonly curb: Band;
  /** Beside the kerb: lamp columns, street trees, benches, bins. */
  readonly furnishing: Band;
  /** Where people walk. */
  readonly through: Band;
  /** Along the outer edge, on a footway wide enough for one; zero width otherwise. */
  readonly frontage: Band;
}

// src/world/section.ts:25-31 interface TravelLane
interface TravelLane {
  /** Signed offset of the lane's centreline (negative: right of the forward direction). */
  readonly offset: number;
  readonly width: number;
  /** Lane index within its direction, 0 innermost. */
  readonly index: number;
}

// src/world/section.ts:33-46 interface CrossSection
export interface CrossSection {
  /** Half the carriageway, kerb face to kerb face. */
  readonly carriageway: number;
  readonly median: number;
  /** Lanes of the forward (a to b) direction; a two-way road mirrors them for the other. */
  readonly lanes: readonly TravelLane[];
  /** The zones of a side when both sides are the same; on an asymmetric road the right one. */
  readonly side: SideZones;
  /** The footway zones left and right of a -> b. */
  readonly left: SideZones;
  readonly right: SideZones;
  /** Whether people walk along this road at all. */
  readonly walkable: boolean;
}

// src/world/section.ts:67-77 function sectionOf
/** The cross-section of a resolved road profile (`roadProfile`). */
export function sectionOf(rt: RoadType, direction: SegmentDirection = 'both'): CrossSection;

// src/world/section.ts:83-102 function sideZones
/** One side's footway, `sidewalk` wide (kerb included), cut into zones. */
function sideZones(rt: RoadType, sidewalk: number, flush: boolean, walkable: boolean): SideZones;
