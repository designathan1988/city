// The code of src/render/buildings/flagLayer.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/flagLayer.ts:163-168 method createFlagLayer.dispose: the feature's own
    dispose() {
      uncast?.();
      if (mesh) { mesh.geometry.dispose(); mesh.dispose(); }
      geometry.dispose();
      mat.dispose();
    },
