// The code of src/render/buildings/layer.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/layer.ts:289-307 slice of src/render/buildings/layer.ts#createBuildingLayer.capTriangles (shared): the feature's statements; "…" marks code left out
  const capTriangles = (): void => {
    if (!eye || !Number.isFinite(frame.budget)) return;
    // … 3 lines
    massCells.cap(eye.x, eye.y, frame.budget - masses, (key, fine) => landedTriangles.of(fine ? 'cell' : 'facade', key), forcedNear, (key) => massCells.massTriangles(key));
    // … 7 lines
    if (heldFar.size || heldCoarse.size) {
      letBackFitting(heldFar, heldCoarse, chosenTriangles(), frame.budget, rankOf, (key, far) =>
        (far ? landedTriangles.of('facade', key) - massCells.massTriangles(key) : landedTriangles.of('cell', key) - landedTriangles.of('facade', key)));
    }
  // … 1 lines
  };

// src/render/buildings/layer.ts:981-1035 slice of src/render/buildings/layer.ts#createBuildingLayer.levelOfDetail (shared): the feature's statements; "…" marks code left out
    levelOfDetail(eyeX, eyeY, eyeZ, pxAtMetre, orthoPx, reach = Infinity, view = null) {
      // … 14 lines
      read[6] = frame.budget; read[7] = 1; read[8] = room(); read[9] = view ? 1 : 0;
      // … 1 lines
      if (lodStale || !sameRead || !lodSettled) {
        // … 2 lines
        if (fitted(1) > frame.budget) frame.fit(fitted); else frame.scale = 1;
      // … 6 lines
      }
    // … 26 lines
    },

// src/render/buildings/layer.ts:1066-1080 slice of src/render/buildings/layer.ts#createBuildingLayer.levels (shared): the feature's statements; "…" marks code left out
    levels() {
      // … 12 lines
      return { ...count, evictions: massCells.books()['massEvictions'] ?? 0, massing: requests.massing, levelsWaiting: massCells.waiting, levelsWaitingInView: massCells.waitingInView, basesWaiting: massCells.basesWaiting, scale: frame.scale, budgetTriangles: frame.budget, chosenTriangles: chosenTriangles(), residentBytes: resident.total, detailBudgetBytes: budget, roomBytes: room() };
    },
