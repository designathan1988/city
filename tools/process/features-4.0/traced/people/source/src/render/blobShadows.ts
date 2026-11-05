// The code of src/render/blobShadows.ts that is the feature's (the rest of the file is shared).

// src/render/blobShadows.ts:5-14 interface BlobShadows: the feature's own
export interface BlobShadows {
  readonly mesh: InstancedMesh;
  /** Starts a frame's list. */
  begin(): void;
  /** One under `matrix` (its origin on the ground): `radius` across and `length` along, in the matrix's own units. */
  add(matrix: Matrix4, radius: number, length?: number): void;
  /** Ends the list: only what was written goes to the GPU. */
  finish(): void;
  dispose(): void;
}

// src/render/blobShadows.ts:63-103 function createBlobShadows: the feature's own
export function createBlobShadows(name: string, capacity: number, opacity = 0.34): BlobShadows {
  const mesh = blobMesh(name, capacity, opacity);
  const local = new Matrix4();
  const placed = new Matrix4();
  let n = 0;
  /** `capacity` is where it starts, not a ceiling: DO NOT drop what passes it */
  const grow = (needed: number): void => {
    let size = mesh.instanceMatrix.count;
    while (size < needed) size *= 2;
    const matrices = new InstancedBufferAttribute(new Float32Array(size * 16), 16);
    matrices.array.set(mesh.instanceMatrix.array);
    matrices.setUsage(DynamicDrawUsage);
    mesh.dispose();
    mesh.instanceMatrix = matrices;
  };
  return {
    mesh,
    begin() { n = 0; },
    add(matrix, radius, length = radius) {
      if (n >= mesh.instanceMatrix.count) grow(n + 1);
      local.makeScale(radius, 1, length);
      local.elements[13] = LIFT;
      mesh.setMatrixAt(n++, placed.multiplyMatrices(matrix, local));
    },
    finish() {
      mesh.count = n;
      mesh.visible = n > 0;
      const matrices = mesh.instanceMatrix;
      matrices.clearUpdateRanges();
      if (n > 0) {
        matrices.addUpdateRange(0, n * 16);
        matrices.needsUpdate = true;
      }
    },
    dispose() {
      mesh.dispose();
      mesh.geometry.dispose();
      (mesh.material as Material).dispose();
    },
  };
}
