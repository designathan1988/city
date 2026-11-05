// The code of src/editor/persistence.ts that is the feature's (the rest of the file is shared).

// src/editor/persistence.ts:498-595 slice of src/editor/persistence.ts#isSerializedDoc (shared): the feature's statements; "…" marks code left out
export function isSerializedDoc(value: unknown): value is SerializedDoc {
  // … 89 lines
  if (!isSerializedBuildings(value.buildings)) return false;
// … 6 lines
}
