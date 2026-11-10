// The code of src/render/people/heldProps.ts that is the feature's (the rest of the file is shared).

// src/render/people/heldProps.ts:128-134 function createHeldProps.put: the feature's own
  const put = (mesh: InstancedMesh, matrix: Matrix4): void => {
    mesh.instanceMatrix.addUpdateRange(mesh.count * 16, 16);
    mesh.setMatrixAt(mesh.count++, matrix);
    mesh.instanceMatrix.needsUpdate = true;
    fitInstances(mesh, mesh.count);
    mesh.visible = true;
  };

// src/render/people/heldProps.ts:137-145 method createHeldProps.begin: the feature's own
    begin() {
      heldScale = 0;
      // Empty until something is placed: not drawn
      for (const mesh of meshes.values()) {
        mesh.count = 0;
        mesh.visible = false;
        mesh.instanceMatrix.clearUpdateRanges();
      }
    },

// src/render/people/heldProps.ts:146-161 method createHeldProps.place: the feature's own
    place(kind, hand, scale) {
      const mesh = meshes.get(kind)!;
      if (mesh.count >= CAPACITY) return;
      // The hand's place and turn; the thing's own size, at the body's scale.
      hand.decompose(position, turn, size);
      heldScale = Math.max(heldScale, scale);
      const at = UPRIGHT[kind];
      if (at) {
        // Where the fist is, standing straight, turned to the thumb.
        position.add(grip.set(at[0], at[1], at[2]).multiplyScalar(scale).applyQuaternion(turn));
        thumb.set(0, 1, 0).applyQuaternion(turn);
        upright.setFromAxisAngle(UP, Math.atan2(-thumb.z, thumb.x));
        out.compose(position, upright, size.set(scale, scale, scale));
      } else out.compose(position, turn, size.set(scale, scale, scale));
      put(mesh, out);
    },

// src/render/people/heldProps.ts:162-167 method createHeldProps.placeMatrix: the feature's own
    placeMatrix(kind, matrix) {
      const mesh = meshes.get(kind)!;
      if (mesh.count >= CAPACITY) return;
      put(mesh, matrix);
      heldScale = Math.max(heldScale, matrix.getMaxScaleOnAxis());
    },
