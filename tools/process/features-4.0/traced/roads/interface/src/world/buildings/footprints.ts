// What the feature uses of src/world/buildings/footprints.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/footprints.ts:63-67 function overlapArea
/** The area two rings share. DO NOT clip two rings whose boxes are apart (a building's boundary piece against every ring of its neighbours, each a polygon clipping: 11.7 s of a tile maker's 89 s, 2026-10-16): no area in common, exactly. */
/** DO NOT sweep two convex rings with the general clipper (Martinez, `polygon-clipping`): nearly every block, lot and part is a rectangle, and the share of two convex rings is one ring clipped by the other's edges (Sutherland and Hodgman 1974). Only a ring that is not convex goes to the sweep. */
export function overlapArea(a: readonly Vec2[], b: readonly Vec2[]): number;

// src/world/buildings/footprints.ts:170-181 function offsetRing
/** Small miter offsets for selection outlines and ground clearance. */
export function offsetRing(ring: readonly Vec2[], distance: number): Vec2[];
