// The code of src/render/destruction.ts that is the feature's (the rest of the file is shared).

// src/render/destruction.ts:67-409 slice of src/render/destruction.ts#createDestruction (shared): the feature's statements; "…" marks code left out
export function createDestruction(
  exhaust: Exhaust,
// … 340 lines
}

// src/render/destruction.ts:237-254 slice of src/render/destruction.ts#createDestruction.loosen (shared): the feature's statements; "…" marks code left out
  const loosen = (ruin: Ruin, piece: Piece, from: Vector3, power: number): void => {
    // … 12 lines
    if (Math.random() < 0.5) {
      // … 1 lines
      exhaust.burst(p.x, -p.z, p.y, 3, 3, piece.fragment.radius * 0.6, m(2.5), 3.5);
    }
  };

// src/render/destruction.ts:256-310 slice of src/render/destruction.ts#createDestruction.strike (shared): the feature's statements; "…" marks code left out
  const strike = (ruin: Ruin, floor: number, x: number, y: number, z: number, strength: number, eye?: Vector3): boolean => {
      // … 34 lines
      exhaust.burst(impact.x, -impact.z, impact.y, 10 + strength * 2, 3, reach * 0.6, m(2.5 + strength * 0.2), 4);
      exhaust.burst(impact.x, -impact.z, impact.y, 20 + strength * 4, 9, reach * 0.4, m(0.3), 18);
      // … 4 lines
      if (upper.filter((p) => !p.falling).length < upper.length * COLLAPSE_BELOW) {
        // … 6 lines
        exhaust.burst(x, y, floor, 60, 3, m(10), m(5), 6);
        // … 1 lines
        exhaust.burst(x, y, z, 120, 9, m(8), m(0.35), 22);
      // … 1 lines
      }
  // … 1 lines
  };

// src/render/destruction.ts:335-351 slice of src/render/destruction.ts#createDestruction.hit (shared): the feature's statements; "…" marks code left out
    hit(b, floor, x, y, z, strength, eye) {
      // … 1 lines
      if (!ruin) {
        // … 9 lines
        exhaust.burst(x, y, z, 12 + strength * 2, 3, m(2 + strength * 0.4), m(2.5 + strength * 0.2), 4);
      // … 1 lines
      }
    // … 1 lines
    },

// src/render/destruction.ts:352-391 slice of src/render/destruction.ts#createDestruction.update (shared): the feature's statements; "…" marks code left out
    update(dt) {
      // … 11 lines
      for (let i = loose.length - 1; i >= 0; i--) {
        // … 3 lines
        if (!p.settled) {
          // … 4 lines
          if (mesh.position.y <= rest) {
            // … 1 lines
            if (p.v.y < -m(3)) {
              // … 1 lines
              if (p.fragment.radius > m(1.2) && Math.random() < 0.3) exhaust.burst(mesh.position.x, -mesh.position.z, p.floor, 3, 3, m(1.5), m(3), 4);
            } else {
            // … 7 lines
            }
          }
        }
      }
    // … 1 lines
    },
