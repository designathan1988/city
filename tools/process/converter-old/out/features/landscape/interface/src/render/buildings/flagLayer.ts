// What the feature uses of src/render/buildings/flagLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/flagLayer.ts:20-27 interface FlagInstance
/** Every flag in the city, one instanced draw: a cloth that waves in the wind. */

/** One flag, in three's frame (y up): the top of the hoist, and its size. */
export interface FlagInstance {
  readonly x: number;
  readonly y: number;
  readonly z: number;
  readonly width: number;
  readonly height: number;
  readonly design: FlagDesign;
}

// src/render/buildings/flagLayer.ts:56-101 function material
function material(): MeshStandardMaterial;
