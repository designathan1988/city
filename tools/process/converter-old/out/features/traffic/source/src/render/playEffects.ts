// The code of src/render/playEffects.ts that is the feature's (the rest of the file is shared).

// src/render/playEffects.ts:40-56 slice of src/render/playEffects.ts#PlayEffectsContext (shared): the feature's statements; "…" marks code left out
export interface PlayEffectsContext {
  // … 1 lines
  readonly exhaust: Exhaust;
  readonly agents: AgentMeshes;
// … 12 lines
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
  // … 13 lines
  agents.setBleed((id, x, y, z) => {
    const last = lastDrip.get(id);
    if (last && Math.hypot(last.x - x, last.y - y) < m(0.9)) return;
    lastDrip.set(id, { x, y });
    if (lastDrip.size > 500) lastDrip.clear();
    ragdolls.drip(x + (Math.random() - 0.5) * m(0.3), y + (Math.random() - 0.5) * m(0.3), z + m(0.02), m(0.06 + Math.random() * 0.08));
  });
// … 275 lines
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

// src/render/playEffects.ts:373-386 slice of src/render/playEffects.ts#createPlayEffects.dropVehicle (shared): the feature's statements; "…" marks code left out
    dropVehicle(v) {
      const own = agents.carcass({ id: v.id, archetype: v.archetype }, true);
      if (!own) return;
      own.computeBoundingBox();
      const size = own.boundingBox!.getSize(new Vector3());
      // … 4 lines
      blast.debris({ shape: 'mesh', geometry: own, kind: 'metal', color: v.color, at: new Vector3(v.x, g + size.y / 2 + m(0.05), -v.y),
        size, turn, velocity: forward.clone().multiplyScalar(m(2.5)).add(new Vector3(v.dirX * m(0.8), 0, -v.dirY * m(0.8))),
        spin: forward.multiplyScalar(Math.random() < 0.5 ? 2.2 : -2.2) });
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
        // … 5 lines
        const own = v.archetype && v.id !== undefined ? agents.carcass({ id: v.id, archetype: v.archetype }) : null;
        if (own) {
          own.computeBoundingBox();
          const size = own.boundingBox!.getSize(new Vector3());
          const light = v.archetype!.shape === 'bicycle' || v.archetype!.shape === 'motorcycle';
          blast.debris({ shape: 'mesh', geometry: own, kind: 'char', color: v.color, at: new Vector3(v.x, g + size.y / 2 + m(0.05), -v.y),
            size, turn, velocity: light ? push.clone().multiplyScalar(1.6) : push, spin: light ? spin.multiplyScalar(2) : spin,
            burn: light ? 8 + Math.random() * 6 : 30 + Math.random() * 20 });
          exhaust.burst(v.x, v.y, g + m(1), light ? 15 : 40, 5, m(light ? 0.6 : 1.2), m(1.4), 1.0);
          if (light) continue;
        } else {
          const charred = new Color(v.color).lerp(new Color(0x1f1b18), 0.95).getHex();
          blast.debris({ shape: 'car', kind: 'char', color: charred, at: new Vector3(v.x, g + v.height * 0.45, -v.y),
            size: new Vector3(v.length, v.height * 0.85, v.width), turn, velocity: push, spin, burn: 30 + Math.random() * 20 });
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

// src/render/playEffects.ts:567-574 slice of src/render/playEffects.ts#createPlayEffects.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      // … 5 lines
      agents.setBleed(() => {});
    },
