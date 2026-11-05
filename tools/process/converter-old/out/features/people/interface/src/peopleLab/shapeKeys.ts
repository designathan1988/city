// What the feature uses of src/peopleLab/shapeKeys.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/shapeKeys.ts:7-18 function effective
/** WHAT A CHANGE TO A PERSON NEEDS: drawing on the person built (`reshape.ts`) when only the shape moved, a build when what is worn did. */

/** What is built and saved: a child wears the tailored shells (the garments are cut for grown bodies, as the game has it). */
export function effective(person: LabPerson): LabPerson;

// src/peopleLab/shapeKeys.ts:21-27 function buildKey
/** What a build depends on beyond the body's shape: what is worn (a child wears the tailored shells), the sex's motion capture. Equal keys reshape, unequal ones build. */
export function buildKey(p: LabPerson): string;

// src/peopleLab/shapeKeys.ts:30-30 function shapeOf
/** Everything the body's shape is made from. */
export const shapeOf = (p: LabPerson): string => …;

// src/peopleLab/shapeKeys.ts:33-33 interface Slider
/** One slider of the shape: its value in a person, and that person with it set. */
export interface Slider { readonly key: string; readonly range: readonly [number, number]; get(p: LabPerson): number; set(p: LabPerson, v: number): LabPerson }

// src/peopleLab/shapeKeys.ts:35-36 function bodySlider
export const bodySlider = (k: keyof MacroParams): Slider => …;

// src/peopleLab/shapeKeys.ts:37-38 function featureSlider
export const featureSlider = (k: string): Slider => …;
