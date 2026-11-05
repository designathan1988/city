// The code of src/graphicsWiring.ts that is the feature's (the rest of the file is shared).

// src/graphicsWiring.ts:12-12 slice of src/graphicsWiring.ts#QualityPreset (shared): the feature's statements; "…" marks code left out
export type QualityPreset = QualityLevel | 'auto';

// src/graphicsWiring.ts:13-13 slice of src/graphicsWiring.ts#isQualityPreset (shared): the feature's statements; "…" marks code left out
export const isQualityPreset = (value: unknown): value is QualityPreset => value === 'auto' || isQualityLevel(value);

// src/graphicsWiring.ts:85-98 slice of src/graphicsWiring.ts#settingsFor (shared): the feature's statements; "…" marks code left out
function settingsFor(level: QualityLevel, kept: KeptOptions): QualitySettings {
  const tier = QUALITY[level];
  const shadows = kept.shadows ?? (tier.shadows ? tier.shadowMapSize : 0);
  const ambientOcclusion = kept.ambientOcclusion ?? tier.ambientOcclusion;
  const smaa = kept.smaa ?? tier.smaa;
  return {
    ...tier,
    shadows: shadows > 0,
    shadowMapSize: shadows > 0 ? shadows : tier.shadowMapSize,
    ambientOcclusion,
    smaa,
    postProcessing: tier.postProcessing || ambientOcclusion || smaa,
  };
}

// src/graphicsWiring.ts:100-115 slice of src/graphicsWiring.ts#GraphicsWiring (shared): the feature's statements; "…" marks code left out
export interface GraphicsWiring {
  // … 1 lines
  readonly level: QualityLevel;
  // … 2 lines
  readonly settings: QualitySettings;
// … 9 lines
}

// src/graphicsWiring.ts:118-282 slice of src/graphicsWiring.ts#createGraphicsWiring (shared): the feature's statements; "…" marks code left out
export function createGraphicsWiring(onPreset: (preset: QualityPreset) => void, onLevel: (level: QualityLevel, by: 'auto' | 'player') => void = () => {}): GraphicsWiring {
  // … 6 lines
  let level: QualityLevel = preset === 'auto' ? hardware : preset;
  const governor = new QualityGovernor(level, hardware);
  // … 2 lines
  let settings = settingsFor(level, kept);
  // … 4 lines
  const resolution = new DynamicResolution();
  // … 34 lines
  const wiring: GraphicsWiring = {
    level,
    // … 1 lines
    settings,
    // … 7 lines
    setPreset(next) {
      // … 2 lines
      governor.set(level);
      onLevel(level, 'player');
      const { shadows: _s, ambientOcclusion: _a, smaa: _m, ...lasting } = kept;
      kept = lasting;
    // … 4 lines
    },
    // … 1 lines
    openPanel() {
      // … 1 lines
      if (!bridge) {
        panel.open();
      // … 1 lines
      }
      void bridge.state().then((state) => {
        desktopState = state;
        panel.open();
      }, () => panel.open());
    },
    frame() {
      // … 6 lines
      const fresh = scene.gpu.supported ? Math.min(ring.pushed - seenSamples, ring.size) : 0;
      // … 1 lines
      if (preset === 'auto' && delta > 0) {
        // … 1 lines
        let next: QualityLevel | null = null;
        if (scene.gpu.supported) for (let back = fresh - 1; back >= 0; back--) next = governor.sample(delta / Math.max(1, fresh), ring.newest(back), budget) ?? next;
        else next = governor.sample(delta, delta * 1000, budget * 1.1);
        if (next !== null && next !== level) {
          level = next;
          governor.set(level);
          onLevel(level, 'auto');
          apply();
          return;
        }
      }
      // … 3 lines
      for (let back = fresh - 1; back >= 0; back--) scale = resolution.sample(ring.newest(back), budget) ?? scale;
      if (scale !== null) applyScale(scale);
    },
  };
  // … 1 lines
  const panel = createGraphicsPanel({
    read: choices,
    desktop: () => desktopState !== null,
    gpu: () => scene?.gpu.label ?? gpuName(),
    change(patch) {
      if (patch.preset !== undefined && patch.preset !== preset) {
        wiring.setPreset(patch.preset);
        return;
      }
      if (patch.shadows !== undefined || patch.ambientOcclusion !== undefined || patch.smaa !== undefined) {
        kept = { ...kept, ...pick(patch) };
        save();
        apply();
      }
      if (patch.dynamicResolution !== undefined) {
        kept = { ...kept, dynamicResolution: patch.dynamicResolution };
        save();
        // Off: the whole ratio back at once.
        if (!patch.dynamicResolution) {
          resolution.reset();
          applyScale(1);
        }
        if (scene) seenSamples = scene.gpu.samples.pushed;
      }
      if (patch.fpsLimit !== undefined) {
        kept = { ...kept, fpsLimit: patch.fpsLimit };
        save();
        clock?.limit(patch.fpsLimit);
        redraw();
      }
      const bridge = desktop();
      if (patch.vsync !== undefined && bridge && desktopState) {
        desktopState = { ...desktopState, vsync: patch.vsync };
        void bridge.set({ vsync: patch.vsync });
      }
      if (patch.fullscreen !== undefined) {
        if (bridge && desktopState) {
          desktopState = { ...desktopState, fullscreen: patch.fullscreen };
          void bridge.set({ fullscreen: patch.fullscreen });
        } else if (patch.fullscreen) {
          void document.documentElement.requestFullscreen().catch((error: unknown) => console.warn('full screen refused', error));
        } else if (document.fullscreenElement) {
          void document.exitFullscreen().catch((error: unknown) => console.warn('full screen not left', error));
        }
      }
    },
  });
// … 1 lines
}

// src/graphicsWiring.ts:143-143 slice of src/graphicsWiring.ts#createGraphicsWiring.fullRatio (shared): the feature's statements; "…" marks code left out
  const fullRatio = (): number => Math.min(WEAK_MACHINE.pixelRatio ?? (window.devicePixelRatio || 1), settings.pixelRatio);

// src/graphicsWiring.ts:149-156 slice of src/graphicsWiring.ts#createGraphicsWiring.apply (shared): the feature's statements; "…" marks code left out
  const apply = (): void => {
    // … 1 lines
    resolution.reset();
  // … 4 lines
  };

// src/graphicsWiring.ts:158-167 slice of src/graphicsWiring.ts#createGraphicsWiring.choices (shared): the feature's statements; "…" marks code left out
  const choices = (): GraphicsChoices => ({
    preset,
    shadows: settings.shadows ? settings.shadowMapSize : 0,
    ambientOcclusion: settings.ambientOcclusion,
    smaa: settings.smaa,
    dynamicResolution: dynamicOn(),
    fpsLimit: fps(),
    vsync: desktopState?.vsync ?? true,
    fullscreen: desktopState ? desktopState.fullscreen : document.fullscreenElement !== null,
  });

// src/graphicsWiring.ts:284-290 slice of src/graphicsWiring.ts#pick (shared): the feature's statements; "…" marks code left out
function pick(patch: Partial<GraphicsChoices>): KeptOptions {
  // … 1 lines
  if (patch.shadows !== undefined) out.shadows = patch.shadows;
  if (patch.ambientOcclusion !== undefined) out.ambientOcclusion = patch.ambientOcclusion;
  if (patch.smaa !== undefined) out.smaa = patch.smaa;
// … 1 lines
}
