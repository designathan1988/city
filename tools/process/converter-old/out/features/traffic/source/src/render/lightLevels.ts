// The code of src/render/lightLevels.ts that is the feature's (the rest of the file is shared).

// src/render/lightLevels.ts:34-36 function nightGain: the feature's own
export function nightGain(hex: number, target: number): number {
  return Math.max(0, target / Math.max(1e-4, luminanceOf(hex)) - 1);
}
