// The code of src/render/buildingSwitches.ts that is the feature's (the rest of the file is shared).

// src/render/buildingSwitches.ts:9-16 slice of src/render/buildingSwitches.ts#ROOT_SWITCH (shared): the feature's statements; "…" marks code left out
const ROOT_SWITCH: Readonly<Record<BuildingRoot, BuildingSwitch>> = {
  masses: 'bMasses',
  facades: 'bFacadeLevel',
  cells: 'bDetail',
  parts: 'bParts',
  signatures: 'bSignatures',
  flags: 'bFlags',
};

// src/render/buildingSwitches.ts:19-28 slice of src/render/buildingSwitches.ts#KIND_SWITCH (shared): the feature's statements; "…" marks code left out
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

// src/render/buildingSwitches.ts:29-30 slice of src/render/buildingSwitches.ts#kindOf (shared): the feature's statements; "…" marks code left out
const kindOf = (name: string): BuildingSwitch | null =>
  KIND_SWITCH[name] ?? (name.startsWith('building-furniture-') ? 'bFurniture' : null);

// src/render/buildingSwitches.ts:31-31 slice of src/render/buildingSwitches.ts#KIND_SWITCHES (shared): the feature's statements; "…" marks code left out
const KIND_SWITCHES: readonly BuildingSwitch[] = ['bShell', 'bGlass', 'bFrames', 'bFacadeDetails', 'bLots', 'bProps', 'bFurniture'];

// src/render/buildingSwitches.ts:33-41 slice of src/render/buildingSwitches.ts#BuildingSwitcher (shared): the feature's statements; "…" marks code left out
export interface BuildingSwitcher {
  // … 1 lines
  beforeRender(on: EffectSwitches, choices: BuildingChoices, hider: FrameHider): void;
  // … 3 lines
  casters(on: EffectSwitches, texel: number | null): void;
// … 1 lines
}

// src/render/buildingSwitches.ts:57-84 slice of src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender (shared): the feature's statements; "…" marks code left out
    beforeRender(on, choices, hider) {
      // … 3 lines
      hider.hide(toHide);
      // … 2 lines
      const swap = choices.material === 'normal' ? null : plain[choices.material];
      if (!byKind && !swap) return;
      // … 16 lines
      hider.hide(toHide);
    // … 1 lines
    },
