// What the feature uses of src/editor/editRules.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/editRules.ts:17-17 type RoadEditRefusal
/** What a road edit may not leave behind, judged on the network it builds. */
export type RoadEditRefusal = 'sharp' | 'squeezed' | 'overlap' | 'steep' | 'funds';

// src/editor/editRules.ts:19-23 interface RoadState
export interface RoadState {
  readonly doc: RoadDoc;
  /** Built for `doc` (`revision` current). */
  readonly net: Network;
}

// src/editor/editRules.ts:43-60 function refuseRoadEdit
/** The reason to refuse turning `before` into `after`, or null when the edit may stand. */
export function refuseRoadEdit(before: RoadState, after: RoadState): RoadEditRefusal | null;

// src/editor/editRules.ts:81-95 function guardRoadEdit
/** DO NOT copy the map to judge an edit against (`doc.clone()` and the whole network, every in-place edit) */
export function guardRoadEdit(doc: RoadDoc, net: Network, edit: (doc: RoadDoc, net: Network) => boolean):
  { readonly changed: boolean; readonly refused: RoadEditRefusal | null };
