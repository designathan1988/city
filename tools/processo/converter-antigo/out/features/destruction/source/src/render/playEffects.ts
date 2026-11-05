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

// src/render/playEffects.ts:160-576 slice of src/render/playEffects.ts#createPlayEffects (shared): the feature's statements; "…" marks code left out
export function createPlayEffects(ctx: PlayEffectsContext): PlayEffects {
  // … 2 lines
  const destruction = createDestruction(exhaust, buildings.chunkOf);
  scene.add(destruction.group);
  destruction.onRuined = () => { buildings.setRuined(destruction.ruined); };
  // … 1 lines
  const casualties = createCasualties();
  scene.add(casualties.group);
  // … 1 lines
  const blast = createBlast(exhaust, ctx.lights.blast);
  scene.add(blast.group);
  void compileAhead(blast.group);
  // … 48 lines
  const ragdollWorld: RagdollWorld = {
    // … 2 lines
    wallsNear(x, y, reach) {
      // … 7 lines
      for (const b of land?.buildings.near(x - near, y - near, x + near, y + near) ?? []) {
        if (destruction.ruined.has(b.id) || Math.hypot(b.x - x, b.y - y) > near) continue;
      // … 10 lines
      }
    // … 34 lines
    },
  };
  // … 1 lines
  ragdolls.onGore = (x, y, z, dx, dy, speed, kind) => {
    // … 2 lines
    for (let k = 0; k < organs; k++) {
      // … 1 lines
      blast.debris({ shape: Math.random() < 0.7 ? 'cylinder' : 'box', kind: 'flesh', color: [0x3a0507, 0x4a0b0e, 0x561418, 0x2e0405][k % 4]!,
        at: new Vector3(x, z, -y), size: new Vector3(s0, s0 * (0.6 + Math.random()), s0 * (0.7 + Math.random() * 0.6)),
        velocity: new Vector3(dx * speed * 0.5 + (Math.random() - 0.5) * m(6), m(2 + Math.random() * 4), -dy * speed * 0.5 + (Math.random() - 0.5) * m(6)) });
    }
  };
// … 284 lines
}

// src/render/playEffects.ts:207-216 slice of src/render/playEffects.ts#createPlayEffects.groundChanged (shared): the feature's statements; "…" marks code left out
  const groundChanged = (): void => {
    // … 3 lines
    const ruined = destruction.ruined.size;
    if (land === groundSeen.doc && serial === groundSeen.serial && built === groundSeen.built && ruined === groundSeen.ruined) return;
    groundSeen.doc = land; groundSeen.serial = serial; groundSeen.built = built; groundSeen.ruined = ruined;
  // … 2 lines
  };

// src/render/playEffects.ts:342-349 slice of src/render/playEffects.ts#createPlayEffects.update (shared): the feature's statements; "…" marks code left out
    update(wallDt, fxDt) {
      destruction.update(wallDt);
      casualties.sync(ragdolls.decals);
      blast.update(fxDt, ragdollWorld);
    // … 3 lines
    },

// src/render/playEffects.ts:350-350 slice of src/render/playEffects.ts#createPlayEffects.shake (shared): the feature's statements; "…" marks code left out
    shake: () => blast.shake(),

// src/render/playEffects.ts:351-351 slice of src/render/playEffects.ts#createPlayEffects.busy (shared): the feature's statements; "…" marks code left out
    busy: () => blast.active() || ragdolls.active(),

// src/render/playEffects.ts:352-352 slice of src/render/playEffects.ts#createPlayEffects.castersMoving (shared): the feature's statements; "…" marks code left out
    castersMoving: () => blast.casting() || ragdolls.active(),

// src/render/playEffects.ts:359-359 slice of src/render/playEffects.ts#createPlayEffects.clearCasualties (shared): the feature's statements; "…" marks code left out
    clearCasualties() { ragdolls.clear(); gore.clear(); blast.clear(); onAssetsReady(); },

// src/render/playEffects.ts:360-372 slice of src/render/playEffects.ts#createPlayEffects.vehicleHit (shared): the feature's statements; "…" marks code left out
    vehicleHit(x, y, z, dirX, dirY, glass, blood) {
      if (glass) {
        for (let k = 0; k < 14; k++) {
          blast.debris({ shape: 'box', kind: 'glass', at: new Vector3(x, z, -y),
            size: new Vector3(m(0.03 + Math.random() * 0.05), m(0.006), m(0.03 + Math.random() * 0.05)),
            velocity: new Vector3(dirX * m(2) + (Math.random() - 0.5) * m(2.5), m(0.5 + Math.random() * 1.5), -dirY * m(2) + (Math.random() - 0.5) * m(2.5)) });
        }
      // … 1 lines
      } else {
      // … 1 lines
      }
    // … 1 lines
    },

// src/render/playEffects.ts:373-386 slice of src/render/playEffects.ts#createPlayEffects.dropVehicle (shared): the feature's statements; "…" marks code left out
    dropVehicle(v) {
      // … 8 lines
      blast.debris({ shape: 'mesh', geometry: own, kind: 'metal', color: v.color, at: new Vector3(v.x, g + size.y / 2 + m(0.05), -v.y),
        size, turn, velocity: forward.clone().multiplyScalar(m(2.5)).add(new Vector3(v.dirX * m(0.8), 0, -v.dirY * m(0.8))),
        spin: forward.multiplyScalar(Math.random() < 0.5 ? 2.2 : -2.2) });
    // … 1 lines
    },

// src/render/playEffects.ts:406-412 slice of src/render/playEffects.ts#createPlayEffects.strikeBuilding (shared): the feature's statements; "…" marks code left out
    strikeBuilding(b, x, y, z, strength) {
      // … 2 lines
      const down = destruction.hit(b, floor, x, y, z, strength, ctx.camera().getWorldDirection(new Vector3()));
      buildings.setRuined(destruction.ruined);
      return down;
    },

// src/render/playEffects.ts:413-555 slice of src/render/playEffects.ts#createPlayEffects.explode (shared): the feature's statements; "…" marks code left out
    explode(x, y, z, radius, hit) {
      // … 1 lines
      blast.explode(x, y, z, radius, hit.ground);
      if (hit.crater && hit.ground !== 'building') blast.crater(x, y, ground(x, y), radius * 0.75, hit.ground);
      // … 6 lines
      for (const pole of hit.poles) {
        // … 6 lines
        if (pole.mode === 'whole') {
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + POLE_HEIGHT / 2 + m(0.05), -pole.y),
            size: new Vector3(r, POLE_HEIGHT, r), velocity: push.clone().multiplyScalar(0.4), spin: axis.clone().multiplyScalar(0.9 + Math.random() * 0.6) });
        } else if (pole.mode === 'snap') {
          // … 2 lines
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + cut / 2, -pole.y),
            size: new Vector3(r * 1.1, cut, r * 1.1), velocity: new Vector3(), spin: new Vector3() });
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + cut + (POLE_HEIGHT - cut) / 2 + m(0.1), -pole.y),
            size: new Vector3(r, POLE_HEIGHT - cut, r), velocity: push.clone().multiplyScalar(0.7), spin: axis.clone().multiplyScalar(1.5 + Math.random()) });
        } else {
          // … 2 lines
          while (h < POLE_HEIGHT - m(0.5)) {
            // … 1 lines
            blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + h + l / 2, -pole.y),
              size: new Vector3(r * (0.6 + Math.random() * 0.4), l, r * (0.6 + Math.random() * 0.4)),
              velocity: push.clone().multiplyScalar(0.8 + Math.random()).add(new Vector3((Math.random() - 0.5) * m(4), m(2 + Math.random() * 4), (Math.random() - 0.5) * m(4))) });
          // … 1 lines
          }
        }
        // … 1 lines
        blast.debris({ shape: 'box', kind: 'wood', color: wood, at: top.clone(), size: new Vector3(POLE_ARM_HALF * 2, m(0.1), m(0.1)),
          velocity: push.clone().add(new Vector3(0, m(2), 0)) });
        if (pole.lamp) blast.debris({ shape: 'box', kind: 'metal', at: top.clone().add(new Vector3(POLE_LAMP_REACH * 0.5, 0, 0)), size: new Vector3(m(0.5), m(0.14), m(0.26)), velocity: push.clone().multiplyScalar(1.2) });
        // … 1 lines
        blast.arc(top, 1.5 + Math.random() * 2);
      }
      for (const w of hit.wires) {
        // … 3 lines
        for (const offset of [-0.8, 0, 0.8]) {
          // … 3 lines
          blast.wire(from, to, away(w.toX, w.toY, m(3)));
        }
      }
      for (const post of hit.posts) {
        // … 3 lines
        blast.debris({ shape: 'cylinder', kind: 'metal', color: 0x2a2d31, at: new Vector3(post.x, g + m(3.1) + m(0.05), -post.y),
          size: new Vector3(m(0.165), m(6.2), m(0.165)), velocity: push.clone().multiplyScalar(0.3), spin: axis.multiplyScalar(1.2) });
        // … 1 lines
        blast.debris({ shape: 'box', kind: 'metal', color: 0x1b1d20, at: head, size: new Vector3(m(0.35), m(1.0), m(0.35)), velocity: push.clone().add(new Vector3(0, m(3), 0)) });
        blast.debris({ shape: 'cylinder', kind: 'metal', color: 0x2a2d31, at: head.clone().lerp(new Vector3(post.x, g + m(5.9), -post.y), 0.5),
          size: new Vector3(m(0.06), m(3.5), m(0.06)), turn: new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), Math.PI / 2), velocity: push.clone() });
        blast.arc(head, 1 + Math.random() * 2);
      }
      for (const v of hit.vehicles) {
        // … 6 lines
        if (own) {
          // … 3 lines
          blast.debris({ shape: 'mesh', geometry: own, kind: 'char', color: v.color, at: new Vector3(v.x, g + size.y / 2 + m(0.05), -v.y),
            size, turn, velocity: light ? push.clone().multiplyScalar(1.6) : push, spin: light ? spin.multiplyScalar(2) : spin,
            burn: light ? 8 + Math.random() * 6 : 30 + Math.random() * 20 });
        // … 2 lines
        } else {
          // … 1 lines
          blast.debris({ shape: 'car', kind: 'char', color: charred, at: new Vector3(v.x, g + v.height * 0.45, -v.y),
            size: new Vector3(v.length, v.height * 0.85, v.width), turn, velocity: push, spin, burn: 30 + Math.random() * 20 });
        }
        for (let k = 0; k < 2; k++) {
          blast.debris({ shape: 'cylinder', kind: 'char', color: 0x141414, at: new Vector3(v.x, g + m(0.35), -v.y),
            size: new Vector3(m(0.32), m(0.22), m(0.32)), turn: new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), Math.PI / 2),
            velocity: push.clone().multiplyScalar(0.6 + Math.random()).add(new Vector3((Math.random() - 0.5) * m(8), m(2 + Math.random() * 4), (Math.random() - 0.5) * m(8))) });
        }
      // … 1 lines
      }
      for (const item of hit.items) {
        // … 2 lines
        if (item.kind === 'tree') {
          // … 3 lines
          blast.debris({ shape: 'cylinder', kind: 'char', color: 0x2b211a, at: new Vector3(item.x, g + cut / 2, -item.y), size: new Vector3(m(0.22), cut, m(0.22)),
            velocity: new Vector3(), spin: new Vector3() });
          blast.debris({ shape: 'cylinder', kind: 'wood', at: new Vector3(item.x, g + cut + (tall - cut) / 2, -item.y), size: new Vector3(m(0.18), tall - cut, m(0.18)),
            velocity: push.clone().multiplyScalar(0.6), spin: axis.clone().multiplyScalar(1.4 + Math.random()), burn: 12 + Math.random() * 10 });
          // … 1 lines
          for (let k = 0; k < 6; k++) {
            blast.debris({ shape: 'box', kind: 'wood', at: new Vector3(item.x, g + cut, -item.y), size: new Vector3(m(0.05), m(0.3 + Math.random() * 0.5), m(0.05)),
              velocity: push.clone().add(new Vector3((Math.random() - 0.5) * m(5), m(2 + Math.random() * 4), (Math.random() - 0.5) * m(5))) });
          }
          // … 1 lines
          for (let k = 0; k < 24; k++) {
            blast.debris({ shape: 'box', kind: 'leaf', at: new Vector3(item.x, g + tall * 0.85, -item.y), size: new Vector3(m(0.4 + Math.random() * 0.6), m(0.1), m(0.4 + Math.random() * 0.6)),
              velocity: push.clone().add(new Vector3((Math.random() - 0.5) * m(7), m(1 + Math.random() * 5), (Math.random() - 0.5) * m(7))) });
          }
        // … 1 lines
        } else if (item.kind === 'shrub') {
          // … 1 lines
          blast.debris({ shape: 'cylinder', kind: 'wood', at: new Vector3(item.x, g + tall / 2, -item.y), size: new Vector3(m(0.14), tall, m(0.14)),
            velocity: push.clone().multiplyScalar(0.4), spin: new Vector3(push.z, 0, -push.x).normalize().multiplyScalar(1.2) });
          for (let k = 0; k < 14; k++) {
            blast.debris({ shape: 'box', kind: 'leaf', at: new Vector3(item.x, g + tall * 0.8, -item.y), size: new Vector3(m(0.3), m(0.05), m(0.3)),
              velocity: push.clone().add(new Vector3((Math.random() - 0.5) * m(6), m(2 + Math.random() * 5), (Math.random() - 0.5) * m(6))) });
          }
        } else if (item.kind === 'lamp') {
          // … 2 lines
          blast.debris({ shape: 'cylinder', kind: 'metal', color: 0x3a3e43, at: new Vector3(item.x, g + m(3), -item.y), size: new Vector3(m(0.1), m(6), m(0.1)),
            velocity: push.clone().multiplyScalar(0.5), spin: axis.multiplyScalar(1.6) });
          blast.debris({ shape: 'box', kind: 'metal', color: 0x2a2d31, at: new Vector3(item.x, g + m(6), -item.y), size: new Vector3(m(0.6), m(0.15), m(0.3)),
            velocity: push.clone().add(new Vector3(0, m(4), 0)) });
        // … 1 lines
        } else {
          for (let k = 0; k < 4; k++) {
            blast.debris({ shape: 'box', kind: item.kind === 'bench' ? 'wood' : 'metal', at: new Vector3(item.x, g + m(0.5), -item.y),
              size: new Vector3(m(0.2 + Math.random() * 0.5), m(0.06 + Math.random() * 0.2), m(0.1 + Math.random() * 0.3)),
              velocity: push.clone().add(new Vector3((Math.random() - 0.5) * m(4), m(2 + Math.random() * 5), (Math.random() - 0.5) * m(4))) });
          }
        }
      }
    },

// src/render/playEffects.ts:556-556 slice of src/render/playEffects.ts#createPlayEffects.burn (shared): the feature's statements; "…" marks code left out
    burn: (x, y, z, size, seconds) => blast.burn(x, y, z, size, seconds),

// src/render/playEffects.ts:557-557 slice of src/render/playEffects.ts#createPlayEffects.soot (shared): the feature's statements; "…" marks code left out
    soot: (x, y, z, r) => blast.soot(x, y, z, r),

// src/render/playEffects.ts:558-558 slice of src/render/playEffects.ts#createPlayEffects.geyser (shared): the feature's statements; "…" marks code left out
    geyser: (x, y, z, seconds) => blast.geyser(x, y, z, seconds),

// src/render/playEffects.ts:559-559 slice of src/render/playEffects.ts#createPlayEffects.sparkAt (shared): the feature's statements; "…" marks code left out
    sparkAt: (x, y, z, seconds) => blast.arc(new Vector3(x, z, -y), seconds),

// src/render/playEffects.ts:560-560 slice of src/render/playEffects.ts#createPlayEffects.leak (shared): the feature's statements; "…" marks code left out
    leak: (x, y, z, seconds) => blast.leak(x, y, z, seconds),

// src/render/playEffects.ts:561-561 slice of src/render/playEffects.ts#createPlayEffects.onBuildingDown (shared): the feature's statements; "…" marks code left out
    onBuildingDown: (listener) => { destruction.onDown = listener; },

// src/render/playEffects.ts:563-566 slice of src/render/playEffects.ts#createPlayEffects.forgetRuin (shared): the feature's statements; "…" marks code left out
    forgetRuin(id) {
      // … 1 lines
      buildings.setRuined(destruction.ruined);
    },

// src/render/playEffects.ts:567-574 slice of src/render/playEffects.ts#createPlayEffects.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      ragdolls.clear(); gore.clear(); blast.clear();
      // … 1 lines
      scene.remove(destruction.group, casualties.group, blast.group, gore.group, muzzle);
      // … 1 lines
      destruction.dispose(); casualties.dispose(); blast.dispose(); gore.dispose();
    // … 1 lines
    },
