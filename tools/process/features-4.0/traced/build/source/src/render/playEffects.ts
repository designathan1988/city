// The code of src/render/playEffects.ts that is the feature's (the rest of the file is shared).

// src/render/playEffects.ts:40-56 slice of src/render/playEffects.ts#PlayEffectsContext (shared): the feature's statements; "…" marks code left out
export interface PlayEffectsContext {
  // … 3 lines
  readonly buildings: {
    chunkOf: Parameters<typeof createDestruction>[1];
    setRuined(ruined: ReadonlySet<number>): void;
    lotHeightAt(id: BuildingId, x: number, y: number): number;
  };
// … 7 lines
}

// src/render/playEffects.ts:58-92 slice of src/render/playEffects.ts#PlayEffects (shared): the feature's statements; "…" marks code left out
export interface PlayEffects {
  // … 22 lines
  strikeBuilding(b: Building, x: number, y: number, z: number, strength: number): boolean;
// … 10 lines
}

// src/render/playEffects.ts:160-576 slice of src/render/playEffects.ts#createPlayEffects (shared): the feature's statements; "…" marks code left out
export function createPlayEffects(ctx: PlayEffectsContext): PlayEffects {
  // … 32 lines
  const ragdollGround = createGroundCorners((cx, cy) => {
    // … 2 lines
    for (const b of lotsAt(cx, cy)) {
      h = buildings.lotHeightAt(b.id, cx, cy);
      if (Number.isFinite(h)) break;
    }
    if (!Number.isFinite(h)) h = ctx.pavedHeightAt(cx, cy);
    if (!Number.isFinite(h)) h = ctx.renderedHeightAt(cx, cy);
    return h;
  });
  // … 17 lines
  const ragdollWorld: RagdollWorld = {
    // … 2 lines
    wallsNear(x, y, reach) {
      // … 7 lines
      for (const b of land?.buildings.near(x - near, y - near, x + near, y + near) ?? []) {
        if (destruction.ruined.has(b.id) || Math.hypot(b.x - x, b.y - y) > near) continue;
        const floor = floorHeight(b, ctx.naturalRenderedHeightAt, ctx.pavedHeightAt);
        for (const v of b.volumes) {
          if (v.base !== 0 || v.mode === 'void' || v.mode === 'intersect' || v.open) continue;
          // The walls up to the eaves, the roof over them: a body lands on it and slides down a pitch.
          const eaves = floor + volumeElevation(b, v, v.base + v.storeys.length);
          out.push({ ring: volumeCorners(b, v), top: eaves + roofRise(b, v), roof: (wx, wy) => eaves + Math.max(0, roofHeightAt(b, v, worldToLocal(b, { x: wx, y: wy }))) });
        }
        for (const e of b.elements ?? []) {
          if ((e.kind === 'wall' || e.kind === 'fence' || e.kind === 'hedge') && e.z <= 0.01) out.push({ ring: elementRing(b, e), top: floor + e.h });
        }
      }
    // … 34 lines
    },
  };
// … 295 lines
}

// src/render/playEffects.ts:190-191 slice of src/render/playEffects.ts#createPlayEffects.lotsAt (shared): the feature's statements; "…" marks code left out
  const lotsAt = (x: number, y: number): readonly Building[] =>
    ragdollSim?.doc.buildings.near(x - LOT_GROW, y - LOT_GROW, x + LOT_GROW, y + LOT_GROW) ?? [];

// src/render/playEffects.ts:207-216 slice of src/render/playEffects.ts#createPlayEffects.groundChanged (shared): the feature's statements; "…" marks code left out
  const groundChanged = (): void => {
    // … 2 lines
    const built = land ? land.buildings.size : -1;
    // … 1 lines
    if (land === groundSeen.doc && serial === groundSeen.serial && built === groundSeen.built && ruined === groundSeen.ruined) return;
    groundSeen.doc = land; groundSeen.serial = serial; groundSeen.built = built; groundSeen.ruined = ruined;
  // … 2 lines
  };

// src/render/playEffects.ts:406-412 slice of src/render/playEffects.ts#createPlayEffects.strikeBuilding (shared): the feature's statements; "…" marks code left out
    strikeBuilding(b, x, y, z, strength) {
      // … 1 lines
      const floor = floorHeight(b, ctx.naturalRenderedHeightAt, ctx.pavedHeightAt);
      const down = destruction.hit(b, floor, x, y, z, strength, ctx.camera().getWorldDirection(new Vector3()));
      // … 1 lines
      return down;
    },
