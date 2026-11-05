// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:27-31 slice of src/ui/v2/shell.ts#PAINT_SWATCH (shared): the feature's statements; "…" marks code left out
const PAINT_SWATCH: Readonly<Record<PaintKind, string>> = {
  sand: '#ccb380', soil: '#7a5a3c', meadow: '#7d8f3a', snow: '#eef1f6', gravel: '#8b8a84', asphalt: '#2d2e32', concrete: '#a8a69f', grass: '#4f7a36', forest: '#2c4f26',
  scrub: '#4c6a2c', flowers: '#c86a8e', rocks: '#7d776c', granite: '#8d8a84', sandstone: '#c27a44', basalt: '#3b3836',
  cerrado: '#b8933e', atlantic: '#2f6b34', amazon: '#1b4f27', caatinga: '#a08b68', pampa: '#86ad4c', pantanal: '#4e8f7c',
};

// src/ui/v2/shell.ts:1433-1577 slice of src/ui/v2/shell.ts#mountShell.renderLandscape (shared): the feature's statements; "…" marks code left out
  function renderLandscape(current: string): void {
    // … 16 lines
    if (now === 'relief') {
    // … 20 lines
    } else if (now === 'ground') {
      // … 4 lines
      for (const kind of PAINT_KINDS) {
        const isBiome = (BIOME_SWATCHES as readonly string[]).includes(kind);
        (isBiome ? biomes : ground).appendChild(card(t(`paint.kind.${kind}`), painting && paintKind() === kind, () => { setPaintKind(kind); useTerrain('paint'); render(); }, undefined, swatchArt(PAINT_SWATCH[kind])));
      }
      // … 3 lines
      options.append(subhead(t('v2.map')), orow(t('v2.row.biome'), choices(['none', ...BIOME_SWATCHES].map((key) => {
        return {
          // … 3 lines
          icon: key === 'none' ? '<i class="v2-ground none"></i>' : `<i class="v2-ground" style="--c:${PAINT_SWATCH[key as PaintKind]}"></i>`,
        };
      }))));
    } else if (now === 'nature') {
      // … 2 lines
      for (const kind of TREE_KINDS) {
        trees.appendChild(card(t(`tree.kind.${kind}`), inTerrain && mode === 'trees' && treeKind() === kind, () => { setTreeKind(kind); useTerrain('trees'); render(); }, undefined, svg(`tk_${kind}`, 30)));
      }
    // … 14 lines
    } else if (now === 'sky') {
    // … 33 lines
    } else {
      // … 3 lines
      for (const kind of LANDSCAPE_KINDS) {
        furniture.appendChild(card(t(`streetscape.${kind}`), current === 'streetscape' && kindNow === kind, () => { setStreetscapeKind(kind); useTool('streetscape'); render(); }, undefined, svg(`ls_${kind}`, 30)));
      }
      // … 9 lines
      if (current === 'streetscape' && kindNow === 'sign') {
        // … 1 lines
        const type = dropdown(t('sign.type'), SIGN_TYPES.map((s) => ({ value: s, label: t(`sign.type.${s}`) })), signChoice.type,
          (v) => { signChoice.type = v as typeof signChoice.type; render(); });
        options.appendChild(orow(t('v2.row.sign'), type));
        if (SIGN_HAS_TEXT.has(signChoice.type)) options.appendChild(orow(t('v2.row.text'), textInput(t(signChoice.type === 'speed' ? 'sign.text.speed' : 'sign.text'), signChoice.text, (v) => { signChoice.text = v; })));
      } else if (current === 'streetscape' && kindNow === 'streetname') {
      // … 6 lines
      }
    // … 1 lines
    }
  }

// src/ui/v2/shell.ts:1590-1599 slice of src/ui/v2/shell.ts#mountShell.textInput (shared): the feature's statements; "…" marks code left out
  function textInput(placeholder: string, value: string, set: (v: string) => void): HTMLInputElement {
    // … 2 lines
    input.maxLength = SIGN_TEXT_MAX;
  // … 5 lines
  }
