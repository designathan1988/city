// What the feature uses of src/ui/v2/shell.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/v2/shell.ts:27-31 variable PAINT_SWATCH
/** The colour of each paintable ground, for its button. */
const PAINT_SWATCH: Readonly<Record<PaintKind, string>> = {
  sand: '#ccb380', soil: '#7a5a3c', meadow: '#7d8f3a', snow: '#eef1f6', gravel: '#8b8a84', asphalt: '#2d2e32', concrete: '#a8a69f', grass: '#4f7a36', forest: '#2c4f26',
  scrub: '#4c6a2c', flowers: '#c86a8e', rocks: '#7d776c', granite: '#8d8a84', sandstone: '#c27a44', basalt: '#3b3836',
  cerrado: '#b8933e', atlantic: '#2f6b34', amazon: '#1b4f27', caatinga: '#a08b68', pampa: '#86ad4c', pantanal: '#4e8f7c',
};

// src/ui/v2/shell.ts:33-33 variable BIOME_SWATCHES
/** The biomes, painted or chosen for the whole map (`world/ecology.ts`). */
const BIOME_SWATCHES = ['cerrado', 'atlantic', 'amazon', 'caatinga', 'pampa', 'pantanal'] as const;

// src/ui/v2/shell.ts:247-248 function svg
const svg = (name: string, size = 22): string => …;

// src/ui/v2/shell.ts:313-321 interface ShellSlider
/** A number the player sets with a slider: its range, and how the game reads and takes it. */
export interface ShellSlider {
  readonly min: number;
  readonly max: number;
  readonly step: number;
  get(): number;
  set(value: number): void;
  /** The drag ended (one undo step a drag, for the map's own numbers). */
  end?(): void;
}
