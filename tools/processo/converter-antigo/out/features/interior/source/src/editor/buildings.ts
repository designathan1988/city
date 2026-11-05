// The code of src/editor/buildings.ts that is the feature's (the rest of the file is shared).

// src/editor/buildings.ts:76-76 type BuildingContext: the feature's own
export type BuildingContext = SiteContext;

// src/editor/buildings.ts:78-83 interface EditResult: the feature's own
export interface EditResult {
  readonly ok: boolean;
  readonly problem?: BuildingProblem | 'missing';
  readonly id?: BuildingId;
  readonly volume?: number;
}

// src/editor/buildings.ts:85-85 variable FAIL_MISSING: the feature's own
const FAIL_MISSING: EditResult = { ok: false, problem: 'missing' };

// src/editor/buildings.ts:86-86 variable UNCHANGED: the feature's own
const UNCHANGED: EditResult = { ok: false };

// src/editor/buildings.ts:91-96 function storeyTemplate: the feature's own
function storeyTemplate(v: Volume): Storey {
  const top = v.storeys[v.storeys.length - 1];
  if (!top) return { facade: { fill: 'window' } };
  if (v.base + v.storeys.length - 1 === 0) return upperStoreyFrom(top);
  return JSON.parse(JSON.stringify(top)) as Storey;
}

// src/editor/buildings.ts:99-99 function snapLength: the feature's own
const snapLength = (v: number): number => Math.round(v / GRID) * GRID;

// src/editor/buildings.ts:102-111 function rideWith: the feature's own
function rideWith(b: Building, v: Volume, oldTop: number, delta: number, seen = new Set<number>()): void {
  if (delta === 0) return;
  seen.add(v.id);
  for (const other of b.volumes) {
    if (seen.has(other.id) || other.base !== oldTop || !planOverlap(v, other)) continue;
    const otherTop = volumeTop(other);
    other.base += delta;
    rideWith(b, other, otherTop, delta, seen);
  }
}

// src/editor/buildings.ts:114-124 function opSetStoreys: the feature's own
export function opSetStoreys(b: Building, volumeId: number, count: number): boolean {
  const v = volumeById(b, volumeId);
  if (!v) return false;
  const next = clamp(Math.round(count), 1, Math.max(1, MAX_STOREYS - v.base));
  if (next === v.storeys.length) return false;
  const oldTop = volumeTop(v);
  while (v.storeys.length < next) v.storeys.push(storeyTemplate(v));
  v.storeys.length = next;
  rideWith(b, v, oldTop, volumeTop(v) - oldTop);
  return true;
}

// src/editor/buildings.ts:127-144 function shiftBays: the feature's own
function shiftBays(v: Volume, sides: readonly FaceId[], shift: number): void {
  if (shift === 0) return;
  for (const storey of v.storeys) {
    const bays = storey.facade.bays;
    if (!bays) continue;
    const next: Record<string, BayComponent> = {};
    for (const [key, value] of Object.entries(bays)) {
      const [s, i] = key.split(':').map(Number) as [number, number];
      if (sides.includes(s)) {
        const moved = i + shift;
        if (moved >= 0) next[bayKey(s, moved)] = value;
      } else {
        next[key] = value;
      }
    }
    storey.facade.bays = next;
  }
}

// src/editor/buildings.ts:147-167 function opResize: the feature's own
export function opResize(b: Building, volumeId: number, side: Side, delta: number, snap = true): boolean {
  const v = volumeById(b, volumeId);
  if (!v || delta === 0) return false;
  const along = side === 1 || side === 3 ? 'w' : 'd';
  // Snapped to the grid unless the player holds Alt; free, to the centimetre.
  const size = clamp(snap ? snapLength(v[along] + delta) : Math.round((v[along] + delta) * METERS_PER_UNIT * 100) / 100 / METERS_PER_UNIT, MIN_SIZE, MAX_SIZE);
  const change = size - v[along];
  if (Math.abs(change) < 1e-9) return false;
  // The sides that run along the one moved, whose bays recount.
  const runs: [Side, Side] = along === 'w' ? [0, 2] : [1, 3];
  const before = baysOn(b, v, runs[0]);
  v[along] = size;
  if (side === 3) {
    v.x -= change;
    shiftBays(v, runs, baysOn(b, v, runs[0]) - before);
  } else if (side === 0) {
    v.y -= change;
    shiftBays(v, runs, baysOn(b, v, runs[0]) - before);
  }
  return true;
}

// src/editor/buildings.ts:170-184 function copyStoreys: the feature's own
function copyStoreys(v: Volume, count = v.storeys.length): Storey[] {
  const out: Storey[] = [];
  for (let k = 0; k < count; k++) {
    const source = v.storeys[Math.min(k, v.storeys.length - 1)] as Storey;
    const facade: Facade = { fill: source.facade.fill };
    if (source.facade.pattern) facade.pattern = source.facade.pattern;
    if (source.facade.patterns) facade.patterns = { ...source.facade.patterns };
    if (source.facade.sides) facade.sides = { ...source.facade.sides };
    const storey: Storey = { facade };
    if (source.use) storey.use = source.use;
    if (source.materials) storey.materials = structuredClone(source.materials);
    out.push(storey);
  }
  return out;
}

// src/editor/buildings.ts:187-239 function opAddWing: the feature's own
export function opAddWing(b: Building, volumeId: number, side: FaceId, depth?: number, length?: number): number | null {
  const v = volumeById(b, volumeId);
  if (!v) return null;
  if (v.outline) {
    const f = edgeFrame(v, side);
    const len = Math.min(f.length, Math.max(MIN_SIZE, snapLength(length ?? f.length * .6)));
    const out = Math.max(MIN_SIZE, snapLength(depth ?? 3 * b.module));
    const a = (f.length - len) / 2;
    const p0 = { x: f.x + f.tx * a, y: f.y + f.ty * a };
    const p1 = { x: p0.x + f.tx * len, y: p0.y + f.ty * len };
    const wing: Volume = { id: b.nextVolumeId++, x: 0, y: 0, w: 1, d: 1, base: v.base,
      roof: v.roof === 'terrace' ? 'flat' : v.roof, storeys: copyStoreys(v) };
    if (!setVolumePlan(wing, [p1, p0,
      { x: p0.x + f.nx * out, y: p0.y + f.ny * out },
      { x: p1.x + f.nx * out, y: p1.y + f.ny * out }])) return null;
    if (v.materials) wing.materials = JSON.parse(JSON.stringify(v.materials)) as NonNullable<Volume['materials']>;
    if (v.facadePattern) wing.facadePattern = v.facadePattern;
    if (wing.base === 0 && wing.storeys[0]) wing.storeys[0].facade.bays = { [bayKey(2, Math.floor(baysOn(b, wing, 2) / 2))]: 'door' };
    b.volumes.push(wing);
    return wing.id;
  }
  const sideLength = side === 0 || side === 2 ? v.w : v.d;
  const half = Math.max(2 * b.module, Math.ceil(sideLength / 2 / b.module) * b.module);
  const len = clamp(snapLength(Math.min(sideLength, length ?? half)), MIN_SIZE, MAX_SIZE);
  const out = clamp(snapLength(depth ?? 3 * b.module), MIN_SIZE, MAX_SIZE);
  const offset = snapLength((sideLength - len) / 2);
  const wing: Volume = {
    id: b.nextVolumeId++,
    x: 0,
    y: 0,
    w: 1,
    d: 1,
    base: v.base,
    roof: v.roof === 'terrace' ? 'flat' : v.roof,
    storeys: copyStoreys(v),
  };
  // A wing is built in what its volume is built in.
  if (v.materials) wing.materials = JSON.parse(JSON.stringify(v.materials)) as NonNullable<Volume['materials']>;
  if (v.facadePattern) wing.facadePattern = v.facadePattern;
  switch (side) {
    case 0: Object.assign(wing, { x: v.x + offset, y: v.y - out, w: len, d: out }); break;
    case 2: Object.assign(wing, { x: v.x + offset, y: v.y + v.d, w: len, d: out }); break;
    case 1: Object.assign(wing, { x: v.x + v.w, y: v.y + offset, w: out, d: len }); break;
    case 3: Object.assign(wing, { x: v.x - out, y: v.y + offset, w: out, d: len }); break;
  }
  // A wing's ground floor gets its own way in, on its outer face.
  if (wing.base === 0 && wing.storeys[0]) {
    const facade = wing.storeys[0].facade;
    facade.bays = { [bayKey(side, Math.floor(baysOn(b, wing, side) / 2))]: 'door' };
  }
  b.volumes.push(wing);
  return wing.id;
}

// src/editor/buildings.ts:242-276 function opAddSetback: the feature's own
export function opAddSetback(b: Building, volumeId: number, inset?: number, storeys = 2): number | null {
  const v = volumeById(b, volumeId);
  if (!v) return null;
  const requested = Math.max(0, snapLength(inset ?? b.module));
  const step = v.outline && inset === undefined
    ? Math.min(requested, Math.max(GRID, snapLength(Math.min(v.w, v.d) * .1)))
    : requested;
  const ix = v.w - 2 * step >= MIN_SIZE ? step : 0;
  const iy = v.d - 2 * step >= MIN_SIZE ? step : 0;
  const base = volumeTop(v);
  const template = storeyTemplate(v);
  const count = clamp(Math.round(storeys), 1, Math.max(1, MAX_STOREYS - base));
  const volume: Volume = {
    id: b.nextVolumeId++,
    x: v.x + ix,
    y: v.y + iy,
    w: v.w - 2 * ix,
    d: v.d - 2 * iy,
    base,
    roof: v.roof === 'terrace' ? 'flat' : v.roof,
    storeys: copyStoreys({ ...v, storeys: [template] }, count),
  };
  if (v.outline) {
    const ring = offsetRing(localFootprint(v), -step);
    if (!setVolumePlan(volume, ring) || !supportedBy(localFootprint(volume), [localFootprint(v)])) return null;
  }
  if (v.materials) volume.materials = JSON.parse(JSON.stringify(v.materials)) as NonNullable<Volume['materials']>;
  if (v.facadePattern) volume.facadePattern = v.facadePattern;
  if (v.facadeGeometry) volume.facadeGeometry = structuredClone(v.facadeGeometry);
  v.roof = 'terrace';
  if (v.roofDetails) v.roofDetails = v.roofDetails.filter((part) =>
    overlapArea(roofDetailRing(part), localFootprint(volume)) < AREA_EPS);
  b.volumes.push(volume);
  return volume.id;
}

// src/editor/buildings.ts:279-297 function opRemoveVolume: the feature's own
export function opRemoveVolume(b: Building, volumeId: number): boolean {
  const index = b.volumes.findIndex((v) => v.id === volumeId);
  if (index < 0) return false;
  const remaining = b.volumes.filter((v) => v.id !== volumeId);
  if (!remaining.some((v) => v.base === 0)) return false;
  b.volumes = remaining;
  // Anything now hanging over nothing goes too, repeatedly: a tower on a podium goes with the podium.
  for (let changed = true; changed;) {
    changed = false;
    for (const v of b.volumes) {
      if (!isSupported(b, v)) {
        b.volumes = b.volumes.filter((u) => u.id !== v.id);
        changed = true;
        break;
      }
    }
  }
  return true;
}

// src/editor/buildings.ts:299-304 function opSetRoof: the feature's own
export function opSetRoof(b: Building, volumeId: number, roof: RoofKind): boolean {
  const v = volumeById(b, volumeId);
  if (!v || v.roof === roof) return false;
  v.roof = roof;
  return true;
}

// src/editor/buildings.ts:306-311 interface BuildingParameters: the feature's own
export interface BuildingParameters {
  module?: number;
  groundHeight?: number;
  storeyHeight?: number;
  palette?: number;
}

// src/editor/buildings.ts:313-326 function opSetParameters: the feature's own
export function opSetParameters(b: Building, p: BuildingParameters): boolean {
  let changed = false;
  const set = <K extends keyof BuildingParameters>(key: K, value: number): void => {
    if (b[key] !== value) {
      (b as unknown as Record<string, number>)[key] = value;
      changed = true;
    }
  };
  if (p.module !== undefined) set('module', clamp(p.module, MIN_MODULE, MAX_MODULE));
  if (p.groundHeight !== undefined) set('groundHeight', clamp(p.groundHeight, MIN_STOREY_HEIGHT, MAX_GROUND_HEIGHT));
  if (p.storeyHeight !== undefined) set('storeyHeight', clamp(p.storeyHeight, MIN_STOREY_HEIGHT, MAX_STOREY_HEIGHT));
  if (p.palette !== undefined) set('palette', ((Math.round(p.palette) % PALETTE_COUNT) + PALETTE_COUNT) % PALETTE_COUNT);
  return changed;
}

// src/editor/buildings.ts:329-340 function opSetLevelHeight: the feature's own
export function opSetLevelHeight(b: Building, level: number, height: number | null): boolean {
  if (level < 0 || level >= MAX_STOREYS) return false;
  const levels = b.levels ? [...b.levels] : [];
  while (levels.length <= level) levels.push(null);
  const value = height === null ? null : clamp(height, MIN_STOREY_HEIGHT, level === 0 ? MAX_GROUND_HEIGHT : MAX_STOREY_HEIGHT);
  if (levels[level] === value) return false;
  levels[level] = value;
  while (levels.length > 0 && levels[levels.length - 1] === null) levels.pop();
  if (levels.length === 0) delete b.levels;
  else b.levels = levels;
  return true;
}

// src/editor/buildings.ts:344-344 type FacadeScope: the feature's own
export type FacadeScope = 'bay' | 'row' | 'column' | 'storey' | 'side' | 'volume' | 'zone';

// src/editor/buildings.ts:347-374 function opSetComponentZone: the feature's own
export function opSetComponentZone(
  b: Building,
  volumeId: number,
  side: FaceId,
  storey0: number,
  storey1: number,
  index0: number,
  index1: number,
  component: BayComponent,
): boolean {
  const v = volumeById(b, volumeId);
  if (!v) return false;
  const before = JSON.stringify(v.storeys);
  const [s0, s1] = storey0 <= storey1 ? [storey0, storey1] : [storey1, storey0];
  const [i0, i1] = index0 <= index1 ? [index0, index1] : [index1, index0];
  for (let s = Math.max(0, s0); s <= Math.min(v.storeys.length - 1, s1); s++) {
    const target = v.storeys[s]!;
    const bays = { ...(target.facade.bays ?? {}) };
    for (let i = i0; i <= i1; i++) {
      const key = bayKey(side, i);
      delete bays[key];
      if (componentAt({ ...target.facade, bays }, side, i) !== component) bays[key] = component;
    }
    if (Object.keys(bays).length > 0) target.facade.bays = bays;
    else delete target.facade.bays;
  }
  return JSON.stringify(v.storeys) !== before;
}

// src/editor/buildings.ts:377-430 function opSetComponent: the feature's own
export function opSetComponent(
  b: Building,
  volumeId: number,
  storey: number,
  side: FaceId,
  index: number,
  component: BayComponent,
  scope: FacadeScope = 'bay',
): boolean {
  const v = volumeById(b, volumeId);
  const target = v?.storeys[storey];
  if (!v || !target) return false;
  const before = JSON.stringify(v.storeys);
  switch (scope) {
    case 'bay': {
      const key = bayKey(side, index);
      const bays = { ...(target.facade.bays ?? {}) };
      delete bays[key];
      const inherited = componentAt({ ...target.facade, bays }, side, index);
      if (inherited !== component) bays[key] = component;
      if (Object.keys(bays).length > 0) target.facade.bays = bays;
      else delete target.facade.bays;
      break;
    }
    case 'row': {
      target.facade.sides = { ...(target.facade.sides ?? {}), [side]: component };
      if (target.facade.bays) {
        for (const key of Object.keys(target.facade.bays)) if (key.startsWith(`${side}:`)) delete target.facade.bays[key];
        if (Object.keys(target.facade.bays).length === 0) delete target.facade.bays;
      }
      break;
    }
    case 'column': {
      const key = bayKey(side, index);
      for (const s of v.storeys) s.facade.bays = { ...(s.facade.bays ?? {}), [key]: component };
      break;
    }
    case 'storey':
      target.facade = { fill: component };
      break;
    case 'side':
      for (const s of v.storeys) {
        s.facade.sides = { ...(s.facade.sides ?? {}), [side]: component };
        if (s.facade.bays) {
          for (const key of Object.keys(s.facade.bays)) if (key.startsWith(`${side}:`)) delete s.facade.bays[key];
        }
      }
      break;
    case 'volume':
      for (const s of v.storeys) s.facade = { fill: component };
      break;
  }
  return JSON.stringify(v.storeys) !== before;
}

// src/editor/buildings.ts:433-447 function opRotate: the feature's own
export function opRotate(b: Building, angle: number, pivot?: Vec2): boolean {
  if (angle === 0) return false;
  const f = footprintBox(b);
  const centreLocal = { x: (f.x0 + f.x1) / 2, y: (f.y0 + f.y1) / 2 };
  const toCentre = localDirToWorld(b, centreLocal.x, centreLocal.y);
  const p = pivot ?? { x: b.x + toCentre.x, y: b.y + toCentre.y };
  const c = Math.cos(angle);
  const s = Math.sin(angle);
  const dx = b.x - p.x;
  const dy = b.y - p.y;
  b.x = p.x + dx * c - dy * s;
  b.y = p.y + dx * s + dy * c;
  b.rotation = normaliseAngle(b.rotation + angle);
  return true;
}

// src/editor/buildings.ts:450-472 function opScalePlan: the feature's own
export function opScalePlan(b: Building, factor: number): boolean {
  if (!(factor > 0) || Math.abs(factor - 1) < 1e-6) return false;
  const f = footprintBox(b);
  const cx = (f.x0 + f.x1) / 2;
  const cy = (f.y0 + f.y1) / 2;
  for (const v of b.volumes) {
    v.x = cx + (v.x - cx) * factor;
    v.y = cy + (v.y - cy) * factor;
    v.w *= factor;
    v.d *= factor;
    if (v.w < MIN_SIZE || v.d < MIN_SIZE || v.w > MAX_SIZE || v.d > MAX_SIZE) return false;
    for (const part of v.roofDetails ?? []) {
      part.x = cx + (part.x - cx) * factor;
      part.y = cy + (part.y - cy) * factor;
    }
    delete v.reliefs;
  }
  for (const e of b.elements ?? []) {
    e.x = cx + (e.x - cx) * factor;
    e.y = cy + (e.y - cy) * factor;
  }
  return true;
}

// src/editor/buildings.ts:474-480 function normaliseAngle: the feature's own
export const normaliseAngle = (a: number): number => {
  const tau = Math.PI * 2;
  let r = a % tau;
  if (r > Math.PI) r -= tau;
  if (r <= -Math.PI) r += tau;
  return Math.abs(r) < 1e-12 ? 0 : r;
};

// src/editor/buildings.ts:500-509 function addBuildingRecord: the feature's own
export function addBuildingRecord(
  ctx: BuildingContext,
  record: Omit<Building, 'id'>,
  ignore?: BuildingId | readonly BuildingId[],
): EditResult {
  const draft = { ...record, id: ctx.doc.buildings.nextId } as Building;
  const problem = validateBuilding(ctx, draft, ignore);
  if (problem) return { ok: false, problem };
  return { ok: true, id: ctx.doc.buildings.add(draft).id };
}

// src/editor/buildings.ts:512-521 function editBuilding: the feature's own
export function editBuilding(ctx: BuildingContext, id: BuildingId, op: (draft: Building) => boolean): EditResult {
  const current = ctx.doc.buildings.get(id);
  if (!current) return FAIL_MISSING;
  const draft = cloneBuilding(current);
  if (!op(draft)) return UNCHANGED;
  const problem = validateBuilding(ctx, draft, id);
  if (problem) return { ok: false, problem };
  ctx.doc.buildings.put(draft);
  return { ok: true, id };
}

// src/editor/buildings.ts:524-534 function replaceBuilding: the feature's own
export function replaceBuilding(
  ctx: BuildingContext,
  draft: Building,
  ignore?: readonly BuildingId[],
): EditResult {
  if (!ctx.doc.buildings.has(draft.id)) return FAIL_MISSING;
  const problem = validateBuilding(ctx, draft, ignore === undefined ? draft.id : [draft.id, ...ignore]);
  if (problem) return { ok: false, problem };
  ctx.doc.buildings.put(draft);
  return { ok: true, id: draft.id };
}

// src/editor/buildings.ts:536-538 function deleteBuilding: the feature's own
export function deleteBuilding(ctx: BuildingContext, id: BuildingId): boolean {
  return ctx.doc.buildings.remove(id);
}

// src/editor/buildings.ts:541-553 function removeVolume: the feature's own
export function removeVolume(ctx: BuildingContext, id: BuildingId, volumeId: number): EditResult {
  const current = ctx.doc.buildings.get(id);
  if (!current) return FAIL_MISSING;
  const draft = cloneBuilding(current);
  if (!opRemoveVolume(draft, volumeId)) {
    ctx.doc.buildings.remove(id);
    return { ok: true };
  }
  const problem = validateBuilding(ctx, draft, id);
  if (problem) return { ok: false, problem };
  ctx.doc.buildings.put(draft);
  return { ok: true, id };
}

// src/editor/buildings.ts:556-573 function duplicateBuilding: the feature's own
export function duplicateBuilding(ctx: BuildingContext, id: BuildingId): EditResult {
  const source = ctx.doc.buildings.get(id);
  if (!source) return FAIL_MISSING;
  const f = footprintBox(source);
  const width = f.x1 - f.x0;
  const depth = f.y1 - f.y0;
  let last: EditResult = { ok: false, problem: 'building' };
  for (const [lx, ly] of [[width, 0], [-width, 0], [0, depth], [0, -depth], [width * 2, 0], [-width * 2, 0]] as const) {
    const shift = localDirToWorld(source, lx, ly);
    const copy = cloneBuilding(source) as Omit<Building, 'id'> & { id?: BuildingId };
    delete copy.id;
    copy.x += shift.x;
    copy.y += shift.y;
    last = addBuildingRecord(ctx, copy);
    if (last.ok) return last;
  }
  return last;
}

// src/editor/buildings.ts:576-594 function clearBuildingsOnRoads: the feature's own
export function clearBuildingsOnRoads(ctx: BuildingContext, rects: readonly ChangeRect[] | null): number {
  if (!ctx.net || ctx.doc.buildings.size === 0 || rects?.length === 0) return 0;
  const doomed: BuildingId[] = [];
  for (const b of rects === null ? everyBuilding(ctx) : buildingsIn(ctx, rects)) {
    if (footprintRects(b, -m(0.02)).some((rect) => touchesRoad(ctx.net!, rect))) {
      doomed.push(b.id);
      continue;
    }
    // A stair or a wall the road now crosses goes; the building stays.
    const hit = (b.elements ?? []).filter((e) => onGround(e) && touchesRoad(ctx.net!, elementRing(b, e, -m(0.02))));
    if (hit.length > 0) {
      const draft = cloneBuilding(b);
      for (const e of hit) opRemoveElement(draft, e.id);
      ctx.doc.buildings.put(draft);
    }
  }
  for (const id of doomed) ctx.doc.buildings.remove(id);
  return doomed.length;
}

// src/editor/buildings.ts:597-601 function clearBuildingsAfterEdit: the feature's own
export function clearBuildingsAfterEdit(ctx: BuildingContext, before: object): number {
  // Where the edit itself changed the roads (`Network.changesSince` the state read just before it).
  const delta = ctx.net ? ctx.net.changesSince(before) : null;
  return clearBuildingsOnRoads(ctx, delta === null ? null : delta.rects);
}

// src/editor/buildings.ts:604-610 function everyBuilding: the feature's own
function* everyBuilding(ctx: BuildingContext): Generator<Building, void, void> {
  const ids = [...ctx.doc.buildings.idList()];
  for (const id of ids) {
    const b = ctx.doc.buildings.get(id);
    if (b) yield b;
  }
}

// src/editor/buildings.ts:613-620 function buildingsIn: the feature's own
function buildingsIn(ctx: BuildingContext, rects: readonly ChangeRect[]): Building[] {
  const seen = new Set<BuildingId>();
  const out: Building[] = [];
  for (const [x0, y0, x1, y1] of rects) {
    for (const b of ctx.doc.buildings.near(x0, y0, x1, y1)) if (!seen.has(b.id)) { seen.add(b.id); out.push(b); }
  }
  return out;
}

// src/editor/buildings.ts:623-670 function groupInto: the feature's own
export function groupInto(ctx: BuildingContext, targetId: BuildingId, sourceId: BuildingId): EditResult {
  const target = ctx.doc.buildings.get(targetId);
  const source = ctx.doc.buildings.get(sourceId);
  if (!target || !source || targetId === sourceId) return FAIL_MISSING;
  const draft = cloneBuilding(target);
  for (const v of source.volumes) {
    const ring = localFootprint(v).map((p) => worldToLocal(draft, localToWorld(source, p.x, p.y)));
    let minX = Infinity;
    let minY = Infinity;
    let maxX = -Infinity;
    let maxY = -Infinity;
    for (const p of ring) {
      minX = Math.min(minX, p.x);
      minY = Math.min(minY, p.y);
      maxX = Math.max(maxX, p.x);
      maxY = Math.max(maxY, p.y);
    }
    const w = Math.max(MIN_SIZE, maxX - minX);
    const d = Math.max(MIN_SIZE, maxY - minY);
    const volume: Volume = JSON.parse(JSON.stringify(v)) as Volume;
    volume.id = draft.nextVolumeId++;
    volume.x = minX;
    volume.y = minY;
    volume.w = w;
    volume.d = d;
    // A ring that is not the plain rectangle keeps its shape as an outline.
    const plain =
      ring.length === 4 &&
      ring.every((p) =>
        (Math.abs(p.x - minX) < 1e-6 || Math.abs(p.x - maxX) < 1e-6) &&
        (Math.abs(p.y - minY) < 1e-6 || Math.abs(p.y - maxY) < 1e-6),
      );
    // An outline is stored normalized to its bounds (0..1).
    if (plain) delete volume.outline;
    else if (!setVolumePlan(volume, ring)) return { ok: false, problem: 'outline' };
    draft.volumes.push(volume);
  }
  for (const el of source.elements ?? []) {
    const p = worldToLocal(draft, localToWorld(source, el.x, el.y));
    const element: BuildingElement = JSON.parse(JSON.stringify(el)) as BuildingElement;
    draft.elements = [...(draft.elements ?? []), { ...element, id: takeElementId(draft), x: p.x, y: p.y }];
  }
  const problem = validateBuilding(ctx, draft, targetId);
  if (problem) return { ok: false, problem };
  ctx.doc.buildings.put(draft);
  ctx.doc.buildings.remove(sourceId);
  return { ok: true, id: targetId };
}

// src/editor/buildings.ts:675-711 function opFuseElement: the feature's own
export function opFuseElement(b: Building, id: number): boolean {
  const e = b.elements?.find((x) => x.id === id);
  if (!e || e.angle) return false;
  const alongY = e.facing === 1 || e.facing === 3;
  for (const other of b.elements ?? []) {
    if (other.id === e.id || other.kind !== e.kind || other.angle) continue;
    if (other.facing !== e.facing) continue;
    if (Math.abs(other.z - e.z) > 1e-6 || Math.abs(other.h - e.h) > 1e-6) continue;
    if (alongY) {
      // Their runs are along x: width along y, length along x.
      if (Math.abs(e.x - other.x) > 1e-6 || Math.abs(e.d - other.d) > 1e-6) continue;
      const gap = Math.min(
        Math.abs(other.y + other.w / 2 - (e.y - e.w / 2)),
        Math.abs(e.y + e.w / 2 - (other.y - other.w / 2)),
      );
      if (gap > FUSE_GAP) continue;
      const lo = Math.min(e.y - e.w / 2, other.y - other.w / 2);
      const hi = Math.max(e.y + e.w / 2, other.y + other.w / 2);
      e.w = hi - lo;
      e.y = (lo + hi) / 2;
    } else {
      if (Math.abs(e.y - other.y) > 1e-6 || Math.abs(e.d - other.d) > 1e-6) continue;
      const gap = Math.min(
        Math.abs(other.x + other.w / 2 - (e.x - e.w / 2)),
        Math.abs(e.x + e.w / 2 - (other.x - other.w / 2)),
      );
      if (gap > FUSE_GAP) continue;
      const lo = Math.min(e.x - e.w / 2, other.x - other.w / 2);
      const hi = Math.max(e.x + e.w / 2, other.x + other.w / 2);
      e.w = hi - lo;
      e.x = (lo + hi) / 2;
    }
    b.elements = (b.elements ?? []).filter((x) => x.id !== other.id);
    return true;
  }
  return false;
}

// src/editor/buildings.ts:714-714 variable FUSE_GAP: the feature's own
const FUSE_GAP = m(0.6);

// src/editor/buildings.ts:717-746 function opMoveBlock: the feature's own
export function opMoveBlock(b: Building, volumeId: number, dx: number, dy: number, snap = true): boolean {
  const v = volumeById(b, volumeId);
  if (!v) return false;
  const step = snap ? GRID : 0;
  const mx = step ? Math.round(dx / step) * step : dx;
  const my = step ? Math.round(dy / step) * step : dy;
  if (Math.abs(mx) < 1e-9 && Math.abs(my) < 1e-9) return false;
  const moving = new Set<number>([v.id]);
  for (let grew = true; grew;) {
    grew = false;
    for (const o of b.volumes) {
      if (moving.has(o.id)) continue;
      const under = b.volumes.find((u) => moving.has(u.id) && o.base === u.base + u.storeys.length && planOverlap(o, u));
      if (under) {
        moving.add(o.id);
        grew = true;
      }
    }
  }
  for (const o of b.volumes) {
    if (!moving.has(o.id)) continue;
    o.x += mx;
    o.y += my;
    for (const part of o.roofDetails ?? []) {
      part.x += mx;
      part.y += my;
    }
  }
  return true;
}

// src/editor/buildings.ts:749-769 function blockSnap: the feature's own
export function blockSnap(b: Building, volumeId: number, reach: number): { dx: number; dy: number } {
  const v = volumeById(b, volumeId);
  if (!v) return { dx: 0, dy: 0 };
  const mine = { x: [v.x, v.x + v.w / 2, v.x + v.w], y: [v.y, v.y + v.d / 2, v.y + v.d] };
  let dx = 0, dy = 0, bestX = reach, bestY = reach;
  for (const o of b.volumes) {
    if (o.id === v.id) continue;
    const theirs = { x: [o.x, o.x + o.w / 2, o.x + o.w], y: [o.y, o.y + o.d / 2, o.y + o.d] };
    for (let i = 0; i < 3; i++) {
      for (let j = 0; j < 3; j++) {
        // Centres meet centres; sides meet sides.
        if ((i === 1) !== (j === 1)) continue;
        const ex = theirs.x[j]! - mine.x[i]!;
        if (Math.abs(ex) < bestX) { bestX = Math.abs(ex); dx = ex; }
        const ey = theirs.y[j]! - mine.y[i]!;
        if (Math.abs(ey) < bestY) { bestY = Math.abs(ey); dy = ey; }
      }
    }
  }
  return { dx, dy };
}

// src/editor/buildings.ts:805-860 function weldInto: the feature's own
export function weldInto(ctx: BuildingContext, draft: Building, skip: readonly BuildingId[] = []): BuildingId[] {
  const absorbed: BuildingId[] = [];
  const mine = buildingBounds(draft, 0.5);
  // The draft's rings and boxes once, before the neighbours (each neighbour's are kept per record, `neighbourOf`).
  let mineRings: { readonly ring: readonly Vec2[]; readonly box: Aabb }[] | null = null;
  // DO NOT walk `buildings.all()` here: the ghost of a placement asks this on every pointer move
  for (const other of ctx.doc.buildings.near(mine.minX - 0.5, mine.minY - 0.5, mine.maxX + 0.5, mine.maxY + 0.5)) {
    if (other.id === draft.id || skip.includes(other.id)) continue;
    const otherRings = neighbourOf(other).obstacles;
    if (!otherRings.some(({ box }) => boxesMeet(box, mine, 0.5))) continue;
    // Does anything actually touch or overlap? Clipped only where the two rings' boxes meet.
    mineRings ??= [...footprintRects(draft, m(0.02)), ...groundProjections(draft, m(0.02)), ...groundElements(draft, m(0.02))]
      .map((ring) => ({ ring, box: ringBox(ring) }));
    const touches = mineRings.some((a) => otherRings.some((c) => boxesMeet(a.box, c.box, 0) && overlapArea(a.ring, c.ring) > 1e-6));
    if (!touches) continue;
    for (const v of other.volumes) {
      const ring = localFootprint(v).map((p) => worldToLocalAt(draft, localToWorld(other, p.x, p.y)));
      const volume = JSON.parse(JSON.stringify(v)) as Volume;
      volume.id = draft.nextVolumeId++;
      // Its ring's bounds once, not five times (EDT-13).
      const b0 = polygonBounds(ring);
      volume.x = b0.minX;
      volume.y = b0.minY;
      volume.w = Math.max(MIN_SIZE, b0.maxX - volume.x);
      volume.d = Math.max(MIN_SIZE, b0.maxY - volume.y);
      // Each mass keeps the look it had: walls and roof of the building it came from.
      volume.materials = {
        ...volume.materials,
        wall: v.materials?.wall ?? other.materials?.wall ?? paletteOf(other).wall,
        roof: roofMaterial(other, v),
      };
      // A box that stays square to this building's frame is a plain volume
      const square = ring.length === 4 && ring.every((p) =>
        (Math.abs(p.x - b0.minX) < 1e-6 || Math.abs(p.x - b0.maxX) < 1e-6) &&
        (Math.abs(p.y - b0.minY) < 1e-6 || Math.abs(p.y - b0.maxY) < 1e-6));
      if (square) delete volume.outline;
      else if (!setVolumePlan(volume, ring)) delete volume.outline;
      draft.volumes.push(volume);
    }
    // One new list of the draft's elements for this neighbour's, not one per element (EDT-13): the list the draft had is never written.
    if (other.elements?.length) {
      const elements = [...(draft.elements ?? [])];
      draft.elements = elements;
      for (const el of other.elements) {
        const p = worldToLocal(draft, localToWorld(other, el.x, el.y));
        const element: BuildingElement = JSON.parse(JSON.stringify(el)) as BuildingElement;
        elements.push({ ...element, id: takeElementId(draft), x: p.x, y: p.y });
      }
    }
    absorbed.push(other.id);
    // The draft grew by it: its rings made again, so a building touching the one absorbed welds in too (a chain).
    mineRings = null;
  }
  // Nothing is cut: the blocks of both buildings stay whole, overlapping where they overlap, and can be moved apart again.
  return absorbed;
}

// src/editor/buildings.ts:863-864 function boxesMeet: the feature's own
const boxesMeet = (a: Aabb, b: Aabb, pad: number): boolean =>
  a.minX - pad <= b.maxX && a.maxX + pad >= b.minX && a.minY - pad <= b.maxY && a.maxY + pad >= b.minY;

// src/editor/buildings.ts:867-872 function clipRing: the feature's own
export function clipRing(ring: readonly Vec2[], within: readonly Vec2[]): Vec2[] | null {
  const pieces = clipping.intersection(asPolygon(ring), asPolygon(within)).flatMap((p) => simplePieces(p));
  let best: Vec2[] | null = null;
  for (const p of pieces) if (!best || Math.abs(signedArea(p)) > Math.abs(signedArea(best))) best = p;
  return best && Math.abs(signedArea(best)) > MIN_SIZE * MIN_SIZE ? best : null;
}

// src/editor/buildings.ts:874-886 function polygonBounds: the feature's own
function polygonBounds(ring: readonly Vec2[]): { minX: number; minY: number; maxX: number; maxY: number } {
  let minX = Infinity;
  let minY = Infinity;
  let maxX = -Infinity;
  let maxY = -Infinity;
  for (const p of ring) {
    minX = Math.min(minX, p.x);
    minY = Math.min(minY, p.y);
    maxX = Math.max(maxX, p.x);
    maxY = Math.max(maxY, p.y);
  }
  return { minX, minY, maxX, maxY };
}

// src/editor/buildings.ts:889-905 function simplePieces: the feature's own
function simplePieces(poly: clipping.Polygon, depth = 0): Vec2[][] {
  const outer = poly[0];
  if (!outer) return [];
  if (poly.length === 1 || depth > 3) {
    const ring = outer.slice(0, -1).map(([x, y]) => ({ x, y }));
    return [signedArea(ring) < 0 ? ring.reverse() : ring];
  }
  const hole = poly[1]!;
  const xs = hole.map(([x]) => x);
  const cut = (Math.min(...xs) + Math.max(...xs)) / 2;
  const ys = outer.map(([, y]) => y);
  const allX = outer.map(([x]) => x);
  const y0 = Math.min(...ys) - 1, y1 = Math.max(...ys) + 1;
  const left: clipping.Polygon = [[[Math.min(...allX) - 1, y0], [cut, y0], [cut, y1], [Math.min(...allX) - 1, y1], [Math.min(...allX) - 1, y0]]];
  const right: clipping.Polygon = [[[cut, y0], [Math.max(...allX) + 1, y0], [Math.max(...allX) + 1, y1], [cut, y1], [cut, y0]]];
  return [...clipping.intersection(poly, left), ...clipping.intersection(poly, right)].flatMap((p) => simplePieces(p, depth + 1));
}

// src/editor/buildings.ts:911-917 interface FaceRegion: the feature's own
export interface FaceRegion {
  readonly side: FaceId;
  readonly bay0: number;
  readonly bay1: number;
  readonly storey0: number;
  readonly storey1: number;
}

// src/editor/buildings.ts:920-920 variable RELIEF_STEP: the feature's own
const RELIEF_STEP = m(0.05);

// src/editor/buildings.ts:923-936 function cutRelief: the feature's own
function cutRelief(r: Relief, region: FaceRegion): Relief[] {
  if (r.side !== region.side || r.bay0 > region.bay1 || region.bay0 > r.bay1 || r.storey0 > region.storey1 || region.storey0 > r.storey1) {
    return [r];
  }
  const out: Relief[] = [];
  // Below and above the region, full width; then left and right of it, beside it.
  if (r.storey0 < region.storey0) out.push({ ...r, storey1: region.storey0 - 1 });
  if (r.storey1 > region.storey1) out.push({ ...r, storey0: region.storey1 + 1 });
  const storey0 = Math.max(r.storey0, region.storey0);
  const storey1 = Math.min(r.storey1, region.storey1);
  if (r.bay0 < region.bay0) out.push({ ...r, storey0, storey1, bay1: region.bay0 - 1 });
  if (r.bay1 > region.bay1) out.push({ ...r, storey0, storey1, bay0: region.bay1 + 1 });
  return out;
}

// src/editor/buildings.ts:939-950 function opSetRelief: the feature's own
export function opSetRelief(b: Building, volumeId: number, region: FaceRegion, depth: number, snap = true): boolean {
  const v = volumeById(b, volumeId);
  if (!v) return false;
  const step = snap ? RELIEF_STEP : m(0.01);
  const d = clamp(Math.round(depth / step) * step, -MAX_RECESS, MAX_PROJECTION);
  const before = JSON.stringify(v.reliefs ?? []);
  const kept = (v.reliefs ?? []).flatMap((r) => cutRelief(r, region));
  if (Math.abs(d) > 1e-9) kept.push({ ...region, depth: d });
  if (kept.length > 0) v.reliefs = kept;
  else delete v.reliefs;
  return JSON.stringify(v.reliefs ?? []) !== before;
}

// src/editor/buildings.ts:952-957 interface RoofShape: the feature's own
export interface RoofShape {
  /** Degrees; null restores the roof kind's default. */
  readonly pitch?: number | null;
  readonly ridge?: 'x' | 'y' | null;
  readonly fall?: Side | null;
}

// src/editor/buildings.ts:960-977 function opSetRoofShape: the feature's own
export function opSetRoofShape(b: Building, volumeId: number, shape: RoofShape): boolean {
  const v = volumeById(b, volumeId);
  if (!v) return false;
  const before = JSON.stringify([v.pitch, v.ridge, v.fall]);
  if (shape.pitch !== undefined) {
    if (shape.pitch === null) delete v.pitch;
    else v.pitch = clamp(Math.round(shape.pitch), MIN_PITCH, MAX_PITCH);
  }
  if (shape.ridge !== undefined) {
    if (shape.ridge === null) delete v.ridge;
    else v.ridge = shape.ridge;
  }
  if (shape.fall !== undefined) {
    if (shape.fall === null) delete v.fall;
    else v.fall = shape.fall;
  }
  return JSON.stringify([v.pitch, v.ridge, v.fall]) !== before;
}

// src/editor/buildings.ts:982-986 function opAddElement: the feature's own
export function opAddElement(b: Building, element: Omit<BuildingElement, 'id'>): number {
  const id = takeElementId(b);
  b.elements = [...(b.elements ?? []), { ...element, id }];
  return id;
}

// src/editor/buildings.ts:988-988 type ElementPatch: the feature's own
export type ElementPatch = Partial<Pick<BuildingElement, 'w' | 'd' | 'h' | 'z' | 'facing' | 'material'>>;

// src/editor/buildings.ts:991-1013 function opUpdateElement: the feature's own
export function opUpdateElement(b: Building, id: number, patch: ElementPatch): boolean {
  const e = b.elements?.find((x) => x.id === id);
  if (!e) return false;
  const before = JSON.stringify(e);
  const size = (v: number): number => clamp(Math.round(v / RELIEF_STEP) * RELIEF_STEP, MIN_ELEMENT, MAX_ELEMENT);
  if (patch.w !== undefined) e.w = size(patch.w);
  if (patch.d !== undefined) e.d = size(patch.d);
  if (patch.h !== undefined) {
    e.h = size(patch.h);
    if ((e.kind === 'stair' || e.kind === 'ramp') && patch.d === undefined) {
      // The run grows from the top: the foot moves, the head stays at the door.
      const run = runFor(e.kind, e.h);
      const n = SIDE_NORMAL[e.facing];
      e.x += (n.x * (run - e.d)) / 2;
      e.y += (n.y * (run - e.d)) / 2;
      e.d = run;
    }
  }
  if (patch.z !== undefined) e.z = Math.max(0, patch.z);
  if (patch.facing !== undefined) e.facing = patch.facing;
  if (patch.material !== undefined) e.material = patch.material;
  return JSON.stringify(e) !== before;
}

// src/editor/buildings.ts:1015-1022 function opRemoveElement: the feature's own
export function opRemoveElement(b: Building, id: number): boolean {
  const list = b.elements ?? [];
  const next = list.filter((e) => e.id !== id);
  if (next.length === list.length) return false;
  if (next.length > 0) b.elements = next;
  else delete b.elements;
  return true;
}

// src/editor/buildings.ts:1026-1026 variable MIRROR_SIDE: the feature's own
const MIRROR_SIDE: Readonly<Record<Side, Side>> = { 0: 0, 1: 3, 2: 2, 3: 1 };

// src/editor/buildings.ts:1029-1100 function opMirror: the feature's own
export function opMirror(b: Building): boolean {
  const before = footprintCentre(b);
  const flipSide = (side: Side): Side => MIRROR_SIDE[side];
  for (const v of b.volumes) {
    const sidesCount = v.outline?.length ?? 4;
    const flipFace = (side: FaceId): FaceId => {
      if (v.outline) return (sidesCount - 2 - side + sidesCount) % sidesCount;
      const cardinal = SIDES.find((candidate) => candidate === side);
      return cardinal === undefined ? side : flipSide(cardinal);
    };
    const previousCounts = Array.from({ length: sidesCount }, (_, side) => baysOn(b, v, side));
    const count = (side: FaceId): number => previousCounts[side] ?? 1;
    v.x = -(v.x + v.w);
    if (v.outline) v.outline = v.outline.map((p) => ({ x: 1 - p.x, y: p.y })).reverse();
    for (const storey of v.storeys) {
      const facade = storey.facade;
      if (facade.sides) {
        const sides: Partial<Record<FaceId, BayComponent>> = {};
        for (const [key, value] of Object.entries(facade.sides)) sides[flipFace(Number(key))] = value;
        facade.sides = sides;
      }
      if (facade.patterns) {
        const patterns: NonNullable<Facade['patterns']> = {};
        for (const [key, value] of Object.entries(facade.patterns)) patterns[flipFace(Number(key))] = value;
        facade.patterns = patterns;
      }
      if (facade.bays) {
        const bays: Record<string, BayComponent> = {};
        for (const [key, value] of Object.entries(facade.bays)) {
          const [s, i] = key.split(':').map(Number) as [FaceId, number];
          const side = flipFace(s);
          bays[bayKey(side, v.outline || side === 0 || side === 2 ? count(s) - 1 - i : i)] = value;
        }
        facade.bays = bays;
      }
      for (const space of storey.spaces ?? []) space.x = -(space.x + space.w);
    }
    for (const r of v.reliefs ?? []) {
      r.side = flipFace(r.side);
      if (v.outline || r.side === 0 || r.side === 2) {
        const n = count(flipFace(r.side));
        [r.bay0, r.bay1] = [n - 1 - r.bay1, n - 1 - r.bay0];
      }
    }
    if (v.materials?.sides) {
      const sides: Partial<Record<FaceId, NonNullable<Volume['materials']>['wall']>> = {};
      for (const [key, value] of Object.entries(v.materials.sides)) sides[flipFace(Number(key))] = value;
      v.materials.sides = sides as NonNullable<NonNullable<Volume['materials']>['sides']>;
    }
    if (v.facadeGeometry) {
      const controls: NonNullable<Volume['facadeGeometry']> = {};
      for (const [key, value] of Object.entries(v.facadeGeometry)) controls[flipFace(Number(key))] = value;
      v.facadeGeometry = controls;
    }
    if (v.fall !== undefined) v.fall = flipSide(v.fall);
    // The roof equipment goes with its roof
    for (const part of v.roofDetails ?? []) {
      part.x = -part.x;
      part.rotation = -part.rotation;
    }
  }
  for (const e of b.elements ?? []) {
    e.x = -e.x;
    e.facing = flipSide(e.facing);
    if (e.angle) e.angle = -e.angle;
  }
  for (const core of b.cores) core.x = -(core.x + b.module);
  const after = footprintCentre(b);
  b.x += before.x - after.x;
  b.y += before.y - after.y;
  return true;
}

// src/editor/buildings.ts:1103-1125 function opRepeatElement: the feature's own
export function opRepeatElement(b: Building, id: number, spacing?: number): number {
  const e = b.elements?.find((x) => x.id === id);
  if (!e) return 0;
  const alongX = e.facing === 0 || e.facing === 2;
  const step = snapLength(spacing ?? Math.max(e.w * 2, b.module));
  const box = footprintBox(b);
  // Out to the building's corners: a row of pillars ends on them.
  const lo = alongX ? box.x0 : box.y0;
  const hi = alongX ? box.x1 : box.y1;
  const start = alongX ? e.x : e.y;
  let added = 0;
  for (const dir of [1, -1]) {
    for (let k = 1; k < 64; k++) {
      const at = start + dir * k * step;
      if (at < lo - 1e-6 || at > hi + 1e-6) break;
      const copy = { ...e, x: alongX ? at : e.x, y: alongX ? e.y : at };
      if (elementClash(b, { ...copy, id: -1 })) break;
      opAddElement(b, copy);
      added++;
    }
  }
  return added;
}
