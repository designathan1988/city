// The code of src/peopleLab/dress.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/dress.ts:11-40 slice of src/peopleLab/dress.ts#dressPerson (shared): the feature's statements; "…" marks code left out
export async function dressPerson(person: BuiltPerson, spec: PersonSpec, layers: PersonLayers, painted: WeakMap<Texture, Texture>, sculpted = false): Promise<SkinAppearance> {
  polishPerson(person.meshes, spec.look, sculpted);
  // … 1 lines
  const own = spec.look.hairCut?.startsWith(PROC_PREFIX) || spec.look.hairCut === GROOM;
  // … 1 lines
  const skin = await loadSkinAppearance({ ...spec, look: { ...spec.look, outfitTint: null, ...(own ? { hairCut: 'none' } : {}) } });
  let soft = painted.get(skin.texture);
  if (!soft) {
    soft = paintedSkin(skin.texture, 1) ?? skin.texture;
    painted.set(skin.texture, soft);
  }
  skin.texture = soft;
  // … 1 lines
  delete skin.hairTexture;
  for (const m of person.meshes) {
    const materials = (Array.isArray(m.material) ? m.material : [m.material]).map((source) => {
      // … 4 lines
      applySkinAppearance(material, m.geometry, skin, layers);
      // … 1 lines
      sharpEyes(material, skin.eyeTexture);
    // … 1 lines
    });
  // … 1 lines
  }
  // … 1 lines
  return skin;
}
