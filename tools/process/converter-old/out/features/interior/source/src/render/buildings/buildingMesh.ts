// The code of src/render/buildings/buildingMesh.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/buildingMesh.ts:343-353 slice of src/render/buildings/buildingMesh.ts#Grow.room (shared): the feature's statements; "…" marks code left out
  room(n: number): T {
    const need = this.length + n;
    if (need > this.array.length) {
      let size = this.array.length * 2;
      while (size < need) size *= 2;
      const next = this.make(size);
      (next as Typed).set(this.array);
      this.array = next;
    }
  // … 1 lines
  }

// src/render/buildings/buildingMesh.ts:356-358 slice of src/render/buildings/buildingMesh.ts#Grow.view (shared): the feature's statements; "…" marks code left out
  view(): T {
    return this.array.subarray(0, this.length) as T;
  }

// src/render/buildings/buildingMesh.ts:361-363 slice of src/render/buildings/buildingMesh.ts#Grow.copy (shared): the feature's statements; "…" marks code left out
  copy(): T {
    return this.array.slice(0, this.length) as T;
  }

// src/render/buildings/buildingMesh.ts:386-391 slice of src/render/buildings/buildingMesh.ts#PartRows.constructor (shared): the feature's statements; "…" marks code left out
  constructor(coloured: boolean, looked: boolean, readonly glassy: boolean, private readonly fallback: Rgb = WHITE_RGB) {
    // … 3 lines
    this.rooms = glassy ? new Grow((n) => new Uint16Array(n), 8 * ROOM_FLOATS) : null;
  }

// src/render/buildings/buildingMesh.ts:394-401 slice of src/render/buildings/buildingMesh.ts#PartRows.clear (shared): the feature's statements; "…" marks code left out
  clear(): void {
    // … 1 lines
    this.poses.length = 0;
    if (this.colours) this.colours.length = 0;
    if (this.looks) this.looks.length = 0;
    if (this.slots) this.slots.length = 0;
    if (this.rooms) this.rooms.length = 0;
  }

// src/render/buildings/buildingMesh.ts:404-431 slice of src/render/buildings/buildingMesh.ts#PartRows.add (shared): the feature's statements; "…" marks code left out
  add(x: number, y: number, z: number, yaw: number, sx: number, sy: number, sz: number): number {
    // … 5 lines
    this.poses.length = o + POSE_FLOATS;
    if (this.colours) {
      // … 2 lines
      this.colours.length = i * 3 + 3;
    }
    if (this.looks) {
      // … 2 lines
      this.looks.length = (i + 1) * LOOK_FLOATS;
    }
    if (this.slots) {
      // … 1 lines
      this.slots.length = i + 1;
    }
    if (this.rooms) {
      const r = this.rooms.room(ROOM_FLOATS);
      for (let k = 0; k < ROOM_FLOATS; k++) r[i * ROOM_FLOATS + k] = 0;
      this.rooms.length = (i + 1) * ROOM_FLOATS;
    }
  // … 1 lines
  }

// src/render/buildings/buildingMesh.ts:448-452 slice of src/render/buildings/buildingMesh.ts#PartRows.pane (shared): the feature's statements; "…" marks code left out
  pane(i: number, slot: number, room: readonly number[] | null): void {
    // … 2 lines
    if (room) for (let k = 0; k < ROOM_OWN; k++) this.rooms.array[i * ROOM_FLOATS + k] = DataUtils.toHalfFloat(room[k]!);
  }

// src/render/buildings/buildingMesh.ts:455-459 slice of src/render/buildings/buildingMesh.ts#PartRows.paneFrame (shared): the feature's statements; "…" marks code left out
  paneFrame(i: number, r: number, g: number, b: number, thickness: number): void {
    // … 1 lines
    const o = i * ROOM_FLOATS + PANE_FRAME_AT, a = this.rooms.array;
    a[o] = DataUtils.toHalfFloat(r); a[o + 1] = DataUtils.toHalfFloat(g); a[o + 2] = DataUtils.toHalfFloat(b); a[o + 3] = DataUtils.toHalfFloat(thickness);
  }

// src/render/buildings/buildingMesh.ts:462-465 slice of src/render/buildings/buildingMesh.ts#PartRows.paneRecess (shared): the feature's statements; "…" marks code left out
  paneRecess(i: number, depth: number): void {
    // … 1 lines
    this.rooms.array[i * ROOM_FLOATS + PANE_RECESS_AT] = DataUtils.toHalfFloat(depth);
  }

// src/render/buildings/buildingMesh.ts:478-478 slice of src/render/buildings/buildingMesh.ts#FurnitureSink (shared): the feature's statements; "…" marks code left out
type FurnitureSink = Partial<Record<FurnitureKind, PartRows>>;

// src/render/buildings/buildingMesh.ts:482-482 slice of src/render/buildings/buildingMesh.ts#furnitureRows (shared): the feature's statements; "…" marks code left out
const furnitureRows = (sink: FurnitureSink, kind: FurnitureKind): PartRows => (sink[kind] ??= new PartRows(false, false, false));

// src/render/buildings/buildingMesh.ts:536-543 slice of src/render/buildings/buildingMesh.ts#ShellPart.clear (shared): the feature's statements; "…" marks code left out
  clear(): void {
    // … 3 lines
    for (const s of this.streams) s.grow.length = 0;
    this.index.length = 0;
    this.hashes.length = 0;
  }

// src/render/buildings/buildingMesh.ts:574-591 slice of src/render/buildings/buildingMesh.ts#ShellPart.weld (shared): the feature's statements; "…" marks code left out
  weld(): number {
    // … 4 lines
    for (let s = h & mask; ; s = (s + 1) & mask) {
      // … 1 lines
      if (at === 0) {
        // … 2 lines
        this.hashes.length = v + 1;
        // … 1 lines
        for (const st of this.streams) st.grow.length = this.vertices * st.size;
      // … 1 lines
      }
    // … 1 lines
    }
  }

// src/render/buildings/buildingMesh.ts:704-775 slice of src/render/buildings/buildingMesh.ts#Shell.face (shared): the feature's statements; "…" marks code left out
  face(points: readonly (readonly [number, number, number])[], n: readonly [number, number, number], c: Paint, shades?: readonly number[]): void {
    // … 9 lines
    if (part.index.length === 0) (st ? this.runOrder : this.order).push(c.finish);
    // … 53 lines
    let k3 = part.index.length;
    for (let k = 1; k + 1 < count; k++) {
      index[k3++] = ids[0]!;
      index[k3++] = forward ? ids[k]! : ids[k + 1]!;
      index[k3++] = forward ? ids[k + 1]! : ids[k]!;
    }
    part.index.length = k3;
  }

// src/render/buildings/buildingMesh.ts:1066-1077 slice of src/render/buildings/buildingMesh.ts#slotOfBay (shared): the feature's statements; "…" marks code left out
function slotOfBay(b: Building, v: Volume, bay: FacadeBay, spaces: ReturnType<typeof deriveSpaces>, slots: SlotAllocator): number {
  // … 1 lines
  const floor = spaces.find((f) => f.volume === v.id && f.level === bay.level);
  if (!floor) return -1;
  // … 4 lines
  const sp = floor.spaces.find((q) => inside.x >= q.x - 1 && inside.x <= q.x + q.w + 1 && inside.y >= q.y - 1 && inside.y <= q.y + q.d + 1)
    ?? floor.spaces[0];
  return sp ? slots(b.id, bay.level, v.id, sp.x, sp.y) : -1;
}

// src/render/buildings/buildingMesh.ts:1082-1349 slice of src/render/buildings/buildingMesh.ts#emitBuilding (shared): the feature's statements; "…" marks code left out
function emitBuilding(
  // … 10 lines
  covers: readonly Cover[] = [],
): Foundation {
  // … 62 lines
  const spacesOf = deriveSpaces(b);
  // … 5 lines
  const roomsOf = new Map<readonly Furniture[], Map<number, ReturnType<typeof roomsOfStorey>>>();
  // … 4 lines
  for (const bay of bays) {
    // … 3 lines
    if (covers.length > 0) {
      const [x, y] = e.L(face.ax, face.ay, 0), [tx, ty] = e.N(face.tx, face.ty), [nx, ny] = e.N(face.nx, face.ny);
      if (hiddenByCovers(covers, x, y, tx, ty, nx, ny, face.W, face.z0, face.z0 + face.H)) continue;
    }
    // … 32 lines
    if (opening !== undefined && (opening === null || opening.depth <= FLAT_DEPTH)) {
    // … 9 lines
    } else {
      e.slot = slotOfBay(b, v, bay, spacesOf, slots);
      if (b.id >= 0 && b.cutaway === undefined) {
        // … 1 lines
        const inside = interiorAt(b, bay.level);
        let byBlock = roomsOf.get(inside.furniture);
        if (!byBlock) roomsOf.set(inside.furniture, byBlock = new Map());
        let look = byBlock.get(v.id);
        if (!look) byBlock.set(v.id, look = roomsOfStorey(b, v, inside, bay.level));
        const behind = look;
        e.room = (a, h) => behind(face.ax, face.ay, face.tx, face.ty, face.nx, face.ny, a, h, face.H - m(0.3));
      }
    // … 6 lines
    }
  // … 14 lines
  }
// … 97 lines
}

// src/render/buildings/buildingMesh.ts:1371-1455 slice of src/render/buildings/buildingMesh.ts#emitInterior (shared): the feature's statements; "…" marks code left out
function emitInterior(e: Emitter, b: Building, floor: number, cut: number): void {
  // … 3 lines
  const kind = floorKind(b, cut);
  const floorLook = kind === 1 ? FLOOR_FINISH : kind === 2 ? PORCELAIN_FLOOR : WOOD_FLOOR;
  for (const v of b.volumes) {
    // … 5 lines
    for (let i = 0; i < triangles.length; i += 3) {
      // … 1 lines
      e.shell.face(p as [V3, V3, V3], [0, 0, 1], floorLook);
    }
  }
  // … 35 lines
  for (const f of levelFlights(b, cut)) {
    const run = Math.hypot(f.top.x - f.foot.x, f.top.y - f.foot.y);
    const ux = (f.top.x - f.foot.x) / run, uy = (f.top.y - f.foot.y) / run;
    const tread = run / f.risers, rise = (f.z1 - f.z0) / f.risers, half = f.width / 2;
    for (let k = 0; k < f.risers; k++) {
      const a = { x: f.foot.x + ux * tread * k, y: f.foot.y + uy * tread * k };
      const c = { x: f.foot.x + ux * tread * (k + 1), y: f.foot.y + uy * tread * (k + 1) };
      const x0 = Math.min(a.x, c.x) - Math.abs(uy) * half, x1 = Math.max(a.x, c.x) + Math.abs(uy) * half;
      const y0 = Math.min(a.y, c.y) - Math.abs(ux) * half, y1 = Math.max(a.y, c.y) + Math.abs(ux) * half;
      e.box(x0, y0, x1, y1, floor + f.z0, floor + f.z0 + rise * (k + 1), STEP);
    }
  }
  // … 1 lines
  const inside = interiorAt(b, cut);
  // … 4 lines
  for (const p of inside.partitions) {
    const t = m(0.06);
    const len = Math.hypot(p.x1 - p.x0, p.y1 - p.y0) || 1;
    // The wall's normal against the view: near 1 means it faces the camera.
    const facing = viewLocal ? Math.abs((-(p.y1 - p.y0) / len) * viewLocal.x + ((p.x1 - p.x0) / len) * viewLocal.y) : 0;
    const z = zAt((p.x0 + p.x1) / 2, (p.y0 + p.y1) / 2);
    const top = facing > 0.6 ? z + m(0.9) : z + wallRise;
    e.box(Math.min(p.x0, p.x1) - t, Math.min(p.y0, p.y1) - t, Math.max(p.x0, p.x1) + t, Math.max(p.y0, p.y1) + t, z, top, PARTITION);
  }
  // … 1 lines
  for (const f of inside.furniture) {
    const z = zAt(f.x, f.y);
    placeFurniture(e, f, f.kind === 'ceilingLamp' ? z + levelHeight(b, cut) - m(0.06) - f.h : z);
  }
}

// src/render/buildings/buildingMesh.ts:1458-1463 slice of src/render/buildings/buildingMesh.ts#placeFurniture (shared): the feature's statements; "…" marks code left out
function placeFurniture(e: Emitter, f: Furniture, floorZ: number): void {
  const [w, d, h] = FURNITURE_SIZE[f.kind];
  const p = e.L(f.x, f.y, floorZ + 0.05);
  const front = e.N(Math.sin(f.angle), -Math.cos(f.angle));
  furnitureRows(e.furniture, f.kind).add(p[0], p[1], p[2], Math.atan2(front[0], -front[1]), f.w / w, f.h / h, f.d / d);
}

// src/render/buildings/buildingMesh.ts:1628-1831 slice of src/render/buildings/buildingMesh.ts#emitBay (shared): the feature's statements; "…" marks code left out
function emitBay(
// … 12 lines
): void {
  // … 42 lines
  if (!e.flat && !flush) {
    // … 5 lines
    const reveal = shaded(recess > 0 ? wall : trim, REVEAL_SHADE);
    e.jamb(f, o.a0, o.h0, o.h1, 0, o.depth, along, reveal);
    e.jamb(f, o.a1, o.h0, o.h1, 0, o.depth, back, reveal);
    // … 1 lines
    if (recess <= 0) e.strip(f, o.a0, o.a1, o.h0 > 1e-6 ? o.h0 : PLINTH_RISE, 0, o.depth, UP, shaded(trim, SILL_SHADE));
    e.strip(f, o.a0, o.a1, o.h1, 0, o.depth, DOWN, shaded(reveal, HEAD_SHADE / REVEAL_SHADE));
  }
// … 134 lines
}

// src/render/buildings/buildingMesh.ts:2355-2360 method lotGroundOf.heightAt: the feature's own
    heightAt(x, y) {
      if (x < x0 || x > x1 || y < y0 || y > y1) return NaN;
      for (const s of surfaces) if (insideRing(s.ring, x, y)) return s.heightAt(x, y);
      if (level !== undefined) for (const ring of bays) if (insideRing(ring, x, y)) return level;
      return NaN;
    },

// src/render/buildings/buildingMesh.ts:2364-2371 function insideRing: the feature's own
function insideRing(ring: readonly { x: number; y: number }[], x: number, y: number): boolean {
  let inside = false;
  for (let i = 0, j = ring.length - 1; i < ring.length; j = i++) {
    const a = ring[i]!, c = ring[j]!;
    if ((a.y > y) !== (c.y > y) && x < ((c.x - a.x) * (y - a.y)) / (c.y - a.y) + a.x) inside = !inside;
  }
  return inside;
}

// src/render/buildings/buildingMesh.ts:3699-3703 function signatureFloorOf: the feature's own
export function signatureFloorOf(b: Building, naturalAt: GroundAt, pavedAt?: PavedAt): number | undefined {
  if (!drawsSignature(b)) return undefined;
  const resolved = resolveBlocks(b);
  return resolved.volumes.some((v) => !v.open) ? floorHeight(resolved, naturalAt, pavedAt) : undefined;
}

// src/render/buildings/buildingMesh.ts:3709-3713 slice of src/render/buildings/buildingMesh.ts#letGoEmitted (shared): the feature's statements; "…" marks code left out
export function letGoEmitted(b: Building): void {
  // … 1 lines
  letGoInterior(closed);
// … 1 lines
}

// src/render/buildings/buildingMesh.ts:3723-3728 slice of src/render/buildings/buildingMesh.ts#ShellWriter.next (shared): the feature's statements; "…" marks code left out
  next(): Shell {
    // … 2 lines
    for (const kind of FURNITURE_KINDS) this.furniture[kind]?.clear();
  // … 1 lines
  }

// src/render/buildings/buildingMesh.ts:3735-3778 slice of src/render/buildings/buildingMesh.ts#emitChunk (shared): the feature's statements; "…" marks code left out
export function emitChunk(b: Building, groundAt: GroundAt, pavedAt?: PavedAt, naturalAt: GroundAt = groundAt, slots: SlotAllocator = spaceSeed, far = false, apart = true,
  party?: PartyWalls, writer?: ShellWriter): BuildingChunk {
  // … 15 lines
  const designedFloor = party ? party.floorOf(b) : closed.length > 0 ? floorHeight(resolved, naturalAt, pavedAt) : undefined;
  // … 4 lines
  const body = !signature && closed.length > 0
    ? emitBuilding({ ...resolved, volumes: closed }, groundAt, shell, parts, pavedAt, furnished, inLot, designedFloor, slots, party?.coversNear(b))
    : undefined;
  const floor = signature ? designedFloor : body?.floor;
  // … 1 lines
  const lotGround = emitLots(resolved, lots, closed.length === 0, groundAt, shell, parts, floor, pavedAt, inLot, body?.entrances);
  // … 3 lines
  let furniture: Partial<Record<FurnitureKind, PartBatch>> | undefined;
  for (const kind of FURNITURE_KINDS) {
    const rows = furnished[kind];
    if (!rows || rows.count === 0) continue;
    (furniture ??= {})[kind] = rows.batch();
  }
  // … 2 lines
  const out = furniture ? { ...laid, parts: batches, furniture } : { ...laid, parts: batches };
  const withGround = lotGround ? { ...out, lotGround } : out;
  const withSignature = signature && floor !== undefined ? { ...withGround, signatureFloor: floor } : withGround;
  return flags.length ? { ...withSignature, flags } : withSignature;
}

// src/render/buildings/buildingMesh.ts:3781-3806 slice of src/render/buildings/buildingMesh.ts#chunkSummary (shared): the feature's statements; "…" marks code left out
export function chunkSummary(b: Building, groundAt: GroundAt, pavedAt?: PavedAt, naturalAt: GroundAt = groundAt, party?: PartyWalls): Pick<BuildingChunk, 'flags' | 'lotGround' | 'signatureFloor'> {
  // … 1 lines
  if (fliesFlag(resolved)) return emitChunk(b, groundAt, pavedAt, naturalAt, undefined, false, true, party);
  // … 2 lines
  const floor = party ? party.floorOf(b) : closed.length > 0 ? floorHeight(resolved, naturalAt, pavedAt) : undefined;
  // … 2 lines
  if (lots.length === 0) lotGround = lotGroundOf(resolved, [], floor, false);
  else {
    // The level `emitLots` lays the lots at: the building's floor, else the highest ground of the lots a little over.
    let level = floor;
    if (level === undefined) {
      const f = buildingFrame(resolved);
      let top = -Infinity;
      for (const v of lots) {
        for (const q of localFootprint(v)) top = Math.max(top, groundAt(frameX(f, q.x, q.y), frameY(f, q.x, q.y)));
        top = Math.max(top, groundAt(frameX(f, v.x + v.w / 2, v.y + v.d / 2), frameY(f, v.x + v.w / 2, v.y + v.d / 2)));
      }
      level = top + m(0.12);
    }
    lotGround = lotGroundOf(resolved, lotSurfaces(resolved, level, pavedAt), level, true);
  }
  const out = lotGround ? { lotGround } : {};
  return signature && floor !== undefined ? { ...out, signatureFloor: floor } : out;
}

// src/render/buildings/buildingMesh.ts:3814-3818 slice of src/render/buildings/buildingMesh.ts#shellChunks (shared): the feature's statements; "…" marks code left out
function shellChunks(shell: Shell): Partial<Record<Finish, ShellChunk>> {
  // … 1 lines
  for (const finish of shell.order) { const part = shell.parts.get(finish)!; if (part.index.length > 0) shells[finish] = shellChunkOf(part, shell.reused); }
// … 1 lines
}

// src/render/buildings/buildingMesh.ts:3821-3831 slice of src/render/buildings/buildingMesh.ts#runChunks (shared): the feature's statements; "…" marks code left out
function runChunks(shell: Shell): Partial<Record<Finish, RunChunk>> | undefined {
  // … 1 lines
  for (const finish of shell.runOrder) {
    // … 1 lines
    if (part.index.length === 0 || !part.facade || !part.grid || !part.opening) continue;
  // … 3 lines
  }
// … 1 lines
}

// src/render/buildings/buildingMesh.ts:3907-3924 slice of src/render/buildings/buildingMesh.ts#kitBoxes (shared): the feature's statements; "…" marks code left out
export function kitBoxes(kit: BuildingKit): KitBoxes {
  // … 1 lines
  if (!boxes) {
    // … 6 lines
    boxes = {
      // … 1 lines
      furniture: Object.fromEntries(FURNITURE_KINDS.map((kind) => [kind, boxOf(furniture.geometry[kind])])) as Record<FurnitureKind, number[]>,
    };
  // … 1 lines
  }
// … 2 lines
}

// src/render/buildings/buildingMesh.ts:3943-4145 slice of src/render/buildings/buildingMesh.ts#meshesOf (shared): the feature's statements; "…" marks code left out
export function meshesOf(arrays: CellArrays, kit: BuildingKit, ghost: boolean, dim: boolean, refill?: () => CellArrays): BuildingMeshes {
  // … 17 lines
  arrays.shells.forEach((shell, piece) => {
    // … 49 lines
    triangles += shell.index.length / 3;
  });
  // … 1 lines
  for (const kind of PART_KINDS) {
    // … 11 lines
    if (glassy) {
      // … 2 lines
      roomBuffer = new InstancedInterleavedBuffer(part.rooms!, ROOM_FLOATS);
      // … 1 lines
      (roomBuffer as InstancedInterleavedBuffer & { isFloat16BufferAttribute: boolean }).isFloat16BufferAttribute = true;
      geometry.setAttribute('roomA', new InterleavedBufferAttribute(roomBuffer, 4, 0));
      geometry.setAttribute('roomB', new InterleavedBufferAttribute(roomBuffer, 4, 4));
      geometry.setAttribute('roomC', new InterleavedBufferAttribute(roomBuffer, 4, 8));
      // … 1 lines
      geometry.setAttribute('roomD', new InterleavedBufferAttribute(roomBuffer, 4, PANE_FRAME_AT));
      geometry.setAttribute('roomE', new InterleavedBufferAttribute(roomBuffer, 4, PANE_RECESS_AT));
    }
    // … 20 lines
    if (!ghost) {
      // … 2 lines
      if (glassy) {
        // … 1 lines
        own.rooms = roomBuffer!;
      }
    // … 13 lines
    }
    // … 2 lines
    triangles += ((g.index ? g.index.count : g.getAttribute('position').count) / 3) * count;
  }
  // … 3 lines
  const furnished: FurnitureKind[] = [];
  for (const kind of FURNITURE_KINDS) if (pieces[kind]) furnished.push(kind);
  if (furnished.length) {
    const furniture = kit.furniture();
    for (const kind of furnished) {
      const piece = pieces[kind]!;
      const poseBuffer = new InstancedInterleavedBuffer(piece.poses, POSE_FLOATS);
      const material = dim ? kit.dimParts : furniture.material;
      const mesh = new InstancedMesh(posedCopy(furniture.geometry[kind], poseBuffer), material, 0);
      mesh.count = piece.count;
      mesh.userData['posedFrom'] = furniture.geometry[kind];
      mesh.name = `building-furniture-${kind}`;
      mesh.castShadow = true;
      mesh.customDepthMaterial = partPoseDepth(material);
      mesh.receiveShadow = true;
      // Faded, white instance colours as every faded part has (`cellArrays.ts`): one program variant of `dimParts`.
      if (dim) mesh.instanceColor = new InstancedBufferAttribute(new Float32Array(piece.count * 3).fill(1), 3);
      mesh.boundingBox = boxOf(piece.bounds);
      mesh.boundingSphere = sphereOf(piece.bounds);
      // On the GPU only, made again from the chunks (`mesh/gpuOnly.ts`).
      gpuOnlyAttributes(mesh, { poses: poseBuffer }, () => {
        const again = refill?.().furniture[kind];
        return again ? { poses: again.poses } : {};
      });
      meshes.push(mesh);
      const g = furniture.geometry[kind];
      triangles += ((g.index ? g.index.count : g.getAttribute('position').count) / 3) * piece.count;
    }
  }
  // … 9 lines
  return {
    // … 1 lines
    triangles,
  // … 16 lines
  };
}

// src/render/buildings/buildingMesh.ts:4128-4130 method meshesOf.setVisible: the feature's own
    setVisible(visible) {
      group.visible = visible;
    },

// src/render/buildings/buildingMesh.ts:4175-4196 slice of src/render/buildings/buildingMesh.ts#interiorFurniture (shared): the feature's statements; "…" marks code left out
export function interiorFurniture(b: Building, floor: number): Partial<Record<FurnitureKind, PartBatch>> {
  // … 4 lines
  for (let level = 0; level < top; level++) {
    for (const f of interiorAt(b, level).furniture) {
      // Each piece on its own block's floor (a split level, `Volume.lift`).
      const z = floor + liftAt(b, level, f.x, f.y) + levelElevation(b, level);
      placeFurniture(e, f, f.kind === 'ceilingLamp' ? z + levelHeight(b, level) - m(0.06) - f.h : z);
    }
  // … 2 lines
  }
  const out: Partial<Record<FurnitureKind, PartBatch>> = {};
  for (const kind of FURNITURE_KINDS) {
    const rows = placed[kind];
    if (!rows?.count) continue;
    out[kind] = rows.batch();
  }
  return out;
}
