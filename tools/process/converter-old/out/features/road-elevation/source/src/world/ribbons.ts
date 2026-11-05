// The code of src/world/ribbons.ts that is the feature's (the rest of the file is shared).

// src/world/ribbons.ts:199-211 slice of src/world/ribbons.ts#junctionBounds (shared): the feature's statements; "…" marks code left out
export function junctionBounds(byLevel: ReadonlyMap<SurfaceLevel, Junction>): ChangeRect | null {
  // … 1 lines
  if (byLevel instanceof JunctionLevels) return byLevel.bounds;
// … 9 lines
}
