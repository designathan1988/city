// The code of src/world/buildings/serialize.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/serialize.ts:305-395 slice of src/world/buildings/serialize.ts#migrateBuilding (shared): the feature's statements; "…" marks code left out
export function migrateBuilding(raw: unknown): Building | null {
  // … 75 lines
  if (raw.furnishing && typeof raw.furnishing === 'object') {
    for (const [level, list] of Object.entries(raw.furnishing as Record<string, unknown>)) {
      // … 1 lines
      const items = list.filter((it): it is PlacedFurniture => !!it && typeof it === 'object' && isFurnitureKind((it as PlacedFurniture).kind) &&
        [(it as PlacedFurniture).x, (it as PlacedFurniture).y, (it as PlacedFurniture).angle].every(Number.isFinite))
        .slice(0, 400).map((it) => ({ kind: it.kind, x: it.x, y: it.y, angle: it.angle }));
      furnishing[level] = items;
    }
  }
// … 5 lines
}
