// The code of src/render/buildings/kit.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/kit.ts:474-729 slice of src/render/buildings/kit.ts#createBuildingKit (shared): the feature's statements; "…" marks code left out
export function createBuildingKit(anisotropy = 4): BuildingKit {
  // … 28 lines
  const own: Record<Exclude<PartKind, ShellPartKind>, Material> = {
    // … 45 lines
    wash: new MeshBasicMaterial({
      map: lightPoolTexture(), color: PORCH_LIGHT, transparent: true, opacity: 0, blending: AdditiveBlending,
    // … 1 lines
    }),
  };
// … 175 lines
}

// src/render/buildings/kit.ts:697-717 slice of src/render/buildings/kit.ts#createBuildingKit.setNight (shared): the feature's statements; "…" marks code left out
    setNight(dark, hour) {
      // … 5 lines
      const room = NIGHT_LUMINANCE.window / ROOM_LIT_MEAN;
      for (const [kind, k] of [['glass', 1], ['glassDark', 0.64]] as const) {
        // … 2 lines
        mat.emissiveIntensity = dark * k * room;
      }
      // … 1 lines
      massingNight.value = dark * room;
      // … 2 lines
      (material.lamp as MeshStandardMaterial).emissiveIntensity = dark * NIGHT_LUMINANCE.porchLamp / luminanceOf(PORCH_LIGHT);
    // … 3 lines
    },
