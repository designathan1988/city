// The code of src/render/materials.ts that is the feature's (the rest of the file is shared).

// src/render/materials.ts:35-148 slice of src/render/materials.ts#createMaterials (shared): the feature's statements; "…" marks code left out
export function createMaterials(anisotropy: number): SceneMaterials {
  const road = requireSurfaceMaps('asphalt', anisotropy);
  // … 1 lines
  const raised: SurfaceBake = { ...road, map: surfaceTexture('asphalt-raised:map', anisotropy) };
  const footway = requireSurfaceMaps('footway', anisotropy);
  const kerb = requireSurfaceMaps('kerb', anisotropy);
  const verge = requireSurfaceMaps('verge', anisotropy);
  const deck = requireSurfaceMaps('deck', anisotropy);
  // … 33 lines
  const asphalt = keep(surface(road, 0xffffff, 1, 0.02, 1, true));
  const asphaltRaised = keep(surface(raised, 0xffffff, 1, 0.02, 0.9, true));
  // … 1 lines
  const footwayMaterial = keep(surface(footway, 0xffffff, 1, 0, 1, true));
  const kerbMaterial = keep(surface(kerb, 0xffffff, 1, 0, 0.85));
  const vergeMaterial = keep(surface(verge, 0xffffff, 1, 0, 0.9));
  const deckMaterial = keep(surface(deck, 0xffffff, 1, 0.02, 1));
  const parapetMaterial = keep(surface(deck, 0xffffff, 1, 0.02, 1));
  // … 2 lines
  applyDetail(asphalt, { kind: 'asphalt', macroTile: ASPHALT_TILE, albedo: 0.6, normal: 0.65, macroBlur: 1.6 }, anisotropy);
  applyDetail(asphaltRaised, { kind: 'asphalt', macroTile: ASPHALT_TILE, albedo: 0.6, normal: 0.65, macroBlur: 1.6 }, anisotropy);
  applyDetail(footwayMaterial, { kind: 'concrete', macroTile: FOOTWAY_TILE, albedo: 0.7, normal: 0.8, macroBlur: 0.6 }, anisotropy);
  // … 1 lines
  applyRoadEdges(asphalt, 'carriageway');
  applyRoadEdges(asphaltRaised, 'carriageway');
  applyRoadEdges(footwayMaterial, 'footway');
  applyDetail(kerbMaterial, { kind: 'concrete', macroTile: KERB_TILE, albedo: 0.55, normal: 0.7, macroBlur: 0.8 }, anisotropy);
  applyDetail(vergeMaterial, { kind: 'grass', macroTile: VERGE_TILE, albedo: 1, normal: 1.2, macroBlur: 2 }, anisotropy);
  applyDetail(deckMaterial, { kind: 'concrete', macroTile: DECK_TILE, albedo: 0.6, normal: 0.8, macroBlur: 1 }, anisotropy);
  applyDetail(parapetMaterial, { kind: 'concrete', macroTile: DECK_TILE, albedo: 0.6, normal: 0.8, macroBlur: 1 }, anisotropy);
  // … 1 lines
  applyLotOverlay(asphalt);
  applyLotOverlay(asphaltRaised);
  applyLotOverlay(footwayMaterial);
  applyLotOverlay(kerbMaterial);
  applyLotOverlay(vergeMaterial);
  applyLotOverlay(deckMaterial);
  // … 1 lines
  return {
    asphalt,
    asphaltRaised,
    footway: footwayMaterial,
    kerb: kerbMaterial,
    verge: vergeMaterial,
    deck: deckMaterial,
    parapet: parapetMaterial,
    concrete: keep(
      new MeshStandardMaterial({
        // … 1 lines
        map: deck.map,
        normalMap: deck.normalMap,
        roughnessMap: deck.roughnessMap,
      // … 3 lines
      }),
    ),
    // … 8 lines
    scale: {
      asphalt: ASPHALT_TILE,
      footway: FOOTWAY_TILE,
      kerb: KERB_TILE,
      verge: VERGE_TILE,
      deck: DECK_TILE,
    },
  // … 8 lines
  };
}

// src/render/materials.ts:44-68 slice of src/render/materials.ts#createMaterials.surface (shared): the feature's statements; "…" marks code left out
  const surface = (
    bake: SurfaceBake,
    tint: number,
    roughness: number,
    metalness: number,
    normalScale: number,
    /** Whether the mesh's own `color` attribute multiplies this material. */
    vertexColors = false,
  ): MeshStandardMaterial => {
    const material = new MeshStandardMaterial({
      color: new Color(tint),
      map: bake.map,
      normalMap: bake.normalMap,
      roughnessMap: bake.roughnessMap,
      roughness,
      metalness,
      side: FrontSide,
      // Both sides into the shadow map.
      shadowSide: DoubleSide,
      envMapIntensity: 0.55,
      vertexColors,
    });
    material.normalScale.set(normalScale, normalScale);
    return material;
  };

// src/render/materials.ts:142-146 slice of src/render/materials.ts#createMaterials.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      // … 2 lines
      disposeSurfaceTextures();
    },
