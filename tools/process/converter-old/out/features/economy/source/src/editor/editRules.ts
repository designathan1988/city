// The code of src/editor/editRules.ts that is the feature's (the rest of the file is shared).

// src/editor/editRules.ts:81-95 slice of src/editor/editRules.ts#guardRoadEdit (shared): the feature's statements; "…" marks code left out
export function guardRoadEdit(doc: RoadDoc, net: Network, edit: (doc: RoadDoc, net: Network) => boolean):
  { readonly changed: boolean; readonly refused: RoadEditRefusal | null } {
  // … 1 lines
  const money = roadsBefore(doc);
  // … 5 lines
  const refused = refuseRoadEdit({ doc, net }, { doc: work, net: workNet }) ?? (settleRoadEdit(money, work).affordable ? null : 'funds');
  if (refused) return { changed: false, refused };
// … 3 lines
}
