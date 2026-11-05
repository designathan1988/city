// What the feature uses of src/world/buildings/materials.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/materials.ts:6-6 variable FINISHES
/** What a building is made of. */

export const FINISHES = ['plaster', 'stucco', 'ceramic', 'brick', 'stone', 'concrete', 'wood', 'metal', 'glass', 'tile', 'slate', 'panel', 'roofing'] as const;

// src/world/buildings/materials.ts:7-7 type Finish
export type Finish = (typeof FINISHES)[number];

// src/world/buildings/materials.ts:9-13 interface MaterialSpec
export interface MaterialSpec {
  readonly finish: Finish;
  /** 0xRRGGBB, in sRGB. */
  readonly colour: number;
}

// src/world/buildings/materials.ts:16-16 variable MATERIAL_SLOTS
/** Surfaces a building-wide material can be set for. */
const MATERIAL_SLOTS = ['wall', 'trim', 'roof', 'plinth'] as const;

// src/world/buildings/materials.ts:17-17 type MaterialSlot
type MaterialSlot = (typeof MATERIAL_SLOTS)[number];

// src/world/buildings/materials.ts:19-19 type BuildingMaterials
export type BuildingMaterials = Partial<Record<MaterialSlot, MaterialSpec>>;

// src/world/buildings/materials.ts:21-27 interface VolumeMaterials
export interface VolumeMaterials {
  /** Every wall of the volume. */
  wall?: MaterialSpec;
  /** One side's walls, over `wall`. */
  sides?: Partial<Record<FaceId, MaterialSpec>>;
  roof?: MaterialSpec;
}
