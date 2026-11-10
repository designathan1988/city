// What the feature uses of src/render/mesh/hiddenFaces.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/hiddenFaces.ts:22-95 function dropHiddenFaces
/** The faces of the parts no one can see taken out: one lying on the floor facing down (`floor`: the model stands on y = 0), or one whose every point, `COVER_PROBE` out of it, is inside
 *  another part's box (a book's bottom on its shelf, a tread inside the next, a leg's ends in the floor and in the top). A face only partly
 *  covered is kept whole. DO NOT keep them: they are drawn and cast for every instance of the model. */
export function dropHiddenFaces(parts: readonly BufferGeometry[], floor = true): void;
