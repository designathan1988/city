// The code of src/render/terrain.ts that is the feature's (the rest of the file is shared).

// src/render/terrain.ts:395-420 slice of src/render/terrain.ts#terrainMaterial (shared): the feature's statements; "…" marks code left out
function terrainMaterial(bakes: TerrainBakes, data: TerrainData, lod: TerrainLodShader | null): MeshStandardMaterial {
  // … 22 lines
  applyLotOverlay(material, { world: 'vTerrainAtlas' });
// … 1 lines
}
