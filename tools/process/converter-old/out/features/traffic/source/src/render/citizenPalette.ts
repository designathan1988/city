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

// src/render/citizenPalette.ts:41-55 function blendPackedFrames: the feature's own
export function blendPackedFrames(target: Float32Array, at: number, source: Float32Array, from: number,
  frameWidth: number, bones: number, a: number, b: number): void {
  for (let bone = 0; bone < bones; bone++) {
    const s = from + bone * PACKED_BONE_FLOATS;
    const d = at + bone * SKIN_BONE_FLOATS;
    for (let column = 0; column < 4; column++) {
      const input = s + column * 3;
      const output = d + column * 4;
      for (let row = 0; row < 3; row++) {
        target[output + row] = target[output + row]! + a * source[input + row]! + b * source[input + frameWidth + row]!;
      }
    }
    target[d + 15] = target[d + 15]! + a + b;
  }
}
