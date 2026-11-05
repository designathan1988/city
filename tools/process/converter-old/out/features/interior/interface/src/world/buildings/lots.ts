// What the feature uses of src/world/buildings/lots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/lots.ts:7-15 interface LotSurface
/** The surface of a building's open lot - its car park, its yard - as it is laid */
export interface LotSurface {
  readonly volume: Volume;
  /** World corners of the lot. */
  readonly ring: readonly Vec2[];
  /** The surface as plain numbers (`lotHeightAt`): what the ground's shaping off the page reads of it. */
  readonly slope: LotSlope;
  /** Height of the surface at a world point (extrapolated outside the lot). */
  heightAt(x: number, y: number): number;
}

// src/world/buildings/lots.ts:18-25 type LotSlope
/** A lot's surface as plain numbers, copied to a worker as they are */
export type LotSlope =
  | { readonly kind: 'level'; readonly level: number }
  | { readonly kind: 'ramp'; readonly frame: BuildingFrame; readonly from: number; readonly level: number; readonly y0: number; readonly d: number }
  | {
    readonly kind: 'street'; readonly frame: BuildingFrame; readonly side: number;
    readonly x0: number; readonly y0: number; readonly w: number; readonly d: number;
    readonly ha: number; readonly hb: number; readonly floor: number;
  };

// src/world/buildings/lots.ts:47-47 type PavingAt
/** The pavement height at a world point, or NaN where there is none. */
export type PavingAt = (x: number, y: number) => number;

// src/world/buildings/lots.ts:54-87 function lotSurfaces
export function lotSurfaces(b: Building, floor: number, pavedAt?: PavingAt): LotSurface[];
