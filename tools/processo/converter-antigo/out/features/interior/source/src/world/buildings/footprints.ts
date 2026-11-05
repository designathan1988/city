// The code of src/world/buildings/footprints.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/footprints.ts:138-142 function supportedBy: the feature's own
export function supportedBy(ring: readonly Vec2[], supports: readonly Vec2[][]): boolean {
  if (supports.length === 0) return false;
  const area = Math.abs(signedArea(ring));
  return area - coveredArea(ring, supports, area) < AREA_EPS;
}
