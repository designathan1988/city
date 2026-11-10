// What the feature uses of src/render/surfaceTextures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/surfaceTextures.ts:5-9 interface SurfaceBake
/** The surfaces baked at build time (`bake/catalogue.ts`), by name, as `surfaceBakeClient.ts` read them: every material takes its maps from here */

export interface SurfaceBake {
  readonly map: Texture;
  readonly normalMap: Texture;
  readonly roughnessMap: Texture;
}

// src/render/surfaceTextures.ts:66-74 function surfaceTextureOrNone
/** A baked texture by name, its anisotropy at least `anisotropy`; undefined where none was read (a worker, a test). */
function surfaceTextureOrNone(name: string, anisotropy: number): Texture | undefined;
