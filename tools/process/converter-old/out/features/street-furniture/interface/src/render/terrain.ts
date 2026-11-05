// What the feature uses of src/render/terrain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrain.ts:120-123 type TerrainSource
/** What the surface reads of the document: one plate's share of it, in the plate's own coordinates (`flatAtlas.ts`). */
export type TerrainSource = Pick<RoadDoc, 'changes' | 'terrainRevision' | 'terrainStamps' | 'terrainRelief' | 'natureRevision' | 'nature' | 'paintRevision' | 'terrainPaint'> & {
  /** The map's relief under the stamps, in the plate's own coordinates. */
  readonly terrainGround?: (x: number, y: number) => number;
};
