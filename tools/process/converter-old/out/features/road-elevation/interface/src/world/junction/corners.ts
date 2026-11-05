// What the feature uses of src/world/junction/corners.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/corners.ts:32-39 function cornerOf
/** A corner kept compact (`PackedCorner`) from its fields. */
export function cornerOf(c: Corner): Corner;

// src/world/junction/corners.ts:45-53 function packCorners
/** Corners as one run of numbers (`packLegs`): each its legs, mode, whether it has a point, its run's length, then its run (`PackedCorner`). */
export function packCorners(corners: readonly Corner[]): number[];

// src/world/junction/corners.ts:56-64 function unpackCorners
/** The corners of `packCorners`. */
export function unpackCorners(nums: readonly number[]): Corner[];

// src/world/junction/corners.ts:72-86 function computeCorners
/** Resolves every corner of a junction. */
export function computeCorners(
  legs: readonly Leg[],
  radiusScale: readonly number[] = [],
  mouthTrims: readonly number[] = [],
): Corner[];
