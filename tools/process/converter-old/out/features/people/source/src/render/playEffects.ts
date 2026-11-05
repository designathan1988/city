// The code of src/render/playEffects.ts that is the feature's (the rest of the file is shared).

// src/render/playEffects.ts:58-92 slice of src/render/playEffects.ts#PlayEffects (shared): the feature's statements; "…" marks code left out
export interface PlayEffects {
  // … 15 lines
  shootBody(a: readonly [number, number, number], b: readonly [number, number, number]): { alive: number; part: BodyPart } | 'hit' | null;
  // … 4 lines
  wound(x: number, y: number, z: number, dirX: number, dirY: number, severed: Severable | null): void;
// … 12 lines
}

// src/render/playEffects.ts:315-340 slice of src/render/playEffects.ts#createPlayEffects.frame (shared): the feature's statements; "…" marks code left out
    frame(citizens, sim, wallDt, fxDt) {
      // … 9 lines
      for (const ped of sim.pedViews) {
        const g = ped.gesture;
        if (g?.kind !== 'fall') continue;
        ragdollDown.add(ped.id);
        if (g.t < 0.5 && !ragdolls.hides(ped.id)) {
          // Knocked from a point (a punch, a shove): down away from it; else a trip, forwards.
          const away = g.fromX !== undefined && g.fromY !== undefined && Math.hypot(ped.x - g.fromX, ped.y - g.fromY) > 1e-3
            ? Math.atan2(ped.y - g.fromY, ped.x - g.fromX) : null;
          ragdolls.trip(ped.id, ped.heading, citizens, ragdollWorld, away);
        }
      }
    // … 4 lines
    },
