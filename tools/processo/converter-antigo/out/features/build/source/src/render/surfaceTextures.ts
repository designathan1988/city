// The code of src/render/surfaceTextures.ts that is the feature's (the rest of the file is shared).

// src/render/surfaceTextures.ts:84-89 function surfaceMaps: the feature's own
export function surfaceMaps(key: string, anisotropy: number): SurfaceBake | undefined {
  const map = surfaceTextureOrNone(`${key}:map`, anisotropy);
  const normalMap = surfaceTextureOrNone(`${key}:normal`, anisotropy);
  const roughnessMap = surfaceTextureOrNone(`${key}:rough`, anisotropy);
  return map && normalMap && roughnessMap ? { map, normalMap, roughnessMap } : undefined;
}
