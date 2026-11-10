// The code of src/actionsWiring.ts that is the feature's (the rest of the file is shared).

// src/actionsWiring.ts:24-24 slice of src/actionsWiring.ts#RAY_ABOVE (shared): the feature's statements; "…" marks code left out
const RAY_ABOVE = MAX_STOREYS * MAX_STOREY_HEIGHT + m(40);

// src/actionsWiring.ts:57-66 slice of src/actionsWiring.ts#Actions (shared): the feature's statements; "…" marks code left out
export interface Actions {
  // … 5 lines
  explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet?: boolean): void;
// … 2 lines
}

// src/actionsWiring.ts:68-475 slice of src/actionsWiring.ts#createActions (shared): the feature's statements; "…" marks code left out
export function createActions(deps: ActionsDeps): Actions {
  // … 320 lines
  scene.onBuildingDown((id) => {
    if (!doc.buildings.has(id as BuildingId)) return;
    deps.mutate(() => { doc.buildings.remove(id as BuildingId); scene.forgetRuin(id); burning.delete(id); return true; });
  // … 1 lines
  });
// … 81 lines
}

// src/actionsWiring.ts:213-226 slice of src/actionsWiring.ts#createActions.strikeAt (shared): the feature's statements; "…" marks code left out
  function strikeAt(sx: number, sy: number, world: Vec2): void {
    // … 3 lines
    const b = id !== null ? doc.buildings.get(id as BuildingId)
      : buildingsUnderRay(sx, sy, ground(world)).find((c) => rayOnBuilding(c, sx, sy, ground({ x: c.x, y: c.y })) !== null);
    // … 2 lines
    if (b) {
      const hit = rayOnBuilding(b, sx, sy, z);
      if (hit) { target = hit; z = hit.z; }
    }
    explodeAt(target, z, b ?? null, strength);
  }

// src/actionsWiring.ts:229-386 slice of src/actionsWiring.ts#createActions.explodeAt (shared): the feature's statements; "…" marks code left out
  function explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet = false): void {
    // … 7 lines
    const struck: BlastHit['ground'] = b && z > ground(world) + m(0.5) ? 'building' : paved ? 'road' : 'earth';
    const hit: { ground: BlastHit['ground']; crater: boolean; poles: BlastHit['poles'][number][]; wires: BlastHit['wires'][number][];
      // … 1 lines
      { ground: struck, crater: struck !== 'building', poles: [], wires: [], posts: [], vehicles: [], items: [] };
    // … 1 lines
    deps.mutate(() => {
      // … 2 lines
      for (const c of doc.buildings.near(world.x - radius, world.y - radius, world.x + radius, world.y + radius)) {
        let best = Infinity, px = world.x, py = world.y;
        // Its farthest corner from its own centre: past that and the reach, nothing of it is struck.
        const own = solidFootprints(c);
        let extent = 0;
        for (const ring of own) for (const q of ring) extent = Math.max(extent, Math.hypot(q.x - c.x, q.y - c.y));
        if (Math.hypot(c.x - world.x, c.y - world.y) - extent > radius) continue;
        for (const ring of own) {
          if (pointInPolygon(world, ring)) { best = 0; break; }
          for (let i = 0; i < ring.length; i++) {
            const q = closestOnSegment(world, ring[i]!, ring[(i + 1) % ring.length]!).point;
            const d = Math.hypot(q.x - world.x, q.y - world.y);
            if (d < best) { best = d; px = q.x; py = q.y; }
          }
        }
        if (best > radius) continue;
        // Where it is struck.
        const at = c === b ? { x: world.x, y: world.y, z } : { x: px, y: py, z: Math.max(z, ground({ x: px, y: py }) + m(1.5)) };
        const force = c === b ? strength : Math.max(1, strength * (1 - best / radius) * 1.2);
        // Breaking a building into its pieces is the costly part (a Voronoi fracture of its meshes)
        if (c !== b) { if (best < radius * 0.75 || force >= 4) { deferredHits.push({ id: c.id, ...at, force }); scheduleBreak(); } continue; }
        if (scene.strikeBuilding(c, at.x, at.y, at.z, force)) {
          doc.buildings.remove(c.id);
          scene.forgetRuin(c.id);
          burning.delete(c.id);
          downs++;
          changed = true;
        } else {
          // Its wiring shorting and its pipes bursting at a few points on the side the blast struck
          const base = ground({ x: c.x, y: c.y });
          const floors = Math.max(1, Math.max(...c.volumes.map((v) => v.base + v.storeys.length)));
          const spots = shorted.has(c.id) ? 0 : Math.min(2, 1 + Math.round(force / 8));
          shorted.add(c.id);
          for (let k = 0; k < spots; k++) {
            const h = base + levelElevation(c, Math.floor(Math.random() * floors)) + m(1 + Math.random() * 1.5);
            const jx = px + (Math.random() - 0.5) * m(6), jy = py + (Math.random() - 0.5) * m(6);
            if (Math.random() < 0.75) scene.sparkAt(jx, jy, h, 0.6 + Math.random() * 0.8);
            else scene.leak(jx, jy, h, 1 + Math.random() * 1.5);
          }
        }
      }
      // … 2 lines
      for (const c of doc.buildings.near(world.x - soot, world.y - soot, world.x + soot, world.y + soot)) {
        const d = Math.hypot(c.x - world.x, c.y - world.y);
        if (d > soot) continue;
        const add = 0.35 * (1 - d / soot) * Math.min(1, strength / 8);
        if (add > 0.02) { deps.caused('explosion soot', () => doc.buildings.wear(c.id, Math.min(1, (c.decay ?? 0) + add))); changed = true; }
      }
      // … 50 lines
      if (struck === 'earth') {
        doc.addTerrainStamp({ x: world.x, y: world.y, radius: radius * 0.5, strength: m(0.6 + strength * 0.12), mode: 'lower' });
        changed = true;
      }
      return changed;
    });
    // … 31 lines
    if (struck === 'road') scene.strikeGround(world.x, world.y, strength);
    // … 1 lines
    if (!quiet) deps.hint(downs > 0 ? 'hint.strike.down' : deadNear > 0 ? 'hint.strike.deaths' : b ? 'hint.strike.hit' : 'hint.strike.ground');
    if (b && doc.buildings.has(b.id)) ignite(b.id);
  // … 1 lines
  }

// src/actionsWiring.ts:406-412 slice of src/actionsWiring.ts#createActions.breakOne (shared): the feature's statements; "…" marks code left out
  function breakOne(next: { id: number; x: number; y: number; z: number; force: number }): void {
    const c = doc.buildings.get(next.id as BuildingId);
    if (c && scene.strikeBuilding(c, next.x, next.y, next.z, next.force)) {
      deps.mutate(() => { doc.buildings.remove(c.id); scene.forgetRuin(c.id); burning.delete(c.id); return true; });
    }
  // … 1 lines
  }

// src/actionsWiring.ts:422-445 slice of src/actionsWiring.ts#createActions.tick (shared): the feature's statements; "…" marks code left out
  function tick(now: number): void {
    // … 3 lines
    for (const [id, f] of [...burning]) {
      const b = doc.buildings.get(id as BuildingId);
      if (!b || now > f.until) { burning.delete(id); continue; }
      const pts = solidFootprints(b).flat();
      if (!pts.length) { burning.delete(id); continue; }
      const cx = pts.reduce((a, p) => a + p.x, 0) / pts.length, cy = pts.reduce((a, p) => a + p.y, 0) / pts.length;
      const size = Math.min(m(30), Math.max(m(6), Math.hypot(pts[0]!.x - cx, pts[0]!.y - cy)));
      const base = ground({ x: cx, y: cy });
      if (now >= f.nextFlame) {
        // … 1 lines
        for (let k = 0; k < 2; k++) {
          const p = pts[Math.floor(Math.random() * pts.length)]!;
          const x = cx + (p.x - cx) * Math.random(), y = cy + (p.y - cy) * Math.random();
          scene.burn(x, y, base + m(2 + Math.random() * 10), Math.min(size * 0.3, m(3)), 7);
        }
      // … 1 lines
      }
    }
  // … 1 lines
  }

// src/actionsWiring.ts:448-453 slice of src/actionsWiring.ts#createActions.buildingsUnderRay (shared): the feature's statements; "…" marks code left out
  function buildingsUnderRay(sx: number, sy: number, groundZ: number): Building[] {
    // … 2 lines
    const high = view.toWorldAt(sx, sy, groundZ + RAY_ABOVE, w, h), low = view.toWorldAt(sx, sy, groundZ - RAY_BELOW, w, h);
    return doc.buildings.near(Math.min(high.x, low.x), Math.min(high.y, low.y), Math.max(high.x, low.x), Math.max(high.y, low.y));
  }

// src/actionsWiring.ts:456-472 slice of src/actionsWiring.ts#createActions.rayOnBuilding (shared): the feature's statements; "…" marks code left out
  function rayOnBuilding(b: Building, sx: number, sy: number, base: number): (Vec2 & { z: number }) | null {
    // … 2 lines
    const volumes = resolveBlocks(b).volumes.filter((v) => !v.open);
    // … 2 lines
    for (const v of volumes) { top = Math.max(top, volumeElevation(b, v, v.base + v.storeys.length)); low = Math.min(low, volumeElevation(b, v, v.base)); }
    for (let h = top + m(1); h >= low; h -= m(0.5)) {
      const p = view.toWorldAt(sx, sy, base + h, w, height);
      const l = worldToLocal(b, p);
      for (const v of volumes) {
        if (l.x < v.x || l.x > v.x + v.w || l.y < v.y || l.y > v.y + v.d) continue;
        if (h <= volumeElevation(b, v, v.base + v.storeys.length) && h >= volumeElevation(b, v, v.base)) return { x: p.x, y: p.y, z: base + h };
      }
    }
  // … 1 lines
  }
