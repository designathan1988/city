// What the feature uses of src/world/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/structures.ts:5-5 type RoadStructure
/** The structural levels a road can be built at. */
export type RoadStructure = 'ground' | 'elevated' | 'bridge' | 'tunnel';

// src/world/structures.ts:17-27 interface RoadStructureSpec
export interface RoadStructureSpec {
  readonly id: RoadStructure;
  /** Translation key for the structure's name, resolved by `ui/i18n`. */
  readonly key: string;
  /** Height the deck keeps above the highest ground under the span, in units. */
  readonly clearance: number;
  /** Thickness of the structural deck drawn under the asphalt. */
  readonly deck: number;
  /** Whether the structure stands on piers. */
  readonly supports: boolean;
}

// src/world/structures.ts:30-30 variable TUNNEL_HEADROOM
/** Clear height inside a tunnel bore, in units (about 4.4 m). */
export const TUNNEL_HEADROOM = m(4.4);

// src/world/structures.ts:32-32 variable TUNNEL_ARCH
/** Thickness of the arch over the bore. */
export const TUNNEL_ARCH = m(0.6);

// src/world/structures.ts:34-34 variable TUNNEL_DEPTH
/** Depth of a tunnel's floor below the lowest ground along its span. */
const TUNNEL_DEPTH = TUNNEL_HEADROOM + TUNNEL_ARCH + m(2);

// src/world/structures.ts:36-36 variable TUNNEL_CLOSE
/** Cover at which the ground closes over the road: the mouth of the bore. */
const TUNNEL_CLOSE = TUNNEL_HEADROOM + TUNNEL_ARCH + m(0.4);

// src/world/structures.ts:37-37 variable TUNNEL_ROOF
export const TUNNEL_ROOF = TUNNEL_CLOSE - m(0.16);

// src/world/structures.ts:38-38 variable TUNNEL_BORE
export const TUNNEL_BORE = TUNNEL_CLOSE + m(0.16);

// src/world/structures.ts:40-40 variable TUNNEL_PORTAL_COVER
/** Cover at which the portal stands: where the ground FIRST rises over the road. */
export const TUNNEL_PORTAL_COVER = m(0.6);

// src/world/structures.ts:42-42 variable TUNNEL_GRADE
/** Design gradient of a tunnel's approach ramps. */
export const TUNNEL_GRADE = ROAD_TUNING.grade.tunnel;

// src/world/structures.ts:44-50 variable ROAD_STRUCTURES
export const ROAD_STRUCTURES: readonly RoadStructureSpec[] = [
  { id: 'ground', key: 'structure.ground', clearance: 0, deck: m(0.22), supports: false },
  // Fourteen units (5.6 m) to the deck SURFACE, of which the structure itself takes 3.0
  { id: 'elevated', key: 'structure.elevated', clearance: ROAD_TUNING.clearance.elevated, deck: m(1.04), supports: true },
  { id: 'bridge', key: 'structure.bridge', clearance: ROAD_TUNING.clearance.bridge, deck: m(0.96), supports: true },
  { id: 'tunnel', key: 'structure.tunnel', clearance: -TUNNEL_DEPTH, deck: m(0.24), supports: false },
] as const;

// src/world/structures.ts:53-53 variable ROAD_GROUND_CLEARANCE
/** Clearance a road at grade keeps over the terrain it is laid on. */
export const ROAD_GROUND_CLEARANCE = m(0.12);

// src/world/structures.ts:55-56 function roadStructure
export const roadStructure = (id: RoadStructure): RoadStructureSpec => …;

// src/world/structures.ts:65-65 variable RAISED_LIFT
/** Deck over the ground past which a road at grade stands on piers (2 m): the renderer builds them past it */
export const RAISED_LIFT = m(2);

// src/world/structures.ts:68-69 function isRaised
/** True for the structures that stand clear of the ground on piers. */
export const isRaised = (id: RoadStructure): boolean => …;

// src/world/structures.ts:75-75 variable TUNNELS_DRAWN
/** Whether tunnels are part of the game at all. */
export const TUNNELS_DRAWN = true;
