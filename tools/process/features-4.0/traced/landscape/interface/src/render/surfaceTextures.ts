// What the feature uses of src/render/surfaceTextures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/surfaceTextures.ts:77-81 function surfaceTexture
/** A baked texture by name (`bake/catalogue.ts`); the shaders need it, so a missing one is an error. */
export function surfaceTexture(name: string, anisotropy: number): Texture;
