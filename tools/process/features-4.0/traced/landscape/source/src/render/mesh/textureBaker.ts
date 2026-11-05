// The code of src/render/mesh/textureBaker.ts that is the feature's (the rest of the file is shared).

// src/render/mesh/textureBaker.ts:55-76 function normalTexels: the feature's own
export function normalTexels(height: Float32Array, size: number, strength: number, out: Uint8Array | Uint8ClampedArray): void {
  for (let y = 0; y < size; y++) {
    const row = y * size;
    const above = (y === 0 ? size - 1 : y - 1) * size;
    const below = (y + 1 === size ? 0 : y + 1) * size;
    let left = size - 1;
    for (let x = 0; x < size; x++) {
      const right = x + 1 === size ? 0 : x + 1;
      const dx = ((height[row + right] as number) - (height[row + left] as number)) * strength;
      const dy = ((height[below + x] as number) - (height[above + x] as number)) * strength;
      left = x;
      // Recipe heights are bounded near 0..1, so direct length cannot overflow
      const length = Math.sqrt(dx * dx + dy * dy + 1);
      const index = (y * size + x) * 4;
      // Every channel in [-1, 1] as [0, 1]: three decodes all three with `* 2.0 - 1.0`
      out[index] = Math.round(((-dx / length) * 0.5 + 0.5) * 255);
      out[index + 1] = Math.round(((-dy / length) * 0.5 + 0.5) * 255);
      out[index + 2] = Math.round(((1 / length) * 0.5 + 0.5) * 255);
      out[index + 3] = 255;
    }
  }
}
