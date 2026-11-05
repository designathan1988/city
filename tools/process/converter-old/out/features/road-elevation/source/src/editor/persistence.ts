// The code of src/editor/persistence.ts that is the feature's (the rest of the file is shared).

// src/editor/persistence.ts:498-595 slice of src/editor/persistence.ts#isSerializedDoc (shared): the feature's statements; "…" marks code left out
export function isSerializedDoc(value: unknown): value is SerializedDoc {
  // … 17 lines
  for (const segment of value.segments) {
    // … 15 lines
    if (segment.structure !== undefined && migrateStructure(segment.structure) === null) return false;
  // … 7 lines
  }
// … 54 lines
}
