// What the feature uses of src/render/mesh/surfaceMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/surfaceMesh.ts:15-23 interface SurfaceMeshOptions
/** Turns a clipped surface band into a solid */

export interface SurfaceMeshOptions extends TileMeshOptions {
  readonly name: string;
  readonly polygons: MultiPoly;
  readonly material: Material;
  /** Longest triangle edge, in world units, before it is refined. */
  readonly maxEdge: number;
  readonly castShadow?: boolean;
  readonly receiveShadow?: boolean;
}
