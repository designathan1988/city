// What the feature uses of src/render/changeBoxes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/changeBoxes.ts:19-19 type ChangeBox
/** A box of what changed and the widest cascade texel the change shows in, world units (`reach`): a caster drawn into a cascade only where
 *  it spans enough of its texels (`shadowCasters.ts` castBySize) changes nothing in a cascade of coarser texels (`sunCascades.ts` rectsOf).
 *  None: every cascade. */
export type ChangeBox = Box3 & { reach?: number };
