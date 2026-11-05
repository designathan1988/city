// The code of src/buildingsWiring.ts that is the feature's (the rest of the file is shared).

// src/buildingsWiring.ts:31-47 slice of src/buildingsWiring.ts#BuildingWiringDeps (shared): the feature's statements; "…" marks code left out
export interface BuildingWiringDeps {
  // … 2 lines
  readonly history: History;
  // … 1 lines
  view(): Viewport;
// … 10 lines
}

// src/buildingsWiring.ts:81-1216 slice of src/buildingsWiring.ts#createBuildingWiring (shared): the feature's statements; "…" marks code left out
export function createBuildingWiring(deps: BuildingWiringDeps): BuildingWiring {
  // … 4 lines
  const view: ToolView = {
    // … 1 lines
    project: (x, y, z) => { const size = deps.size(); return deps.view().toScreen({ x, y }, size.w, size.h, z); },
    planeAt: (s, z) => { const size = deps.size(); return deps.view().toWorldAt(s.x, s.y, z, size.w, size.h); },
    ray(s) {
      // … 2 lines
      const sight = deps.view().rayAt?.(s.x, s.y, w, h);
      if (sight) return sight;
      // … 1 lines
      const low = deps.view().toWorldAt(s.x, s.y, 0, w, h);
      const high = deps.view().toWorldAt(s.x, s.y, 100, w, h);
      const dx = low.x - high.x;
      const dy = low.y - high.y;
      // … 1 lines
      const len = Math.hypot(dx, dy, dz);
      // … 1 lines
      return {
        ox: high.x - dx * back,
        oy: high.y - dy * back,
        // … 1 lines
        dx: dx / len,
        dy: dy / len,
        dz: dz / len,
      };
    },
  // … 4 lines
  };
  // … 23 lines
  const host: ToolHost = {
    // … 1 lines
    commit(edit: () => EditResult): EditResult {
      history.begin(doc);
      // … 1 lines
      history.end(doc);
    // … 4 lines
    },
  // … 17 lines
  };
  // … 327 lines
  const actions: BuilderActions = {
    // … 52 lines
    selectionAction: (name) => {
      // … 1 lines
      switch (name) {
        // … 28 lines
        case 'save': {
          // … 1 lines
          const name = window.prompt(t('building.blueprintName'), '');
          if (name !== null && name.trim() !== '') {
            // A save that did not happen is told, not left for the player to find missing later.
            if (library.save(name, bodyOf(building))) {
              userBlueprints = library.list();
              notify('building.blueprintSaved');
            } else notify('building.blueprintNotSaved');
          }
        // … 1 lines
        }
      // … 3 lines
      }
    // … 1 lines
    },
    // … 20 lines
    chooseUserBlueprint(key) {
      const blueprint = userBlueprints.find((item) => item.key === key);
      // … 2 lines
      if (blueprint) tool.useBody(blueprint.body, key);
    // … 1 lines
    },
    // … 97 lines
    view: (id) => {
      // … 1 lines
      switch (id) {
        // … 6 lines
        case 'top':
          // … 1 lines
          deps.view().setOrbit(deps.view().azimuth, Math.PI / 2);
          break;
        case 'turnLeft':
          deps.view().rotate(-1, w / 2, h / 2, w, h);
          break;
        case 'turnRight':
          deps.view().rotate(1, w / 2, h / 2, w, h);
          break;
      // … 2 lines
      }
    // … 1 lines
    },
  };
// … 469 lines
}

// src/buildingsWiring.ts:227-243 slice of src/buildingsWiring.ts#createBuildingWiring.loadReference (shared): the feature's statements; "…" marks code left out
  const loadReference = (): void => {
    // … 3 lines
    input.onchange = () => {
      // … 2 lines
      reference.load(file).then((info) => {
        // … 2 lines
        deps.flash('hint.reference.loaded', {
          name: info.name, w: formatDecimal(info.size[0], 1), d: formatDecimal(info.size[1], 1), h: formatDecimal(info.size[2], 1),
        });
      }).catch(() => deps.flash('hint.reference.failed'));
    };
  // … 1 lines
  };

// src/buildingsWiring.ts:807-901 slice of src/buildingsWiring.ts#createBuildingWiring.inspectorFields (shared): the feature's statements; "…" marks code left out
  function inspectorFields(): BuilderField[] {
    // … 14 lines
    if (volume) {
      // … 8 lines
      if (!tool.selection?.bay && !tool.selectedElement() && tool.selectedRoofDetail === null) {
        // … 15 lines
        fields.push({ id: 'mode', labelKey: 'builder.field.blockMode', value: 0, text: t(`builder.blockMode.${volume.mode ?? 'solid'}`) });
      }
      fields.push({ id: 'area', labelKey: 'builder.field.area', value: area, unit: 'm', text: `${formatDecimal(area, 1)} m²` });
    // … 4 lines
    }
  // … 46 lines
  }

// src/buildingsWiring.ts:904-913 slice of src/buildingsWiring.ts#createBuildingWiring.selectionName (shared): the feature's statements; "…" marks code left out
  function selectionName(): string {
    // … 3 lines
    if (tool.selectedElement()) return t('builder.sel.element');
    if (tool.selection?.bay && volume) return `${t('builder.sel.face')} · ${t('builder.sel.floor', { n: volume.base + tool.selection.bay.storey + 1 })}`;
    if (tool.selectedRoofDetail !== null) return t('builder.sel.detail');
    if (volume) return t('builder.sel.volume', { n: volume.id });
    return t('builder.sel.building');
  }

// src/buildingsWiring.ts:915-918 slice of src/buildingsWiring.ts#createBuildingWiring.materialLine (shared): the feature's statements; "…" marks code left out
  const materialLine = (): string | undefined => {
    // … 1 lines
    return material ? `${t(`building.finish.${material.finish}`)} · #${material.colour.toString(16).padStart(6, '0')}` : undefined;
  };

// src/buildingsWiring.ts:920-967 slice of src/buildingsWiring.ts#createBuildingWiring.state (shared): the feature's statements; "…" marks code left out
  const state = (): BuilderState => {
    // … 4 lines
    return {
      // … 12 lines
      canUndo: history.canUndo,
      canRedo: history.canRedo,
      // … 10 lines
      hint: t(`hint.builder.${hintKey()}`),
    // … 15 lines
    };
  };

// src/buildingsWiring.ts:1120-1146 slice of src/buildingsWiring.ts#createBuildingWiring.beforeDraw (shared): the feature's statements; "…" marks code left out
    beforeDraw(active) {
      // … 12 lines
      const c = deps.view().toWorld(w / 2, h / 2, w, h);
      const ahead = deps.view().toWorld(w / 2, h / 2 - 100, w, h);
      const len = Math.hypot(ahead.x - c.x, ahead.y - c.y) || 1;
      tool.cutView = { x: (ahead.x - c.x) / len, y: (ahead.y - c.y) / len };
      // … 1 lines
      if (target) {
        scene.setBuildingCutaway({
          // … 1 lines
          view: { x: (ahead.x - c.x) / len, y: (ahead.y - c.y) / len },
        });
      } else scene.setBuildingCutaway(null);
    // … 2 lines
    },

// src/buildingsWiring.ts:1147-1186 slice of src/buildingsWiring.ts#createBuildingWiring.drawOverlay (shared): the feature's statements; "…" marks code left out
    drawOverlay(ctx) {
      // … 6 lines
      if (preview) {
        // … 2 lines
        const size = `${formatDecimal((f.x1 - f.x0) * METERS_PER_UNIT, 1)} × ${formatDecimal((f.y1 - f.y0) * METERS_PER_UNIT, 1)} m`;
        const text = preview.problem
          ? t(`building.problem.${preview.problem}`)
          : `${plural('building.floors', floors)} · ${size}`;
        label = { text, valid: preview.valid, building: preview.building, floor: tool.floorOf(preview.building) };
      }
      drawBuildingOverlay(ctx, {
        // … 10 lines
        measure: tool.measure
          ? {
            text: tool.measure.kind === 'floors'
              ? plural('building.floors', tool.measure.value)
              : `${formatDecimal(tool.measure.value * METERS_PER_UNIT, tool.measure.kind === 'depth' ? 2 : 1)} m`,
          // … 3 lines
          }
          : null,
      });
    // … 1 lines
    },
