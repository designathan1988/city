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

// src/world/buildings/materials.ts:30-44 variable FINISH_COLOUR
/** Each finish's own colour, as its swatch shows it: brick is brick-red before it is painted. */
export const FINISH_COLOUR: Readonly<Record<Finish, number>> = {
  plaster: 0xe6e0d4,
  stucco: 0xdcd4c4,
  ceramic: 0xcfd6da,
  brick: 0xa4563f,
  stone: 0xb7b2a6,
  concrete: 0xb4b4b0,
  wood: 0x9c6b43,
  metal: 0x9aa3a8,
  glass: 0x9db6c2,
  tile: 0xbd6a52,
  slate: 0x585b5f,
  panel: 0x8f9ba5,
  roofing: 0x7b4a37,
};

// src/world/buildings/materials.ts:55-55 function spec
const spec = (finish: Finish, colour: number): MaterialSpec => …;

// src/world/buildings/materials.ts:260-281 variable STYLES
/** Factory styles: a coherent set of materials for a whole building, applied in one click and freely changed afterwards */
export const STYLES: readonly { readonly key: string; readonly materials: Required<BuildingMaterials> }[] = [
  {
    key: 'brick',
    materials: { wall: spec('brick', 0x9a5540), trim: spec('stone', 0xe3dccd), roof: spec('tile', 0x5a4a44), plinth: spec('stone', 0x6f6a62) },
  },
  {
    key: 'modern',
    materials: { wall: spec('plaster', 0xf1efe9), trim: spec('concrete', 0xb9bcbb), roof: spec('metal', 0x4d5256), plinth: spec('concrete', 0x8a8a84) },
  },
  {
    key: 'mediterranean',
    materials: { wall: spec('plaster', 0xe8cfa0), trim: spec('plaster', 0xf6efe2), roof: spec('tile', 0xa65a3c), plinth: spec('stone', 0x8f8069) },
  },
  {
    key: 'industrial',
    materials: { wall: spec('metal', 0x7f8a86), trim: spec('concrete', 0xc7c6be), roof: spec('metal', 0x5d6462), plinth: spec('concrete', 0x77766f) },
  },
  {
    key: 'timber',
    materials: { wall: spec('wood', 0x8a6a4a), trim: spec('plaster', 0xeee8dc), roof: spec('metal', 0x3f4447), plinth: spec('stone', 0x6d665c) },
  },
];
