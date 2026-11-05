// The code of src/render/terrain.ts that is the feature's (the rest of the file is shared).

// src/render/terrain.ts:125-208 slice of src/render/terrain.ts#TerrainSurface (shared): the feature's statements; "…" marks code left out
export interface TerrainSurface {
  // … 9 lines
  setGullies(dabs: readonly GullyDab[], auto: number): void;
// … 72 lines
}

// src/render/terrain.ts:242-250 slice of src/render/terrain.ts#terrainBakes (shared): the feature's statements; "…" marks code left out
function terrainBakes(anisotropy: number): TerrainBakes {
  const map = surfaceTexture('terrain-grass:map', anisotropy);
  grassMean ??= meanLuma(map);
  return {
    grass: { map, normalMap: surfaceTexture('terrain-grass:normal', anisotropy), mean: grassMean },
    landMaps: surfaceTexture('terrain-land:map', anisotropy),
    landNormals: surfaceTexture('terrain-land:normal', anisotropy),
  };
}
