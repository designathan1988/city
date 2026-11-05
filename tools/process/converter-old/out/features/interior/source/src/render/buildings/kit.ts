// The code of src/render/buildings/kit.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/kit.ts:146-179 slice of src/render/buildings/kit.ts#BuildingKit (shared): the feature's statements; "…" marks code left out
export interface BuildingKit {
  // … 27 lines
  furniture(): { readonly geometry: Readonly<Record<FurnitureKind, BufferGeometry>>; readonly material: MeshStandardMaterial };
// … 4 lines
}

// src/render/buildings/kit.ts:474-729 slice of src/render/buildings/kit.ts#createBuildingKit (shared): the feature's statements; "…" marks code left out
export function createBuildingKit(anisotropy = 4): BuildingKit {
  // … 88 lines
  for (const kind of ['glass', 'glassDark'] as const) roomBehindGlass(own[kind] as MeshStandardMaterial, { tones, roomsAwake, roomDaylight, tvClock });
  // … 103 lines
  let furniture: { geometry: Record<FurnitureKind, BufferGeometry>; material: MeshStandardMaterial } | null = null;
// … 61 lines
}

// src/render/buildings/kit.ts:697-717 method createBuildingKit.setNight: the feature's own
    setNight(dark, hour) {
      if (hour !== undefined) {
        roomsAwake.value = awakeShare(hour);
        outsideOn.value = outsideLit(hour);
      }
      // Rooms lit behind the glass: a warm glow, brighter than any lit wall (`lightLevels.ts`), less behind the darker glass.
      const room = NIGHT_LUMINANCE.window / ROOM_LIT_MEAN;
      for (const [kind, k] of [['glass', 1], ['glassDark', 0.64]] as const) {
        const mat = material[kind] as MeshStandardMaterial;
        mat.emissive.setHex(0xffffff);
        mat.emissiveIntensity = dark * k * room;
      }
      tvClock.value = performance.now() / 1000;
      massingNight.value = dark * room;
      roomDaylight.value = (1 - dark) * ROOM_DAYLIGHT;
      // The lanterns at the front doors, and their light on the wall.
      (material.lamp as MeshStandardMaterial).emissiveIntensity = dark * NIGHT_LUMINANCE.porchLamp / luminanceOf(PORCH_LIGHT);
      const wash = material.wash as MeshBasicMaterial;
      wash.opacity = 0.55 * dark;
      wash.visible = dark > 0.02;
    },

// src/render/buildings/kit.ts:718-723 method createBuildingKit.setGhostValid: the feature's own
    setGhostValid(valid) {
      for (const m of [ghostShell, ghostParts]) {
        m.color.setHex(valid ? 0x65e5c3 : 0xff6f63);
        m.emissive.setHex(valid ? 0x1d5a4a : 0x5a1d1d);
      }
    },

// src/render/buildings/kit.ts:724-727 method createBuildingKit.dispose: the feature's own
    dispose() {
      for (const g of geometries) g.dispose();
      for (const m of unique) m.dispose();
    },
