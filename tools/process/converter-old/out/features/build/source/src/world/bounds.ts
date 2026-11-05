// The code of src/world/bounds.ts that is the feature's (the rest of the file is shared).

// src/world/bounds.ts:84-85 function insideWorld: the feature's own
export const insideWorld = (p: Vec2, margin = 0): boolean =>
  Math.abs(p.x) <= WORLD_HALF - margin && Math.abs(p.y) <= WORLD_HALF - margin;
