// What the feature uses of src/render/people/cartoon.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/cartoon.ts:5-14 interface CartoonStyle
/** PEOPLE IN AN ANIMATED FILM'S STYLE: a MakeHuman body reproportioned the way Pixar's people are drawn, as parameters. */

export interface CartoonStyle {
  /** Head size, 1 as the body has it. */
  readonly head: number;
  /** The eyes and their lids, 1 as they are. */
  readonly eyes: number;
  /** Leg length, thigh and shin, 1 as they are. */
  readonly legs: number;
  readonly hands: number;
  readonly feet: number;
}

// src/render/people/cartoon.ts:17-17 variable REALISTIC
/** No change. */
export const REALISTIC: CartoonStyle = { head: 1, eyes: 1, legs: 1, hands: 1, feet: 1 };

// src/render/people/cartoon.ts:20-20 variable PIXAR
/** An adult as Pixar draws one: head near a fifth of the height, eyes large, legs a little short. */
export const PIXAR: CartoonStyle = { head: 1.42, eyes: 1.45, legs: 0.84, hands: 1.15, feet: 1.12 };
