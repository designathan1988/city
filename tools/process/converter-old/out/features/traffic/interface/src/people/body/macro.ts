// What the feature uses of src/people/body/macro.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/body/macro.ts:4-20 interface MacroParams
/** MakeHuman's macro sliders, as weights on its macro targets. */

/** The macro sliders, each 0..1 as MakeHuman has them; the three ethnic shares are normalised. */
export interface MacroParams {
  /** 0 female, 1 male. */
  readonly gender: number;
  /** 0 a baby, 0.1875 eleven, 0.5 twenty-five, 1 ninety (`ageFromYears`). */
  readonly age: number;
  readonly muscle: number;
  readonly weight: number;
  /** 0.5 is the average for the sex and age. */
  readonly height: number;
  /** 0.5 neutral; above it towards ideal, below towards uncommon. */
  readonly proportions: number;
  readonly african: number;
  readonly asian: number;
  readonly caucasian: number;
  readonly cupsize: number;
  readonly firmness: number;
}

// src/people/body/macro.ts:27-32 interface MacroPart
interface MacroPart {
  readonly lowest: number;
  readonly highest: number;
  readonly low: string;
  readonly high: string;
}

// src/people/body/macro.ts:35-38 interface MacroDefinition
/** `modifiers.json` `.macro`. */
export interface MacroDefinition {
  readonly macrotargets: Readonly<Record<string, { readonly label: string; readonly parts: readonly MacroPart[] }>>;
  readonly combinations: Readonly<Record<string, readonly string[]>>;
}

// src/people/body/macro.ts:60-67 function ageFromYears
export function ageFromYears(years: number): number;

// src/people/body/macro.ts:69-76 function yearsFromAge
export function yearsFromAge(age: number): number;

// src/people/body/macro.ts:79-114 function macroTargetWeights
/** The weight of every macro target in `names` */
export function macroTargetWeights(def: MacroDefinition, names: readonly string[], params: MacroParams): Map<string, number>;
