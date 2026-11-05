// The code of src/lab/catalog/city.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/city.ts:379-407 slice of src/lab/catalog/city.ts#drawCallsByGroup (shared): the feature's statements; "…" marks code left out
export async function drawCallsByGroup(): Promise<{ total: number; triangles: number; groups: Record<string, number> }> {
  // … 16 lines
  for (const key of [...LAYER_SWITCHES, ...BUILDING_SWITCHES]) {
    // Its own frame with everything on, right before it: the city streams on meanwhile.
    p.variant(null);
    const on = await frame();
    total = Math.max(total, on.calls); drawn = Math.max(drawn, on.triangles);
    p.variant({ [key]: false });
    const off = await frame();
    groups[key] = on.calls - off.calls;
  }
  // … 1 lines
  return { total, triangles: drawn, groups };
}
