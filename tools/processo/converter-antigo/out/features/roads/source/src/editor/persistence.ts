// The code of src/editor/persistence.ts that is the feature's (the rest of the file is shared).

// src/editor/persistence.ts:498-595 slice of src/editor/persistence.ts#isSerializedDoc (shared): the feature's statements; "…" marks code left out
export function isSerializedDoc(value: unknown): value is SerializedDoc {
  // … 17 lines
  for (const segment of value.segments) {
    // … 4 lines
    if (
      typeof segment.type !== 'number' ||
      !Number.isInteger(segment.type) ||
      segment.type < 0 ||
      segment.type >= ROAD_TYPES.length
    ) {
      return false;
    }
    // … 2 lines
    if (segment.section !== undefined && !normalizeRoadSection(segment.section)) return false;
  // … 8 lines
  }
// … 54 lines
}

// src/editor/persistence.ts:601-603 slice of src/editor/persistence.ts#isSegmentDirection (shared): the feature's statements; "…" marks code left out
function isSegmentDirection(value: unknown): value is SegmentDirection {
// … 1 lines
}
