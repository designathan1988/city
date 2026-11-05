// The code of src/editor/persistence.ts that is the feature's (the rest of the file is shared).

// src/editor/persistence.ts:498-595 slice of src/editor/persistence.ts#isSerializedDoc (shared): the feature's statements; "…" marks code left out
export function isSerializedDoc(value: unknown): value is SerializedDoc {
  // … 44 lines
  if (value.relief !== undefined && readRelief(value.relief) === null) return false;
  if (value.nature !== undefined && !isNatureSettings(value.nature)) return false;
  if (value.terrain !== undefined) {
    // … 2 lines
    for (const stamp of value.terrain) {
      // … 1 lines
      if (!isFiniteNumber(stamp.x) || !isFiniteNumber(stamp.y) ||
        !isFiniteNumber(stamp.radius) || stamp.radius <= 0 || stamp.radius > MAX_STAMP_RADIUS ||
        (stamp.level !== undefined && !isFiniteNumber(stamp.level)) ||
        (stamp.stroke !== undefined && !isId(stamp.stroke)) ||
        (stamp.ox !== undefined && !isFiniteNumber(stamp.ox)) ||
        (stamp.oy !== undefined && !isFiniteNumber(stamp.oy)) ||
        (stamp.rough !== undefined && typeof stamp.rough !== 'boolean') ||
        (stamp.hardness !== undefined && (!isFiniteNumber(stamp.hardness) || stamp.hardness < 0 || stamp.hardness > 1)) ||
        (stamp.profile !== undefined && !isTerrainProfile(stamp.profile)) ||
        !isFiniteNumber(stamp.strength) || stamp.strength < 0 ||
        !isTerrainMode(stamp.mode)) return false;
    // … 1 lines
    }
  }
// … 31 lines
}
