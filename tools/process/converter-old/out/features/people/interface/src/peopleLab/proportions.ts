// What the feature uses of src/peopleLab/proportions.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/proportions.ts:11-11 type Influence
/** THE FACE'S PROPORTIONS, measured and set to the classical canons (Da Vinci and Dürer's neoclassical canons; the drawing manuals' rules):
 *  eyes halfway down the head, the face in equal thirds (brow, nose base, chin), one eye's width between the eyes, the nose as wide as that gap,
 *  the mouth half as wide again as the nose and a third of the way from the nose to the chin. Each feature is found by the very target that
 *  shapes it (the vertices the slider moves), never by guessed heights; each slider is solved by the secant method on the measure it controls. */

/** The base mesh's vertices a slider moves, weighted 0..1 by how far (its target at +1 against the face at rest). */
export type Influence = Float32Array;

// src/peopleLab/proportions.ts:13-30 interface FaceMeasure
export interface FaceMeasure {
  /** Down from the skull's top to the eyes, over the skull's top to the chin: 0.5 in the canon. */
  readonly eyeLine: number;
  /** Nose base to chin over brow to nose base: 1 in the canon. */
  readonly thirds: number;
  /** The gap between the eyes over one eye's width: 1. */
  readonly eyeGap: number;
  /** The nose's width over the gap between the eyes: 1. */
  readonly nose: number;
  /** The mouth's width over the nose's: 1.5. */
  readonly mouth: number;
  /** Nose base to mouth over nose base to chin: 1/3. */
  readonly mouthLine: number;
  /** One eye's width over the head's width at the eyes: the head is five eyes wide, 0.2. */
  readonly eyeSize: number;
  /** The jaw's width (the head's width a little below the mouth) over the cheekbones' (at the nose's base): bigonial over bizygomatic. */
  readonly jaw: number;
}

// src/peopleLab/proportions.ts:85-204 class FaceProportions
export class FaceProportions { … }

// src/peopleLab/proportions.ts:106-121 method FaceProportions.influenceOf
/** How much each base vertex a slider moves, 0..1: its target at +1 against the face at rest. */
  influenceOf(slider: string, spec: PersonSpec): Influence;

// src/peopleLab/proportions.ts:124-133 method FaceProportions.landmark
/** The landmark a slider marks: the vertex it moves most, on the left (x > 0) or across the middle. */
  private landmark(slider: string, spec: PersonSpec, side: 'left' | 'any'): number;

// src/peopleLab/proportions.ts:135-173 method FaceProportions.measure
measure(spec: PersonSpec): FaceMeasure;

// src/peopleLab/proportions.ts:209-209 interface BodyMeasure
/** The body's proportions, measured on the shaped mesh as drawn (the lab's style applied), against the anthropometric and figure-drawing canons:
 *  ~7.5 heads tall; a woman's shoulders ~1.15 times her hips, a man's ~1.35; the neck ~0.6 of the head's width in a woman, ~0.75 in a man
 *  (neck circumference over pi against head breadth: 293.5/154 mm in women, 350/158 in men). */
export interface BodyMeasure { readonly heads: number; readonly shoulders: number; readonly neck: number }

// src/peopleLab/proportions.ts:213-213 variable BODY_CANON
/** Heads tall as a stylized game figure is drawn (The Sims' adults ~6-6.5; a real adult's 7.5 left the head small and plain, 2026-10-21). */
/** The neck slimmer than the anthropometric mean (0.6, 0.75): a stylized figure's neck at the mean read thick, 'a horse's' (the player, 2026-10-21). */
export const BODY_CANON = { female: { heads: 6.3, shoulders: 1.15, neck: 0.48 }, male: { heads: 6.5, shoulders: 1.35, neck: 0.66 } } as const;

// src/peopleLab/proportions.ts:215-253 function measureBody
export function measureBody(assets: PeopleAssets, p: Float32Array): BodyMeasure;
