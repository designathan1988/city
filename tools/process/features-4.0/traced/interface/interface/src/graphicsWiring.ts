// What the feature uses of src/graphicsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/graphicsWiring.ts:12-12 type QualityPreset
/** THE GRAPHICS OPTIONS' WIRING, out of `main.ts`: the player's choices kept, the first tier by the GPU, and each applied live. `auto` (the first start's) moves between the tiers in play by the frames' cost (`QualityGovernor`), never above the GPU's own tier. */

/** A tier the player picked, or the automatic one. */
export type QualityPreset = QualityLevel | 'auto';

// src/graphicsWiring.ts:13-13 function isQualityPreset
export const isQualityPreset = (value: unknown): value is QualityPreset => …;

// src/graphicsWiring.ts:16-19 interface DesktopBridge
/** The desktop shell's calls (`desktop/preload.cjs`); absent in a browser. */
interface DesktopBridge {
  state(): Promise<{ vsync: boolean; fullscreen: boolean; steam: boolean; steamDeck: boolean } | null>;
  set(patch: { vsync?: boolean; fullscreen?: boolean }): Promise<boolean>;
}

// src/graphicsWiring.ts:20-20 function desktop
const desktop = (): DesktopBridge | null => …;

// src/graphicsWiring.ts:26-32 interface KeptOptions
/** What the player set apart from the tier's own (`roadcraft.graphics`); a tier picked clears the first three. */
interface KeptOptions {
  shadows?: number;
  ambientOcclusion?: boolean;
  smaa?: boolean;
  dynamicResolution?: boolean;
  fpsLimit?: number;
}

// src/graphicsWiring.ts:52-66 function gpuName
/** The GPU's name from a context of its own, let go at once: the tier is needed before the game's renderer exists. */
function gpuName(): string;

// src/graphicsWiring.ts:85-98 function settingsFor
/** The tier's settings with the player's own over them; occlusion and SMAA are passes of the post chain, so either brings it. */
function settingsFor(level: QualityLevel, kept: KeptOptions): QualitySettings;

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

// src/graphicsWiring.ts:284-290 function pick
function pick(patch: Partial<GraphicsChoices>): KeptOptions;
