// The code of src/render/buildings/partPose.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/partPose.ts:22-26 function poseMatrices: the feature's own
export function poseMatrices(poses: Float32Array, count: number): Float32Array {
  const out = new Float32Array(count * 16);
  for (let i = 0; i < count; i++) poseMatrixInto(poses, i * POSE_FLOATS, out, i * 16);
  return out;
}
