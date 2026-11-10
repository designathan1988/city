// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:1433-1577 slice of src/ui/v2/shell.ts#mountShell.renderLandscape (shared): the feature's statements; "…" marks code left out
  function renderLandscape(current: string): void {
    // … 16 lines
    if (now === 'relief') {
    // … 20 lines
    } else if (now === 'ground') {
    // … 19 lines
    } else if (now === 'nature') {
    // … 19 lines
    } else if (now === 'sky') {
    // … 33 lines
    } else {
      // … 8 lines
      for (const kind of BARRIER_KINDS) {
        walls.appendChild(card(t(`barrier.kind.${kind}`), current === 'barrier' && barrierNow === kind, () => useBarrier(kind), undefined, BARRIER_ICON(kind, 30)));
      }
    // … 19 lines
    }
  }
