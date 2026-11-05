// What the feature uses of src/render/platePaint.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/platePaint.ts:6-6 function sameList
/** THE DABS A PLATE OF THE GROUND LAYS (`flatAtlas.ts` takePaint): the document's that reach it, kept as laid when their tile leaves the page */

export const sameList = (a: readonly unknown[], b: readonly unknown[]): boolean => …;

// src/render/platePaint.ts:9-9 function dabKey
/** A dab's identity on a plate: a tile come back brings its dabs as new objects of the same paint (a dab a player repeats exactly, at the same place and strength, counts once too). */
const dabKey = (d: PaintDab): string => …;

// src/render/platePaint.ts:10-11 function dabInRects
const dabInRects = (d: PaintDab, rects: readonly ChangeRect[]): boolean => …;

// src/render/platePaint.ts:14-35 function plateDabs
/** THE DABS A PLATE LAYS brought up to the document's (`reaching`: those reaching the plate now). What was laid stays as laid: a tile that left the page took its dabs off the document, not off the world's ground (DO NOT lay every plate again at each tile let go: every shown plate's paint rastered and sent again, and the ecology read again, at each kilometre moved). The dabs new to the plate go after it, so the surface lays only those (`terrain.ts` paintStep); a stroke's rectangles (`edited`) say where a dab gone from the document is taken off. `edited` null: the diary could not say, the document's dabs alone, laid again. Null back: nothing to change. */
export function plateDabs(laid: readonly PaintDab[], keys: Set<string>, reaching: readonly PaintDab[], edited: readonly ChangeRect[] | null): PaintDab[] | null;
