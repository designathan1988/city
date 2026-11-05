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

// src/render/buildingSwitches.ts:19-28 variable KIND_SWITCH
/** A piece by the name its mesh is given (`buildingMesh.ts`, `partCells.ts`: `building-<kind>`), and the switch that hides it. */
const KIND_SWITCH: Readonly<Record<string, BuildingSwitch>> = Object.freeze({
  [SHELL_MESH]: 'bShell',
  // The cells' walls in their multi-draw batches (`shellBatch.ts`): the switch hid none of them once the shells left their cells' meshes.
  ...Object.fromEntries(['detail', 'facade', 'runs'].map((set) => [`building-shell-batch-${set}`, 'bShell'])),
  ...Object.fromEntries(['glass', 'glassDark', 'glassRail'].map((k) => [`building-${k}`, 'bGlass'])),
  ...Object.fromEntries(['frame', 'door', 'shutter'].map((k) => [`building-${k}`, 'bFrames'])),
  ...Object.fromEntries(['railing', 'roofRailing', 'awning', 'column', 'water', 'lamp', 'wash', 'fin', 'louvre', 'brise'].map((k) => [`building-${k}`, 'bFacadeDetails'])),
  ...Object.fromEntries(['lotBox', 'lotRail', 'lotSlab', 'lotSides'].map((k) => [`building-${k}`, 'bLots'])),
  ...Object.fromEntries(FACADE_PROP_KINDS.map((k) => [`building-${k}`, 'bProps'])),
}) as Readonly<Record<string, BuildingSwitch>>;

// src/render/buildingSwitches.ts:29-30 function kindOf
const kindOf = (name: string): BuildingSwitch | null => …;

// src/render/buildingSwitches.ts:31-31 variable KIND_SWITCHES
const KIND_SWITCHES: readonly BuildingSwitch[] = ['bShell', 'bGlass', 'bFrames', 'bFacadeDetails', 'bLots', 'bProps', 'bFurniture'];

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
