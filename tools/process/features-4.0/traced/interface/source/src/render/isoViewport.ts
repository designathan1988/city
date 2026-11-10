// The code of src/render/isoViewport.ts that is the feature's (the rest of the file is shared).

// src/render/isoViewport.ts:48-63 slice of src/render/isoViewport.ts#IsoRig (shared): the feature's statements; "…" marks code left out
export interface IsoRig {
  // … 5 lines
  readonly viewport: Viewport;
// … 8 lines
}

// src/render/isoViewport.ts:78-460 slice of src/render/isoViewport.ts#createIsoRig (shared): the feature's statements; "…" marks code left out
export function createIsoRig(
// … 3 lines
): IsoRig {
  // … 224 lines
  const viewport: Viewport = {
    kind: 'iso',
    toWorld: (px, py) => worldAt(px, py),
    toWorldAt: (px, py, atHeight) => worldAt(px, py, atHeight),
    // The camera's own ray (three's `Raycaster.setFromCamera`), in the map's axes.
    rayAt(px, py) {
      ndc.set((px / Math.max(1, width)) * 2 - 1, 1 - (py / Math.max(1, height)) * 2);
      raycaster.setFromCamera(ndc, camera);
      const { origin: o, direction: d } = raycaster.ray;
      return { ox: o.x, oy: -o.z, oz: o.y, dx: d.x, dy: -d.z, dz: d.y };
    },
    toScreen(p, cssW, cssH, atHeight = 0) {
      projected.set(p.x, atHeight, -p.y).project(camera);
      return {
        x: (projected.x * 0.5 + 0.5) * cssW,
        y: (-projected.y * 0.5 + 0.5) * cssH,
      };
    },
    // The same test the zoom's hold makes (`holdHeight`): met within three times the view's distance.
    holds: (px, py, atHeight) => camera !== persp || holdHeight(px, py, atHeight) === atHeight,
    panTo(grabbed, px, py, _cssW, _cssH, atHeight = 0) {
      // The grabbed point is held on its own plane (the height of what was under the pointer), as zoom and orbit hold theirs
      for (let pass = camera === persp && groundAt ? 2 : 1; pass > 0; pass--) {
        const found = hitAt(px, py, atHeight);
        if (!found) break;
        target.x += grabbed.x - found.x;
        target.z -= grabbed.y - found.y;
        apply();
      }
    },
    grab(px, py, atHeight) {
      const held = holdHeight(px, py, atHeight);
      const world = hitAt(px, py, held);
      return world ? { world, height: held } : null;
    },
    zoomAt(px, py, factor, _cssW, _cssH, atHeight = 0, grabbed) {
      if (!(factor > 0) || !Number.isFinite(factor)) return;
      const wanted = halfHeight / factor;
      // The closest zoom is a person's eyes on the ground, and the zoom stops there
      if (wanted < MIN_HALF_HEIGHT && halfHeight <= MIN_HALF_HEIGHT) return;
      keeping(px, py, () => {
        halfHeight = Math.min(MAX_HALF_HEIGHT, Math.max(MIN_HALF_HEIGHT, wanted));
      }, atHeight, grabbed);
    },
    slide(right, forward) {
      // Along the ground the camera faces: forward is away from the camera over the ground, whatever the tilt
      const fx = -Math.cos(azimuth), fz = -Math.sin(azimuth);
      target.x += fx * forward - fz * right;
      target.z += fz * forward + fx * right;
      apply();
    },
    rotate(quarterTurns, px, py) {
      keeping(px, py, () => {
        azimuth = wrapAzimuth(azimuth + quarterTurns * Math.PI * 0.5);
      });
    },
    orbit(dAzimuth, dElevation, pivot) {
      // About the point the player grabbed (the player, 2026-10-07)
      const turn = (): void => {
        azimuth = wrapAzimuth(azimuth + dAzimuth);
        askTilt(tiltNow() + dElevation);
      };
      if (pivot) keeping(pivot.px, pivot.py, turn, pivot.height);
      else { turn(); apply(); }
    },
    setOrbit(nextAzimuth, nextElevation) {
      azimuth = wrapAzimuth(Number.isFinite(nextAzimuth) ? nextAzimuth : DEFAULT_AZIMUTH);
      askTilt(Number.isFinite(nextElevation) ? nextElevation : DEFAULT_ELEVATION);
      apply();
    },
    get azimuth() {
      return azimuth;
    },
    get elevation() {
      return elevation;
    },
    get eye() {
      return { x: camera.position.x, y: -camera.position.z, z: camera.position.y };
    },
    get centre() {
      return { x: target.x, y: -target.z };
    },
    moveTo(p) {
      target.set(p.x, 0, -p.y);
      apply();
    },
    get zoom() {
      return height / Math.max(1, halfHeight * 2);
    },
    get facing() {
      const turns = Math.round((azimuth - DEFAULT_AZIMUTH) / (Math.PI * 0.5));
      return (((turns % 4) + 4) % 4) as Facing;
    },
    get scaleAtCentre() {
      return scaleNow;
    },
    get zoomBounds() {
      return isoZoomBounds(height);
    },
  };
  // … 2 lines
  return {
    // … 12 lines
    viewport,
  // … 36 lines
  };
}

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
