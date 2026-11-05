// What the feature uses of src/render/roadProgramStandIns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadProgramStandIns.ts:24-58 function roadProgramStandIns
/** THE ROADS' PROGRAMS AMONG THE WORLD'S, never drawn: one invisible triangle per look of a road surface (`surfaceLook`), raised and not (a raised surface casts: its depth program too), so the opening's catalogue (`shaderCatalogue.ts`, it walks the scene, the hidden too) builds every program a block of the roads can be drawn with before the curtain lifts.
 *  DO NOT leave them to their first draw: the opening's district met only some of them, and every other one (the decks, the raised asphalt, a paint colour, their shadows) was built in the frame it first came into view, one after the other, behind the curtain (2026-10-21: ~10 s more of curtain once the far roads, which carried these stand-ins, were deleted). */
export function roadProgramStandIns(materials: SceneMaterials, groundMaterial: Material | undefined, paintFor: (color: string) => Material): { readonly group: Group; dispose(): void };
