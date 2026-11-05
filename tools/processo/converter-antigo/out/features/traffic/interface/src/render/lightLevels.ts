// What the feature uses of src/render/lightLevels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/lightLevels.ts:7-7 function fromCandela
/** I in candela as luminance on the scale above. */
const fromCandela = (cd: number): number => …;

// src/render/lightLevels.ts:9-23 variable NIGHT_LUMINANCE
export const NIGHT_LUMINANCE = {
  /** A lit room seen through its glass, on average: brighter than a lit wall, but UNDER the bloom's threshold */
  window: 0.72,
  /** The lantern beside a front door. */
  porchLamp: 2.4,
  headlamp: 3.0,
  /** 12 cd, within R1's 4-17. */
  tailLamp: fromCandela(12),
  /** 120 cd, within S1's 60-260: ten times a tail lamp. */
  brakeLamp: fromCandela(120),
  /** 150 cd, within R6's 50-200. */
  indicator: fromCandela(150),
  /** A bus's lit destination blind. */
  destination: 1.4,
} as const;

// src/render/lightLevels.ts:28-31 function luminanceOf
/** The luminance of an sRGB hex colour, in the working (linear) space. */
export function luminanceOf(hex: number): number;
