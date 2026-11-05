// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:446-2084 slice of src/ui/v2/shell.ts#mountShell (shared): the feature's statements; "…" marks code left out
export function mountShell(deps: ShellDeps): ShellHandle {
  // … 1621 lines
  onRoadKeysChange(() => render());
// … 15 lines
}

// src/ui/v2/shell.ts:502-507 slice of src/ui/v2/shell.ts#mountShell.syncMoney (shared): the feature's statements; "…" marks code left out
  const syncMoney = (value: number): void => {
    // … 2 lines
    moneyText.data = formatMoney(value);
    money.title = balanceTip();
  };

// src/ui/v2/shell.ts:1223-1338 slice of src/ui/v2/shell.ts#mountShell.renderRoads (shared): the feature's statements; "…" marks code left out
  function renderRoads(current: string): void {
    // … 10 lines
    if (current === 'road') {
      // … 27 lines
      const fromCatalog = drawProfile() !== null;
      // … 1 lines
      if (!fromCatalog) options.appendChild(orow(t('v2.row.lanes'), choices(deps.laneChoices().map((c) => ({
        label: c.label,
        on: c.on,
        run: () => { deps.setLanes(c.id); render(); },
        icon: svg(c.id === 'median' ? 'median' : `lanes${c.id}`, 18),
      })))));
      // … 3 lines
      if (!fromCatalog) options.appendChild(orow(t('v2.row.width'),
        stepper(t('palette.width'), w === null ? t('palette.width.auto') : `${w} m`, (d) => { setRoadWidth((roadWidth() ?? classWidth) + d); }),
        w === null ? null : button('v2-icon', t('palette.width.auto'), () => { setRoadWidth(null); render(); }, svg('undo', 14))));
      // … 8 lines
      const walls = cutWallsChosen();
      // … 1 lines
      for (const on of [false, true]) {
        const b = button(`v2-seg-b rp-seg-text${walls === on ? ' on' : ''}`, t(on ? 'cutSides.walls' : 'cutSides.batter'), () => { setCutWalls(on); render(); });
        b.setAttribute('aria-pressed', String(walls === on));
        b.dataset['cutWalls'] = String(on);
        cutRow.appendChild(b);
      }
      // … 2 lines
      const furnitureNow = furnitureChosen();
      // … 1 lines
      for (const set of FURNITURE_SETS) {
        const b = button(`v2-seg-b rp-seg-text${furnitureNow === set ? ' on' : ''}`, t(`roadFurniture.${set}`), () => { setFurnitureChosen(set); render(); });
        b.setAttribute('aria-pressed', String(furnitureNow === set));
        b.dataset['furniture'] = set;
        furnitureRow.appendChild(b);
      }
      // … 4 lines
      const keysB = button('v2-pill rp-open', `${keyLabel(roadKey('heightUp'), true)} · ${keyLabel(roadKey('heightDown'), true)} · ${keyLabel(roadKey('alignment'), true)}`,
        () => (roadKeysOpen() ? closeRoadKeys() : openRoadKeys(keysB)), svg('keyboard', 16));
      options.appendChild(orow(t('roadKeys.row'), keysB));
      // … 2 lines
      if (!fromCatalog) options.appendChild(orow(t('v2.row.parking'), choices(ROAD_PARKING_PRESETS.map((preset) => ({
        label: t(`parking.preset.${preset}`),
        on: parkingNow === preset,
        run: () => { setRoadParkingPreset(preset); render(); },
        icon: svg(`park-${preset}`, 18),
      })))));
    }
    if (current === 'road') {
      // … 1 lines
      strip.appendChild(catalogStrip({
        classIndex: () => game.roadTypeIndex,
        pickClass: (index) => deps.selectRoadType(index),
        refresh: () => render(),
      }));
    } else if (current === 'upgrade') {
    // … 11 lines
    }
  }
