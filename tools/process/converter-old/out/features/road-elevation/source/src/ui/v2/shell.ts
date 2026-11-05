// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:1223-1338 slice of src/ui/v2/shell.ts#mountShell.renderRoads (shared): the feature's statements; "…" marks code left out
  function renderRoads(current: string): void {
    // … 10 lines
    if (current === 'road') {
      // … 49 lines
      const walls = cutWallsChosen();
      // … 1 lines
      for (const on of [false, true]) {
        const b = button(`v2-seg-b rp-seg-text${walls === on ? ' on' : ''}`, t(on ? 'cutSides.walls' : 'cutSides.batter'), () => { setCutWalls(on); render(); });
        b.setAttribute('aria-pressed', String(walls === on));
        b.dataset['cutWalls'] = String(on);
        cutRow.appendChild(b);
      }
    // … 25 lines
    }
  // … 20 lines
  }
