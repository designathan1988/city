// What the feature uses of src/peopleLab/clothStyle.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/clothStyle.ts:9-9 variable CLOTH_PATTERNS
/** THE OUTFIT RECOLOURED AND PATTERNED, as The Sims 3's Create-a-Style did it on any garment: a main colour and an accent, a woven pattern
 *  (stripes, gingham, dots, denim twill, rib) in the cloth's bind-space position (continuous across seams, fixed to the cloth when the body
 *  moves; triplanar on the bind normal), box-filtered so it never shimmers (Quilez's filtered checker, the filtered pulse train of GPU Gems
 *  ch. 25), the garment texture's own shading kept (its luminance times the colour, Photoshop's Color mode). All uniforms: changing any of
 *  it never rebuilds the person nor recompiles a shader. Drawn on the outfit only (garment slot 1). */

export const CLOTH_PATTERNS = ['plain', 'stripes', 'gingham', 'dots', 'denim', 'rib'] as const;

// src/peopleLab/clothStyle.ts:10-10 type ClothPattern
export type ClothPattern = (typeof CLOTH_PATTERNS)[number];

// src/peopleLab/clothStyle.ts:13-25 interface OutfitStyle
/** The outfit's look as kept with the person. */
export interface OutfitStyle {
  readonly pattern: ClothPattern;
  /** The pattern's colour (the main colour is `spec.look.outfitTint`). */
  readonly accent: number;
  /** Repeats a metre. */
  readonly scale: number;
  /** How much of the pattern shows, 0..1. */
  readonly amount: number;
  /** How much of the texture's own weave and print shows under the colour, 0 (flat) .. 1. */
  readonly detail: number;
  /** 0 matte cloth .. 1 satin. */
  readonly shine: number;
}

// src/peopleLab/clothStyle.ts:27-27 variable PLAIN_STYLE
export const PLAIN_STYLE: OutfitStyle = { pattern: 'plain', accent: 0xffffff, scale: 12, amount: 1, detail: 1, shine: 0 };

// src/peopleLab/clothStyle.ts:29-38 interface ClothUniforms
export interface ClothUniforms {
  readonly clothDyed: { value: number };
  readonly clothMain: { value: Color };
  readonly clothAccent: { value: Color };
  readonly clothPattern: { value: number };
  readonly clothScale: { value: number };
  readonly clothAmount: { value: number };
  readonly clothDetail: { value: number };
  readonly clothShine: { value: number };
}

// src/peopleLab/clothStyle.ts:46-56 function setCloth
/** The uniforms set to a person's outfit colour (null: the garment's own colours) and style. */
export function setCloth(u: ClothUniforms, tint: number | null | undefined, style: OutfitStyle | undefined): void;
