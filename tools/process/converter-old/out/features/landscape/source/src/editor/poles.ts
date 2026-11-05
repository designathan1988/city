// The code of src/editor/poles.ts that is the feature's (the rest of the file is shared).

// src/editor/poles.ts:229-237 slice of src/editor/poles.ts#inLanding (shared): the feature's statements; "…" marks code left out
function inLanding(accesses: readonly CrossingAccess[], p: Vec2): boolean {
  // … 1 lines
  return accesses.some((access) => {
    if (access.structure !== 'ground') return false;
    const dx = p.x - access.x, dy = p.y - access.y;
    return Math.abs(dx * access.tx + dy * access.ty) < CROSSWALK_DEPTH / 2 + margin &&
      Math.abs(-dx * access.ty + dy * access.tx) < access.across + margin;
  });
}
