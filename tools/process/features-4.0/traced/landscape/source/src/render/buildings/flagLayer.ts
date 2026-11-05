// The code of src/render/buildings/flagLayer.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/flagLayer.ts:56-101 slice of src/render/buildings/flagLayer.ts#material (shared): the feature's statements; "…" marks code left out
function material(): MeshStandardMaterial {
  // … 2 lines
  m.onBeforeCompile = (shader) => {
    Object.assign(shader.uniforms, windUniforms);
  // … 37 lines
  };
// … 2 lines
}

// src/render/buildings/flagLayer.ts:117-162 slice of src/render/buildings/flagLayer.ts#createFlagLayer.set (shared): the feature's statements; "…" marks code left out
    set(flags) {
      // … 16 lines
      const dir = windUniforms.uWindDir.value;
      const yaw = Math.atan2(-dir.y, dir.x);
      const q = new Quaternion().setFromAxisAngle(new Vector3(0, 1, 0), yaw);
      // … 3 lines
      flags.forEach((f, i) => {
        mtx.compose(new Vector3(f.x, f.y, f.z), q, new Vector3(f.width, f.height, 1));
      // … 7 lines
      });
    // … 12 lines
    },
