// The code of src/render/exhaust.ts that is the feature's (the rest of the file is shared).

// src/render/exhaust.ts:33-239 slice of src/render/exhaust.ts#createExhaust (shared): the feature's statements; "…" marks code left out
export function createExhaust(): Exhaust {
  // … 15 lines
  const material = new ShaderMaterial({
    uniforms: {
      // … 1 lines
      uWindDir: windUniforms.uWindDir,
    // … 1 lines
    },
  // … 115 lines
  });
// … 68 lines
}
