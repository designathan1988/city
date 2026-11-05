// What the feature uses of src/peopleLab/presets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/presets.ts:8-11 interface PartPreset
/** READY-MADE CHOICES for each part of a person, as The Sims' Create-a-Sim offers them: a tap sets the part, the sliders and the click-and-drag refine it */

/** A preset of one part: its regional sliders (-1..1, `Morpher` names), every other slider of the part back at rest. */
export interface PartPreset {
  readonly label: string;
  readonly features: Readonly<Record<string, number>>;
}

// src/peopleLab/presets.ts:13-19 interface Part
export interface Part {
  readonly id: string;
  readonly label: string;
  /** The sliders a preset of this part owns: set to the preset's value or to rest. */
  readonly sliders: readonly string[];
  readonly presets: readonly PartPreset[];
}

// src/peopleLab/presets.ts:125-125 interface FacePreset
/** Whole faces: a head shape and a choice for every part. */
export interface FacePreset { readonly label: string; readonly head: string; readonly parts: Readonly<Record<string, string>> }

// src/peopleLab/presets.ts:127-136 variable FACES
export const FACES: readonly FacePreset[] = [
  { label: 'Fofa', head: 'head-round', parts: { eyes: 'Grandes', brows: 'Arqueadas', nose: 'Arrebitado', mouth: 'Coração', jaw: 'Suave', cheeks: 'Bochechudas', ears: 'Pequenas' } },
  { label: 'Simpática', head: 'head-oval', parts: { eyes: 'Redondos', brows: 'Naturais', nose: 'Pequeno', mouth: 'Sorridente', jaw: 'Suave', cheeks: 'Maçãs altas', ears: 'Normais' } },
  { label: 'Elegante', head: 'head-oval', parts: { eyes: 'Amendoados', brows: 'Arqueadas', nose: 'Fino', mouth: 'Carnuda', jaw: 'Fino', cheeks: 'Maçãs altas', ears: 'Pequenas' } },
  { label: 'Marcante', head: 'head-square', parts: { eyes: 'Amendoados', brows: 'Baixas', nose: 'Romano', mouth: 'Larga', jaw: 'Marcante', cheeks: 'Naturais', ears: 'Normais' } },
  { label: 'Durão', head: 'head-rectangular', parts: { eyes: 'Pequenos', brows: 'Bravas', nose: 'Largo', mouth: 'Séria', jaw: 'Quadrado', cheeks: 'Fundas', ears: 'Normais' } },
  { label: 'Sonhadora', head: 'head-invertedtriangular', parts: { eyes: 'Caídos', brows: 'Tristes', nose: 'Pequeno', mouth: 'Biquinho', jaw: 'Pontudo', cheeks: 'Naturais', ears: 'Pequenas' } },
  { label: 'Divertido', head: 'head-round', parts: { eyes: 'Afastados', brows: 'Altas', nose: 'Batatinha', mouth: 'Larga', jaw: 'Com furinho', cheeks: 'Bochechudas', ears: 'De abano' } },
  { label: 'Misterioso', head: 'head-diamond', parts: { eyes: 'Puxados', brows: 'Baixas', nose: 'Aquilino', mouth: 'Fina', jaw: 'Fino', cheeks: 'Fundas', ears: 'Élficas' } },
];

// src/peopleLab/presets.ts:139-143 function withPart
/** A part's preset applied: the part's sliders all at rest, then the preset's. */
export function withPart(features: Readonly<Record<string, number>>, part: Part, preset: PartPreset): Record<string, number>;

// src/peopleLab/presets.ts:145-154 function withFace
export function withFace(features: Readonly<Record<string, number>>, face: FacePreset): Record<string, number>;

// src/peopleLab/presets.ts:157-162 function currentPreset
/** The part whose sliders the person's face matches now, by preset label, or null. */
export function currentPreset(features: Readonly<Record<string, number>>, part: Part): string | null;

// src/peopleLab/presets.ts:165-165 interface BodyPreset
/** Bodies: the macro sliders and a few regional ones. */
export interface BodyPreset { readonly label: string; readonly body: Partial<MacroParams>; readonly features: Readonly<Record<string, number>> }

// src/peopleLab/presets.ts:170-181 variable BODIES
export const BODIES: readonly BodyPreset[] = [
  { label: 'Magro', body: { weight: 0.2, muscle: 0.35 }, features: {} },
  { label: 'Esbelto', body: { weight: 0.35, muscle: 0.45, height: 0.6 }, features: { 'measure-waist-circ-decr-incr': -0.3 } },
  { label: 'Médio', body: { weight: 0.5, muscle: 0.5, height: 0.5 }, features: {} },
  { label: 'Atlético', body: { weight: 0.45, muscle: 0.8 }, features: { 'torso-vshape-decr-incr': 0.4, 'measure-waist-circ-decr-incr': -0.3 } },
  { label: 'Musculoso', body: { weight: 0.6, muscle: 1 }, features: { 'torso-vshape-decr-incr': 0.7, 'measure-shoulder-dist-decr-incr': 0.4 } },
  { label: 'Curvilíneo', body: { weight: 0.55, muscle: 0.45 }, features: { 'measure-hips-circ-decr-incr': 0.55, 'measure-waist-circ-decr-incr': -0.45, 'measure-bust-circ-decr-incr': 0.35, 'buttocks-volume-decr-incr': 0.5 } },
  { label: 'Gordinho', body: { weight: 0.82, muscle: 0.4 }, features: { 'stomach-pregnant-decr-incr': 0.3 } },
  { label: 'Fortão', body: { weight: 0.8, muscle: 0.8 }, features: { 'measure-shoulder-dist-decr-incr': 0.3 } },
  { label: 'Baixinho', body: { height: 0.12 }, features: {} },
  { label: 'Alto', body: { height: 0.88 }, features: {} },
];

// src/peopleLab/presets.ts:183-188 function withBody
export function withBody(body: MacroParams, features: Readonly<Record<string, number>>, preset: BodyPreset): { body: MacroParams; features: Record<string, number> };

// src/peopleLab/presets.ts:191-191 interface LookPreset
/** Whole outfits, cut for either sex. */
export interface LookPreset { readonly label: string; readonly female: Partial<PersonLook>; readonly male: Partial<PersonLook> }

// src/peopleLab/presets.ts:193-207 variable LOOKS
export const LOOKS: readonly LookPreset[] = [
  { label: 'Casual', female: { outfit: 'female_casualsuit02', outfitTint: 0x6a8fbf, footwear: 'shoes01', extras: [] },
    male: { outfit: 'male_casualsuit02', outfitTint: null, footwear: 'shoes01', extras: [] } },
  { label: 'Social', female: { outfit: 'female_elegantsuit01', outfitTint: 0x22252b, footwear: 'elvs_flatshoe_pointy1', extras: [] },
    male: { outfit: 'male_elegantsuit01', outfitTint: null, footwear: 'shoes02', extras: [] } },
  // Not female_sportsuit01: its texture's holes show white specks at the ankles.
  { label: 'Esporte', female: { outfit: 'female_casualsuit02', outfitTint: 0xe05a8a, footwear: 'shoes01', extras: [] },
    male: { outfit: 'male_casualsuit06', outfitTint: 0x2f9d8f, footwear: 'shoes01', extras: [] } },
  { label: 'Trabalho', female: { outfit: 'female_casualsuit01', outfitTint: 0x3b4a6b, footwear: 'shoes02', extras: [] },
    male: { outfit: 'male_worksuit01', outfitTint: null, footwear: 'shoes02', extras: [] } },
  { label: 'Colorido', female: { outfit: 'female_casualsuit01', outfitTint: 0xffd166, footwear: 'shoes01', extras: [] },
    male: { outfit: 'male_casualsuit03', outfitTint: 0xe8b820, footwear: 'shoes01', extras: [] } },
  { label: 'Festa', female: { outfit: 'female_elegantsuit01', outfitTint: 0xb03a2e, footwear: 'elvs_flatshoe_gemmed1', extras: [] },
    male: { outfit: 'male_elegantsuit01', outfitTint: 0x3b4a6b, footwear: 'shoes02', extras: [] } },
];

// src/peopleLab/presets.ts:224-228 function variedFace
/** A base face varied a little per person (each slider up to 0.12 either way): no two alike, none bent out of shape. */
export function variedFace(sex: 'female' | 'male', r: () => number): Record<string, number>;
