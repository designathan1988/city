// What the feature uses of src/core/perfBudget.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/perfBudget.ts:3-12 interface Budget
/** THE PERFORMANCE BUDGETS: one table for the game's own watch (the F9 monitor, `ui/healthWatch.ts`), the specs */

export interface Budget {
  /** The most it may be. */
  readonly max: number;
  /** The least it may be (a floor on the picture's detail). */
  readonly min?: number;
  /** `error` fails the check or the gate; `warn` is only reported. */
  readonly level: 'error' | 'warn';
  /** What it guards, and what it was when set. */
  readonly why: string;
}

// src/core/perfBudget.ts:15-26 variable STEP_BUDGETS
/** A step the game measures (`performance.measure('hitch:<name>')`), ms of one occurrence */
const STEP_BUDGETS: Readonly<Record<string, Budget>> = {
  'mutate/edit+after': { max: 150, level: 'warn', why: 'the click of an edit: commit and the undo text (65-112 ms)' },
  'mutate/after edit': { max: 150, level: 'warn', why: 'the click of an edit, with the network rebuilt (65-112 ms)' },
  'road-edit/utilities': { max: 120, level: 'warn', why: 'poles and wires rebuilt in an edit (68-86 ms)' },
  'road-edit/world rebuilt': { max: 2500, level: 'warn', why: 'a whole road edit, sliced (0.3-1.6 s)' },
  'commit/stations': { max: 120, level: 'warn', why: 'stations and stops in the commit (55-58 ms)' },
  'elevation/full': { max: 120, level: 'warn', why: 'road heights rebuilt (54-56 ms)' },
  'network/rebuild': { max: 60, level: 'warn', why: 'the network rebuilt (up to 12 ms)' },
  'draw/Rebuild': { max: 200, level: 'warn', why: 'a rebuild inside a frame (up to 187 ms)' },
  'draw/Pump': { max: 120, level: 'warn', why: 'a frame slice of work (up to 100 ms)' },
  'draw/Render': { max: 100, level: 'warn', why: 'drawing one frame, CPU (up to 83 ms)' },
};

// src/core/perfBudget.ts:40-40 type CityView
/** THE CITY PLAN'S BUDGETS: what the whole-map city may cost, from the plan */
export type CityView = 'street' | 'district' | 'map';

// src/core/perfBudget.ts:44-48 variable VIEW_BUDGETS
/** Per view: triangles and draw calls (colour + shadow) of one frame, GPU ms (p95), fps (a floor).
 *  DO NOT set the triangles under what the view draws at full facade level on a dedicated GPU (district 13 M, street 5.4 M, map 4.9 M measured 2026-10-16): the levels' error scale rises until they fit (`buildings/frameBudget.ts`), and at 8 M the district view fell to masses - the picture lost for a number. `gpuMs` is a gate: a view over it is `over` (the frame's time is the quality governor's, `render/quality.ts`). */
export const VIEW_BUDGETS: Readonly<Record<CityView, { readonly triangles: number; readonly drawCalls: number; readonly gpuMs: number; readonly fps: number }>> = {
  street: { triangles: 16_000_000, drawCalls: 700 + 300, gpuMs: 12, fps: 60 },
  district: { triangles: 16_000_000, drawCalls: 800 + 300, gpuMs: 12, fps: 60 },
  map: { triangles: 8_000_000, drawCalls: 300 + 100, gpuMs: 12, fps: 50 },
};

// src/core/perfBudget.ts:90-110 interface CityMeasure
/** What `__roadcraft.budget` measured in the running game. */
export interface CityMeasure {
  readonly view: CityView;
  /** Of the last frame drawn, every pass (`renderer.info.render`). */
  readonly triangles: number;
  readonly drawCalls: number;
  /** p95 of the last frames' GPU time; null when the GPU timer is not there. */
  readonly gpuMs: number | null;
  /** Frames a second of a stretch drawn back to back, from the frames' own times; null when the game was not drawing every refresh (the loop on demand: a smoothed 1/delta then told how often it drew, report UI-15 d). */
  readonly fps: number | null;
  /** GPU geometry bytes by object name (`render/memoryCounters.ts`). */
  readonly bytesByName: ReadonlyMap<string, number>;
  readonly textureBytes: number;
  /** The render targets and shadow maps (`render/targetBytes.ts`, an estimate); null where they are not counted. */
  readonly targetBytes?: number | null;
  /** Geometry still held by JavaScript arrays: exact, part of the heap. */
  readonly geometryCpuBytes: number;
  /** Chromium's `performance.memory.usedJSHeapSize`: V8 heap PLUS external memory (the arrays), bucketed; null elsewhere. */
  readonly heapBytes: number | null;
  /** The workers' heaps (`measureUserAgentSpecificMemory`), added to the page's when measured; null: the page's alone, said so. */
  readonly workerHeapBytes?: number | null;
}

// src/core/perfBudget.ts:112-120 interface BudgetRow
export interface BudgetRow {
  readonly item: string;
  readonly measured: string;
  readonly budget: string;
  /** `over` first: what to work on. */
  readonly status: 'over' | 'ok' | 'no budget in the plan' | 'not measured' | 'approximate';
  /** How far over (measured / budget); 0 when within, or not comparable. */
  readonly times: number;
}

// src/core/perfBudget.ts:126-166 function cityBudgetTable
/** The plan's budgets against a measure, the rows furthest over first. */
export function cityBudgetTable(m: CityMeasure): BudgetRow[];
