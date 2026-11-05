// What the feature uses of src/render/mesh/detailLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/detailLayer.ts:9-14 interface DetailTextures
/** The close-up detail layer: what a surface looks like at the camera's nearest zoom. ## Why the ground went to mush up */

interface DetailTextures {
  readonly map: Texture;
  readonly normalMap: Texture;
  /** World units one tile covers. */
  readonly worldSize: number;
}

// src/render/mesh/detailLayer.ts:17-23 function detailTextures
/** The detail textures for one family, baked at build time (`bake/detailRecipes.ts`) and loaded once (`surfaceBakeClient.ts`). */
function detailTextures(kind: DetailKind, anisotropy: number): DetailTextures;

// src/render/mesh/detailLayer.ts:56-66 interface DetailSpec
export interface DetailSpec {
  readonly kind: DetailKind;
  /** World units one tile of the MACRO map covers - how the mesh UVs are scaled. */
  readonly macroTile: number;
  /** How hard the detail albedo modulates, 0..1. */
  readonly albedo: number;
  /** Strength of the detail normal. */
  readonly normal: number;
  /** Mip bias applied to the macro maps at full detail, to soften magnified grain. */
  readonly macroBlur: number;
}

// src/render/mesh/detailLayer.ts:69-122 function applyDetail
/** Installs the detail layer on a standard material whose UVs are in macro tiles */
export function applyDetail(material: MeshStandardMaterial, spec: DetailSpec, anisotropy: number): void;
