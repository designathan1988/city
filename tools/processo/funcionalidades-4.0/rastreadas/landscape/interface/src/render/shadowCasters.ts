// What the feature uses of src/render/shadowCasters.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/shadowCasters.ts:36-46 function castCastersInto
/** Each caster's `castShadow` for a cascade of `texel` world units; null: back to what its maker set, between the shadow passes (what the programs compiled ahead look at, `uploads.ts` shadowStandIns). */
export function castCastersInto(texel: number | null): void;
