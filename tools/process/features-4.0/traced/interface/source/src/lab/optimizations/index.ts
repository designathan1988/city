// The code of src/lab/optimizations/index.ts that is the feature's (the rest of the file is shared).

// src/lab/optimizations/index.ts:23-32 slice of src/lab/optimizations/index.ts#rowOf (shared): the feature's statements; "…" marks code left out
const rowOf = (state: OptimizationState): OptRow => ({
  id: state.spec.id,
  system: state.spec.system,
  title: state.spec.title,
  // … 1 lines
  default: state.spec.choices[0]!.value,
  choices: state.spec.choices.map((c) => c.value),
  range: state.spec.range ?? null,
  affects: state.spec.affects,
});

// src/lab/optimizations/index.ts:34-222 slice of src/lab/optimizations/index.ts#createOptimizationTabs (shared): the feature's statements; "…" marks code left out
export function createOptimizationTabs(ctx: LabContext): LabTab[] {
  // … 7 lines
  (window as unknown as { __labOpt: unknown }).__labOpt = {
    // … 5 lines
    affecting: (target: OptTarget, withAll = false): OptRow[] => control.list().filter((s) => affectsTarget(s.spec, target, withAll)).map(rowOf),
  };
// … 172 lines
}

// src/lab/optimizations/index.ts:62-93 slice of src/lab/optimizations/index.ts#createOptimizationTabs.drawList (shared): the feature's statements; "…" marks code left out
  const drawList = (): void => {
    // … 4 lines
    summary.textContent = active.length ? `${active.length} active: ${active.map((s) => s.spec.title).join(', ')}` : 'None active: the game as it draws today.';
    // … 1 lines
    const shown = states.filter((s) => (!filter.system || s.spec.system === filter.system) && (!filter.changedOnly || !s.isDefault)
      && (!filter.tag || s.spec.affects.includes(filter.tag))
      && (!text || `${s.spec.title} ${s.spec.what} ${s.spec.tradeoff} ${s.spec.id} ${OPT_SYSTEM_TITLES[s.spec.system]} ${s.spec.affects.join(' ')}`.toLowerCase().includes(text)));
    // … 1 lines
    for (const system of Object.keys(OPT_SYSTEM_TITLES) as OptSystem[]) {
      const of = shown.filter((s) => s.spec.system === system);
      if (!of.length) continue;
      // … 1 lines
      h.textContent = `${OPT_SYSTEM_TITLES[system]} (${of.length})`;
      // … 1 lines
      for (const state of of) sections.push(rowElement(state));
    }
  // … 12 lines
  };

// src/lab/optimizations/index.ts:97-162 slice of src/lab/optimizations/index.ts#createOptimizationTabs.rowElement (shared): the feature's statements; "…" marks code left out
  const rowElement = (state: OptimizationState): HTMLElement => {
    // … 3 lines
    row.dataset['opt'] = spec.id;
    // … 3 lines
    title.textContent = spec.title;
    title.title = spec.id;
    // … 5 lines
    for (const choice of spec.choices) {
      const b = document.createElement('button');
      b.type = 'button';
      b.textContent = choice.label;
      const on = choice.value === state.value;
      b.classList.toggle('on', on);
      b.setAttribute('aria-pressed', String(on));
      b.onclick = () => set(spec.id, choice.value);
      choices.appendChild(b);
    }
    if (spec.range) {
      // Any number in its range: a slider, and a field for an exact value.
      const { min, max, step } = spec.range;
      const shown = typeof state.value === 'number' ? state.value : null;
      const slider = document.createElement('input');
      slider.type = 'range';
      slider.min = String(min); slider.max = String(max); slider.step = String(step);
      slider.value = String(shown ?? min);
      slider.className = 'opt-slider';
      const field = document.createElement('input');
      field.type = 'number';
      field.min = String(min); field.max = String(max); field.step = 'any';
      field.value = shown === null ? '' : String(shown);
      field.placeholder = 'value';
      field.className = 'opt-number';
      slider.oninput = () => { field.value = slider.value; };
      slider.onchange = () => set(spec.id, Number(slider.value));
      field.onchange = () => { if (field.value.trim() !== '') set(spec.id, Number(field.value)); };
      choices.append(slider, field);
    }
    // … 3 lines
    what.textContent = spec.what;
    // … 2 lines
    tradeoff.textContent = `Expected gain and cost: ${spec.tradeoff}`;
    // … 2 lines
    for (const tag of spec.affects) {
      const chip = document.createElement('button');
      chip.type = 'button';
      chip.className = `opt-tag${filter.tag === tag ? ' on' : ''}`;
      chip.textContent = tag;
      chip.title = 'Show only the options that affect this';
      chip.onclick = () => { filter.tag = filter.tag === tag ? '' : tag; drawList(); };
      affects.appendChild(chip);
    }
  // … 2 lines
  };
