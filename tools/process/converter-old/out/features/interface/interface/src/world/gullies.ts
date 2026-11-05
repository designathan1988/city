// What the feature uses of src/world/gullies.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/gullies.ts:3-11 interface GullyDab
/** GULLIES where the player wants them. */

export interface GullyDab {
  readonly x: number;
  readonly y: number;
  readonly radius: number;
  /** 0..1 at the centre. */
  readonly strength: number;
  /** Wipes gullies away (the land's own too) instead of cutting them. */
  readonly erase?: boolean;
}

// src/world/gullies.ts:16-16 variable DEFAULT_GULLY_AUTO
/** How much of the steep land carries gullies of itself, 0..1, when the map says nothing. */
export const DEFAULT_GULLY_AUTO = 0.25;
