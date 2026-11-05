// What the feature uses of src/lab/tests/benchmarks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/tests/benchmarks.ts:13-13 type BenchmarkKind
/** THE BENCHMARKS: the standard views against the plan's budgets (`VIEW_BUDGETS`, `__roadcraft.budget`), the simulation pushed (its speed,
 *  its traffic and people), the leak test (the camera moved for a while: the heap and the GPU's objects must not climb), the shaders' hitches
 *  (switches that compile programs flipped: the hitches and the programs counted) and a long run with the game going. */

export type BenchmarkKind = 'views' | 'stress' | 'leak' | 'hitch' | 'longrun';

// src/lab/tests/benchmarks.ts:19-24 interface BenchmarkOptions
export interface BenchmarkOptions {
  readonly frames?: number;
  /** Seconds (leak, long run). */
  readonly seconds?: number;
  readonly origin?: boolean;
}

// src/lab/tests/benchmarks.ts:27-35 interface BenchRow
/** One line of a benchmark: what was looked at, the figure, its budget or reference, a status. */
export interface BenchRow {
  readonly bench: BenchmarkKind;
  readonly case: string;
  readonly metric: string;
  readonly value: number | null;
  readonly unit: string;
  readonly reference: string;
  readonly status: 'ok' | 'over' | 'grows' | 'info' | 'hitch';
}

// src/lab/tests/benchmarks.ts:118-155 function stress
/** The simulation pushed: its speed 1, 2, 4×, then its traffic and people at the most (`MAX_INTENSITY` 3×), each a while on the fixed step. */
async function stress(ctx: LabContext, options: BenchmarkOptions, cancel: Cancel, progress: (l: string) => void, rows: BenchRow[]): Promise<void>;
