// What the feature uses of src/lab/inspector/entity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/inspector/entity.ts:35-35 function toMap
/** Three's world point (x, height, -y) as the map's (x, y, z up). */
export const toMap = (p: Vector3): { x: number; y: number; z: number } => …;

// src/lab/inspector/entity.ts:67-102 function entitiesAt
/** Every entity under a pixel, the first the one the selection takes: the layer of the 3D object hit first says which kind leads. */
export function entitiesAt(ctx: LabContext, px: number, py: number, ray: Ray | null, first: Hit | null): EntityRef[];
