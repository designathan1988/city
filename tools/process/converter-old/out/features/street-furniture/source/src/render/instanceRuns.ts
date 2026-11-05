// The code of src/render/instanceRuns.ts that is the feature's (the rest of the file is shared).

// src/render/instanceRuns.ts:37-37 function nearCode: the feature's own
export const nearCode = (cx: number, cy: number): number => (cx + NEAR_CODE_SPAN / 2) * NEAR_CODE_SPAN + (cy + NEAR_CODE_SPAN / 2);

// src/render/instanceRuns.ts:196-198 function warmMeshes: the feature's own
export function warmMeshes(specs: readonly RunStore[], format: PoseFormat = YAW_POSE): InstancedMesh[] {
  return specs.flatMap((spec) => kindOf(spec).meshes.map((mesh) => warmMesh(mesh, spec.tinted, format)));
}
