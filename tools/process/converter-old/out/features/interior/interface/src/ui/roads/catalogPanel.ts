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

// src/ui/roads/catalogPanel.ts:26-26 variable strip
/** The tab shown and the strip last built. */
const strip: { category: CatalogCategory | null; built: BuiltStrip | null } = { category: null, built: null };

// src/ui/roads/catalogPanel.ts:53-63 interface BuiltStrip
/** The strip as last built and what it was built from: the same inputs keep it, only the picked card changes. */
interface BuiltStrip {
  readonly root: HTMLElement;
  readonly language: string;
  readonly category: CatalogCategory;
  readonly saved: readonly RoadTemplate[];
  readonly loose: DrawProfile | null;
  readonly cards: readonly { readonly id: string; readonly card: HTMLElement; readonly main: HTMLButtonElement; readonly road: CatalogRoad }[];
  /** Each shown card's plan-view picture, by profile and scale: only the cards shown now are kept. */
  readonly swatches: ReadonlyMap<string, string>;
  host: CatalogHost;
}

// src/ui/roads/catalogPanel.ts:167-186 function customButton
function customButton(built: BuiltStrip, road: CatalogRoad): HTMLButtonElement;
