// The code of src/render/destruction.ts that is the feature's (the rest of the file is shared).

// src/render/destruction.ts:24-38 slice of src/render/destruction.ts#Piece (shared): the feature's statements; "…" marks code left out
interface Piece {
  readonly fragment: Fragment;
// … 12 lines
}

// src/render/destruction.ts:40-48 slice of src/render/destruction.ts#Ruin (shared): the feature's statements; "…" marks code left out
interface Ruin {
  // … 5 lines
  readonly data: StillData;
// … 1 lines
}

// src/render/destruction.ts:53-65 slice of src/render/destruction.ts#Destruction (shared): the feature's statements; "…" marks code left out
export interface Destruction {
  // … 4 lines
  hit(b: Building, floor: number, x: number, y: number, z: number, strength: number, eye?: Vector3): boolean;
// … 6 lines
}

// src/render/destruction.ts:67-409 slice of src/render/destruction.ts#createDestruction (shared): the feature's statements; "…" marks code left out
export function createDestruction(
  // … 1 lines
  chunkOf: (b: Building) => { chunk: BuildingChunk; kit: BuildingKit } | null,
): Destruction {
  // … 11 lines
  const jobs = new Map<number, { b: Building; floor: number; materials: Material[]; hits: [number, number, number, number, Vector3 | undefined][]; worker?: Worker }>();
// … 326 lines
}

// src/render/destruction.ts:84-90 slice of src/render/destruction.ts#createDestruction.fail (shared): the feature's statements; "…" marks code left out
  const fail = (id: number, why: string): void => {
    // … 3 lines
    if (pending.get(job.b.id) === id) pending.delete(job.b.id);
    console.error(`[fracture] building ${job.b.id}: ${why}`);
  };

// src/render/destruction.ts:93-118 slice of src/render/destruction.ts#createDestruction.workerOf (shared): the feature's statements; "…" marks code left out
  const workerOf = (): Worker => {
    // … 1 lines
    if (workers.length < size) {
      // … 5 lines
      worker.onmessage = (e: MessageEvent<{ id: number; still?: StillData; error?: string }>) => {
        if (e.data.error !== undefined || !e.data.still) { fail(e.data.id, e.data.error ?? 'no pieces'); return; }
        const job = jobs.get(e.data.id);
        jobs.delete(e.data.id);
        if (!job) return;
        pending.delete(job.b.id);
        const ruin = place(e.data.still, job.materials, job.floor);
        ruins.set(job.b.id, ruin);
        ruined.add(job.b.id);
        api.onRuined?.();
        let down = false;
        for (const [x, y, z, strength, eye] of job.hits) down = strike(ruin, job.floor, x, y, z, strength, eye) || down;
        if (down) api.onDown?.(job.b.id);
      };
    // … 1 lines
    }
  // … 1 lines
  };

// src/render/destruction.ts:121-154 slice of src/render/destruction.ts#createDestruction.place (shared): the feature's statements; "…" marks code left out
  const place = (data: StillData, materials: Material[], floor: number): Ruin => {
    const fragments = data.fragments;
    // The worker's arrays as they are
    const geometry = new BufferGeometry();
    geometry.setAttribute('position', new BufferAttribute(data.position, 3));
    geometry.setAttribute('normal', new BufferAttribute(data.normal, 3));
    geometry.setAttribute('color', new BufferAttribute(data.colour, 3));
    geometry.setAttribute('uv', new BufferAttribute(data.uv, 2));
    geometry.setAttribute('aDecay', new BufferAttribute(data.decay, 1));
    // The shell's pieces in its one material, each vertex its finish (`finishes.ts`).
    geometry.setAttribute(FINISH_ATTRIBUTE, new BufferAttribute(data.finish, 1));
    for (const g of data.groups) geometry.addGroup(g.start, g.count, g.material);
    const bounds = new Sphere();
    for (const f of fragments) {
      const own = reach.set(scratch.set(f.centre[0], f.centre[1], f.centre[2]), f.radius);
      if (bounds.isEmpty()) bounds.copy(own); else bounds.union(own);
    }
    geometry.boundingSphere = bounds;
    const still = new Mesh(geometry, materials);
    still.castShadow = true;
    still.receiveShadow = true;
    still.matrixAutoUpdate = false;
    group.add(still);
    const pieces: Piece[] = fragments.map((fragment, i) => {
      const mesh = new Mesh(NO_GEOMETRY, materials);
      mesh.position.set(fragment.centre[0], fragment.centre[1], fragment.centre[2]);
      mesh.castShadow = true;
      mesh.receiveShadow = true;
      return { fragment, index: i, mesh, floor, falling: false, settled: false, slots: data.slots[i] ?? new Int32Array(0), v: new Vector3(), w: new Vector3(), age: 0 };
    });
    const ruin: Ruin = { pieces, floor, still, data, standing: pieces.length };
    for (const piece of pieces) owner.set(piece, ruin);
    return ruin;
  };

// src/render/destruction.ts:170-202 slice of src/render/destruction.ts#createDestruction.land (shared): the feature's statements; "…" marks code left out
  const land = (ruin: Ruin, piece: Piece): void => {
    // … 26 lines
    geometry.boundingSphere?.union(reach.set(mesh.position, piece.fragment.radius));
  // … 4 lines
  };

// src/render/destruction.ts:205-226 slice of src/render/destruction.ts#createDestruction.make (shared): the feature's statements; "…" marks code left out
  const make = (b: Building, floor: number): number | null => {
    const source = chunkOf(b);
    if (!source) return null;
    // Floor slabs inside every volume, storey by storey.
    const slabs: { corners: Vector3[]; y: number }[] = [];
    for (const v of resolveBlocks(b).volumes) {
      if (v.open) continue;
      const corners = [[v.x, v.y], [v.x + v.w, v.y], [v.x + v.w, v.y + v.d], [v.x, v.y + v.d]].map(([lx, ly]) => {
        const p = localToWorld(b, lx!, ly!);
        return new Vector3(p.x, 0, -p.y);
      });
      // At each block's own floors (a split level stands at its own, `Volume.lift`).
      for (let level = v.base + 1; level < v.base + v.storeys.length; level++) slabs.push({ corners, y: floor + volumeElevation(b, v, level) });
    }
    // Its lots' boxes broken as the shell they are drawn as (`shellParts.ts`): their finish, tone and wear on every fragment.
    const { input, materials } = prepareFracture(withShellParts(source.chunk), source.kit, slabs, b.id * 2654435761, interiorFurniture(b, floor));
    const id = nextJob++;
    const worker = workerOf();
    jobs.set(id, { b, floor, materials, hits: [], worker });
    worker.postMessage({ id, input });
    return id;
  };

// src/render/destruction.ts:237-254 slice of src/render/destruction.ts#createDestruction.loosen (shared): the feature's statements; "…" marks code left out
  const loosen = (ruin: Ruin, piece: Piece, from: Vector3, power: number): void => {
    // … 1 lines
    piece.mesh.geometry = fragmentGeometry(ruin.data, piece.index);
    // … 10 lines
    if (Math.random() < 0.5) {
      // … 1 lines
      exhaust.burst(p.x, -p.z, p.y, 3, 3, piece.fragment.radius * 0.6, m(2.5), 3.5);
    }
  };

// src/render/destruction.ts:256-310 slice of src/render/destruction.ts#createDestruction.strike (shared): the feature's statements; "…" marks code left out
  const strike = (ruin: Ruin, floor: number, x: number, y: number, z: number, strength: number, eye?: Vector3): boolean => {
      // … 2 lines
      if (eye) {
        // `eye` is the camera's looking direction (an orthographic camera's rays are parallel).
        const dir = eye.clone().normalize();
        eye = impact.clone().addScaledVector(dir, -m(2000));
        let best: Vector3 | null = null, bestT = Infinity;
        const to = new Vector3();
        for (const p of ruin.pieces) {
          if (p.falling) continue;
          to.copy(p.mesh.position).sub(eye);
          const t = to.dot(dir);
          if (t <= 0) continue;
          const off = to.clone().addScaledVector(dir, -t).length();
          if (off < p.fragment.radius * 0.8 + m(1.5) && t < bestT) { bestT = t; best = p.mesh.position.clone(); }
        }
        // Nothing on the line: the standing piece nearest the point aimed at.
        if (!best) {
          let bd = Infinity;
          for (const p of ruin.pieces) {
            if (p.falling || p.fragment.centre[1] < floor + m(0.6)) continue;
            const d = p.mesh.position.distanceTo(impact);
            if (d < bd) { bd = d; best = p.mesh.position.clone(); }
          }
        }
        if (best) impact = best;
      }
      // … 2 lines
      for (const piece of ruin.pieces) {
        // … 1 lines
        const d = piece.mesh.position.distanceTo(impact) - piece.fragment.radius * 0.5;
        if (d < reach) release(ruin, piece, impact, power * Math.max(0.3, 1 - d / reach));
      }
      exhaust.burst(impact.x, -impact.z, impact.y, 10 + strength * 2, 3, reach * 0.6, m(2.5 + strength * 0.2), 4);
      exhaust.burst(impact.x, -impact.z, impact.y, 20 + strength * 4, 9, reach * 0.4, m(0.3), 18);
      // … 3 lines
      const upper = ruin.pieces.filter((p) => p.fragment.centre[1] > floor + m(0.6));
      if (upper.filter((p) => !p.falling).length < upper.length * COLLAPSE_BELOW) {
        for (const piece of ruin.pieces) {
          // What lies on the ground (the lot's paving, the plinth) stays as it is in the still mesh, and goes with the rubble later.
          if (piece.fragment.centre[1] < floor + m(0.6)) {
            if (!piece.falling) { piece.falling = true; piece.settled = true; ruin.standing--; }
          } else release(ruin, piece, impact, m(0.6));
        }
        exhaust.burst(x, y, floor, 60, 3, m(10), m(5), 6);
        // Papers from the offices and homes, fluttering over the street for a while.
        exhaust.burst(x, y, z, 120, 9, m(8), m(0.35), 22);
        return true;
      }
  // … 1 lines
  };

// src/render/destruction.ts:313-326 slice of src/render/destruction.ts#createDestruction.unsupported (shared): the feature's statements; "…" marks code left out
  const unsupported = (ruin: Ruin): Piece[] => {
    // … 2 lines
    ruin.pieces.forEach((p, i) => {
      if (!p.falling && p.fragment.low <= ruin.floor + m(0.6)) { held[i] = 1; stack.push(i); }
    });
    while (stack.length) {
      // … 1 lines
      for (const j of ruin.pieces[i]!.fragment.neighbours) {
        if (!held[j] && !ruin.pieces[j]!.falling) { held[j] = 1; stack.push(j); }
      }
    }
  // … 1 lines
  };

// src/render/destruction.ts:335-351 slice of src/render/destruction.ts#createDestruction.hit (shared): the feature's statements; "…" marks code left out
    hit(b, floor, x, y, z, strength, eye) {
      const ruin = ruins.get(b.id);
      if (!ruin) {
        let job = pending.get(b.id);
        if (job === undefined) {
          const made = make(b, floor);
          if (made === null) return false;
          job = made;
          pending.set(b.id, job);
        }
        jobs.get(job)?.hits.push([x, y, z, strength, eye?.clone()]);
        // Dust where it was struck, at once, while its pieces are being made.
        exhaust.burst(x, y, z, 12 + strength * 2, 3, m(2 + strength * 0.4), m(2.5 + strength * 0.2), 4);
        return false;
      }
      return strike(ruin, floor, x, y, z, strength, eye);
    },

// src/render/destruction.ts:352-391 slice of src/render/destruction.ts#createDestruction.update (shared): the feature's statements; "…" marks code left out
    update(dt) {
      // … 11 lines
      for (let i = loose.length - 1; i >= 0; i--) {
        // … 3 lines
        if (!p.settled) {
          // … 3 lines
          const rest = p.floor + p.fragment.radius * 0.3;
          if (mesh.position.y <= rest) {
            mesh.position.y = rest;
            if (p.v.y < -m(3)) {
              p.v.multiplyScalar(0.3); p.v.y = Math.abs(p.v.y) * 0.5; p.w.multiplyScalar(0.4);
              if (p.fragment.radius > m(1.2) && Math.random() < 0.3) exhaust.burst(mesh.position.x, -mesh.position.z, p.floor, 3, 3, m(1.5), m(3), 4);
            } else {
              // Settled: it stays where it fell, a heap of the building, for good (the player: "do not make the rubble vanish")
              p.settled = true;
              p.v.set(0, 0, 0);
              p.age = 0;
              loose.splice(i, 1);
              const ruin = owner.get(p);
              if (ruin) land(ruin, p);
            }
          }
        }
      }
    // … 1 lines
    },
