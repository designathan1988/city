// What the feature uses of src/render/terrain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrain.ts:120-123 type TerrainSource
/** What the surface reads of the document: one plate's share of it, in the plate's own coordinates (`flatAtlas.ts`). */
export type TerrainSource = Pick<RoadDoc, 'changes' | 'terrainRevision' | 'terrainStamps' | 'terrainRelief' | 'natureRevision' | 'nature' | 'paintRevision' | 'terrainPaint'> & {
  /** The map's relief under the stamps, in the plate's own coordinates. */
  readonly terrainGround?: (x: number, y: number) => number;
};

// src/render/terrain.ts:374-380 interface TerrainBakes
/** The land's baked textures (`terrainBakes`). */
interface TerrainBakes {
  /** The grass's maps, and its colour map's mean luma (`meanLuma`). */
  readonly grass: { readonly map: Texture; readonly normalMap: Texture; readonly mean: number };
  /** The detail's grass and soil, the rocks and the soil as the layers of one colour array and one normal array (`LAND_ROCK`, `LAND_DIRT`). */
  readonly landMaps: Texture;
  readonly landNormals: Texture;
}

// src/render/terrain.ts:383-386 interface TerrainData
/** What a plate's land materials share: their uniforms, kept in `userData` too for the plate to change. */
interface TerrainData {
  readonly userData: Record<string, unknown>;
  readonly uniforms: Record<string, { value: unknown }>;
}

// src/render/terrain.ts:389-392 interface TerrainLodShader
/** The ground of a plate drawn by distance (`terrainLod.ts`): its grid and the uniforms its patches read. */
interface TerrainLodShader {
  readonly grid: LandGrid;
  readonly uniforms: Record<string, { value: unknown }>;
}

// src/render/terrain.ts:395-420 function terrainMaterial
/** The land's material: on an ordinary mesh of the ground (`lod` null: the far patches, the verges), or on a plate's patches (`terrainLod.ts`). */
function terrainMaterial(bakes: TerrainBakes, data: TerrainData, lod: TerrainLodShader | null): MeshStandardMaterial;
