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

// src/render/buildingSwitches.ts:43-102 slice of src/render/buildingSwitches.ts#createBuildingSwitcher (shared): the feature's statements; "…" marks code left out
export function createBuildingSwitcher(layer: BuildingLayer): BuildingSwitcher {
// … 58 lines
}

// src/render/buildingSwitches.ts:50-54 slice of src/render/buildingSwitches.ts#createBuildingSwitcher.giveBackShadow (shared): the feature's statements; "…" marks code left out
  const giveBackShadow = (): void => {
    // … 1 lines
    layer.group.visible = shadowWas;
  // … 1 lines
  };

// src/render/buildingSwitches.ts:57-84 slice of src/render/buildingSwitches.ts#createBuildingSwitcher.beforeRender (shared): the feature's statements; "…" marks code left out
    beforeRender(on, choices, hider) {
      if (!layer.group.visible) return;
      const roots = layer.roots();
      for (const root of Object.keys(ROOT_SWITCH) as BuildingRoot[]) if (!on[ROOT_SWITCH[root]]) toHide.push(roots[root]!);
      // … 20 lines
      walk(layer.group);
    // … 2 lines
    },

// src/render/buildingSwitches.ts:91-96 slice of src/render/buildingSwitches.ts#createBuildingSwitcher.casters (shared): the feature's statements; "…" marks code left out
    casters(on, texel) {
      // … 2 lines
      shadowWas = layer.group.visible;
      layer.group.visible = false;
    },
