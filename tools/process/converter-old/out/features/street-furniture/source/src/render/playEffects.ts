// The code of src/render/playEffects.ts that is the feature's (the rest of the file is shared).

// src/render/playEffects.ts:413-555 slice of src/render/playEffects.ts#createPlayEffects.explode (shared): the feature's statements; "…" marks code left out
    explode(x, y, z, radius, hit) {
      // … 9 lines
      for (const pole of hit.poles) {
        // … 2 lines
        const top = new Vector3(pole.x, g + POLE_HEIGHT - POLE_ARM_DROP, -pole.y);
        // … 3 lines
        if (pole.mode === 'whole') {
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + POLE_HEIGHT / 2 + m(0.05), -pole.y),
            size: new Vector3(r, POLE_HEIGHT, r), velocity: push.clone().multiplyScalar(0.4), spin: axis.clone().multiplyScalar(0.9 + Math.random() * 0.6) });
        } else if (pole.mode === 'snap') {
          // … 1 lines
          const cut = POLE_HEIGHT * (0.25 + Math.random() * 0.3);
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + cut / 2, -pole.y),
            size: new Vector3(r * 1.1, cut, r * 1.1), velocity: new Vector3(), spin: new Vector3() });
          blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + cut + (POLE_HEIGHT - cut) / 2 + m(0.1), -pole.y),
            size: new Vector3(r, POLE_HEIGHT - cut, r), velocity: push.clone().multiplyScalar(0.7), spin: axis.clone().multiplyScalar(1.5 + Math.random()) });
        } else {
          // … 2 lines
          while (h < POLE_HEIGHT - m(0.5)) {
            const l = Math.min(POLE_HEIGHT - h, m(1 + Math.random() * 2.5));
            blast.debris({ shape: 'cylinder', kind: 'wood', color: wood, at: new Vector3(pole.x, g + h + l / 2, -pole.y),
              size: new Vector3(r * (0.6 + Math.random() * 0.4), l, r * (0.6 + Math.random() * 0.4)),
              velocity: push.clone().multiplyScalar(0.8 + Math.random()).add(new Vector3((Math.random() - 0.5) * m(4), m(2 + Math.random() * 4), (Math.random() - 0.5) * m(4))) });
            h += l;
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
          const side = new Vector3(-dy / d, 0, -dx / d).multiplyScalar(offset * POLE_ARM_HALF);
          const from = new Vector3(w.fromX, g + POLE_HEIGHT - POLE_ARM_DROP, -w.fromY).add(side);
          const to = new Vector3(w.toX, gt + POLE_HEIGHT * 0.6, -w.toY).add(side);
          blast.wire(from, to, away(w.toX, w.toY, m(3)));
        }
      }
    // … 86 lines
    },
