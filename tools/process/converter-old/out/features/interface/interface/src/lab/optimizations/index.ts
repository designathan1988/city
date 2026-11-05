// What the feature uses of src/lab/optimizations/index.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/optimizations/index.ts:10-21 interface OptRow
/** THE LAB'S OPTIMISATIONS (`render/optimizations.ts`): every optimisation the game does not make and every field of the tier's settings, grouped by system, each set alone and live to measure what it gains; "Reset to default" puts the game back as it draws. Each change is an event (`opt.<id>`) the recorder marks, the ones set go into every report (`optimizations`), and the tests and the object inspector reach them through `window.__labOpt`. */

/** An optimisation as the tests, the inspector and the reports read it: plain JSON. */
interface OptRow {
  readonly id: string;
  readonly system: OptSystem;
  readonly title: string;
  readonly value: OptValue;
  readonly default: OptValue;
  readonly choices: readonly OptValue[];
  /** Any number in it too (a tier's field). */
  readonly range: { readonly min: number; readonly max: number; readonly step: number } | null;
  /** What it changes (`OptimizationSpec.affects`): `all`, `shadows`, `post`, `textures`, `layer:<layer>`, `pass:<pass>`, `name:<glob>`. */
  readonly affects: readonly string[];
}

// src/lab/optimizations/index.ts:23-32 function rowOf
const rowOf = (state: OptimizationState): OptRow => …;

// src/lab/optimizations/index.ts:34-222 function createOptimizationTabs
export function createOptimizationTabs(ctx: LabContext): LabTab[];
