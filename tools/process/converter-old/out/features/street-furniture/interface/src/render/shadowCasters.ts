// What the feature uses of src/render/shadowCasters.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/shadowCasters.ts:6-16 interface SizedCaster
/** CASTERS BY SIZE AND BY CASCADE: a mesh drawn into a sun cascade only when it holds instances and its widest thing reaches a texel there (`sunCascades.ts`). */

/** A mesh that casts as its maker says (`casts`), into the cascades where `radius` (world units, its widest thing drawn) is at least a texel: under that, its shadow is a smudge PCF spreads away, as a mover's (`sunCascades.ts` drawnIn). */
export interface SizedCaster {
  readonly mesh: Mesh;
  readonly casts: boolean;
  radius(): number;
  /** Texels its radius must span to cast (1 when none): a small thing's shadow narrower than that is a smudge PCF spreads away. */
  readonly texels?: number;
  /** Its instances drawn in the colour pass and into the cascades, when they differ (`cellInstances.ts`: what stands off the frame under a cascade casts and is not drawn): `count` set to each for its passes. */
  readonly counts?: { colour(): number; shadow(): number };
  /** Its shape in a cascade of `texel` and in the colour pass, when the camera chooses the colour's (`poleLayer.ts`): the cascade's by its texel alone, so the shadow does not follow the camera's level. */
  readonly levels?: { shadow(texel: number): BufferGeometry; colour(): BufferGeometry };
}

// src/render/shadowCasters.ts:22-22 variable casterTexel
/** The texel of the cascade being drawn (world units), 0 between the passes: a uniform for a depth material whose geometry merges parts of
 *  different sizes, each leaving the cascade where it would have as a mesh of its own (`signals.ts` frame). */
export const casterTexel = { value: 0 };

// src/render/shadowCasters.ts:26-33 function castBySize
/** `caster` chosen at each cascade from now on; the function returned takes it off (its mesh disposed). Its mesh knows it (`userData.sizedCaster`):
 *  the casters' signature reads what it casts, not what the colour pass draws (`castShadowKey`). */
export function castBySize(caster: SizedCaster): () => void;
