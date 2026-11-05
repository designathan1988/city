// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:446-2084 slice of src/ui/v2/shell.ts#mountShell (shared): the feature's statements; "…" marks code left out
export function mountShell(deps: ShellDeps): ShellHandle {
  // … 97 lines
  const LAYERS: readonly { key: string; tip: string; icon: string; on: () => boolean; flip: () => void }[] = [
    { key: 'v2.layer.grid', tip: 'v2.grid.toggle', icon: svg('grid', 18), on: roadGridShown, flip: () => setRoadGridShown(!roadGridShown()) },
    { key: 'zone.colours', tip: 'zone.colours', icon: svg('zones', 18), on: zoneColoursShown, flip: () => setZoneColoursShown(!zoneColoursShown()) },
  // … 2 lines
  ];
// … 1534 lines
}

// src/ui/v2/shell.ts:1223-1338 slice of src/ui/v2/shell.ts#mountShell.renderRoads (shared): the feature's statements; "…" marks code left out
  function renderRoads(current: string): void {
    // … 10 lines
    if (current === 'road') {
      const g = blockGridChoice;
      // … 1 lines
      const traces = (['straight', 'curve', 'free'] as const).map((a) => ({
        // … 1 lines
        on: !g.armed && game.alignment === a,
        run: () => { g.armed = false; deps.setAlignment(a); render(); },
      // … 1 lines
      }));
      traces.push({ label: t('palette.blocks'), on: g.armed, run: () => { g.armed = !g.armed; render(); }, icon: svg('blocks', 18) });
      // … 2 lines
      if (g.armed) {
        options.appendChild(orow(t('v2.blocks.count'),
          stepper(t('palette.blocks.cols'), `${g.cols}`, (d) => { g.cols = Math.max(1, Math.min(12, g.cols + d)); }),
          stepper(t('palette.blocks.rows'), `${g.rows}`, (d) => { g.rows = Math.max(1, Math.min(12, g.rows + d)); })));
        options.appendChild(orow(t('v2.blocks.measure'),
          stepper(t('palette.blocks.size'), `${g.blockMetres} m`, (d) => { g.blockMetres = Math.max(30, Math.min(300, g.blockMetres + d * 10)); }),
          stepper(t('palette.blocks.angle'), `${Math.round((g.angle * 180) / Math.PI)}°`, (d) => { g.angle += (d * 15 * Math.PI) / 180; })));
      }
      // … 17 lines
      const w = roadWidth();
      // … 1 lines
      if (!fromCatalog) options.appendChild(orow(t('v2.row.width'),
        stepper(t('palette.width'), w === null ? t('palette.width.auto') : `${w} m`, (d) => { setRoadWidth((roadWidth() ?? classWidth) + d); }),
        w === null ? null : button('v2-icon', t('palette.width.auto'), () => { setRoadWidth(null); render(); }, svg('undo', 14))));
    // … 41 lines
    }
  // … 20 lines
  }

// src/ui/v2/shell.ts:1399-1407 slice of src/ui/v2/shell.ts#mountShell.landTabOf (shared): the feature's statements; "…" marks code left out
  const landTabOf = (current: string): LandTab => {
    // … 4 lines
    if (mode === 'elements') return isEffect(elementKind()) ? 'sky' : 'nature';
  // … 2 lines
  };

// src/ui/v2/shell.ts:1416-1419 slice of src/ui/v2/shell.ts#mountShell.useElement (shared): the feature's statements; "…" marks code left out
  const useElement = (kind: ElementKind): void => {
    setElementKind(kind);
  // … 1 lines
  };

// src/ui/v2/shell.ts:1433-1577 slice of src/ui/v2/shell.ts#mountShell.renderLandscape (shared): the feature's statements; "…" marks code left out
  function renderLandscape(current: string): void {
    // … 5 lines
    landLast[now] = now === 'street' ? current : mode === 'elements' ? `el:${elementKind()}` : mode;
    // … 10 lines
    if (now === 'relief') {
      // … 4 lines
      if (mode === 'gully') {
        // … 1 lines
        options.appendChild(orow(t('v2.row.brush'), choices([
          { label: t('gully.cut'), on: !gullyErase(), run: () => { setGullyErase(false); render(); }, icon: svg('brush', 18) },
          { label: t('gully.erase'), on: gullyErase(), run: () => { setGullyErase(true); render(); }, icon: svg('eraser', 18) },
        ])));
      // … 3 lines
      }
    // … 6 lines
    } else if (now === 'ground') {
      // … 4 lines
      for (const kind of PAINT_KINDS) {
        // … 1 lines
        (isBiome ? biomes : ground).appendChild(card(t(`paint.kind.${kind}`), painting && paintKind() === kind, () => { setPaintKind(kind); useTerrain('paint'); render(); }, undefined, swatchArt(PAINT_SWATCH[kind])));
      }
    // … 11 lines
    } else if (now === 'nature') {
      // … 2 lines
      for (const kind of TREE_KINDS) {
        trees.appendChild(card(t(`tree.kind.${kind}`), inTerrain && mode === 'trees' && treeKind() === kind, () => { setTreeKind(kind); useTerrain('trees'); render(); }, undefined, svg(`tk_${kind}`, 30)));
      }
      // … 1 lines
      for (const kind of PROPS) {
        plants.appendChild(card(t(`el.kind.${kind}`), inTerrain && mode === 'elements' && elementKind() === kind, () => { useElement(kind); render(); }, undefined, svg(`el_${kind}`, 30)));
      }
      if (inTerrain && mode === 'trees') {
        const icons: Record<(typeof TREE_MODES)[number], string> = { plant: 'brush', one: 'plus', cut: 'eraser' };
        options.appendChild(orow(t('v2.row.brush'), choices(TREE_MODES.map((m) => ({
          label: t(`tree.mode.${m}`), on: treeMode() === m, run: () => { setTreeMode(m); render(); }, icon: svg(icons[m], 18),
        })))));
      // … 2 lines
      } else if (inTerrain && mode === 'elements') {
      // … 1 lines
      }
    } else if (now === 'sky') {
      // … 3 lines
      for (const kind of EFFECT_KINDS) {
        effects.appendChild(card(t(`el.kind.${kind}`), inTerrain && mode === 'elements' && elementKind() === kind, () => { useElement(kind); render(); }, undefined, svg(`el_${kind}`, 30)));
      }
      if (mode === 'cloud') {
        // … 1 lines
        const icons: Record<(typeof CLOUD_MODES)[number], string> = { add: 'plus', move: 'move', edit: 'draw', remove: 'eraser' };
        options.appendChild(orow(t('v2.row.brush'), choices(CLOUD_MODES.map((m) => ({
          label: t(`cloud.${m}`), on: cloudMode() === m, run: () => { setCloudMode(m); render(); }, icon: svg(icons[m], 18),
        })))));
      // … 4 lines
      } else if (mode === 'fog') {
        // … 1 lines
        options.appendChild(orow(t('v2.row.brush'), choices([
          { label: t('fog.lay'), on: !fogErase(), run: () => { setFogErase(false); render(); }, icon: svg('brush', 18) },
          { label: t('fog.erase'), on: fogErase(), run: () => { setFogErase(true); render(); }, icon: svg('eraser', 18) },
        ])));
      // … 4 lines
      } else if (mode === 'weather') {
      // … 5 lines
      }
    } else {
      // … 2 lines
      const kindNow = streetscapeKind();
      for (const kind of LANDSCAPE_KINDS) {
        furniture.appendChild(card(t(`streetscape.${kind}`), current === 'streetscape' && kindNow === kind, () => { setStreetscapeKind(kind); useTool('streetscape'); render(); }, undefined, svg(`ls_${kind}`, 30)));
      }
      // … 6 lines
      for (const next of POLE_TOOL_MODES) {
        poles.appendChild(card(t(`pole.mode.${next}`), current === 'pole' && poleToolMode() === next, () => { setPoleToolMode(next); useTool('pole'); render(); }, undefined, svg(`pole_${next}`, 30)));
      }
      if (current === 'streetscape' && kindNow === 'sign') {
        // Which sign, and the words on those that carry them.
        const type = dropdown(t('sign.type'), SIGN_TYPES.map((s) => ({ value: s, label: t(`sign.type.${s}`) })), signChoice.type,
          (v) => { signChoice.type = v as typeof signChoice.type; render(); });
        options.appendChild(orow(t('v2.row.sign'), type));
        if (SIGN_HAS_TEXT.has(signChoice.type)) options.appendChild(orow(t('v2.row.text'), textInput(t(signChoice.type === 'speed' ? 'sign.text.speed' : 'sign.text'), signChoice.text, (v) => { signChoice.text = v; })));
      } else if (current === 'streetscape' && kindNow === 'streetname') {
        options.appendChild(orow(t('v2.row.text'), textInput(t('sign.street.placeholder'), signChoice.streetName, (v) => { signChoice.streetName = v; })));
      } else if (current === 'pole' && poleToolMode() === 'build') {
        // Street lights on the poles of the next run.
        options.appendChild(orow(t('v2.row.lights'), choices(POLE_LAMP_MODES.map((m) => ({
          label: `${t('pole.lamps')}: ${t(`pole.lamps.${m}`)}`, on: poleLampMode() === m, run: () => { setPoleLampMode(m); render(); }, icon: svg(`pl_${m}`, 18),
        })))));
      }
      note(t(current === 'barrier' ? 'help.tool.barrier' : current === 'pole' ? (poleToolMode() === 'remove' ? 'help.tool.poleRemove' : 'help.tool.pole') : 'help.tool.streetscape'));
    }
  }

// src/ui/v2/shell.ts:1579-1588 slice of src/ui/v2/shell.ts#mountShell.elementRows (shared): the feature's statements; "…" marks code left out
  function elementRows(): void {
    // … 1 lines
    options.appendChild(orow(t('v2.row.brush'), choices((['lay', 'eraseKind', 'eraseAll'] as const).map((m) => ({
      label: t(`el.mode.${m}`), on: elementMode() === m, run: () => { setElementMode(m); render(); }, icon: svg(icons[m], 18),
    })))));
    // … 2 lines
    if (isEffect(elementKind())) options.appendChild(range('el.intensity', 'elIntensity', 'v2.s.effect'));
  // … 1 lines
  }

// src/ui/v2/shell.ts:1640-1656 slice of src/ui/v2/shell.ts#mountShell.renderSimple (shared): the feature's statements; "…" marks code left out
  function renderSimple(current: string): void {
    // … 1 lines
    if (current === 'bulldoze') {
      // … 1 lines
      for (const [mode, key, icon] of [['demolish', 'tool.bulldoze', 'demolish'], ['shoot', 'actions.pistol', 'actions'], ['strike', 'actions.bomb', 'bomb']] as const) {
        tabs.appendChild(tab(t(key), strikeChoice.mode === mode, () => { strikeChoice.mode = mode; render(); }, false, svg(icon, 18)));
      }
      if (strikeChoice.mode === 'strike') {
        const s = strikeChoice;
        options.appendChild(orow(t('strike.force'), stepper(t('strike.force'), `${s.strength}`, (d) => {
          s.strength = d < 0 ? Math.max(1, s.strength - (s.strength > 10 ? 5 : 1)) : Math.min(50, s.strength + (s.strength >= 10 ? 5 : 1));
        })));
      }
    }
    // … 1 lines
    note(t(current === 'bulldoze' && strikeChoice.mode !== 'demolish' ? `help.strike.${strikeChoice.mode}` : `help.tool.${current}`));
  }
