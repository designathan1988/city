// What the feature uses of src/ui/effectsPanel.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/effectsPanel.ts:36-43 interface EffectsPanel
export interface EffectsPanel {
  toggle(): void;
  open(): void;
  close(): boolean;
  /** Drawn inside `host` (a tab of the lab, `lab/lab.ts`) instead of floating; `release` takes it back out. */
  embed(host: HTMLElement): void;
  release(): void;
}
