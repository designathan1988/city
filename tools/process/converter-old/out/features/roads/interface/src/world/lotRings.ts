// What the feature uses of src/world/lotRings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lotRings.ts:6-20 function simplifyRing
/** A LOT'S RING, pure geometry (`lots.ts`) */

/** A ring with the points that lie within `tol` of the line through their neighbours taken out, again and again */
export function simplifyRing(ring: readonly Vec2[], tol: number): Vec2[];

// src/world/lotRings.ts:56-66 function lotCentre
/** The boundary's area centroid (the corners' mean for a degenerate one). */
export function lotCentre(l: { readonly corners: readonly Vec2[] }): Vec2;
