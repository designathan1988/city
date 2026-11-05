// The code of src/lab/catalog/capture.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/capture.ts:47-70 slice of src/lab/catalog/capture.ts#instanceMatrices (shared): the feature's statements; "…" marks code left out
function instanceMatrices(mesh: InstancedMesh): Float32Array {
  // … 4 lines
  if (pose && material && String(material.customProgramCacheKey()).endsWith('|part-pose')) {
    // … 3 lines
    for (let i = 0; i < mesh.count; i++) poseMatrixInto(data, pose.offset + i * stride, local, i * 16);
  } else if (pose) {
  // … 2 lines
  } else local = (mesh.instanceMatrix.array as Float32Array).slice(0, mesh.count * 16);
// … 9 lines
}
