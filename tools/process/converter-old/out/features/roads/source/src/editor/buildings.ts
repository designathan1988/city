// The code of src/editor/buildings.ts that is the feature's (the rest of the file is shared).

// src/editor/buildings.ts:597-601 slice of src/editor/buildings.ts#clearBuildingsAfterEdit (shared): the feature's statements; "…" marks code left out
export function clearBuildingsAfterEdit(ctx: BuildingContext, before: object): number {
  // … 1 lines
  const delta = ctx.net ? ctx.net.changesSince(before) : null;
  return clearBuildingsOnRoads(ctx, delta === null ? null : delta.rects);
}
