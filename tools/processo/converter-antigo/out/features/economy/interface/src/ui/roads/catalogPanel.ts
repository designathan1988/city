// What the feature uses of src/ui/roads/catalogPanel.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/catalogPanel.ts:16-23 interface CatalogHost
/** THE ROAD CATALOGUE in the road tool's drawer (the player's order of 2026-10-09) */
export interface CatalogHost {
  /** The class the road tool has in hand (when no catalogue road is picked, its own card is the picked one). */
  readonly classIndex: () => number;
  /** Puts a class in the road tool's hand (its colours and rules while drawing). */
  readonly pickClass: (index: number) => void;
  /** Redraws the drawer. */
  readonly refresh: () => void;
}

// src/ui/roads/catalogPanel.ts:65-153 function catalogStrip
export function catalogStrip(host: CatalogHost): HTMLElement;
