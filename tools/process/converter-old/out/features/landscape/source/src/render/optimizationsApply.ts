// The code of src/render/optimizationsApply.ts that is the feature's (the rest of the file is shared).

// src/render/optimizationsApply.ts:39-45 slice of src/render/optimizationsApply.ts#OptimizerDeps (shared): the feature's statements; "…" marks code left out
export interface OptimizerDeps {
  // … 2 lines
  readonly environment: SceneEnvironment;
// … 2 lines
}

// src/render/optimizationsApply.ts:78-262 slice of src/render/optimizationsApply.ts#createOptimizer (shared): the feature's statements; "…" marks code left out
export function createOptimizer(deps: OptimizerDeps): Optimizer {
  // … 6 lines
  environment.setFarCascadeEvery(num(DEFAULT_OPTIMIZATIONS.farCascadeEvery, 1));
// … 176 lines
}

// src/render/optimizationsApply.ts:199-225 slice of src/render/optimizationsApply.ts#createOptimizer.set (shared): the feature's statements; "…" marks code left out
    set(next) {
      // … 7 lines
      if (before.shadowType !== next.shadowType) {
        // … 2 lines
        environment.remakeShadowMaps();
      // … 2 lines
      }
      if (before.farCascadeEvery !== next.farCascadeEvery) {
        environment.setFarCascadeEvery(num(next.farCascadeEvery, 1));
      // … 1 lines
      }
    // … 7 lines
    },
