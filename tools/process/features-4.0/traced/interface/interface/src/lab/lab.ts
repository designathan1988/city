// What the feature uses of src/lab/lab.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/lab.ts:14-23 interface LabDeps
/** THE DEBUGGING LAB (F7): one window of tabs over the game - the switches and optimisations, what every system costs, the code that runs, the GPU's passes, the recordings and the tests. Every tab is handed the same context (`labTypes.ts`). */

export interface LabDeps {
  readonly scene: SceneHandle;
  readonly sim: SimClient;
  readonly engine: Engine;
  readonly frameTimer: FrameTimer;
  readonly effects: EffectsWiring;
  /** Picks a quality preset as the menu does (`graphicsWiring.ts`). */
  setPreset(level: string): void;
  requestDraw(): void;
}

// src/lab/lab.ts:25-33 interface Lab
export interface Lab {
  readonly context: LabContext;
  toggle(): void;
  /** Opens it, on the tab `id` if given. */
  open(id?: string): void;
  close(): boolean;
  /** After each frame drawn (`main.ts` frame). */
  frameEnded(frame: FrameRecord): void;
}

// src/lab/lab.ts:37-140 function createLab
export function createLab(deps: LabDeps): Lab;
