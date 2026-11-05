// The code of src/render/ragdoll.ts that is the feature's (the rest of the file is shared).

// src/render/ragdoll.ts:401-1956 slice of src/render/ragdoll.ts#createRagdolls (shared): the feature's statements; "…" marks code left out
export function createRagdolls(exhaust: Exhaust, getUp: GetUp, gore: Gore | null = null): Ragdolls {
// … 1554 lines
}

// src/render/ragdoll.ts:612-700 slice of src/render/ragdoll.ts#createRagdolls.spawn (shared): the feature's statements; "…" marks code left out
  const spawn = (c: Casualty, citizens: RagdollCitizens, world: RagdollWorld,
    given?: { index: number; palette: Float32Array; transform: Matrix4 }): void => {
    // … 55 lines
    if (c.kind === 'torn') {
      // … 5 lines
      exhaust.burst(chest.x, -chest.z, chest.y, 90, 4, m(0.6), m(0.06), 1.2);
    // … 7 lines
    } else if (c.kind === 'dead') {
      exhaust.burst(chest.x, -chest.z, chest.y, 24, 4, m(0.25), m(0.045), 0.9);
    // … 6 lines
    }
  // … 8 lines
  };

// src/render/ragdoll.ts:721-741 slice of src/render/ragdoll.ts#createRagdolls.mush (shared): the feature's statements; "…" marks code left out
  const mush = (body: Body, citizens: RagdollCitizens): void => {
    // … 10 lines
    exhaust.burst(at.x, -at.z, at.y, 80, 4, m(0.5), m(0.06), 1.1);
  // … 8 lines
  };

// src/render/ragdoll.ts:744-751 slice of src/render/ragdoll.ts#createRagdolls.openBelly (shared): the feature's statements; "…" marks code left out
  const openBelly = (body: Body, kick: Vector3, organs = 0): void => {
    // … 5 lines
    exhaust.burst(body.p[BEL]!.x, -body.p[BEL]!.z, body.p[BEL]!.y, 30, 4, m(0.2), m(0.04), 0.9);
  };

// src/render/ragdoll.ts:800-842 slice of src/render/ragdoll.ts#createRagdolls.detach (shared): the feature's statements; "…" marks code left out
  const detach = (body: Body, limb: Severable, citizens: RagdollCitizens, kick: Vector3): void => {
    // … 34 lines
    exhaust.burst(at.x, -at.z, at.y, 30, 4, m(0.22), m(0.04), 0.9);
  // … 6 lines
  };

// src/render/ragdoll.ts:937-987 slice of src/render/ragdoll.ts#createRagdolls.shootBody (shared): the feature's statements; "…" marks code left out
    shootBody(a, b) {
      // … 27 lines
      exhaust.burst(at.x + dir.x * m(0.15), -(at.z + dir.z * m(0.15)), at.y, 26, 4, m(0.18), m(0.035), 0.8);
    // … 21 lines
    },

// src/render/ragdoll.ts:1402-1436 slice of src/render/ragdoll.ts#createRagdolls.sync (shared): the feature's statements; "…" marks code left out
  function sync(body: Body, world: RagdollWorld): void {
    // … 25 lines
    if (body.torn && moved > m(0.01)) {
      // … 1 lines
      if (body.spray <= 0) {
        // … 2 lines
        exhaust.burst(body.p[k]!.x, -body.p[k]!.z, body.p[k]!.y, 3, 4, m(0.08), m(0.04), 0.7);
      }
    }
  }

// src/render/ragdoll.ts:1579-1646 slice of src/render/ragdoll.ts#createRagdolls.step (shared): the feature's statements; "…" marks code left out
  function step(body: Body, world: RagdollWorld): void {
    // … 58 lines
    if (body.torn && moved > m(0.01)) {
      // … 1 lines
      if (body.spray <= 0) {
        // … 2 lines
        exhaust.burst(p[k]!.x, -p[k]!.z, p[k]!.y, 3, 4, m(0.08), m(0.04), 0.7);
      }
    }
  }

// src/render/ragdoll.ts:1705-1774 slice of src/render/ragdoll.ts#createRagdolls.collide (shared): the feature's statements; "…" marks code left out
  function collide(body: Body, walls: readonly Wall[], last: boolean, world: RagdollWorld): void {
    // … 2 lines
    for (let k = 0; k < p.length; k++) {
      // … 5 lines
      if (v.y < floor) {
        // … 11 lines
        if (bloody && last && vy < -m(4) * STEP && body.splats < 12 && Math.random() < 0.35) {
          // … 2 lines
          exhaust.burst(v.x, -v.z, floor, 5, 4, m(0.08), m(0.04), 0.6);
        }
      }
      for (const wall of walls) {
        // … 34 lines
        if (bloody && last && vn < -m(3) * STEP && body.splats < 12) {
          // … 2 lines
          exhaust.burst(v.x, -v.z, v.y, 6, 4, m(0.1), m(0.04), 0.6);
        }
      }
    }
  }
