// The code of src/render/blast.ts that is the feature's (the rest of the file is shared).

// src/render/blast.ts:208-750 slice of src/render/blast.ts#createBlast (shared): the feature's statements; "…" marks code left out
export function createBlast(exhaust: Exhaust, light: PointLight): Blast {
// … 541 lines
}

// src/render/blast.ts:442-474 slice of src/render/blast.ts#createBlast.stepRope (shared): the feature's statements; "…" marks code left out
  const stepRope = (rope: Rope, dt: number, world: BlastWorld): void => {
    // … 27 lines
    if (rope.age < rope.sparks && end.y < world.groundAt(end.x, -end.z) + m(0.3) && Math.random() < dt * 9) {
      exhaust.burst(end.x, -end.z, end.y + m(0.05), 10, 6, m(0.1), m(0.08), 0.5);
    // … 1 lines
    }
  };

// src/render/blast.ts:486-522 slice of src/render/blast.ts#createBlast.explode (shared): the feature's statements; "…" marks code left out
    explode(x, y, z, radius, ground) {
      // … 4 lines
      exhaust.burst(x, y, z + m(0.8), 110, 5, radius * 0.45, m(2.6) + radius * 0.12, 1.1);
      exhaust.burst(x, y, z + m(2.5), 60, 5, radius * 0.25, m(3.2) + radius * 0.15, 1.6);
      // … 4 lines
      exhaust.burst(x, y, z + m(1.5), 80, 2, radius * 0.45, m(3.5), 7);
      exhaust.burst(x, y, z + m(5), 40, 2, radius * 0.35, m(4.5), 9);
      exhaust.burst(x, y, z + m(0.3), 40, ground === 'earth' ? 1 : 3, radius * 0.6, m(2.2), 6);
      // … 1 lines
      exhaust.burst(x, y, z + m(0.5), 120, 6, m(0.5), m(0.1), 1.1);
      exhaust.burst(x, y, z + m(0.2), 90, 7, m(0.6), m(0.18), 1.4);
    // … 19 lines
    },

// src/render/blast.ts:606-741 slice of src/render/blast.ts#createBlast.update (shared): the feature's statements; "…" marks code left out
    update(dt, world) {
      // … 10 lines
      for (let i = fires.length - 1; i >= 0; i--) {
        // … 4 lines
        while (f.carry >= 1) {
          // … 4 lines
          exhaust.burst(at.x, -at.z, at.y + m(0.3), 1, 5, big, (m(1.1) + big * 0.4) * dying, 0.8);
          // … 1 lines
          if (Math.random() < 0.55) exhaust.burst(at.x, -at.z, at.y + m(1), 1, 2, m(0.4), m(1.3), 12);
          if (Math.random() < 0.6) exhaust.burst(at.x, -at.z, at.y + m(0.6), 1, 0, big * 0.4, m(1.5), 3.5);
        }
      }
      // … 1 lines
      for (let i = geysers.length - 1; i >= 0; i--) {
        // … 4 lines
        while (g.carry >= 1) { g.carry -= 1; exhaust.burst(g.x, g.y, g.z + m(0.4), 1, 8, m(0.1), g.weak ? m(0.25) : m(0.8) * Math.max(0.25, strength), g.weak ? 1.2 : 4.2); }
      }
      for (let i = arcs.length - 1; i >= 0; i--) {
        // … 2 lines
        if (time >= a.next) {
          // … 1 lines
          exhaust.burst(a.at.x, -a.at.z, a.at.y, 18, 6, m(0.2), m(0.1), 0.6);
        // … 1 lines
        }
      }
    // … 91 lines
    },
