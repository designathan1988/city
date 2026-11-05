// The code of src/world/terrain.ts that is the feature's (the rest of the file is shared).

// src/world/terrain.ts:30-30 function isTerrainProfile: the feature's own
export const isTerrainProfile = (value: unknown): value is TerrainProfile => value === 'dome';

// src/world/terrain.ts:294-295 function isTerrainMode: the feature's own
export const isTerrainMode = (value: unknown): value is TerrainMode =>
  value === 'raise' || value === 'lower' || value === 'flatten' || value === 'river';
