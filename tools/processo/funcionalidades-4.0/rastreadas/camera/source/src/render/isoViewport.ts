// The code of src/render/isoViewport.ts that is the feature's (the rest of the file is shared).

// src/render/isoViewport.ts:100-102 slice of src/render/isoViewport.ts#createIsoRig.tiltNow (shared): the feature's statements; "…" marks code left out
  const tiltNow = (): number => camera === persp
    ? Math.min(MAX_ELEVATION, Math.max(minTilt(halfHeight), profileTilt(halfHeight) + farTilt - FAR_TILT))
    : clampElevation(farTilt);

// src/render/isoViewport.ts:104-109 slice of src/render/isoViewport.ts#createIsoRig.askTilt (shared): the feature's statements; "…" marks code left out
  const askTilt = (next: number): void => {
    if (camera === persp) {
      const now = Math.min(MAX_ELEVATION, Math.max(minTilt(halfHeight), next));
      farTilt = now - profileTilt(halfHeight) + FAR_TILT;
    } else farTilt = clampElevation(next);
  };

// src/render/isoViewport.ts:112-245 slice of src/render/isoViewport.ts#createIsoRig.apply (shared): the feature's statements; "…" marks code left out
  const apply = (): void => {
    // … 7 lines
    if (camera === ortho) {
      // … 6 lines
      distance = Math.max(distance, deep + m(400));
      ortho.far = Math.max(m(5600), distance + deep + m(800));
    } else {
      // … 1 lines
      persp.fov = fieldOfView(halfHeight);
      distance = viewDistance(halfHeight);
      // … 1 lines
      persp.near = Math.max(m(0.08), distance * 0.02);
      persp.far = distance * 4 + m(2400);
    }
    // … 2 lines
    if (camera === persp) target.y = (groundAt ? groundAt(target.x, -target.z) : 0) + eyeLift(halfHeight);
    const horizontal = Math.cos(elevation) * distance;
    camera.position.set(
      target.x + Math.cos(azimuth) * horizontal,
      (camera === persp ? target.y : 0) + Math.sin(elevation) * distance,
      target.z + Math.sin(azimuth) * horizontal,
    );
    // … 3 lines
    const pull = camera === persp && solidsAt ? pullWeight(halfHeight) : 0;
    // … 1 lines
    if (pull > 0 && solidsAt) {
      const floorAt = solidsAt;
      const clear1 = target.y >= floorAt(target.x, -target.z);
      if (clear1) {
        for (let i = 0; i < PULL_SAMPLES; i++) {
          const t = i / PULL_SAMPLES;
          const x = camera.position.x + (target.x - camera.position.x) * t;
          const y = camera.position.y + (target.y - camera.position.y) * t;
          const z = camera.position.z + (target.z - camera.position.z) * t;
          const deep = floorAt(x, -z) - y;
          if (deep <= 0) continue;
          const k = Math.min(1, deep / PULL_SOFT);
          wanted = Math.max(wanted, k * k * (3 - 2 * k) * Math.min(1, t + 1 / PULL_SAMPLES) * pull);
        }
      }
    }
    pullWanted = wanted;
    if (pullShown < 0) pullShown = wanted;
    // … 4 lines
    if (camera === persp && solidsAt) {
      // … 12 lines
      if (roof) {
        // … 1 lines
        if (within(1) && within(0)) {
          const lift = roof(target.x, -target.z) + eyeLift(halfHeight) - target.y;
          target.y += lift;
          camera.position.y += lift;
        }
      // … 25 lines
      } else if (inside(0)) {
      // … 9 lines
      }
    // … 3 lines
    }
  // … 12 lines
  };
