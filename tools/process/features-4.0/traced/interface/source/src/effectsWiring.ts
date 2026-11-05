// The code of src/effectsWiring.ts that is the feature's (the rest of the file is shared).

// src/effectsWiring.ts:16-20 slice of src/effectsWiring.ts#BUILDING_CHOICES (shared): the feature's statements; "…" marks code left out
const BUILDING_CHOICES: readonly { key: string; field: keyof BuildingChoices; values: readonly (string | number)[] }[] = [
  { key: 'bMaterial', field: 'material', values: ['normal', 'lambert', 'basic'] },
  { key: 'bTriangles', field: 'triangles', values: [1, 0.5, 0.25, 0.1, 0.02, 0] },
  { key: 'bDetailMemory', field: 'detailMemoryMB', values: [-1, 1024, 512, 256, 128, 32, 0] },
];

// src/effectsWiring.ts:28-33 slice of src/effectsWiring.ts#Kept (shared): the feature's statements; "…" marks code left out
interface Kept {
  readonly effects: EffectSwitches;
  // … 2 lines
  readonly buildings: BuildingChoices;
}

// src/effectsWiring.ts:35-47 slice of src/effectsWiring.ts#readKept (shared): the feature's statements; "…" marks code left out
function readKept(): Kept {
  const kept = { effects: { ...ALL_EFFECTS_ON }, cpu: { ...ALL_CPU_ON }, resolution: 1, buildings: { ...DEFAULT_BUILDING_CHOICES } as Record<keyof BuildingChoices, unknown> };
  try {
    // … 2 lines
    for (const [key, on] of Object.entries(raw.effects ?? {})) if (isEffectSwitch(key) && typeof on === 'boolean') kept.effects[key] = on;
  // … 2 lines
  } catch (error) {
  // … 1 lines
  }
  return { ...kept, buildings: kept.buildings as unknown as BuildingChoices };
}

// src/effectsWiring.ts:49-61 slice of src/effectsWiring.ts#readOptimizations (shared): the feature's statements; "…" marks code left out
function readOptimizations(): Optimizations {
  const kept: Record<string, OptValue> = { ...DEFAULT_OPTIMIZATIONS };
  try {
    // … 1 lines
    for (const [id, value] of Object.entries(raw)) {
      const v = optimizationChoice(id, value);
      if (v !== undefined) kept[id] = v;
    }
  } catch (error) {
  // … 1 lines
  }
  return kept as Optimizations;
}

// src/effectsWiring.ts:64-68 slice of src/effectsWiring.ts#OptimizationState (shared): the feature's statements; "…" marks code left out
export interface OptimizationState {
  readonly spec: OptimizationSpec;
// … 2 lines
}

// src/effectsWiring.ts:71-81 slice of src/effectsWiring.ts#OptimizationControl (shared): the feature's statements; "…" marks code left out
export interface OptimizationControl {
  // … 6 lines
  values(): Optimizations;
  // … 1 lines
  onChange(listener: (id: OptId, value: OptValue) => void): () => void;
}

// src/effectsWiring.ts:83-106 slice of src/effectsWiring.ts#EffectsWiring (shared): the feature's statements; "…" marks code left out
export interface EffectsWiring {
  // … 2 lines
  readonly panel: EffectsPanel;
// … 19 lines
}

// src/effectsWiring.ts:108-303 slice of src/effectsWiring.ts#createEffectsWiring (shared): the feature's statements; "…" marks code left out
export function createEffectsWiring(): EffectsWiring {
  // … 2 lines
  const optListeners: ((id: OptId, value: OptValue) => void)[] = [];
  // … 35 lines
  const optimizationControl: OptimizationControl = {
    list: () => OPTIMIZATIONS.map((spec) => ({ spec, value: optimizations[spec.id], isDefault: optimizations[spec.id] === spec.choices[0]!.value })),
    get: (id) => (optimizationSpec(id) ? optimizations[id as OptId] : undefined),
    set(id, value) {
      const v = optimizationChoice(id, value);
      if (v === undefined) return false;
      setOptimizations({ [id]: v });
    // … 1 lines
    },
    reset: () => setOptimizations(DEFAULT_OPTIMIZATIONS),
    // … 1 lines
    onChange(listener) {
      optListeners.push(listener);
      return () => { const i = optListeners.indexOf(listener); if (i >= 0) optListeners.splice(i, 1); };
    },
  };
  // … 20 lines
  const GROUPS: readonly { id: string; keys: readonly (EffectSwitch | CpuSwitch)[] }[] = [
    { id: 'post', keys: POST_SWITCHES },
    { id: 'light', keys: LIGHT_SWITCHES },
    { id: 'layers', keys: LAYER_SWITCHES },
    { id: 'buildings', keys: BUILDING_SWITCHES },
    { id: 'cpu', keys: CPU_SWITCHES },
  ];
  // … 31 lines
  const panel = createEffectsPanel({
    groups: (): EffectsPanelGroup[] => GROUPS.map((group) => ({
      id: group.id,
      title: t(`fx.group.${group.id}`),
      items: [
        ...(group.id === 'buildings' ? BUILDING_CHOICES.map(({ key, field, values }) => ({
          key, label: t(`fx.${key}`), on: true, value: String(buildings[field]),
          choices: values.map((v) => [String(v), t(`fx.${key}.${v}`)] as const),
        })) : []),
        ...group.keys.map((key) => ({ key, label: t(`fx.${key}`), on: isOn(key) })),
      ],
    })),
    toggle: (key, on) => set([key], on),
    choose(key, value) {
      const choice = BUILDING_CHOICES.find((c) => c.key === key);
      if (!choice) return;
      const v = choice.values.find((x) => String(x) === value);
      if (v === undefined) return;
      buildings = { ...buildings, [choice.field]: v };
      save();
      apply();
      told(key, v);
    },
    setGroup: (id, on) => set(GROUPS.find((group) => group.id === id)?.keys ?? [], on),
    reset() {
      resolution = 1;
      buildings = DEFAULT_BUILDING_CHOICES;
      set([...GROUPS.flatMap((group) => group.keys)], true);
    },
    resolution: () => resolution,
    setResolution(scale) {
      resolution = scale;
      save();
      apply();
      told('resolution', scale);
    },
    stats,
  });
  // … 1 lines
  return {
    // … 8 lines
    panel,
  // … 32 lines
  };
}

// src/effectsWiring.ts:127-134 slice of src/effectsWiring.ts#createEffectsWiring.saveOptimizations (shared): the feature's statements; "…" marks code left out
  const saveOptimizations = (): void => {
    const changed = Object.fromEntries(Object.entries(optimizations).filter(([id, v]) => v !== DEFAULT_OPTIMIZATIONS[id as OptId]));
    try {
      window.localStorage.setItem(OPT_STORE_KEY, JSON.stringify(changed));
    } catch (error) {
    // … 1 lines
    }
  };

// src/effectsWiring.ts:135-146 slice of src/effectsWiring.ts#createEffectsWiring.setOptimizations (shared): the feature's statements; "…" marks code left out
  const setOptimizations = (patch: Readonly<Record<string, OptValue>>): void => {
    const changed = Object.entries(patch).filter(([id, v]) => optimizations[id as OptId] !== v);
    if (!changed.length) return;
    // … 4 lines
    for (const [id, v] of changed) {
      told(`opt.${id}`, v);
      for (const listener of optListeners) listener(id as OptId, v);
    }
  };

// src/effectsWiring.ts:169-181 slice of src/effectsWiring.ts#createEffectsWiring.set (shared): the feature's statements; "…" marks code left out
  const set = (keys: readonly string[], on: boolean): void => {
    // … 2 lines
    for (const key of keys) {
      if (isEffectSwitch(key)) nextEffects[key] = on;
      else if ((CPU_SWITCHES as readonly string[]).includes(key)) nextCpu[key as CpuSwitch] = on;
    }
  // … 5 lines
  };

// src/effectsWiring.ts:190-190 slice of src/effectsWiring.ts#createEffectsWiring.isOn (shared): the feature's statements; "…" marks code left out
  const isOn = (key: EffectSwitch | CpuSwitch): boolean => (isEffectSwitch(key) ? effects[key] : cpu[key as CpuSwitch]);

// src/effectsWiring.ts:193-219 slice of src/effectsWiring.ts#createEffectsWiring.stats (shared): the feature's statements; "…" marks code left out
  const stats = (): (readonly [string, string])[] => {
    // … 11 lines
    return [
      [t('fx.stat.gpu'), scene.gpu.supported ? (gpuCount ? `${(gpuSum / gpuCount).toFixed(2)} ms` : '-') : t('fx.stat.noTimer')],
      [t('fx.stat.cpu'), cpuShown ? `${(cpuSum / cpuShown).toFixed(2)} ms` : '-'],
      [t('fx.stat.fps'), scene.stats.fps.toFixed(0)],
      [t('fx.stat.calls'), String(scene.stats.drawCalls)],
      [t('fx.stat.triangles'), scene.stats.drawnTriangles.toLocaleString()],
      [t('fx.stat.programs'), String(gl.info.programs?.length ?? 0)],
      [t('fx.stat.geometries'), String(gl.info.memory.geometries)],
      [t('fx.stat.textures'), String(gl.info.memory.textures)],
      [t('fx.stat.targets'), mb(scene.targetBytes())],
      [t('fx.stat.buildings'), mb(scene.buildingDetailBytes())],
      [t('fx.stat.heap'), heap ? `${mb(heap.usedJSHeapSize)} / ${mb(heap.totalJSHeapSize)}` : '-'],
      [t('fx.stat.pixels'), `${canvas.width} × ${canvas.height}`],
    ];
  };

// src/effectsWiring.ts:261-268 slice of src/effectsWiring.ts#createEffectsWiring.attach (shared): the feature's statements; "…" marks code left out
    attach(nextScene, nextFullRatio, nextRedraw) {
      // … 5 lines
      if (optimizations !== DEFAULT_OPTIMIZATIONS && Object.entries(optimizations).some(([id, v]) => v !== DEFAULT_OPTIMIZATIONS[id as OptId])) scene.setOptimizations(optimizations);
    },
