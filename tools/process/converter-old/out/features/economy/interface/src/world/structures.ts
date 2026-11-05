// What the feature uses of src/world/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/structures.ts:5-5 type RoadStructure
/** The structural levels a road can be built at. */
export type RoadStructure = 'ground' | 'elevated' | 'bridge' | 'tunnel';

// src/world/structures.ts:30-30 variable TUNNEL_HEADROOM
/** Clear height inside a tunnel bore, in units (about 4.4 m). */
export const TUNNEL_HEADROOM = m(4.4);

// src/world/structures.ts:32-32 variable TUNNEL_ARCH
/** Thickness of the arch over the bore. */
export const TUNNEL_ARCH = m(0.6);

// src/world/structures.ts:36-36 variable TUNNEL_CLOSE
/** Cover at which the ground closes over the road: the mouth of the bore. */
const TUNNEL_CLOSE = TUNNEL_HEADROOM + TUNNEL_ARCH + m(0.4);

// src/world/structures.ts:38-38 variable TUNNEL_BORE
export const TUNNEL_BORE = TUNNEL_CLOSE + m(0.16);

// src/world/structures.ts:65-65 variable RAISED_LIFT
/** Deck over the ground past which a road at grade stands on piers (2 m): the renderer builds them past it */
export const RAISED_LIFT = m(2);

// src/world/structures.ts:68-69 function isRaised
/** True for the structures that stand clear of the ground on piers. */
export const isRaised = (id: RoadStructure): boolean => …;
