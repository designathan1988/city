// The code of src/render/buildings/partWarm.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/partWarm.ts:8-30 slice of src/render/buildings/partWarm.ts#partWarmMeshes (shared): the feature's statements; "…" marks code left out
export function partWarmMeshes(kit: BuildingKit): Mesh[] {
  // … 13 lines
  for (const kind of FURNITURE_KINDS) {
    const mesh = new InstancedMesh(furniture.geometry[kind], furniture.material, 1);
    mesh.castShadow = true;
    mesh.customDepthMaterial = partPoseDepth(furniture.material);
    out.push(mesh);
  }
// … 2 lines
}
