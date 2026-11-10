// The code of src/render/cellInstances.ts that is the feature's (the rest of the file is shared).

// src/render/cellInstances.ts:688-721 slice of src/render/cellInstances.ts#createCellInstances.append (shared): the feature's statements; "…" marks code left out
  const append = (k: Kind, key: object, one: Pack): void => {
    // … 8 lines
    for (let i = 0; i < one.count; i++) {
      // … 12 lines
      k.picks[slot] = impostorPick(scratch[12]!, scratch[13]!, scratch[14]!);
    // … 2 lines
    }
  // … 7 lines
  };

// src/render/cellInstances.ts:885-975 slice of src/render/cellInstances.ts#createCellInstances.visit (shared): the feature's statements; "…" marks code left out
  const visit = (node: CellNode, mask: number): void => {
    // … 26 lines
    for (const k of kinds) {
      // … 10 lines
      const stopAt = far < 0 ? Infinity : thinWant(node.big![k.index]! * (wNear > 1e-4 ? perUnitAtOne / wNear : Infinity)) + PICK_SLACK;
      // … 5 lines
      for (const i of slots) {
        // … 2 lines
        const thinned = picks[i]! > stopAt;
        // … 3 lines
        if (thinned && !mayCover) { if (!covered) break; continue; }
        // … 21 lines
        if (!inFrame || thinned) { lv[i] = UNKNOWN; mark(k, i, NONE, cover); continue; }
        // … 13 lines
        mark(k, i, level === FAR && picks[i]! > thinWant(across) + PICK_SLACK ? NONE : level, cover);
      }
    }
  };
