// The code of src/render/citizenPalette.ts that is the feature's (the rest of the file is shared).

// src/render/citizenPalette.ts:25-38 function copyPackedFrame: the feature's own
export function copyPackedFrame(target: Float32Array, at: number, source: Float32Array, from: number, bones: number): void {
  for (let bone = 0; bone < bones; bone++) {
    const s = from + bone * PACKED_BONE_FLOATS;
    const d = at + bone * SKIN_BONE_FLOATS;
    for (let column = 0; column < 4; column++) {
      const input = s + column * 3;
      const output = d + column * 4;
      target[output] = source[input]!;
      target[output + 1] = source[input + 1]!;
      target[output + 2] = source[input + 2]!;
      target[output + 3] = column === 3 ? 1 : 0;
    }
  }
}
