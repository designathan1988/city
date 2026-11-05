// What the feature uses of src/core/polygon.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/polygon.ts:29-37 function signedArea
export function signedArea(points: readonly Vec2[]): number;

// src/core/polygon.ts:40-51 function pointInPolygon
/** Ray-casting containment test. Points exactly on an edge are unspecified. */
export function pointInPolygon(p: Vec2, points: readonly Vec2[]): boolean;
