// What the feature uses of src/lab/inspector/state.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/inspector/state.ts:7-13 type EntityRef
/** THE INSPECTOR'S STATE: what is selected (the 3D object hit, the game's entity under it, everything else the ray met), told to the tabs; and the log of live edits, each undone alone or all together. */

/** The game's own thing under the cursor, by its id in the document or the simulation. */
export type EntityRef =
  | { readonly kind: 'building'; readonly id: number }
  | { readonly kind: 'segment'; readonly id: number; readonly s?: number }
  | { readonly kind: 'node'; readonly id: number }
  | { readonly kind: 'vehicle'; readonly id: number }
  | { readonly kind: 'person'; readonly id: number }
  | { readonly kind: 'terrain'; readonly x: number; readonly y: number };
