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

// src/world/structures.ts:37-37 variable TUNNEL_ROOF
export const TUNNEL_ROOF = TUNNEL_CLOSE - m(0.16);

// src/world/structures.ts:38-38 variable TUNNEL_BORE
export const TUNNEL_BORE = TUNNEL_CLOSE + m(0.16);

// src/world/structures.ts:42-42 variable TUNNEL_GRADE
/** Design gradient of a tunnel's approach ramps. */
export const TUNNEL_GRADE = ROAD_TUNING.grade.tunnel;

// src/world/structures.ts:53-53 variable ROAD_GROUND_CLEARANCE
/** Clearance a road at grade keeps over the terrain it is laid on. */
export const ROAD_GROUND_CLEARANCE = m(0.12);

// src/world/structures.ts:55-56 function roadStructure
export const roadStructure = (id: RoadStructure): RoadStructureSpec => …;

// src/world/structures.ts:68-69 function isRaised
/** True for the structures that stand clear of the ground on piers. */
export const isRaised = (id: RoadStructure): boolean => …;
