// What the feature uses of src/ui/healthPanel.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/healthPanel.ts:6-12 interface HealthPanelSources
/** The game's health in the interface: a button in the top bar whose dot is green, amber or red */
export interface HealthPanelSources {
  readonly log: HealthLog;
  /** The world's latest changes, newest first, as lines. */
  readonly world: (n: number) => readonly string[];
  /** The game's state now (short lines, in two columns), lines too long for a column */
  readonly state: (n: number) => { readonly now: readonly string[]; readonly wide?: readonly string[]; readonly changes: readonly string[] };
}

// src/ui/healthPanel.ts:57-266 function mountHealthPanel
/** The panel's keys, for the game's keys (`editor/gameKeys.ts`): F9 toggles it, Escape closes it (false: it was closed). */
export function mountHealthPanel(sources: HealthPanelSources): { readonly button: HTMLButtonElement; toggle(): void; close(): boolean };
