// What the feature uses of src/render/buildingSwitches.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildingSwitches.ts:9-16 variable ROOT_SWITCH
/** THE BUILDINGS' SWITCHES (the effects panel, `effectSwitches.ts`): each part of the layer and each kind of piece hidden for the frame, their materials drawn plain, their shadows left out of the sun's passes. */

const ROOT_SWITCH: Readonly<Record<BuildingRoot, BuildingSwitch>> = {
  masses: 'bMasses',
  facades: 'bFacadeLevel',
  cells: 'bDetail',
  parts: 'bParts',
  signatures: 'bSignatures',
  flags: 'bFlags',
};

// src/render/buildingSwitches.ts:33-41 interface BuildingSwitcher
export interface BuildingSwitcher {
  /** Before the frame is drawn: the parts switched off hidden (by `hider`), the materials swapped. */
  beforeRender(on: EffectSwitches, choices: BuildingChoices, hider: FrameHider): void;
  /** After it: the materials given back, the shadows' hiding undone. */
  afterRender(): void;
  /** Each sun cascade's pass (`texel`), and their end (null): the buildings left out of it while their shadows are switched off. */
  casters(on: EffectSwitches, texel: number | null): void;
  dispose(): void;
}

// src/render/buildingSwitches.ts:43-102 function createBuildingSwitcher
export function createBuildingSwitcher(layer: BuildingLayer): BuildingSwitcher;
