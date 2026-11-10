// The code of src/world/buildings/blueprints.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/blueprints.ts:321-323 function blueprintByKey: the feature's own
export function blueprintByKey(key: string): Blueprint | undefined {
  return BLUEPRINTS.find((bp) => bp.key === key);
}

// src/world/buildings/blueprints.ts:326-330 function bodyOf: the feature's own
export function bodyOf(b: Building): BlueprintBody {
  const copy = JSON.parse(JSON.stringify(b)) as Record<string, unknown>;
  for (const key of ['id', 'x', 'y', 'rotation']) delete copy[key];
  return copy as unknown as BlueprintBody;
}
