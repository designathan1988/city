// The code of src/editor/roads/economy.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/economy.ts:11-13 slice of src/editor/roads/economy.ts#roadsBefore (shared): the feature's statements; "…" marks code left out
export function roadsBefore(doc: RoadDoc): RoadsBefore {
  return { value: roadValue(doc), balance: doc.economy.balance };
}
