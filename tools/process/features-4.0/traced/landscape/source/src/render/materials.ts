// The code of src/render/materials.ts that is the feature's (the rest of the file is shared).

// src/render/materials.ts:35-148 slice of src/render/materials.ts#createMaterials (shared): the feature's statements; "…" marks code left out
export function createMaterials(anisotropy: number): SceneMaterials {
  // … 50 lines
  applyDetail(asphalt, { kind: 'asphalt', macroTile: ASPHALT_TILE, albedo: 0.6, normal: 0.65, macroBlur: 1.6 }, anisotropy);
  applyDetail(asphaltRaised, { kind: 'asphalt', macroTile: ASPHALT_TILE, albedo: 0.6, normal: 0.65, macroBlur: 1.6 }, anisotropy);
  applyDetail(footwayMaterial, { kind: 'concrete', macroTile: FOOTWAY_TILE, albedo: 0.7, normal: 0.8, macroBlur: 0.6 }, anisotropy);
  // … 4 lines
  applyDetail(kerbMaterial, { kind: 'concrete', macroTile: KERB_TILE, albedo: 0.55, normal: 0.7, macroBlur: 0.8 }, anisotropy);
  applyDetail(vergeMaterial, { kind: 'grass', macroTile: VERGE_TILE, albedo: 1, normal: 1.2, macroBlur: 2 }, anisotropy);
  applyDetail(deckMaterial, { kind: 'concrete', macroTile: DECK_TILE, albedo: 0.6, normal: 0.8, macroBlur: 1 }, anisotropy);
  applyDetail(parapetMaterial, { kind: 'concrete', macroTile: DECK_TILE, albedo: 0.6, normal: 0.8, macroBlur: 1 }, anisotropy);
// … 51 lines
}

// src/render/materials.ts:139-141 slice of src/render/materials.ts#createMaterials.setDetail (shared): the feature's statements; "…" marks code left out
    setDetail(enabled) {
      detailSwitch.value = enabled ? 1 : 0;
    },
