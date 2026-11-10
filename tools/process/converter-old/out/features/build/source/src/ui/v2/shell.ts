// The code of src/ui/v2/shell.ts that is the feature's (the rest of the file is shared).

// src/ui/v2/shell.ts:363-434 slice of src/ui/v2/shell.ts#ShellDeps (shared): the feature's statements; "…" marks code left out
export interface ShellDeps {
  readonly workspace: BuilderWorkspace;
// … 69 lines
}

// src/ui/v2/shell.ts:446-2084 slice of src/ui/v2/shell.ts#mountShell (shared): the feature's statements; "…" marks code left out
export function mountShell(deps: ShellDeps): ShellHandle {
  // … 2 lines
  const actions = workspace.actions;
  // … 85 lines
  let inside = workspace.inside();
  // … 1 lines
  const insideDownB = button('v2-icon', t('inside.down'), () => seeInside('down'), builderIconSvg('floorDown', 16));
  const insideUpB = button('v2-icon', t('inside.up'), () => seeInside('up'), builderIconSvg('floorUp', 16));
  // … 1 lines
  const insideOff = button('v2-icon on', t('inside.toggle'), () => seeInside('toggle'), builderIconSvg('interiorView', 18));
  // … 1 lines
  insideBar.append(insideOff, insideDownB, insideLevelEl, insideUpB);
  // … 1 lines
  const LAYERS: readonly { key: string; tip: string; icon: string; on: () => boolean; flip: () => void }[] = [
    // … 3 lines
    { key: 'v2.layer.inside', tip: 'inside.toggle', icon: builderIconSvg('interiorView', 18), on: () => inside.on, flip: () => seeInside('toggle') },
  ];
  // … 7 lines
  workspace.onInside((now) => { inside = now; syncInside(); });
  // … 365 lines
  const builderInspector = workspace.root.querySelector<HTMLElement>('.bw-inspector');
  // … 62 lines
  let builder: BuilderState | null = null;
  // … 681 lines
  const THINGS = new Set<string>([...FACADE_PATTERNS, 'window', 'sashWindow', 'wideWindow', 'ribbon', 'bayWindow', 'frenchWindow',
    'door', 'doubleDoor', 'garageDoor', 'loadingDoor', 'balcony', 'shopfront', 'roofFlat', 'roofShed', 'roofGable', 'roofHip',
    'roofSawtooth', 'roofTerrace', 'solar', 'skylight', 'vent', 'chimney', 'waterTank', 'spire', 'lantern', 'stair', 'ramp', 'pillar',
    'canopy', 'wall', 'slab', 'wallRun', 'fenceRun', 'pavementRun', 'railing', 'stairRun', 'tree', 'shrub', 'hedge', 'flowers', 'rocks',
    'bench', 'planter', 'parking', 'ac', 'awning', 'clock']);
  // … 369 lines
  workspace.subscribe((state) => {
    const same = state === builder;
    builder = state;
    if (tool() !== 'building') return;
    const sig = sigOf(state);
    if (same || sig !== builderSig) {
      builderSig = sig;
      later();
    }
  });
// … 32 lines
}

// src/ui/v2/shell.ts:536-536 slice of src/ui/v2/shell.ts#mountShell.seeInside (shared): the feature's statements; "…" marks code left out
  const seeInside = (command: 'toggle' | 'up' | 'down'): void => { inside = actions.seeInside(command); syncInside(); };

// src/ui/v2/shell.ts:550-555 slice of src/ui/v2/shell.ts#mountShell.syncInside (shared): the feature's statements; "…" marks code left out
  const syncInside = (): void => {
    if (insideBar.hidden === inside.on) insideBar.hidden = !inside.on;
    // … 1 lines
    const level = inside.level === 0 ? t('inside.ground') : t('inside.floor', { n: inside.level });
    if (insideLevelEl.textContent !== level) insideLevelEl.textContent = level;
  };

// src/ui/v2/shell.ts:1393-1394 slice of src/ui/v2/shell.ts#mountShell.BARRIER_ICON (shared): the feature's statements; "…" marks code left out
  const BARRIER_ICON = (kind: string, size: number): string =>
    kind === 'guardrail' || kind === 'railing' ? svg(kind, size) : builderIconSvg(kind === 'hedge' ? 'hedge' : kind === 'wall' ? 'wallRun' : 'fenceRun', size);

// src/ui/v2/shell.ts:1659-1662 slice of src/ui/v2/shell.ts#mountShell.picture (shared): the feature's statements; "…" marks code left out
  const picture = (id: string): string | undefined => {
    const shape = DRAW_SHAPES[id] ?? TIER_SHAPES[id];
    return workspace.thumbnail(id) ?? (shape ? planSwatch(shape) : undefined);
  };

// src/ui/v2/shell.ts:1663-1667 slice of src/ui/v2/shell.ts#mountShell.ask (shared): the feature's statements; "…" marks code left out
  const ask = (ids: readonly string[]): void => {
    const wanted = ids.filter((id) => !requested.has(id) && !workspace.thumbnail(id));
    for (const id of wanted) requested.add(id);
    if (wanted.length) actions.requestThumbnails(wanted);
  };

// src/ui/v2/shell.ts:1674-1736 slice of src/ui/v2/shell.ts#mountShell.renderBuild (shared): the feature's statements; "…" marks code left out
  function renderBuild(): void {
    // … 3 lines
    const selected = state.selection !== null;
    // … 2 lines
    const createTabs: BuilderCategoryId[] = ['models', 'draw'];
    const editTabs: BuilderCategoryId[] = ['mass', 'facade', 'roof', 'parts', 'interior', 'paint'];
    const visible = selected ? [...createTabs, ...editTabs] : [...createTabs, 'paint' as const];
    for (const id of visible) {
      const label = id === 'mass' ? t('v2.build.shape') : t(`builder.category.${id}`);
      const icon = ({ models: 'models', draw: 'draw', mass: 'mass', facade: 'face', roof: 'roof', parts: 'components', interior: 'interiorView', paint: 'paint' } as Record<string, string>)[id] ?? 'models';
      tabs.appendChild(tab(label, state.category === id, () => actions.setCategory(id), false, builderIconSvg(icon, 18)));
    }
    if (!selected) tabs.title = t('v2.build.selectToEdit');
    // … 2 lines
    if (selected) {
      const floors = Array.from({ length: Math.max(1, state.floor.total) }, (_, i) => ({ value: String(i), label: `${t('v2.build.floor')} ${i + 1}` }));
      tools.appendChild(dropdown(t('builder.floor.title'), floors, String(state.floor.active), (v) => actions.setFloor(Number(v))));
    }
    const snap = dropdown(t('builder.snap.label'), SNAP_MODES.map((s) => ({ value: s, label: t(`builder.snap.${s}`) })), state.snap,
      (v) => actions.setSnap(v), `${t('builder.snap.label')}: `);
    const grid = button('v2-icon' + (state.grid ? ' on' : ''), t('builder.grid'), () => actions.toggleGrid(), svg('grid', 17));
    const hide = button('v2-icon' + (state.hideOthers ? ' on' : ''), t('builder.hideOthers'), () => actions.toggleHideOthers(), svg('hide', 17));
    tools.append(snap, grid, hide);
    if (selected) tools.appendChild(button('v2-icon', t('builder.view.frame'), () => actions.view('frame'), svg('frame', 17)));
    // … 1 lines
    if (state.planning) {
      options.appendChild(orow(t('v2.build.plan'), choices([
        { label: t('builder.plan.finish'), on: true, run: () => actions.planFinish(), disabled: state.planPoints < 3, icon: svg('check', 18) },
        { label: t('builder.plan.back'), on: false, run: () => actions.planBack(), icon: svg('undo', 18) },
        { label: t('builder.plan.cancel'), on: false, run: () => actions.planCancel(), icon: svg('close', 18) },
      ])));
    }
    // … 1 lines
    const spec = BUILDER_TAB_SPECS.find((s) => s.id === state.category);
    if (!spec) return;
    if (spec.needsSelection && !selected) {
      note(t('builder.needsSelection'));
      return;
    }
    const sections = spec.sections.filter((s) => !s.advanced || advanced);
    // … 1 lines
    for (const s of sections) {
      for (const x of s.tools ?? []) wantPictures.push(x.id);
      if (s.shelf === 'models') wantPictures.push(...CITY_BUILDINGS.map((c) => `city:${c.fn}`), ...BLUEPRINTS.map((b) => b.key), ...state.userBlueprints.map((b) => b.key), ...LOT_NAMES.map(lotKey));
      if (s.shelf === 'patterns') wantPictures.push(...FACADE_PATTERNS);
    }
    // … 1 lines
    for (const s of sections) renderSection(s, state);
    if (spec.sections.some((s) => s.advanced)) {
      const count = spec.sections.filter((s) => s.advanced).reduce((n, s) => n + (s.tools?.length ?? 0), 0);
      const adv = group(advanced ? t('builder.advanced.on') : `${t('builder.advanced.off')} (${count})`);
      adv.appendChild(choices([{ label: advanced ? t('builder.advanced.on') : `${t('builder.advanced.off')} (${count})`, on: advanced, icon: svg('advanced', 18), run: () => {
        advanced = !advanced;
        try { window.localStorage.setItem('roadcraft.builder.advanced', advanced ? '1' : '0'); } catch { /* not kept */ }
        render();
      } }]));
      options.appendChild(adv);
    }
  }

// src/ui/v2/shell.ts:1738-1797 slice of src/ui/v2/shell.ts#mountShell.renderSection (shared): the feature's statements; "…" marks code left out
  function renderSection(s: BuilderSection, state: BuilderState): void {
    const label = s.title ? t(`builder.section.${s.title}`) : '';
    if (s.shelf === 'models') return renderModels(state);
    // … 1 lines
    if (s.shelf === 'scope') {
      options.appendChild(orow(label, choices(FACADE_SCOPES.map((scope) => ({ label: t(`creator.dock.scope.${scope}`), on: state.scope === scope, run: () => actions.setScope(scope), icon: svg(`scope_${scope}`, 18) })))));
      return;
    }
    if (s.shelf === 'drawAction') {
      const drawIcon = { new: 'addVolume', ground: 'wing', top: 'stack', cut: 'cut' } as const;
      // Its name is a sentence: a word at the left, the sentence in the tooltip (it was cut to "A forma d...").
      options.appendChild(namedRow(t('builder.section.drawWhere.short'), label, choices(DRAW_ACTIONS.map((a) => ({ label: t(`builder.drawAction.${a}`), on: state.drawAction === a, run: () => actions.setDrawAction(a), disabled: a !== 'new' && !state.selection, icon: builderIconSvg(drawIcon[a], 18) })))));
      return;
    }
    if (s.shelf === 'roofParams') {
      const roof = state.roof;
      // The Builder republishes its state after the change; the panel follows it.
      options.appendChild(orow(t('builder.field.pitch'), stepper(t('builder.field.pitch'), `${roof?.pitch ?? 30}°`, (d) => actions.roofPitch(d * 5))));
      if (roof?.pitched) {
        options.appendChild(orow(t('builder.roof.ridge.label'), choices((['x', 'y'] as const).map((r) => ({ label: t(`builder.roof.ridge.${r}`), on: roof.ridge === r, run: () => actions.roofRidge(r), icon: svg(`ridge_${r}`, 18) })))));
        options.appendChild(orow(t('builder.roof.side.label'), choices(([[0, 'front'], [1, 'right'], [2, 'back'], [3, 'left']] as const).map(([side, key]) => ({ label: t(`builder.roof.side.${key}`), on: roof.fall === side, run: () => actions.roofFall(side), icon: svg(`fall_${key}`, 18) })))));
      }
      return;
    }
    if (s.shelf === 'patterns') {
      const { items } = section(label);
      // Until its picture comes (`requestThumbnails`), a pattern shows the facade glyph
      for (const p of FACADE_PATTERNS) items.appendChild(card(t(`creator.pattern.${p}`), state.pattern === p, () => actions.choosePattern(p), workspace.thumbnail(p), builderIconSvg('face', 30)));
      return;
    }
    if (s.shelf === 'finishes') {
      const { items } = section(t('v2.build.material'));
      for (const f of FINISHES) items.appendChild(card(t(`building.finish.${f}`), state.material?.finish === f, () => actions.chooseFinish(f), materialSwatch(f), undefined, 'square'));
      const sw = el('div', 'v2-swatches');
      for (const c of SWATCHES) {
        const b = el('button', 'v2-swatch' + (state.material?.colour === c ? ' on' : ''));
        b.type = 'button';
        b.style.setProperty('--c', hexOf(c));
        b.setAttribute('aria-label', hexOf(c));
        b.onclick = () => actions.chooseColour(c);
        sw.appendChild(b);
      }
      const custom = el('input', 'v2-swatch custom');
      custom.type = 'color';
      custom.onchange = () => actions.chooseColour(parseInt(custom.value.slice(1), 16));
      sw.appendChild(custom);
      options.appendChild(orow(t('v2.build.colour'), sw));
      const st = section(t('builder.section.styles'));
      for (const style of STYLES) {
        const chips = [style.materials.wall.colour, style.materials.trim.colour, style.materials.roof.colour].map((c) => `<i style="background:${hexOf(c)}"></i>`).join('');
        st.items.appendChild(card(t(`building.style.${style.key}`), false, () => actions.chooseStyle(style.key), undefined, `<span class="v2-style">${chips}</span>`, 'square'));
      }
      return;
    }
    const list = s.tools ?? [];
    if (!list.length) return;
    const things = list.some((x) => THINGS.has(x.id) || DRAW_SHAPES[x.id] || TIER_SHAPES[x.id]);
    const { items } = section(label || title.textContent || '', things ? 'cards' : 'verbs');
    for (const x of list) items.appendChild(toolButton(x, state, things));
  }

// src/ui/v2/shell.ts:1799-1807 slice of src/ui/v2/shell.ts#mountShell.toolButton (shared): the feature's statements; "…" marks code left out
  function toolButton(x: BuilderToolSpec, state: BuilderState, asCard: boolean): HTMLButtonElement {
    const on = x.kind === 'mode' && (state.tool === x.id || state.armed === x.id);
    const label = t(`builder.tool.${x.id}`);
    // … 1 lines
    const b = asCard ? card(label, on, run, picture(x.id), builderIconSvg(x.id, 32)) : verb(label, on, run, builderIconSvg(x.id, 18), x.danger);
    b.disabled = !state.ready.has(x.id);
    b.dataset['builderTool'] = x.id;
    return b;
  }

// src/ui/v2/shell.ts:1802-1802 slice of src/ui/v2/shell.ts#mountShell.toolButton.run (shared): the feature's statements; "…" marks code left out
    const run = (): void => actions.chooseTool(x.id);

// src/ui/v2/shell.ts:1809-1853 slice of src/ui/v2/shell.ts#mountShell.renderModels (shared): the feature's statements; "…" marks code left out
  function renderModels(state: BuilderState): void {
    // … 7 lines
    const cats = ['all', 'homes', 'public', 'commerce', 'work', 'leisure', 'generic', 'lots', ...(state.userBlueprints.length ? ['mine'] : [])];
    // … 1 lines
    const kind = dropdown(t('builder.category.models'), cats.map((c) => ({
      value: c, label: c === 'all' ? t('builder.city.all') : c === 'mine' ? t('builder.city.mine') : t(`builder.city.${c}`),
    })), modelCat, (v) => { modelCat = v; render(); });
    g.appendChild(kind);
    // … 5 lines
    for (const m of CITY_BUILDINGS) {
      const key = `city:${m.fn}`;
      const label = t(`building.fn.${m.fn}`);
      names.add(label.toLocaleLowerCase());
      entries.push({ key, label, cat: m.category, run: () => actions.choosePreset(key) });
    }
    for (const bp of BLUEPRINTS) {
      const label = bp.nameKey ? t(bp.nameKey) : bp.key;
      if (!names.has(label.toLocaleLowerCase())) entries.push({ key: bp.key, label, cat: 'generic', run: () => actions.choosePreset(bp.key) });
    }
    // … 1 lines
    for (const name of LOT_NAMES) {
      const key = lotKey(name);
      entries.push({ key, label: t(`builder.lot.${name}`), cat: 'lots', run: () => actions.choosePreset(key) });
    }
    for (const bp of state.userBlueprints) entries.push({ key: bp.key, label: bp.name ?? bp.key, cat: 'mine', run: () => actions.chooseUserBlueprint(bp.key) });
  // … 9 lines
  }

// src/ui/v2/shell.ts:1844-1850 slice of src/ui/v2/shell.ts#mountShell.renderModels.fill (shared): the feature's statements; "…" marks code left out
    const fill = (): void => {
      // … 3 lines
      for (const e of shown) items.appendChild(card(e.label, false, e.run, workspace.thumbnail(e.key), builderIconSvg('models', 32), 'model'));
    // … 1 lines
    };

// src/ui/v2/shell.ts:1857-1915 slice of src/ui/v2/shell.ts#mountShell.render (shared): the feature's statements; "…" marks code left out
  function render(): void {
    // … 47 lines
    if (lastSignature === `${cat}|${current}|${builder?.category ?? ''}`) strip.scrollLeft = scroll;
    lastSignature = `${cat}|${current}|${builder?.category ?? ''}`;
  // … 8 lines
  }

// src/ui/v2/shell.ts:1953-1956 slice of src/ui/v2/shell.ts#mountShell.updateHint (shared): the feature's statements; "…" marks code left out
  function updateHint(): void {
    const text = tool() === 'building' ? builder?.hint ?? '' : gameHintText;
    if (hint.textContent !== text) hint.textContent = text;
  }

// src/ui/v2/shell.ts:2040-2041 slice of src/ui/v2/shell.ts#mountShell.sigOf (shared): the feature's statements; "…" marks code left out
  const sigOf = (s: BuilderState): string => JSON.stringify([s.category, s.tool, s.armed, readyOf(s.ready), s.floor, s.snap, s.grid, s.hideOthers,
    s.selection, s.planning, s.planPoints, s.userBlueprints.map((b) => b.key), s.pattern, s.scope, s.roof, s.material, s.drawAction, s.inspectorOpen]);
