// What the feature uses of src/sim/populationGovernor.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/populationGovernor.ts:18-20 function cpuPopulationShare
/** The most of the town a machine of `threads` threads is given to start with (the governor fills it from there when its steps are cheap): DO NOT start a four-thread machine at the full town, cut a 0.85 step every few seconds while its first steps run long (the weak machine's plan, 2026-10-18). */
export function cpuPopulationShare(threads: number): number;
