// What the feature uses of src/graphicsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/graphicsWiring.ts:12-12 type QualityPreset
/** THE GRAPHICS OPTIONS' WIRING, out of `main.ts`: the player's choices kept, the first tier by the GPU, and each applied live. `auto` (the first start's) moves between the tiers in play by the frames' cost (`QualityGovernor`), never above the GPU's own tier. */

/** A tier the player picked, or the automatic one. */
export type QualityPreset = QualityLevel | 'auto';

// src/graphicsWiring.ts:100-115 interface GraphicsWiring
export interface GraphicsWiring {
  /** The tier and settings the renderer starts with. */
  readonly level: QualityLevel;
  /** What the player chose: a tier, or `auto`. */
  readonly preset: QualityPreset;
  readonly settings: QualitySettings;
  /** The renderer and the frame loop made: the options apply to them from now on. */
  attach(scene: SceneHandle, clock: FrameClock, redraw: () => void): void;
  /** A tier picked (the menu's row or the panel): its own settings, the player's apart from it cleared. */
  setPreset(preset: QualityPreset): void;
  openPanel(): void;
  /** The whole pixel ratio the tier allows on this display (the effects panel draws a share of it). */
  fullRatio(): number;
  /** After each frame drawn: the dynamic resolution and the automatic tier read the GPU's times. */
  frame(): void;
}

// src/graphicsWiring.ts:118-282 function createGraphicsWiring
/** `onLevel`: the tier in use changed, by the automatic governor (`auto`) or the player's pick (`player`); told for the field's session summary (`fieldReport.ts`). */
export function createGraphicsWiring(onPreset: (preset: QualityPreset) => void, onLevel: (level: QualityLevel, by: 'auto' | 'player') => void = () => {}): GraphicsWiring;
