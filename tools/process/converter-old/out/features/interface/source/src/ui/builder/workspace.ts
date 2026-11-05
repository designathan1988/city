// The code of src/ui/builder/workspace.ts that is the feature's (the rest of the file is shared).

// src/ui/builder/workspace.ts:117-371 slice of src/ui/builder/workspace.ts#initBuilderWorkspace (shared): the feature's statements; "…" marks code left out
export function initBuilderWorkspace(actions: BuilderActions): BuilderWorkspace {
  // … 34 lines
  applyTranslations(root);
  onLanguageChange(() => applyTranslations(root));
// … 217 lines
}

// src/ui/builder/workspace.ts:170-319 slice of src/ui/builder/workspace.ts#initBuilderWorkspace.renderInspector (shared): the feature's statements; "…" marks code left out
  function renderInspector(state: BuilderState): void {
    // … 8 lines
    if (titledIn !== language()) {
      titledIn = language();
      for (const [name, b] of actionButtons) {
        const key = `builder.quick.${name}`;
        b.title = t(key);
        b.setAttribute('aria-label', t(key));
      }
    }
    // … 5 lines
    const structure = `${language()}|${info.titleKey}|${info.material !== undefined}|${info.fields.map((f) => `${f.id}:${f.labelKey}:${f.text !== undefined ? 'text' : f.options ? f.options.map((o) => `${o.value}=${o.labelKey}`).join('/') : `${f.unit}/${f.min}/${f.max}/${f.step}`}`).join(',')}`;
    if (inspectorBody.dataset['structure'] === structure) {
      inspectorBody.dataset['signature'] = signature;
      if (nameShows) nameShows.textContent = info.name;
      if (materialShows && info.material !== undefined) materialShows.textContent = info.material;
      for (const field of info.fields) {
        liveFields.set(field.id, field);
        const shows = fieldShows.get(field.id);
        if (!shows) continue;
        if (shows instanceof HTMLSelectElement) {
          for (const o of shows.options) o.selected = o.value === String(field.value);
        } else if (shows instanceof HTMLInputElement) {
          if (shows !== focused) shows.value = field.unit === 'count' ? String(Math.round(field.value)) : formatDecimal(field.value, 2);
        } else shows.textContent = field.text ?? '';
      }
      return;
    }
    inspectorBody.dataset['structure'] = structure;
    // … 7 lines
    inspectorTitle.textContent = t(info.titleKey);
    // … 4 lines
    for (const field of info.fields) {
      // … 5 lines
      label.textContent = t(field.labelKey);
      // … 9 lines
      if (field.options) {
        // … 2 lines
        for (const option of field.options) {
          // … 2 lines
          o.textContent = t(option.labelKey);
        // … 2 lines
        }
      // … 5 lines
      }
      // … 7 lines
      const shown = (v: number): string => (field.unit === 'count' ? String(Math.round(v)) : formatDecimal(v, 2));
      // … 2 lines
      input.value = typed?.id === field.id ? typed.text : shown(field.value);
      // … 3 lines
      const nudge = (sign: number): void => {
        const typedNow = parseDecimal(input.value);
        const next = clamp(Math.round(((Number.isFinite(typedNow) ? typedNow : now().value) + sign * step) / step) * step);
        input.value = shown(next);
        actions.setField(field.id, next);
      };
      input.oninput = () => {
        const n = parseDecimal(input.value);
        if (Number.isFinite(n)) actions.setField(field.id, clamp(n));
      };
      input.onblur = () => {
        const n = parseDecimal(input.value);
        input.value = shown(Number.isFinite(n) ? clamp(n) : now().value);
      };
    // … 18 lines
    }
    if (info.material !== undefined) {
      // … 2 lines
      label.textContent = t('builder.field.material');
    // … 5 lines
    }
  // … 8 lines
  }

// src/ui/builder/workspace.ts:341-341 method initBuilderWorkspace.inside: the feature's own
    inside: () => insideNow,

// src/ui/builder/workspace.ts:342-344 method initBuilderWorkspace.onInside: the feature's own
    onInside(listen) {
      insideListeners.push(listen);
    },

// src/ui/builder/workspace.ts:365-368 method initBuilderWorkspace.subscribe: the feature's own
    subscribe(listen) {
      listeners.push(listen);
      if (lastState) listen(lastState);
    },

// src/ui/builder/workspace.ts:369-369 method initBuilderWorkspace.thumbnail: the feature's own
    thumbnail: (id) => thumbnails.get(id),
