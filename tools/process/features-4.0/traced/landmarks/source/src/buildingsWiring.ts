// The code of src/buildingsWiring.ts that is the feature's (the rest of the file is shared).

// src/buildingsWiring.ts:81-1216 slice of src/buildingsWiring.ts#createBuildingWiring (shared): the feature's statements; "…" marks code left out
export function createBuildingWiring(deps: BuildingWiringDeps): BuildingWiring {
  // … 414 lines
  const actions: BuilderActions = {
    // … 105 lines
    choosePreset: (key) => {
      // … 4 lines
      if (key.startsWith('lot:')) {
        void loadLot(key.slice(4)).then((lot) => {
          if (!lot) return;
          tool.useBody(lot.body as BlueprintBody, lot.body.blueprint ?? key);
          host.changed();
        });
      // … 1 lines
      }
    // … 2 lines
    },
  // … 128 lines
  };
// … 469 lines
}
