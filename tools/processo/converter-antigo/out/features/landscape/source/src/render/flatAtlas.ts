// The code of src/render/flatAtlas.ts that is the feature's (the rest of the file is shared).

// src/render/flatAtlas.ts:56-56 slice of src/render/flatAtlas.ts#SIDE (shared): the feature's statements; "…" marks code left out
const SIDE = Math.round((2 * TERRAIN_HALF) / TERRAIN_CELL) + 1;

// src/render/flatAtlas.ts:71-85 slice of src/render/flatAtlas.ts#PlateSource (shared): the feature's statements; "…" marks code left out
class PlateSource implements TerrainSource {
  changes!: RoadDoc['changes'];
  terrainRevision = 0;
  terrainStamps: TerrainStamp[] = [];
  terrainRelief!: RoadDoc['terrainRelief'];
  natureRevision = 0;
  nature: RoadDoc['nature'] = null;
  paintRevision = 0;
  terrainPaint: PaintDab[] = [];
  stampSources: readonly TerrainStamp[] = [];
  paintSources: readonly PaintDab[] = [];
// … 3 lines
}

// src/render/flatAtlas.ts:74-74 slice of src/render/flatAtlas.ts#PlateSource.terrainStamps (shared): the feature's statements; "…" marks code left out
  terrainStamps: TerrainStamp[] = [];

// src/render/flatAtlas.ts:79-79 slice of src/render/flatAtlas.ts#PlateSource.terrainPaint (shared): the feature's statements; "…" marks code left out
  terrainPaint: PaintDab[] = [];

// src/render/flatAtlas.ts:80-80 slice of src/render/flatAtlas.ts#PlateSource.stampSources (shared): the feature's statements; "…" marks code left out
  stampSources: readonly TerrainStamp[] = [];

// src/render/flatAtlas.ts:81-81 slice of src/render/flatAtlas.ts#PlateSource.paintSources (shared): the feature's statements; "…" marks code left out
  paintSources: readonly PaintDab[] = [];

// src/render/flatAtlas.ts:87-109 slice of src/render/flatAtlas.ts#Plate (shared): the feature's statements; "…" marks code left out
interface Plate {
  // … 7 lines
  readonly stampCopies: WeakMap<TerrainStamp, TerrainStamp>;
  readonly dabCopies: WeakMap<PaintDab, PaintDab>;
  surface: PlateSurface | null;
// … 11 lines
}

// src/render/flatAtlas.ts:112-116 slice of src/render/flatAtlas.ts#FarResident (shared): the feature's statements; "…" marks code left out
interface FarResident {
  readonly node: FarNode;
// … 2 lines
}

// src/render/flatAtlas.ts:119-120 slice of src/render/flatAtlas.ts#reaches (shared): the feature's statements; "…" marks code left out
const reaches = (p: { readonly cx: number; readonly cy: number }, x: number, y: number, radius: number): boolean =>
  Math.abs(x - p.cx) <= TERRAIN_HALF + radius && Math.abs(y - p.cy) <= TERRAIN_HALF + radius;

// src/render/flatAtlas.ts:141-143 slice of src/render/flatAtlas.ts#writeFarPatch (shared): the feature's statements; "…" marks code left out
export function writeFarPatch(far: FlatFar, slot: number, cx: number, cy: number, ground: (x: number, y: number) => number): void {
  writeFarNode(far, slot, cx - TERRAIN_HALF, cy - TERRAIN_HALF, PLATE_SIZE / FAR_QUADS, SKIRT, ground);
}

// src/render/flatAtlas.ts:150-187 slice of src/render/flatAtlas.ts#writeFarNode (shared): the feature's statements; "…" marks code left out
export function writeFarNode(far: FlatFar, slot: number, x0: number, y0: number, step: number, skirt: number, ground: (x: number, y: number) => number): void {
  // … 6 lines
  for (let j = 0; j < RING; j++) for (let i = 0; i < RING; i++) h[j * RING + i] = TERRAIN_BASE + ground(x0 + (i - 1) * step, y0 + (j - 1) * step);
// … 29 lines
}

// src/render/flatAtlas.ts:231-960 slice of src/render/flatAtlas.ts#createFlatAtlas (shared): the feature's statements; "…" marks code left out
export function createFlatAtlas(anisotropy: number): TerrainSurface {
  // … 5 lines
  const share = new GroundShare(SIDE, TERRAIN_CELL, TERRAIN_HALF, TERRAIN_BASE);
  // … 2 lines
  const farGround = farLandMaterial(anisotropy);
  // … 1 lines
  const farMesh = new Mesh(far.geometry, farGround.material);
  farMesh.matrixAutoUpdate = false;
  farMesh.name = 'flat-world-far';
  farMesh.frustumCulled = false;
  farMesh.receiveShadow = true;
  farMesh.visible = false;
  root.add(farMesh);
  // … 1 lines
  const slab = new Mesh(new BufferGeometry(), wallMaterial(anisotropy));
  slab.name = 'flat-world-slab';
  slab.receiveShadow = true;
  slab.visible = false;
  root.add(slab);
  // … 1 lines
  const farForest = createFarForest(WOODS_MOST, { side: SIDE, cell: TERRAIN_CELL, half: TERRAIN_HALF, base: TERRAIN_BASE, farQuads: FAR_QUADS }, readFarWoods);
  root.add(farForest.object);
  // … 5 lines
  let farRelief: ReliefVersion | null = null;
  // … 26 lines
  let slabRelief: ReliefVersion | null = null;
  // … 68 lines
  let partsShown: TerrainPart[] = [];
  // … 3 lines
  let lastDoc: TerrainSource | null = null;
  // … 3 lines
  let seenRelief: ReliefVersion | null = null;
  // … 3 lines
  let atlas: TerrainSurface | null = null;
  // … 335 lines
  const geologyChanges = new GroundChanges();
  // … 11 lines
  atlas = {
    // … 3 lines
    ground: firstSurface.ground,
    get parts() { return partsShown; },
    focus(x, y, reach) {
      // … 7 lines
      const radius = reach < NEAR_VIEW ? Math.max(TERRAIN_HALF, reach * 1.6) + TERRAIN_HALF : -1;
      keepReach = radius > 0 ? radius * KEEP_MARGIN : 0;
      const key = `${plateOf(x, y)}:${Math.round(radius / TERRAIN_HALF)}`;
      if (key !== focusKey) {
        focusKey = key;
        wantedNear.length = 0;
        if (radius > 0) {
          // The plates whose centre is within the radius: those under its square, never a walk over the world's.
          for (const k of platesOver([x - radius, y - radius, x + radius, y + radius])) {
            const r = plateRegion(k);
            // Nothing past the map's edge: a plate off the map is never made.
            if (Math.abs(r.cx) > MAP_HALF || Math.abs(r.cy) > MAP_HALF) continue;
            if (Math.abs(r.cx - x) <= radius && Math.abs(r.cy - y) <= radius) wantedNear.push(plateFor(k));
          }
          wantedNear.sort((a, b) => Math.hypot(a.cx - x, a.cy - y) - Math.hypot(b.cx - x, b.cy - y));
          if (wantedNear.length > nearMost) wantedNear.length = nearMost;
        }
        for (const p of plates.values()) if (p.asked && !wantedNear.includes(p) && !content.has(p.key)) { cancelFlatGround(p.key); p.asked = false; }
      }
    // … 15 lines
    },
    // … 5 lines
    setSun(direction) {
      // … 1 lines
      for (const p of shownList) p.surface!.setSun(direction);
    },
    bakeRelief(renderer: WebGLRenderer) {
      // … 2 lines
      for (const r of farNodes.values()) {
        // … 1 lines
        const p = plates.get(r.node.plate!);
        farForest.book.setFull(r.node.plate!, p?.shown ?? false, p?.surface?.ecology() != null, now);
      }
      farForest.update(renderer, now);
      // … 1 lines
      for (const p of shownList) p.surface!.bakeRelief(renderer, null);
    },
    setWaterLook(look: WaterLook) {
      const was = lastLook;
      if (was.waves === look.waves && was.foam === look.foam && was.current === look.current
        && was.windX === look.windX && was.windZ === look.windZ && was.windSpeed === look.windSpeed) return;
      Object.assign(was, look);
      lookGiven = true;
      for (const p of plates.values()) p.surface?.setWaterLook(look);
    },
    // … 3 lines
    setVegetation(amount) {
      farForest.book.setVegetation(amount);
    },
    // … 8 lines
    vergeMaterial: firstSurface.vergeMaterial,
    meshMaterial: firstSurface.meshMaterial,
    lookFrom(camera, pixels) {
      for (const p of shownList) p.surface!.lookFrom?.(camera, pixels);
    },
    heightAt: (x, y) => ask(x, y, (s, lx, ly) => s.heightAt(lx, ly), bareAt),
    naturalRenderedHeightAt: (x, y) => ask(x, y, (s, lx, ly) => s.naturalRenderedHeightAt(lx, ly), bareAt),
    // … 1 lines
    renderedHeightAt: Object.assign((x: number, y: number): number => ask(x, y, (s, lx, ly) => s.renderedHeightAt(lx, ly), bareAt), { share }) satisfies SharedGroundAt,
    wetAt: (x, y) => ask(x, y, (s, lx, ly) => s.wetAt(lx, ly), () => false),
    // … 16 lines
    update(doc, stroking) {
      // … 2 lines
      for (const p of shownList) {
        if (!p.surface!.update(p.source, stroking)) continue;
      // … 3 lines
      }
    // … 2 lines
    },
    updatePaint(doc) {
      // … 1 lines
      for (const p of shownList) {
        p.surface!.updatePaint(p.source, false);
        const log = p.surface!.geologyChanges, v = log.version, was = geologySeen.get(p);
        if (v === was) continue;
        geologySeen.set(p, v);
        // … 1 lines
        const rects = was === undefined ? null : log.rectsSince(was);
        geologyChanges.mark(rects ? rects.map((r) => [r[0] + p.cx, r[1] + p.cy, r[2] + p.cx, r[3] + p.cy] as const)
          : [[p.cx - TERRAIN_HALF, p.cy - TERRAIN_HALF, p.cx + TERRAIN_HALF, p.cy + TERRAIN_HALF]]);
      }
    },
    forestAt: (x, y) => ask(x, y, (s, lx, ly) => s.forestAt(lx, ly), () => 0),
    coverAt: (kind: CoverKind, x, y) => ask(x, y, (s, lx, ly) => s.coverAt(kind, lx, ly), () => 0),
    geologyAt: (x, y) => ask(x, y, (s, lx, ly) => s.geologyAt(lx, ly), (): GeologyKind => firstSurface.geologyAt(0, 0)),
    ecology: () => firstSurface.ecology(),
    geologyChanges,
    // … 1 lines
    shoreLevelAt: (x, y) => ask(x, y, (s, lx, ly) => s.shoreLevelAt(lx, ly), () => null),
    get lastRegion() {
      const region = lastPlate?.surface?.lastRegion ?? null;
      return region && lastPlate ? encode(lastPlate, region) : null;
    },
    settle() { for (const p of shownList) p.surface!.settle(); },
    shapeToRoads(shape, region = null) {
      const list: readonly TerrainRegion[] | null = region === null ? null
        : typeof region[0] === 'number' ? [region as TerrainRegion] : region as readonly TerrainRegion[];
      // … 1 lines
      if (list === null) {
        for (const p of shownList) if (p.surface!.shapeToRoads(shape && shaperFor(p, shape), null)) { moved = true; measureTop(p); }
        if (moved) measureHighest();
        return moved;
      }
      const byPlate = new Map<Plate, TerrainRegion[]>();
      for (const r of list) {
        const { plate, local } = decode(r);
        let mine = byPlate.get(plate);
        if (!mine) byPlate.set(plate, mine = []);
        mine.push(local);
      }
      for (const [p, regions] of byPlate) if (ensure(p).shapeToRoads(shape && shaperFor(p, shape), regions)) { moved = true; measureTop(p); }
      if (moved) measureHighest();
      return moved;
    },
    askShape(region, bounds) {
      const list: readonly TerrainRegion[] | null = region === null ? null
        : typeof region[0] === 'number' ? [region as TerrainRegion] : region as readonly TerrainRegion[];
      // … 1 lines
      const parts: { p: Plate; s: TerrainSurface; asking: ShapeAsking }[] = [];
      if (list === null) {
        const local = (p: Plate): Aabb[] | null => bounds && bounds.map((b) => ({ minX: b.minX - p.cx, minY: b.minY - p.cy, maxX: b.maxX - p.cx, maxY: b.maxY - p.cy }));
        for (const p of shownList) parts.push({ p, s: p.surface!, asking: p.surface!.askShape(null, local(p)) });
      } else {
        const byPlate = new Map<Plate, TerrainRegion[]>();
        for (const r of list) {
          const { plate, local } = decode(r);
          let mine = byPlate.get(plate);
          if (!mine) byPlate.set(plate, mine = []);
          mine.push(local);
        }
        for (const [p, regions] of byPlate) { const s = ensure(p); parts.push({ p, s, asking: s.askShape(regions, null) }); }
      }
      return {
        plates: parts.flatMap(({ p, asking }) => asking.plates.map((a) => ({ ...a, cx: p.cx, cy: p.cy }))),
      // … 12 lines
      };
    },
    naturalCut(boxes) {
      const out = new Map<number, NaturalPlate>();
      for (const box of boxes) {
        for (const [x, y] of [[box[0], box[1]], [box[2], box[1]], [box[0], box[3]], [box[2], box[3]]] as const) {
          // … 1 lines
          const corners = p?.surface?.naturalCorners?.();
          if (!p || !corners || out.has(p.key)) continue;
          out.set(p.key, { index: p.key, cx: p.cx, cy: p.cy, n: SIDE, cell: TERRAIN_CELL, half: TERRAIN_HALF, ...corners });
        }
      }
      return [...out.values()];
    },
    regionOf(box) {
      // … 1 lines
      return encode(p, ensure(p).regionOf([box[0] - p.cx, box[1] - p.cy, box[2] - p.cx, box[3] - p.cy]));
    },
    rectOf(region) {
      // … 1 lines
      const r = ensure(plate).rectOf(local);
      return [r[0] + plate.cx, r[1] + plate.cy, r[2] + plate.cx, r[3] + plate.cy];
    },
    dispose() {
      for (const p of plates.values()) p.surface?.dispose();
      farGround.dispose();
      farForest.dispose();
      // … 1 lines
      slab.geometry.dispose();
      (slab.material as MeshStandardMaterial).map?.dispose();
      (slab.material as MeshStandardMaterial).dispose();
    },
  };
  return atlas;
}

// src/render/flatAtlas.ts:272-277 slice of src/render/flatAtlas.ts#createFlatAtlas.writeFar (shared): the feature's statements; "…" marks code left out
  const writeFar = (r: FarResident): void => {
    // … 3 lines
    writeFarNode(far, r.slot, n.x0, n.y0, n.step, SKIRT * (1 << n.k), (x, y) => reliefHeight(relief, x, y));
  };

// src/render/flatAtlas.ts:292-299 slice of src/render/flatAtlas.ts#createFlatAtlas.buildSlab (shared): the feature's statements; "…" marks code left out
  const buildSlab = (): void => {
    // … 4 lines
    slab.geometry = slabGeometry(-MAP_HALF, MAP_HALF, -MAP_HALF, MAP_HALF, PLATE_SIZE / FAR_QUADS, (x, y) => TERRAIN_BASE + reliefHeight(relief, x, y));
  // … 1 lines
  };

// src/render/flatAtlas.ts:303-314 slice of src/render/flatAtlas.ts#createFlatAtlas.drawFar (shared): the feature's statements; "…" marks code left out
  const drawFar = (): void => {
    // … 1 lines
    for (const r of farNodes.values()) {
      if (r.node.plate !== null && plates.get(r.node.plate)?.shown) continue;
      patches.push({ slot: r.slot, rim: r.node.rim, quadsX: r.node.quadsX, quadsY: r.node.quadsY });
    }
  // … 5 lines
  };

// src/render/flatAtlas.ts:316-345 slice of src/render/flatAtlas.ts#createFlatAtlas.followFar (shared): the feature's statements; "…" marks code left out
  const followFar = (x: number, y: number): boolean => {
    // … 3 lines
    const want = farNodesFor(x, y, FAR_RING, FAR_QUADS);
    const kept = new Set(want.map((n) => n.key));
    // … 1 lines
    for (const [key, r] of farNodes) {
      if (kept.has(key)) continue;
      // … 2 lines
      if (r.node.plate !== null) { farForest.book.drop(r.node.plate); freeWoods.push(r.woods); }
    // … 1 lines
    }
    // … 1 lines
    want.sort((a, b) => Math.hypot(a.x0 + a.step * FAR_QUADS / 2 - x, a.y0 + a.step * FAR_QUADS / 2 - y) - Math.hypot(b.x0 + b.step * FAR_QUADS / 2 - x, b.y0 + b.step * FAR_QUADS / 2 - y));
    for (const node of want) {
      if (farNodes.has(node.key)) continue;
      if (!freeSlots.length) growFar();
      const woods = node.plate !== null ? freeWoods.pop() ?? -1 : -1;
      const r: FarResident = { node, slot: freeSlots.pop()!, woods };
      farNodes.set(node.key, r);
      writeFar(r);
      if (woods >= 0) farForest.book.add(node.plate!, woods, node.x0 + TERRAIN_HALF, node.y0 + TERRAIN_HALF);
      changed = true;
    }
    if (!changed) return false;
  // … 2 lines
  };

// src/render/flatAtlas.ts:346-353 slice of src/render/flatAtlas.ts#createFlatAtlas.buildFar (shared): the feature's statements; "…" marks code left out
  const buildFar = (relief: ReliefVersion): void => {
    if (relief === farRelief) return;
    farRelief = relief;
    for (const r of farNodes.values()) writeFar(r);
    buildSlab();
    farMesh.visible = true;
    drawFar();
  };

// src/render/flatAtlas.ts:372-374 slice of src/render/flatAtlas.ts#createFlatAtlas.measureTop (shared): the feature's statements; "…" marks code left out
  const measureTop = (p: Plate): void => {
    p.top = p.surface!.highest!();
  };

// src/render/flatAtlas.ts:390-397 slice of src/render/flatAtlas.ts#createFlatAtlas.takeStamps (shared): the feature's statements; "…" marks code left out
  const takeStamps = (p: Plate, doc: TerrainSource): boolean => {
    const sources = doc.terrainStamps.filter((s) => copyOn(p.stampCopies, p, s) !== null);
    if (p.source.terrainRevision !== 0 && sameList(sources, p.source.stampSources)) return false;
    p.source.stampSources = sources;
    p.source.terrainStamps = sources.map((s) => p.stampCopies.get(s) as TerrainStamp);
    p.source.terrainRevision = doc.terrainRevision;
    return true;
  };

// src/render/flatAtlas.ts:399-406 slice of src/render/flatAtlas.ts#createFlatAtlas.takePaint (shared): the feature's statements; "…" marks code left out
  const takePaint = (p: Plate, doc: TerrainSource, edited: readonly ChangeRect[] | null): void => {
    const reaching = doc.terrainPaint.filter((d) => copyOn(p.dabCopies, p, d) !== null);
    const next = plateDabs(p.source.paintSources, p.source.laid, reaching, edited);
    if (!next) return;
    p.source.paintSources = next;
    p.source.terrainPaint = next.map((d) => p.dabCopies.get(d) as PaintDab);
    p.source.paintRevision = doc.paintRevision;
  };

// src/render/flatAtlas.ts:408-416 slice of src/render/flatAtlas.ts#createFlatAtlas.fill (shared): the feature's statements; "…" marks code left out
  const fill = (p: Plate, doc: TerrainSource): void => {
    p.source.changes = doc.changes;
    p.source.natureRevision = doc.natureRevision;
    p.source.nature = doc.nature;
    p.source.terrainRelief = doc.terrainRelief;
    p.source.terrainGround = (lx, ly) => groundOn(p, lx, ly);
    takeStamps(p, doc);
    takePaint(p, doc, null);
  };

// src/render/flatAtlas.ts:431-473 slice of src/render/flatAtlas.ts#createFlatAtlas.ensure (shared): the feature's statements; "…" marks code left out
  const ensure = (p: Plate): TerrainSurface => {
    if (p.surface) return p.surface;
    // Each step of a plate's making measured (F9 and the probes read hitch: measures).
    const t0 = performance.now();
    const s = createTerrainSurface(anisotropy, { x: p.cx, y: p.cy }, p.key, p.mesh, share.open(p.key));
    // A dab's levels come after it, off the page (`landStroke.ts`): the plate's top read again when they land, and the atlas's told on.
    s.onLevels = () => {
      if (p.surface !== s) return;
      measureTop(p);
      measureHighest();
      atlas?.onLevels?.();
    };
    // The surface keeps what it copied; the plate keeps only its corners (its land's field).
    p.mesh = null;
    const t1 = performance.now();
    p.surface = s;
    // Its paint's side for the view before the first dab is laid (`PlateSurface.paintDetail`): DO NOT lay a far plate's at full size to let it go at the next refresh.
    s.paintDetail?.(near.includes(p.key) || wantedNear.includes(p));
    if (lastDoc) {
      s.update(p.source, false);
      const t2 = performance.now();
      s.updatePaint(p.source, true);
      measureHitch('hitch:flat-plate/update', t1, t2);
      measureHitch('hitch:flat-plate/paint', t2);
    }
    measureHitch('hitch:flat-plate/create', t0, t1);
    if (lastSun) s.setSun(lastSun);
    if (lookGiven) s.setWaterLook(lastLook);
    // Sent ahead of its first draw (`uploads.ts`), not inside it.
    queueUpload(...s.textures());
    // Shown once its programs are built (`uploads.ts` programsReady, the door's own variants): until then its far patch stands in, not a hole while the scene's door holds its meshes.
    p.ready = !compilesAhead();
    if (!p.ready) {
      programsWaited++;
      const shown = (): void => { programsWaited--; if (p.surface === s) { p.ready = true; contentCame = true; } };
      // DO NOT let a failed wait leave the plate on its far patch for good (no water, no road cut: conf52, conf54): a wait that throws or is rejected shows it all the same, three building its programs at its first draw.
      let waits: Promise<unknown>;
      try { waits = Promise.all((s.meshes as Mesh[]).map((mesh) => programsReady(mesh))); } catch (error) { waits = Promise.reject(error); }
      void waits.then(shown, (error: unknown) => { console.warn('[terrain] a plate\'s programs could not be built ahead; it is shown all the same', error); shown(); });
    }
    measureTop(p);
    return s;
  };

// src/render/flatAtlas.ts:474-479 slice of src/render/flatAtlas.ts#createFlatAtlas.show (shared): the feature's statements; "…" marks code left out
  const show = (p: Plate, on: boolean): void => {
    // … 3 lines
    if (on) root.add(...s.meshes); else root.remove(...s.meshes);
  };

// src/render/flatAtlas.ts:481-494 slice of src/render/flatAtlas.ts#createFlatAtlas.letGo (shared): the feature's statements; "…" marks code left out
  const letGo = (p: Plate): void => {
    // … 3 lines
    cancelUploads(p.surface.textures());
    p.surface.dispose();
  // … 7 lines
  };

// src/render/flatAtlas.ts:502-532 slice of src/render/flatAtlas.ts#createFlatAtlas.refresh (shared): the feature's statements; "…" marks code left out
  const refresh = (): void => {
    // … 11 lines
    for (const p of plates.values()) {
      // … 2 lines
      p.surface.paintDetail?.(near.includes(p.key));
    // … 5 lines
    }
  // … 8 lines
  };

// src/render/flatAtlas.ts:537-547 slice of src/render/flatAtlas.ts#createFlatAtlas.keepWithinBudget (shared): the feature's statements; "…" marks code left out
  const keepWithinBudget = (cached: Plate[]): void => {
    // … 1 lines
    for (const p of shownList) room -= p.surface!.gpuBytes();
    // … 2 lines
    for (const p of cached) {
      // … 1 lines
      room -= p.surface!.gpuBytes();
      if (room < 0 || ++kept > keptMost) letGo(p);
    }
  };

// src/render/flatAtlas.ts:552-572 slice of src/render/flatAtlas.ts#createFlatAtlas.readContent (shared): the feature's statements; "…" marks code left out
  const readContent = (doc: TerrainSource): boolean => {
    const d = doc as Partial<RoadDoc>;
    const key = [d.revision, d.buildings?.revision, d.lotRevision, d.utilityRevision, d.treeRevision, doc.terrainRevision, doc.paintRevision].join(':');
    if (key === contentKey) return false;
    contentKey = key;
    const entries = contentCursor.read(doc.changes);
    const loaded = (doc as Partial<RoadDoc>).loadedTiles;
    // An unloaded tile takes its records away: the plates it reaches asked again (DO NOT read the whole document at each tile let go).
    const unloads = entries?.some((e) => e.cause === TILE_UNLOADED) ?? false;
    if (entries && entries.every((e) => e.rects !== null) && (!unloads || loaded)) {
      const before = new Set(content);
      const emptied = new Set<number>();
      // Marked in place: the plates the changes reach (a stamp or paint has its rects too).
      for (const e of entries) {
        for (const rect of e.rects!) for (const k of platesOver(rect)) (e.cause === TILE_UNLOADED ? emptied : content).add(k);
      }
      for (const k of emptied) if (!plateHolds(doc, k, loaded!)) content.delete(k); else content.add(k);
      return content.size !== before.size || [...content].some((k) => !before.has(k));
    }
    return readWholeContent(doc);
  };

// src/render/flatAtlas.ts:574-582 slice of src/render/flatAtlas.ts#createFlatAtlas.plateHolds (shared): the feature's statements; "…" marks code left out
  const plateHolds = (doc: TerrainSource, key: number, loaded: ReadonlySet<number>): boolean => {
    const r = plateRegion(key);
    for (const t of loaded) {
      const { i, j } = tileOfKey(t);
      if (plateOf((i + 0.5) * TILE, (j + 0.5) * TILE) === key) return true;
    }
    const reach = (x: number, y: number, radius: number): boolean => Math.abs(x - r.cx) <= TERRAIN_HALF + radius && Math.abs(y - r.cy) <= TERRAIN_HALF + radius;
    return doc.terrainStamps.some((s) => reach(s.x, s.y, s.radius)) || doc.terrainPaint.some((d) => reach(d.x, d.y, d.radius));
  };

// src/render/flatAtlas.ts:580-580 slice of src/render/flatAtlas.ts#createFlatAtlas.plateHolds.reach (shared): the feature's statements; "…" marks code left out
    const reach = (x: number, y: number, radius: number): boolean => Math.abs(x - r.cx) <= TERRAIN_HALF + radius && Math.abs(y - r.cy) <= TERRAIN_HALF + radius;

// src/render/flatAtlas.ts:584-602 slice of src/render/flatAtlas.ts#createFlatAtlas.readWholeContent (shared): the feature's statements; "…" marks code left out
  const readWholeContent = (doc: TerrainSource): boolean => {
    const d = doc as Partial<RoadDoc>;
    const next = new Set<number>();
    const mark = (x: number, y: number): void => { next.add(plateOf(x, y)); };
    const disc = (x: number, y: number, r: number): void => { for (const k of platesOver([x - r, y - r, x + r, y + r])) next.add(k); };
    for (const n of d.nodes?.values() ?? []) mark(n.x, n.y);
    for (const b of d.buildings?.all() ?? []) mark(b.x, b.y);
    for (const l of d.lots ?? []) for (const c of l.corners) mark(c.x, c.y);
    for (const p of d.poles?.values() ?? []) mark(p.x, p.y);
    for (const item of d.landscape?.values() ?? []) mark(item.x, item.y);
    for (const tree of d.trees ?? []) mark(tree.x, tree.y);
    for (const barrier of d.barriers?.values() ?? []) for (const q of barrier.points) mark(q.x, q.y);
    // A stamp or a dab puts content on every plate its disc reaches (`copyOn`).
    for (const s of doc.terrainStamps) disc(s.x, s.y, s.radius);
    for (const dab of doc.terrainPaint) disc(dab.x, dab.y, dab.radius);
    const changed = next.size !== content.size || [...next].some((i) => !content.has(i));
    content = next;
    return changed;
  };

// src/render/flatAtlas.ts:609-653 slice of src/render/flatAtlas.ts#createFlatAtlas.sync (shared): the feature's statements; "…" marks code left out
  const sync = (doc: TerrainSource): void => {
    lastDoc = doc;
    for (const p of plates.values()) {
      p.source.changes = doc.changes;
      p.source.natureRevision = doc.natureRevision;
      p.source.nature = doc.nature;
    }
    if (doc.terrainRelief !== undefined) farForest.book.sync(doc.terrainRelief, doc.nature, focusX, focusY);
    if (doc.terrainRevision !== seenTerrain) {
      seenTerrain = doc.terrainRevision;
      const relief = doc.terrainRelief;
      const reliefMoved = seenRelief !== relief;
      seenRelief = relief;
      if (reliefMoved) { bare = (x, y) => reliefHeight(relief, x, y); share.setRelief(relief); }
      buildFar(relief);
      const entries = terrainCursor.read(doc.changes);
      const rects = entries && entries.every((e) => e.rects !== null) ? entries.flatMap((e) => e.rects!) : null;
      for (const p of plates.values()) {
        // A plate no change reached keeps its stamps as they were (the relief moved: every plate).
        if (!reliefMoved && rects && p.source.terrainRevision !== 0
          && !rects.some((r) => reaches(p, (r[0] + r[2]) / 2, (r[1] + r[3]) / 2, Math.hypot(r[2] - r[0], r[3] - r[1]) / 2))) continue;
        p.source.terrainRelief = relief;
        if (reliefMoved || !p.source.terrainGround) {
          if (reliefMoved) { p.heights = null; p.mesh = null; p.asked = false; }
          // Read over the plate's own corners (off the page), not the field again at each.
          p.source.terrainGround = (lx, ly) => groundOn(p, lx, ly);
        }
        if (reliefMoved) p.source.terrainRevision = 0;
        takeStamps(p, doc);
      }
    }
    if (doc.paintRevision !== seenPaint) {
      seenPaint = doc.paintRevision;
      // Tiles came or went (`isStreaming`): what each plate laid stays; a stroke: its rectangles say what to lay again (`plateDabs`).
      const entries = paintCursor.read(doc.changes);
      const edited = entries && entries.every((e) => e.rects !== null) ? entries.filter((e) => !isStreaming(e)).flatMap((e) => e.rects!) : null;
      // DO NOT filter the map's every dab for every plate at a dab: only the plates a stroke's rectangles reach (a dab's rectangle holds its disc); a tile come or gone, every plate (its dabs may reach past its square).
      const strokes = edited && !entries!.some(isStreaming) ? edited : null;
      for (const p of plates.values()) {
        if (strokes && !strokes.some((r) => r[2] >= p.cx - TERRAIN_HALF && r[0] <= p.cx + TERRAIN_HALF && r[3] >= p.cy - TERRAIN_HALF && r[1] <= p.cy + TERRAIN_HALF)) continue;
        takePaint(p, doc, edited);
      }
    }
    if (readContent(doc)) refresh();
  };

// src/render/flatAtlas.ts:655-667 slice of src/render/flatAtlas.ts#createFlatAtlas.groundOn (shared): the feature's statements; "…" marks code left out
  function groundOn(p: Plate, lx: number, ly: number): number {
    // … 1 lines
    if (h) {
      const gx = (lx + TERRAIN_HALF) / TERRAIN_CELL, gy = (TERRAIN_HALF - ly) / TERRAIN_CELL;
      if (gx >= 0 && gy >= 0 && gx <= SIDE - 1 && gy <= SIDE - 1) {
        const i = Math.min(SIDE - 2, Math.floor(gx)), j = Math.min(SIDE - 2, Math.floor(gy));
        const u = gx - i, v = gy - j, k = j * SIDE + i;
        const a = h[k]!, b = h[k + 1]!, c = h[k + SIDE]!, d = h[k + SIDE + 1]!;
        return (a * (1 - u) + b * u) * (1 - v) + (c * (1 - u) + d * u) * v - TERRAIN_BASE;
      }
    }
  // … 1 lines
  }

// src/render/flatAtlas.ts:669-684 slice of src/render/flatAtlas.ts#createFlatAtlas.askCorners (shared): the feature's statements; "…" marks code left out
  const askCorners = (p: Plate): void => {
    // … 3 lines
    readFlatGround({
      key: p.key, relief, at: { x: p.cx, y: p.cy }, cx: p.cx, cy: p.cy, half: TERRAIN_HALF, cell: TERRAIN_CELL, side: SIDE, base: TERRAIN_BASE,
    // … 8 lines
    });
  };

// src/render/flatAtlas.ts:691-695 slice of src/render/flatAtlas.ts#createFlatAtlas.decode (shared): the feature's statements; "…" marks code left out
  const decode = (region: TerrainRegion): { plate: Plate; local: TerrainRegion } => {
    const key = Math.floor(region[0] / REGION_STRIDE);
    const off = key * REGION_STRIDE;
    return { plate: plateFor(key), local: [region[0] - off, region[1] - off, region[2], region[3]] };
  };

// src/render/flatAtlas.ts:696-699 slice of src/render/flatAtlas.ts#createFlatAtlas.encode (shared): the feature's statements; "…" marks code left out
  const encode = (p: Plate, region: TerrainRegion): TerrainRegion => {
    const off = p.key * REGION_STRIDE;
    return [region[0] + off, region[1] + off, region[2], region[3]];
  };

// src/render/flatAtlas.ts:701-704 slice of src/render/flatAtlas.ts#createFlatAtlas.shaperFor (shared): the feature's statements; "…" marks code left out
  const shaperFor = (p: Plate, shape: TerrainShaper): TerrainShaper => ({
    shapeAt: (x, y, ground) => shape.shapeAt(x + p.cx, y + p.cy, ground),
    shapeBounds: () => shape.shapeBounds().map((b) => ({ minX: b.minX - p.cx, minY: b.minY - p.cy, maxX: b.maxX - p.cx, maxY: b.maxY - p.cy })),
  });

// src/render/flatAtlas.ts:709-712 slice of src/render/flatAtlas.ts#createFlatAtlas.ask (shared): the feature's statements; "…" marks code left out
  const ask = <T>(x: number, y: number, read: (s: TerrainSurface, lx: number, ly: number) => T, bareRead: (x: number, y: number) => T): T => {
    const p = plates.get(plateOf(x, y));
    return p?.surface ? read(p.surface, x - p.cx, y - p.cy) : bareRead(x, y);
  };

// src/render/flatAtlas.ts:713-713 slice of src/render/flatAtlas.ts#createFlatAtlas.bareAt (shared): the feature's statements; "…" marks code left out
  const bareAt = (x: number, y: number): number => TERRAIN_BASE + bare(x, y);

// src/render/flatAtlas.ts:823-827 slice of src/render/flatAtlas.ts#createFlatAtlas.one (shared): the feature's statements; "…" marks code left out
      const one = (key: number, x0: number, y0: number, x1: number, y1: number): number => {
        // … 2 lines
        return p.surface.digest(x0 - p.cx, y0 - p.cy, x1 - p.cx, y1 - p.cy) + p.key;
      };

// src/render/flatAtlas.ts:914-925 slice of src/render/flatAtlas.ts#createFlatAtlas.take (shared): the feature's statements; "…" marks code left out
        take(answers) {
          // … 2 lines
          for (const { p, s, asking } of parts) {
            const mine = answers.slice(k, k += asking.plates.length);
            // … 2 lines
            if (asking.take(mine)) { moved = true; measureTop(p); }
          }
          if (moved) measureHighest();
          return moved;
        },
