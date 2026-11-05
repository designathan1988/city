// The code of src/render/people/paletteTargets.ts that is the feature's (the rest of the file is shared).

// src/render/people/paletteTargets.ts:85-103 function growSlab: the feature's own
export function growSlab(slab: Slab, rows: number): void {
  if (rows <= slab.rows) return;
  const page = slab.page;
  const next = page.slabs[page.slabs.indexOf(slab) + 1];
  const limit = next ? next.base : PAGE_ROWS;
  if (slab.base + rows <= limit) {
    slab.rows = rows;
    fit(page);
    return;
  }
  freeSlab(slab);
  const moved = takeSlab(page.width, rows);
  // The same slab object: whoever holds it reads its new place.
  const into = moved.page.slabs.indexOf(moved);
  moved.page.slabs[into] = slab;
  slab.page = moved.page;
  slab.base = moved.base;
  slab.rows = rows;
}
