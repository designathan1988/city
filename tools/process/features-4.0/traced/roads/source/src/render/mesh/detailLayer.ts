// The code of src/render/mesh/detailLayer.ts that is the feature's (the rest of the file is shared).

// src/render/mesh/detailLayer.ts:17-23 slice of src/render/mesh/detailLayer.ts#detailTextures (shared): the feature's statements; "…" marks code left out
function detailTextures(kind: DetailKind, anisotropy: number): DetailTextures {
  return {
    map: surfaceTexture(`${kind}:detail`, anisotropy),
    normalMap: surfaceTexture(`${kind}:detail-normal`, anisotropy),
  // … 1 lines
  };
}
