// What the feature uses of src/core/profiler.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/profiler.ts:58-65 interface ProductTiming
/** One product's refresh as the engine timed it (`core/engine.ts`). */
export interface ProductTiming {
  readonly name: string;
  readonly phase: string;
  readonly ms: number;
  readonly keys: number;
  /** It computed in a refresh where no input it is told of had moved: work for nothing. */
  readonly idle: boolean;
}

// src/core/profiler.ts:74-256 class Profiler
export class Profiler { … }

// src/core/profiler.ts:78-80 getter Profiler.enabled
/** Whether it records (the HUD shown): closed, the default, every method returns at once and its observers are off (`onToggle`). */
  get enabled(): boolean;

// src/core/profiler.ts:81-85 setter Profiler.enabled
set enabled(value: boolean);

// src/core/profiler.ts:231-231 field Profiler.closed
/** The last closed window, as the HUD shows it. */
  closed: ProfilerView | null = null;

// src/core/profiler.ts:234-255 method Profiler.view
/** Everything counted, plain data (the HUD, `__roadcraft.profiler()`, a snapshot copied out). */
  view(): ProfilerView;

// src/core/profiler.ts:258-279 interface ProfilerView
export interface ProfilerView {
  readonly frames: number;
  readonly fps: number;
  readonly frameP50: number;
  readonly frameMax: number;
  readonly events: Record<string, {
    readonly perSecond: number; readonly peakPerFrame: number; readonly total: number;
    readonly mbPerSecond: number; readonly totalMB: number;
    readonly sites: readonly [string, number][];
    readonly last: { readonly site: string; readonly stack: string; readonly at: number } | null;
  }>;
  readonly products: readonly ProductTiming[];
  readonly idleProducts: readonly [string, number][];
  readonly changes: readonly [string, number][];
  /** Ms per second of each frame system (`frame:`) and measured step (`hitch:`). */
  readonly steps: readonly [string, number][];
  readonly heapMB: number | null;
  readonly heapRate: number;
  readonly gcDropMB: number;
  readonly longTasks: readonly { readonly ms: number; readonly agoS: number }[];
  readonly gauges: Readonly<Record<string, number>>;
}

// src/core/profiler.ts:284-284 variable profiler
/** The game's one profiler. */
export const profiler = new Profiler();
