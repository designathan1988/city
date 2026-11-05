// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:1674-1736 slice of src/ui/v2/shell.ts#mountShell.renderBuild (shared): the feature's statements; "…" marks code left out
  function renderBuild(): void {
    // … 44 lines
    for (const s of sections) {
      // … 1 lines
      if (s.shelf === 'models') wantPictures.push(...CITY_BUILDINGS.map((c) => `city:${c.fn}`), ...BLUEPRINTS.map((b) => b.key), ...state.userBlueprints.map((b) => b.key), ...LOT_NAMES.map(lotKey));
    // … 1 lines
    }
  // … 12 lines
  }

// src/ui/v2/shell.ts:1809-1853 slice of src/ui/v2/shell.ts#mountShell.renderModels (shared): the feature's statements; "…" marks code left out
  function renderModels(state: BuilderState): void {
    // … 29 lines
    for (const name of LOT_NAMES) {
      const key = lotKey(name);
      entries.push({ key, label: t(`builder.lot.${name}`), cat: 'lots', run: () => actions.choosePreset(key) });
    }
  // … 10 lines
  }
