// What the feature uses of src/render/buildings/facadeLevel.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/facadeLevel.ts:21-27 interface FacadePanes
/** The detail's materials whose look the far level's openings take (`kit.ts`): the panes, the doors, the shutters, the frames. */
export interface FacadePanes {
  readonly glass: MeshStandardMaterial;
  readonly glassDark: MeshStandardMaterial;
  readonly door: MeshStandardMaterial;
  readonly shutter: MeshStandardMaterial;
  readonly frame: MeshStandardMaterial;
}

// src/render/buildings/facadeLevel.ts:29-29 function vec3Of
const vec3Of = (c: MeshStandardMaterial['color']): string => …;

// src/render/buildings/facadeLevel.ts:33-67 function facadeFragmentPars
/** GLSL: the far level's window grid and its pick of each bay's pane, as `emitBay` picks it (`bayHash`) */
function facadeFragmentPars(p: FacadePanes): string;
