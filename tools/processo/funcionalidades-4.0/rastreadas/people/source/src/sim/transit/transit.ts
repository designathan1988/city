// The code of src/sim/transit/transit.ts that is the feature's (the rest of the file is shared).

// src/sim/transit/transit.ts:493-531 method TransitSim.trainZones: the feature's own
  trainZones(ahead = true, near?: readonly [number, number, number, number]): { x: number; y: number; r: number }[][] {
    const zones = new Map<string, { x: number; y: number; r: number }[]>();
    const zone = (key: string): { x: number; y: number; r: number }[] => { let z = zones.get(key); if (!z) { z = []; zones.set(key, z); } return z; };
    // DO NOT make the discs of every train of the map for the walkers of a corner of it: a train whose cars and track ahead cannot reach `near` (the walkers' box) is left out.
    const far = (rail: RailLine, train: Train): boolean => {
      if (!near) return false;
      const head = this.pointAt(rail, train.s), reach = TRAIN_CARS * CAR_LENGTH + Math.max(TRAIN_AHEAD, train.v * AHEAD_TIME) + m(4);
      return head.x + reach < near[0] || head.x - reach > near[2] || head.y + reach < near[1] || head.y - reach > near[3];
    };
    for (const rail of ahead ? this.rails : []) {
      if (rail.line.mode === 'metro') continue;
      for (const train of rail.trains) {
        if (far(rail, train)) continue;
        // Standing at a station, it is ground to keep off only once it is about to leave.
        if (train.v < m(1) && train.dwell > AHEAD_TIME) continue;
        // Ahead by time, not distance: as long as it takes to walk across the track.
        const reach = Math.max(TRAIN_AHEAD, train.v * AHEAD_TIME);
        for (let d = m(4); d < reach; d += m(3)) {
          const p = this.pointAt(rail, train.s + d);
          zone(train.key).push({ x: p.x, y: p.y, r: m(1.9) });
        }
      }
    }
    // DO NOT make every train's cars (`trains`) to throw the metro's away: the trains at grade only.
    for (const rail of this.rails) {
      if (rail.line.mode === 'metro') continue;
      for (const train of rail.trains) {
        if (far(rail, train)) continue;
        // Discs close enough to overlap: no gap a person could slip through.
        for (let k = 0; k < TRAIN_CARS; k++) {
          const car = this.pointAt(rail, train.s - CAR_LENGTH * (k + 0.5));
          for (let d = -CAR_LENGTH / 2 + m(1.5); d <= CAR_LENGTH / 2 - m(1.5) + 1e-6; d += m(2.5)) {
            zone(train.key).push({ x: car.x + Math.cos(car.heading) * d, y: car.y + Math.sin(car.heading) * d, r: m(1.8) });
          }
        }
      }
    }
    return [...zones.values()];
  }
