// What the feature uses of src/world/buildings/validate.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/validate.ts:49-61 type BuildingProblem
/** Why a building cannot stand where it is. */
export type BuildingProblem =
  | 'cut'
  | 'setback'
  | 'roofSpace'
  | 'outline'
  | 'size'
  | 'overlap'
  | 'support'
  | 'footprint'
  | 'bounds'
  | 'road'
  | 'building'
  | 'slope';

// src/world/buildings/validate.ts:63-71 interface SiteContext
export interface SiteContext {
  readonly doc: RoadDoc;
  /** Null skips the road test (a headless check with no network built). */
  readonly net: Network | null;
  /** Null skips the slope test. */
  readonly groundAt: GroundAt | null;
  /** The paving's height (footways, carriageways), NaN off it: the street floor a grown building takes. */
  readonly pavedAt?: (x: number, y: number) => number;
}

// src/world/buildings/validate.ts:74-78 interface RoadReachRibbon
/** A road as the checks read it: its profile's widths and its whole centreline (`SegmentRibbon`). */
export interface RoadReachRibbon {
  readonly id: SegmentId;
  readonly road: Pick<RoadType, 'width' | 'sidewalk'>;
  readonly full: Polyline;
}

// src/world/buildings/validate.ts:81-85 interface JunctionRing
/** A junction's kerb ring as the checks read it (`core/ring.ts` `Ring`). */
export interface JunctionRing {
  readonly isEmpty: boolean;
  readonly bbox: Aabb;
  flatten(): readonly Vec2[];
}

// src/world/buildings/validate.ts:88-96 interface RoadReach
/** What a check of a building against the roads reads of the network (`touchesRoad`, the growth's corner test): the roads and junctions filed by place, each road's centreline and widths, each junction's kerb ring, each road's structure. `Network` is one; the growth's worker keeps another, of the same numbers, for the places it grows on (`editor/growthReplica.ts`). */
export interface RoadReach {
  readonly doc: { segment(id: SegmentId): { readonly structure: RoadStructure } | undefined };
  readonly segmentCells: { idsMeeting(rect: ChangeRect): readonly SegmentId[] };
  readonly ribbons: { get(id: SegmentId): RoadReachRibbon | undefined };
  readonly junctionCells: { idsMeeting(rect: ChangeRect): readonly NodeId[] };
  readonly junctions: { get(node: NodeId): { get(level: SurfaceLevel): { readonly ring: JunctionRing } | undefined } | undefined };
  /** The roads filed over `rect`, in increasing id order (`Network.ribbonsMeeting`). */
  ribbonsMeeting(rect: ChangeRect): readonly RoadReachRibbon[];
}

// src/world/buildings/validate.ts:117-117 variable ROAD_CLEARANCE
/** Gap kept between a footprint and the back of a footway: next to nothing. */
export const ROAD_CLEARANCE = m(0.008);

// src/world/buildings/validate.ts:198-204 function validateBuilding
/** The first reason `b` cannot stand, or null; `ignore` is the building being edited (not its own obstacle). */
export function validateBuilding(
  ctx: SiteContext,
  b: Building,
  ignore?: BuildingId | readonly BuildingId[],
): BuildingProblem | null;

// src/world/buildings/validate.ts:256-277 function touchesRoad
/** DO NOT go back to a grid of every road and junction of the map made again at each network revision (`RoadContacts`) */
export function touchesRoad(net: RoadReach, rect: readonly Vec2[]): boolean;
