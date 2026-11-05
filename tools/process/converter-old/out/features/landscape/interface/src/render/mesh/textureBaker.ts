// What the feature uses of src/render/mesh/textureBaker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/textureBaker.ts:4-29 function makeNoise
/** Procedural texture recipes and the texels they bake: pure, run at build time (`scripts/vite/surface-bake-plugin.ts`), nothing baked during play. */

/** Deterministic 2D value noise with a positive integer period, so the result tiles. */
export function makeNoise(seed: number): (x: number, y: number, period: number) => number;

// src/render/mesh/textureBaker.ts:32-52 function fbm
/** Sums octaves of tiling value noise. Returns roughly 0..1. */
export function fbm(
  noise: (x: number, y: number, period: number) => number,
  x: number,
  y: number,
  basePeriod: number,
  octaves: number,
): number;

// src/render/mesh/textureBaker.ts:81-95 interface SurfaceRecipe
export interface SurfaceRecipe {
  readonly size: number;
  /** World units covered by one tile of the texture. */
  readonly worldSize: number;
  /** Fills colour (0..1 rgb), height (0..1) and roughness (0..1) for one texel, and metalness */
  readonly shade: (
    u: number,
    v: number,
    out: { r: number; g: number; b: number; h: number; rough: number; metal?: number },
  ) => void;
  /** Relief strength of the derived normal map. */
  readonly relief: number;
  /** A surface of metal and non-metal side by side (a curtain wall's aluminium mullions round its glass) */
  readonly metallic?: boolean;
}
