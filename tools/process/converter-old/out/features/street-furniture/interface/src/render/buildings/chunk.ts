// What the feature uses of src/render/buildings/chunk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/chunk.ts:8-30 type PartKind
/** A part instanced per kind for the whole near ring (`kit.ts` PART_KINDS): declared with the cell's data, which the kit's materials do not need. */
export type PartKind =
  | 'glass'
  | 'frame'
  | 'concrete'
  | 'door'
  | 'shutter'
  | 'railing'
  | 'roofRailing'
  | 'awning'
  | 'column'
  | 'glassDark'
  | 'water'
  | 'lamp'
  | 'wash'
  | 'fin'
  | 'louvre'
  | 'glassRail'
  | 'brise'
  | 'lotBox'
  | 'lotRail'
  | 'lotSlab'
  | 'lotSides'
  | FacadePropKind;
