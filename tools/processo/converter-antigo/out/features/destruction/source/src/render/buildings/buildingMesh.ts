// The code of src/render/buildings/buildingMesh.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/buildingMesh.ts:3837-3871 function shellPartFaces: the feature's own
export function shellPartFaces(parts: Readonly<Record<PartKind, PartBatch>>): Partial<Record<Finish, ShellChunk>> {
  const shell = new Shell();
  for (const kind of SHELL_PART_KINDS) {
    const batch = parts[kind];
    if (!batch?.count || !batch.looks || !batch.colours) continue;
    const { poses, colours, looks } = batch;
    const e = SHELL_PART_MATRIX, o = 0;
    for (let i = 0; i < batch.count; i++) {
      // The instance's matrix as the GPU makes it from its pose (`partPose.ts`).
      poseMatrixInto(poses, i * POSE_FLOATS, e, 0);
      const sx = poses[i * POSE_FLOATS + 5]!, sz = poses[i * POSE_FLOATS + 7]!;
      // A model point (three's axes) through the matrix, then into the map's (x, y north, z up).
      const at = (x: number, y: number, z: number, w: number): V3 => {
        const X = e[o]! * x + e[o + 4]! * y + e[o + 8]! * z + e[o + 12]! * w;
        const Y = e[o + 1]! * x + e[o + 5]! * y + e[o + 9]! * z + e[o + 13]! * w;
        const Z = e[o + 2]! * x + e[o + 6]! * y + e[o + 10]! * z + e[o + 14]! * w;
        return [X, -Z, Y];
      };
      const ground = looks[i * LOOK_FLOATS + 2]!;
      shell.decay = looks[i * LOOK_FLOATS + 1]!;
      shell.ground = ground > NO_GROUND / 2 ? ground : Number.NaN;
      const paint: Paint = { rgb: [colours[i * 3]!, colours[i * 3 + 1]!, colours[i * 3 + 2]!], finish: FINISHES[looks[i * LOOK_FLOATS]!]! };
      for (const f of SHELL_PART_FACES[kind]) {
        const points = f.points.map(([u, v, w]): V3 => {
          // A paving's side stands in from its top's edge by `COVER_INSET`, whatever its size.
          const x = f.inset ? Math.sign(u) * Math.max(0.5 - COVER_INSET / sx, 0) : u;
          const z = f.inset ? Math.sign(-v) * Math.max(0.5 - COVER_INSET / sz, 0) : -v;
          return at(x, w, z, 1);
        });
        shell.face(points, at(f.n[0], f.n[2], -f.n[1], 0), paint);
      }
    }
  }
  return shellChunks(shell);
}

// src/render/buildings/buildingMesh.ts:3874-3902 function withShellParts: the feature's own
export function withShellParts(chunk: BuildingChunk): BuildingChunk {
  const laid = shellPartFaces(chunk.parts);
  const shells: Partial<Record<Finish, ShellChunk>> = { ...chunk.shells };
  for (const [finish, add] of Object.entries(laid) as [Finish, ShellChunk][]) {
    const was = shells[finish];
    if (!was) { shells[finish] = add; continue; }
    const base = was.position.length / 3;
    // `b` after `a` (a part's faces are never a run of bays: the runs stay as they were).
    const joined = <T extends Float32Array | Int16Array | Uint16Array>(a: T, b: T, make: (n: number) => T): T => {
      const out = make(a.length + b.length);
      out.set(a);
      out.set(b, a.length);
      return out;
    };
    const index = new Uint32Array(was.index.length + add.index.length);
    index.set(was.index);
    for (let k = 0; k < add.index.length; k++) index[was.index.length + k] = add.index[k]! + base;
    shells[finish] = {
      position: joined(was.position, add.position, (n) => new Float32Array(n)),
      normal: joined(was.normal, add.normal, (n) => new Int16Array(n)),
      paint: joined(was.paint, add.paint, (n) => new Uint16Array(n)),
      index,
    };
  }
  const none: PartBatch = { poses: new Float32Array(0), colours: new Float32Array(0), count: 0, looks: new Float32Array(0) };
  const parts = { ...chunk.parts } as Record<PartKind, PartBatch>;
  for (const kind of SHELL_PART_KINDS) parts[kind] = none;
  return { ...chunk, shells, parts };
}

// src/render/buildings/buildingMesh.ts:4175-4196 function interiorFurniture: the feature's own
export function interiorFurniture(b: Building, floor: number): Partial<Record<FurnitureKind, PartBatch>> {
  const placed: FurnitureSink = {};
  const e = new Emitter(b, new Shell(), {} as PartSink, placed);
  let top = 0;
  for (const v of b.volumes) if (!v.open && v.mode !== 'void') top = Math.max(top, volumeTop(v));
  for (let level = 0; level < top; level++) {
    for (const f of interiorAt(b, level).furniture) {
      // Each piece on its own block's floor (a split level, `Volume.lift`).
      const z = floor + liftAt(b, level, f.x, f.y) + levelElevation(b, level);
      placeFurniture(e, f, f.kind === 'ceilingLamp' ? z + levelHeight(b, level) - m(0.06) - f.h : z);
    }
    // Enough to fill the air when it breaks; a tower's every chair cost a second.
    if (Object.values(placed).reduce((n, l) => n + (l?.count ?? 0), 0) > 140) break;
  }
  const out: Partial<Record<FurnitureKind, PartBatch>> = {};
  for (const kind of FURNITURE_KINDS) {
    const rows = placed[kind];
    if (!rows?.count) continue;
    out[kind] = rows.batch();
  }
  return out;
}
