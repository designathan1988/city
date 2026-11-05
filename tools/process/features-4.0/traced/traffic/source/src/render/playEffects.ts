// The code of src/render/playEffects.ts that is the feature's (the rest of the file is shared).

// src/render/playEffects.ts:40-56 slice of src/render/playEffects.ts#PlayEffectsContext (shared): the feature's statements; "…" marks code left out
export interface PlayEffectsContext {
  // … 1 lines
  readonly exhaust: Exhaust;
// … 13 lines
}

// src/render/playEffects.ts:58-92 slice of src/render/playEffects.ts#PlayEffects (shared): the feature's statements; "…" marks code left out
export interface PlayEffects {
  // … 19 lines
  dropVehicle(v: { id: number; archetype: Archetype; x: number; y: number; angle: number; color: number; dirX: number; dirY: number }): void;
// … 13 lines
}

// src/render/playEffects.ts:160-576 slice of src/render/playEffects.ts#createPlayEffects (shared): the feature's statements; "…" marks code left out
export function createPlayEffects(ctx: PlayEffectsContext): PlayEffects {
  // … 60 lines
  const ragdollWorld: RagdollWorld = {
    // … 2 lines
    wallsNear(x, y, reach) {
      // … 38 lines
      if (ragdollSim) {
        // … 1 lines
        for (const which of ['traffic', 'parked'] as const) for (const v of ragdollSim.vehiclesIn(nearBox, nearVehicles, which)) {
          // … 6 lines
          const hl = v.archetype.length / 2, hw = v.archetype.width / 2;
          // … 1 lines
          const ring = [[hl, hw], [hl, -hw], [-hl, -hw], [-hl, hw]].map(([u, w]) => ({ x: vx + c * u! - sn * w!, y: vy + sn * u! + c * w! }));
          out.push({ ring, top: ragdollWorld.groundAt(vx, vy) + v.archetype.height * 0.85 });
        }
      }
    // … 1 lines
    },
  };
// … 295 lines
}

// src/render/playEffects.ts:360-372 slice of src/render/playEffects.ts#createPlayEffects.vehicleHit (shared): the feature's statements; "…" marks code left out
    vehicleHit(x, y, z, dirX, dirY, glass, blood) {
      if (glass) {
        // … 5 lines
        if (blood) exhaust.burst(x + dirX * m(0.4), y + dirY * m(0.4), z, 40, 4, m(0.2), m(0.04), 0.8);
      } else {
        exhaust.burst(x, y, z, 18, 6, m(0.12), m(0.2), 0.35);
      }
    // … 1 lines
    },

// src/render/playEffects.ts:387-398 slice of src/render/playEffects.ts#createPlayEffects.wound (shared): the feature's statements; "…" marks code left out
    wound(x, y, z, dirX, dirY, severed) {
      // … 1 lines
      exhaust.burst(x + dirX * m(0.15), y + dirY * m(0.15), z, severed ? 36 : 18, 4, m(severed ? 0.22 : 0.14), m(0.035), 0.7);
    // … 8 lines
    },

// src/render/playEffects.ts:413-555 slice of src/render/playEffects.ts#createPlayEffects.explode (shared): the feature's statements; "…" marks code left out
    explode(x, y, z, radius, hit) {
      // … 67 lines
      for (const v of hit.vehicles) {
        // … 6 lines
        if (own) {
          // … 2 lines
          const light = v.archetype!.shape === 'bicycle' || v.archetype!.shape === 'motorcycle';
          blast.debris({ shape: 'mesh', geometry: own, kind: 'char', color: v.color, at: new Vector3(v.x, g + size.y / 2 + m(0.05), -v.y),
            size, turn, velocity: light ? push.clone().multiplyScalar(1.6) : push, spin: light ? spin.multiplyScalar(2) : spin,
            burn: light ? 8 + Math.random() * 6 : 30 + Math.random() * 20 });
          exhaust.burst(v.x, v.y, g + m(1), light ? 15 : 40, 5, m(light ? 0.6 : 1.2), m(1.4), 1.0);
          if (light) continue;
        } else {
        // … 3 lines
        }
        // … 5 lines
        exhaust.burst(v.x, v.y, g + m(1), 40, 5, m(1.2), m(1.4), 1.0);
      }
      for (const item of hit.items) {
        // … 2 lines
        if (item.kind === 'tree') {
          // … 17 lines
          exhaust.burst(item.x, item.y, g + tall * 0.7, 10, 5, m(1.5), m(1.2), 1);
        } else if (item.kind === 'shrub') {
        // … 7 lines
        } else if (item.kind === 'lamp') {
          // … 6 lines
          exhaust.burst(item.x, item.y, g + m(6), 30, 6, m(0.3), m(0.12), 0.8);
        } else {
        // … 5 lines
        }
      }
    },
