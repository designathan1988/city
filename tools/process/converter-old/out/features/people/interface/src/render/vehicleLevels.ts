// What the feature uses of src/render/vehicleLevels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/vehicleLevels.ts:8-8 function attributeOf
/** An attribute from the numbers sent back. */
const attributeOf = (a: ArrayOf): BufferAttribute => …;

// src/render/vehicleLevels.ts:37-82 function partLevels
/** The levels of `source` (0 is `source` itself; null: nothing left to draw at that level): welded, simplified and creased in the levels' worker (`partLevelsWork.ts`), the page only putting the geometries together.
 *  `copies`: how many a vehicle draws (a car's four tyres), for what a level sheds a vehicle (`SHED`). */
export async function partLevels(source: BufferGeometry, weld: Weld, copies = 1): Promise<(BufferGeometry | null)[]>;
