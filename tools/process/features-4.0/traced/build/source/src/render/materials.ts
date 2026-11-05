// The code of src/render/materials.ts that is the feature's (the rest of the file is shared).

// src/render/materials.ts:35-148 slice of src/render/materials.ts#createMaterials (shared): the feature's statements; "…" marks code left out
export function createMaterials(anisotropy: number): SceneMaterials {
  // … 62 lines
  applyLotOverlay(asphalt);
  applyLotOverlay(asphaltRaised);
  applyLotOverlay(footwayMaterial);
  applyLotOverlay(kerbMaterial);
  applyLotOverlay(vergeMaterial);
  applyLotOverlay(deckMaterial);
// … 44 lines
}
