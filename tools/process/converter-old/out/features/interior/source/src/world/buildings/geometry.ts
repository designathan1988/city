// The code of src/world/buildings/geometry.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/geometry.ts:68-70 function worldToLocalAt: the feature's own
export function worldToLocalAt(b: Building, p: Vec2): Vec2 {
  return worldToLocal(b, p);
}

// src/world/buildings/geometry.ts:199-202 function footprintCentre: the feature's own
export function footprintCentre(b: Building): Vec2 {
  const f = footprintBox(b);
  return localToWorld(b, (f.x0 + f.x1) / 2, (f.y0 + f.y1) / 2);
}

// src/world/buildings/geometry.ts:289-291 function planOverlap: the feature's own
export const planOverlap = (a: Volume, c: Volume): boolean =>
  a.outline || c.outline ? overlapArea(localFootprint(a), localFootprint(c)) > EPS :
  a.x < c.x + c.w - EPS && c.x < a.x + a.w - EPS && a.y < c.y + c.d - EPS && c.y < a.y + a.d - EPS;

// src/world/buildings/geometry.ts:433-441 function reliefAt: the feature's own
export function reliefAt(v: Volume, side: FaceId, index: number, storey: number): Relief | null {
  const list = v.reliefs;
  if (!list) return null;
  for (let i = list.length - 1; i >= 0; i--) {
    const r = list[i] as Relief;
    if (r.side === side && index >= r.bay0 && index <= r.bay1 && storey >= r.storey0 && storey <= r.storey1) return r;
  }
  return null;
}

// src/world/buildings/geometry.ts:498-502 function bayCentreLocal: the feature's own
export function bayCentreLocal(b: Building, v: Volume, side: FaceId, index: number): Vec2 {
  const s = sideStart(v, side);
  const a = (index + 0.5) * bayWidth(b, v, side);
  return { x: s.x + s.tx * a, y: s.y + s.ty * a };
}

// src/world/buildings/geometry.ts:798-802 function buildingHeight: the feature's own
export function buildingHeight(b: Building): number {
  let h = 0;
  for (const v of b.volumes) h = Math.max(h, volumeHeight(b, v) + roofRise(b, v));
  return h;
}
