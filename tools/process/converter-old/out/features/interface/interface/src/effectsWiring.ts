// What the feature uses of src/effectsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/effectsWiring.ts:10-10 variable CPU_SWITCHES
/** THE EFFECTS PANEL'S WIRING, out of `main.ts`: the GPU's effects switched in the renderer (`effectSwitches.ts`), the CPU's systems in the frame, the resolution, kept between runs (`roadcraft.effects`). */

/** The frame's heavy systems on the CPU, each on as the game runs. */
const CPU_SWITCHES = ['simulation', 'traffic', 'pedestrians', 'growth', 'onDemand'] as const;

// src/effectsWiring.ts:11-11 type CpuSwitch
type CpuSwitch = (typeof CPU_SWITCHES)[number];

// src/effectsWiring.ts:12-12 type CpuSwitches
type CpuSwitches = Readonly<Record<CpuSwitch, boolean>>;

// src/effectsWiring.ts:13-13 variable ALL_CPU_ON
const ALL_CPU_ON: CpuSwitches = { simulation: true, traffic: true, pedestrians: true, growth: true, onDemand: true };

// src/effectsWiring.ts:16-20 variable BUILDING_CHOICES
/** The buildings' settings of several values, each with its values as the panel offers them (`BuildingChoices`). */
const BUILDING_CHOICES: readonly { key: string; field: keyof BuildingChoices; values: readonly (string | number)[] }[] = [
  { key: 'bMaterial', field: 'material', values: ['normal', 'lambert', 'basic'] },
  { key: 'bTriangles', field: 'triangles', values: [1, 0.5, 0.25, 0.1, 0.02, 0] },
  { key: 'bDetailMemory', field: 'detailMemoryMB', values: [-1, 1024, 512, 256, 128, 32, 0] },
];

// src/effectsWiring.ts:24-24 variable OPT_STORE_KEY
/** The lab's optimisations kept apart (`optimizations.ts`): only those set away from the game's own. */
const OPT_STORE_KEY = 'roadcraft.optimizations';

// src/effectsWiring.ts:28-33 interface Kept
interface Kept {
  readonly effects: EffectSwitches;
  readonly cpu: CpuSwitches;
  readonly resolution: number;
  readonly buildings: BuildingChoices;
}

// src/effectsWiring.ts:35-47 function readKept
function readKept(): Kept;

// src/effectsWiring.ts:49-61 function readOptimizations
function readOptimizations(): Optimizations;

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

// src/effectsWiring.ts:193-219 function createEffectsWiring.stats
const stats = (): (readonly [string, string])[] =>;
