// The code of src/render/poleLayer.ts that is the feature's (the rest of the file is shared).

// src/render/poleLayer.ts:711-721 slice of src/render/poleLayer.ts#buildUtilities (shared): the feature's statements; "…" marks code left out
export function buildUtilities(
  // … 2 lines
  kit: SceneryKit,
// … 2 lines
): PoleLayer {
  const layer = new PoleLayer(kit, name);
  layer.fill(net, groundAt, source);
  return layer;
}

// src/render/poleLayer.ts:729-750 slice of src/render/poleLayer.ts#buildPolePreview (shared): the feature's statements; "…" marks code left out
export function buildPolePreview(
  // … 2 lines
  kit: SceneryKit,
// … 1 lines
): Utilities {
  // … 13 lines
  return buildUtilities(net, groundAt, kit,
    { poles, spans, spansAt: (id) => reaching.get(id) ?? [], draws: (pole) => !standing.has(pole.id) }, 'utility-preview');
}
