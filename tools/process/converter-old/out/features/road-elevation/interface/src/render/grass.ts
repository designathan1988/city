// What the feature uses of src/render/grass.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/grass.ts:21-29 interface Tuft
export interface Tuft {
  readonly x: number;
  readonly y: number;
  readonly z: number;
  readonly yaw: number;
  readonly height: number;
  readonly width: number;
  readonly tint: Color;
}

// src/render/grass.ts:32-35 interface Clump
/** A clump's grass: its tufts and its wildflowers. */
export interface Clump {
  readonly tufts: readonly Tuft[];
  readonly flowers: readonly Tuft[];
}

// src/render/grass.ts:38-38 variable NONE
const NONE: Clump = { tufts: [], flowers: [] };

// src/render/grass.ts:41-97 function grassClump
/** The clump at a placed item, from its own generator (seeded by its id, so an edit elsewhere moves none of it) */
export function grassClump(
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  placed: { readonly x: number; readonly y: number; readonly id: number },
): Clump;
