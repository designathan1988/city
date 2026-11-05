// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:1223-1338 slice of src/ui/v2/shell.ts#mountShell.renderRoads (shared): the feature's statements; "…" marks code left out
  function renderRoads(current: string): void {
    // … 10 lines
    if (current === 'road') {
      // … 59 lines
      const furnitureNow = furnitureChosen();
      // … 1 lines
      for (const set of FURNITURE_SETS) {
        const b = button(`v2-seg-b rp-seg-text${furnitureNow === set ? ' on' : ''}`, t(`roadFurniture.${set}`), () => { setFurnitureChosen(set); render(); });
        b.setAttribute('aria-pressed', String(furnitureNow === set));
        b.dataset['furniture'] = set;
        furnitureRow.appendChild(b);
      }
    // … 15 lines
    }
  // … 20 lines
  }

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
      // … 4 lines
      if (current === 'streetscape' && kindNow === 'sign') {
      // … 5 lines
      } else if (current === 'streetscape' && kindNow === 'streetname') {
      // … 1 lines
      } else if (current === 'pole' && poleToolMode() === 'build') {
        // … 1 lines
        options.appendChild(orow(t('v2.row.lights'), choices(POLE_LAMP_MODES.map((m) => ({
          label: `${t('pole.lamps')}: ${t(`pole.lamps.${m}`)}`, on: poleLampMode() === m, run: () => { setPoleLampMode(m); render(); }, icon: svg(`pl_${m}`, 18),
        })))));
      }
    // … 1 lines
    }
  }
