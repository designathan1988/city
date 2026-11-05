// What the feature uses of src/effectsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/effectsWiring.ts:64-68 interface OptimizationState
/** One optimisation as the lab lists it: its spec, the value set, whether that is the game's own. */
export interface OptimizationState {
  readonly spec: OptimizationSpec;
  readonly value: OptValue;
  readonly isDefault: boolean;
}

// src/effectsWiring.ts:71-81 interface OptimizationControl
/** The lab's optimisations (`optimizations.ts`), set live, kept between runs (`roadcraft.optimizations`). Every change told to `EffectsWiring.onChange` as `opt.<id>`. */
export interface OptimizationControl {
  list(): OptimizationState[];
  get(id: string): OptValue | undefined;
  /** False when `id` or the value is not one of its choices. */
  set(id: string, value: unknown): boolean;
  /** Every one back to the game's own. */
  reset(): void;
  values(): Optimizations;
  /** Told of every change: the id and its new value. */
  onChange(listener: (id: OptId, value: OptValue) => void): () => void;
}

// src/effectsWiring.ts:83-106 interface EffectsWiring
export interface EffectsWiring {
  /** The renderer made: the switches kept apply to it from now on. `fullRatio` the whole pixel ratio the graphics options allow. */
  attach(scene: SceneHandle, fullRatio: () => number, redraw: () => void): void;
  readonly panel: EffectsPanel;
  /** The simulation steps at all; its traffic, its pedestrians; the zones grow. */
  readonly simulation: boolean;
  readonly traffic: boolean;
  readonly pedestrians: boolean;
  readonly growth: boolean;
  /** Off: a frame drawn every vsync whether anything moved or not (a steady load to measure). */
  readonly onDemand: boolean;
  /** The share of the population the simulation may put out at most, of what the screen allows (the lab's `population`). */
  readonly populationShare: number;
  /** The lab's optimisations. */
  readonly optimizations: OptimizationControl;
  /** A frame's whole time on the CPU, ms (`frameTimer`). */
  frameEnded(ms: number): void;
  /** Every setting as it stands now (the lab's recorder keeps it with each recording). */
  snapshot(): Readonly<Record<string, unknown>>;
  /** Told of every change made from now on: the key and its new value (the lab's recorder marks it). */
  onChange(listener: (key: string, value: unknown) => void): () => void;
  /** Several switches set at once by key (the lab's tests): on or off, or a building setting's value. */
  setMany(values: Readonly<Record<string, boolean | string | number>>): void;
}

// src/effectsWiring.ts:108-303 function createEffectsWiring
export function createEffectsWiring(): EffectsWiring;
