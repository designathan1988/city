// What the feature uses of src/world/roads/markingStyle.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/markingStyle.ts:4-4 type MarkingStyleId
/** THE REGIONAL STANDARD OF ROAD PAINT, chosen for the map: - 'br', Brasil/CONTRAN */
export type MarkingStyleId = 'classic' | 'br' | 'us' | 'eu';

// src/world/roads/markingStyle.ts:6-6 variable YELLOW
const YELLOW = '#e1c45a';

// src/world/roads/markingStyle.ts:7-7 variable WHITE
const WHITE = '#eee8d7';

// src/world/roads/markingStyle.ts:9-9 variable STYLE_PAINT
/** The centre line colours the regional standards paint (`render/markings.ts` `PAINT_COLOURS`). */
export const STYLE_PAINT: readonly string[] = Object.freeze([YELLOW, WHITE]);

// src/world/roads/markingStyle.ts:71-74 variable YIELD_SYMBOL
/** The give-way symbol on the floor (MBST vol. IV, "SIP" / Vienna "shark's tooth") */
export const YIELD_SYMBOL: { strokes: [number, number][][]; width: number } = {
  strokes: [[[0.5, 0], [1, 1], [0, 1], [0.5, 0]], [[0.5, 0.3], [0.78, 0.9], [0.22, 0.9], [0.5, 0.3]]],
  width: 1,
};

// src/world/roads/markingStyle.ts:77-86 function wordStrokes
/** A word's strokes in the unit box per letter, letters side by side with a gap of `gap` letter widths. */
export function wordStrokes(text: string, gap = 0.35): { strokes: [number, number][][]; width: number };
