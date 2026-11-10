// What the feature uses of src/render/citizenPalette.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenPalette.ts:2-2 variable PACKED_BONE_FLOATS
/** A skin matrix is affine: its fourth row is always (0, 0, 0, 1). */
export const PACKED_BONE_FLOATS = 12;

// src/render/citizenPalette.ts:3-3 variable SKIN_BONE_FLOATS
export const SKIN_BONE_FLOATS = 16;

// src/render/citizenPalette.ts:41-55 function blendPackedFrames
/** Add one clip's two neighbouring frames to a blended GPU palette. */
export function blendPackedFrames(target: Float32Array, at: number, source: Float32Array, from: number,
  frameWidth: number, bones: number, a: number, b: number): void;
