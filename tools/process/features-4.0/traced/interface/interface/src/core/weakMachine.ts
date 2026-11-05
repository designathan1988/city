// What the feature uses of src/core/weakMachine.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/weakMachine.ts:3-3 variable __WEAK_MACHINE__
/** A WEAKER MACHINE PLAYED ON THIS ONE, FOR A RUN ONLY (CLAUDE.md: the game proven on an Intel UHD 770 with 8 GB and few cores): set at build time (`vite.config.ts`, ROADCRAFT_WEAK, the page and every worker read the same); nothing set, the machine's own */

declare const __WEAK_MACHINE__: string | undefined;

// src/core/weakMachine.ts:6-11 interface WeakMachine
/** What the run pretends: the threads the pool is sized by (`jobPool.ts` jobWorkers), the GPU's name (its tier and its share of the system's memory, `gpuTier.ts`, `gpuLedger.ts`), the system's memory (GB) and the display's pixel ratio. Null: the machine's own. */
export interface WeakMachine {
  readonly threads: number | null;
  readonly gpu: string | null;
  readonly memoryGB: number | null;
  readonly pixelRatio: number | null;
}

// src/core/weakMachine.ts:21-46 function parseWeakMachine
/** `uhd770`, or `threads=4,gpu=...,memory=8,dpr=1` (any of them, over a preset named first); empty: none. A value out of reach throws: a run must not pass for a weak machine it is not. */
export function parseWeakMachine(text: string | undefined): WeakMachine;

// src/core/weakMachine.ts:49-49 variable WEAK_MACHINE
/** The machine this run pretends, or none. */
export const WEAK_MACHINE: WeakMachine = parseWeakMachine(typeof __WEAK_MACHINE__ !== 'undefined' ? __WEAK_MACHINE__ : '');
