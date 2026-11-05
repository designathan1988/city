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

// src/world/buildings/validate.ts:121-121 variable ROAD_CONTACT_EPS
/** Only floating-point noise at an exactly touching road edge, never a visible overlap. */
const ROAD_CONTACT_EPS = 1e-9;

// src/world/buildings/validate.ts:123-123 variable JUNCTION_TOUCH
/** Area a footprint may share with a junction's carriageway and kerb and still only touch it, square units (0.16 m2) */
const JUNCTION_TOUCH = m(0.4) * m(0.4);

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

// src/world/buildings/validate.ts:280-280 variable lineOf
/** A road's full centreline as points, once per ribbon (ribbons are never changed, only replaced). */
const lineOf = identityMemo((ribbon: RoadReachRibbon): readonly Vec2[] => ribbon.full.toPoints());

// src/world/buildings/validate.ts:283-283 variable ringOf
/** A junction's kerb ring as points, once per ring (a junction solved again is a new one). */
const ringOf = identityMemo((ring: JunctionRing): readonly Vec2[] => ring.flatten());

// src/world/buildings/validate.ts:319-333 function polylineDistance
/** Shortest distance from an open polyline to a closed polygon (0 inside). */
function polylineDistance(line: readonly Vec2[], polygon: readonly Vec2[]): number;

// src/world/buildings/validate.ts:336-347 function polygonsOverlap
/** Whether two simple polygons overlap (share interior area, roughly). */
function polygonsOverlap(a: readonly Vec2[], b: readonly Vec2[]): boolean;
