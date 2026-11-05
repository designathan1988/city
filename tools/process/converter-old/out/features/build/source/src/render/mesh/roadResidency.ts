// The code of src/render/mesh/roadResidency.ts that is the feature's (the rest of the file is shared).

// src/render/mesh/roadResidency.ts:192-217 slice of src/render/mesh/roadResidency.ts#restoreCell (shared): the feature's statements; "…" marks code left out
function restoreCell(cell: Cell, residency: RoadResidency): void {
  // … 4 lines
  void Promise.all(blocks.map((mesh) => {
  // … 3 lines
  })).then((made) => {
    // … 1 lines
    blocks.forEach((mesh, i) => {
      // … 7 lines
      if (!batchedFar(levels.coarse)) void warmLooseAhead([levels.coarse]);
    });
  // … 3 lines
  });
}
