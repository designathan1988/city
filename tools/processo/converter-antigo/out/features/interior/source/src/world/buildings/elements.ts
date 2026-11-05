// The code of src/world/buildings/elements.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/elements.ts:139-142 function runFor: the feature's own
export function runFor(kind: ElementKind, rise: number): number {
  if (kind === 'ramp') return Math.max(m(1), rise * RAMP_RUN_PER_RISE);
  return Math.max(STEP_RUN, Math.ceil(rise / STEP_RISE - 1e-9) * STEP_RUN);
}

// src/world/buildings/elements.ts:262-267 function takeElementId: the feature's own
export function takeElementId(b: Building): number {
  const used = (b.elements ?? []).reduce((n, e) => Math.max(n, e.id), 0);
  const id = Math.max(b.nextElementId ?? 1, used + 1);
  b.nextElementId = id + 1;
  return id;
}

// src/world/buildings/elements.ts:272-277 interface BayRef: the feature's own
export interface BayRef {
  readonly volume: number;
  readonly side: FaceId;
  readonly index: number;
  readonly storey: number;
}

// src/world/buildings/elements.ts:279-279 function snap: the feature's own
const snap = (v: number, step = GRID): number => Math.round(v / step) * step;

// src/world/buildings/elements.ts:282-293 function elementsAgainstBay: the feature's own
export function elementsAgainstBay(b: Building, v: Volume, bay: BayRef, kind: ElementKind): Omit<BuildingElement, 'id'>[] {
  const made = elementsAgainstBayAxis(b, v, bay, kind);
  if (!ON_FACADE.has(kind)) return made;
  // Hung on a face that is not square to the frame
  const frame = edgeFrame(v, bay.side);
  return made.map((e) => {
    const fn = FACING_NORMAL[e.facing];
    let angle = Math.atan2(frame.ny, frame.nx) - Math.atan2(fn.y, fn.x);
    angle = Math.atan2(Math.sin(angle), Math.cos(angle));
    return Math.abs(angle) > 1e-3 ? { ...e, angle } : e;
  });
}

// src/world/buildings/elements.ts:297-420 function elementsAgainstBayAxis: the feature's own
function elementsAgainstBayAxis(b: Building, v: Volume, bay: BayRef, kind: ElementKind): Omit<BuildingElement, 'id'>[] {
  const [dw, dd, dh] = ELEMENT_DEFAULTS[kind];
  const frame = edgeFrame(v, bay.side);
  const n = { x: frame.nx, y: frame.ny };
  const facing: Side = Math.abs(n.x) > Math.abs(n.y) ? (n.x > 0 ? 1 : 3) : (n.y > 0 ? 2 : 0);
  const s = sideStart(v, bay.side);
  const width = bayWidth(b, v, bay.side);
  const along = (bay.index + 0.5) * width;
  // A point on the face line, `out` in front of it and `a` along it.
  const at = (a: number, out: number): Vec2 => ({ x: s.x + s.tx * a + n.x * out, y: s.y + s.ty * a + n.y * out });
  const floorZ = volumeElevation(b, v, v.base + bay.storey);
  switch (kind) {
    case 'canopy': {
      // Over the bay's opening, a little below the next floor.
      const z = Math.max(floorZ + m(2.5), volumeElevation(b, v, v.base + bay.storey + 1) - m(0.6));
      const c = at(along, dd / 2);
      return [{ kind, x: c.x, y: c.y, facing, w: Math.max(width, dw), d: dd, z, h: dh }];
    }
    case 'stair':
    case 'ramp': {
      // From the floor of the picked storey down to the ground floor's level.
      const rise = Math.max(floorZ, STEP_RISE);
      const run = runFor(kind, rise);
      const w = kind === 'ramp' ? dw : Math.max(dw, Math.min(width, m(1.6)));
      const straight = at(along, run / 2);
      const out: Omit<BuildingElement, 'id'>[] = [{ kind, x: straight.x, y: straight.y, facing, w, d: run, z: 0, h: rise }];
      // Turned to run along the facade, either way, its top at the bay.
      for (const dir of [1, -1] as const) {
        const turned = ((facing + (dir === 1 ? 1 : 3)) % 4) as Side;
        const c = at(along + dir * (run / 2 - w / 2), w / 2);
        out.push({ kind, x: c.x, y: c.y, facing: turned, w, d: run, z: 0, h: rise });
      }
      return out;
    }
    case 'pillar': {
      // On the nearest bay line, just in front of the face, as tall as the storey.
      const line = Math.round(along / width) * width;
      const c = at(line, dd / 2 + m(0.6));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: floorZ, h: volumeElevation(b, v, v.base + bay.storey + 1) - floorZ }];
    }
    case 'wall':
    case 'hedge': {
      // Parallel to the face, a module out, on the ground.
      const c = at(along, b.module);
      return [{ kind, x: snap(c.x), y: snap(c.y), facing, w: Math.max(dw, width), d: dd, z: 0, h: dh }];
    }
    case 'slab': {
      // A deck in front of the bay, at its floor.
      const c = at(along, dd / 2);
      return [{ kind, x: c.x, y: c.y, facing, w: Math.max(width, dw), d: dd, z: Math.max(0, floorZ - dh), h: dh }];
    }
    case 'railing': {
      // A run of railing along the facade, a module out.
      const run = Math.max(dw, width);
      const c = at(along, b.module);
      return [{ kind, x: c.x, y: c.y, facing, w: run, d: Math.max(dd, m(0.12)), z: 0, h: dh }];
    }
    case 'awning': {
      // A canvas over the bay's opening, projecting out from above it.
      const z = Math.max(floorZ + m(2.4), volumeElevation(b, v, v.base + bay.storey + 1) - m(0.5));
      const c = at(along, dd / 2);
      return [{ kind, x: c.x, y: c.y, facing, w: Math.max(width, dw), d: dd, z, h: dh }];
    }
    case 'flowers':
    case 'shrub': {
      const c = at(along, m(1.2));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: 0, h: dh }];
    }
    case 'rocks': {
      const c = at(along, m(2));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: 0, h: dh }];
    }
    case 'parking': {
      // A paved apron in front of the bay, wide enough for a car each side.
      const run = Math.max(dd, m(5));
      const c = at(along, run / 2);
      return [{ kind, x: c.x, y: c.y, facing, w: Math.max(width * 2, dw), d: run, z: 0, h: dh }];
    }
    case 'pavement': {
      // A paved apron on the ground in front of the bay: its depth runs out from the wall, its width along it.
      const run = Math.max(dd, m(3));
      const c = at(along, run / 2);
      return [{ kind, x: c.x, y: c.y, facing, w: Math.max(width, dw), d: run, z: 0, h: dh }];
    }
    case 'fence': {
      // A run of fence parallel to the face, a module out.
      const run = Math.max(dw, width);
      const c = at(along, b.module);
      return [{ kind, x: c.x, y: c.y, facing, w: run, d: dd, z: 0, h: dh }];
    }
    case 'tree': {
      // On the ground in front of the bay, clear of the flight.
      const c = at(along, m(2.2));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: 0, h: dh }];
    }
    case 'bench': {
      // On the ground, facing out from the wall.
      const c = at(along, m(1.4));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: 0, h: dh }];
    }
    case 'bin':
    case 'lamp':
    case 'bollard':
    case 'drain':
    case 'gate':
    case 'planter': {
      const c = at(along, m(1.1));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z: 0, h: dh }];
    }
    case 'clock': {
      // A dial on the wall, centred on the bay, in the middle of the storey.
      const c = at(along, dd / 2 + m(0.02));
      const storeyH = volumeElevation(b, v, v.base + bay.storey + 1) - floorZ;
      const size = Math.min(dw, width * 0.9, storeyH * 0.9);
      return [{ kind, x: c.x, y: c.y, facing, w: size, d: dd, z: floorZ + (storeyH - size) / 2, h: size }];
    }
    case 'ac': {
      // Hung on the facade, a metre above the storey's floor.
      const c = at(along, dd / 2 + m(0.05));
      const z = floorZ + Math.min(m(1.1), Math.max(0, (volumeElevation(b, v, v.base + bay.storey + 1) - floorZ) / 2));
      return [{ kind, x: c.x, y: c.y, facing, w: dw, d: dd, z, h: dh }];
    }
  }
}

// src/world/buildings/elements.ts:423-428 function elementAt: the feature's own
export function elementAt(b: Building, kind: ElementKind, p: Vec2, facing: Side): Omit<BuildingElement, 'id'> {
  const [w, d, h] = ELEMENT_DEFAULTS[kind];
  const top = b.volumes.reduce((z, v) => Math.max(z, volumeHeight(b, v)), 0);
  const z = kind === 'canopy' ? Math.min(top, m(2.6)) : 0;
  return { kind, x: snap(p.x), y: snap(p.y), facing, w, d, z, h };
}
