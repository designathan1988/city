// What the feature uses of src/render/canyonLight.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/canyonLight.ts:24-24 variable WINDOW
/** DO NOT lay the window again at every building */

/** The field's side, world units, and its cell. */
const WINDOW = m(2048);

// src/render/canyonLight.ts:25-25 variable CELL
const CELL = m(4);

// src/render/canyonLight.ts:26-26 variable SIDE
const SIDE = Math.round(WINDOW / CELL);

// src/render/canyonLight.ts:28-28 variable STEP
/** Heights stored in 16 bits: (h + OFFSET) / STEP. */
const STEP = m(0.05);

// src/render/canyonLight.ts:29-29 variable OFFSET
const OFFSET = m(100);

// src/render/canyonLight.ts:33-33 variable MASK
/** The field is a torus: world cell (i, j) is texel (i & MASK, j & MASK), so a window moved keeps every cell it still holds. */
const MASK = SIDE - 1;

// src/render/canyonLight.ts:37-46 interface CanyonLight
export interface CanyonLight {
  readonly pass: ShaderPass;
  /** @rebuild The view's centre (map x, y), the town's solids, and where they changed since the last call */
  follow(cx: number, cy: number, solids: (() => CameraSolids) | null, changed: readonly (readonly [number, number, number, number])[] | null): void;
  /** The ambient occlusion multiplied into the picture first (GTAO's denoised target, its blend intensity), or none. */
  setOcclusion(texture: Texture | null, intensity: number): void;
  /** The sun's direction (world, to the sun) and its share of the light on the ground. */
  setSun(direction: Vector3, directShare: number): void;
  dispose(): void;
}
