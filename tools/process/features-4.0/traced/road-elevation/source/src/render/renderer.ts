// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 144 lines
  offerElevation(elevation: RoadElevation, networkRevision: number): void;
  // … 1 lines
  roadsSolve(): { readonly elevation: RoadElevation; readonly terrainRevision: number } | null;
// … 21 lines
}

// src/render/renderer.ts:399-399 slice of src/render/renderer.ts#SURFACE_SETS (shared): the feature's statements; "…" marks code left out
const SURFACE_SETS: readonly ReadonlySet<RoadStructure>[] = [GROUND_ONLY, new Set<RoadStructure>(['elevated', 'bridge'])];

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 584 lines
  let elevation: RoadElevation | null = null;
  // … 56 lines
  const surfaceReuse: SurfaceReuse = {
    // … 6 lines
    dependsOn: (minX, minY, maxX, maxY) => new Digest()
      .add(elevation?.digest(minX, minY, maxX, maxY, SURFACE_READ_REACH) ?? 0)
      .add(terrain.digest(minX, minY, maxX, maxY))
      .value(),
  // … 32 lines
  };
  // … 13 lines
  let offeredElevation: { elevation: RoadElevation; revision: number; terrain: number } | null = null;
  // … 141 lines
  const pavedHeightAt: PavingSource = Object.assign((x: number, y: number): number => {
    if (!elevation || !elevation.has(GROUND_ONLY)) return NaN;
    return pavedHeightFrom(elevation, GROUND_ONLY, footwayRiseHere, x, y);
  }, {
    pack: (rect: readonly [number, number, number, number]): { data: PavingData; transfer: ArrayBuffer[] } | null => {
      const ground = elevation !== null && elevation.has(GROUND_ONLY);
      if (ground && !elevation!.fieldFor) return null;
      const field = ground ? elevation!.fieldFor!([rect], []) : null;
      // … 1 lines
      return { data: { rect, field: field?.data ?? null, flush: flush?.data ?? null },
        transfer: [...field?.transfer ?? [], ...flush?.transfer ?? []] };
    },
  });
  // … 496 lines
  const groundRead = new WeakMap<RoadElevation, Map<number, number>>();
  // … 1 lines
  const groundReading = new WeakMap<RoadElevation, Map<number, Promise<void>>>();
  // … 292 lines
  engine.register({
    // … 1 lines
    refresh: (): number => {
      if (!placedOn || !elevation) return 0;
      // … 2 lines
      const work = furniture.sync(placedOn, elevation, terrain.renderedHeightAt, cityMinutes);
      // … 2 lines
      return work;
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      if (!placedOn || !elevation) return 0;
      // … 1 lines
      return signs.sync(placedOn, elevation);
    },
  }, 'draw');
  // … 1 lines
  engine.register({
    // … 2 lines
    refresh: (): number => {
      if (!placedOn) return 0;
      // … 2 lines
      const work = barriers.sync(placedOn.doc, terrain.renderedHeightAt);
      // … 1 lines
      return work;
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      const net = placedOn;
      if (!net) return 0;
      // … 3 lines
      const work = transit.sync(net.doc, terrain.renderedHeightAt, pavedHeightAt, (p) => {
        if (onCarriageway(net, p)) return true;
        for (const b of net.doc.buildings.near(p.x, p.y, p.x, p.y)) {
          if (solidFootprints(b).some((ring) => pointInPolygon(p, ring))) return true;
        }
        return false;
      });
      // … 1 lines
      return work;
    },
  }, 'draw');
  // … 69 lines
  const roadsRead = new RoadsRead<ReturnType<typeof roadsToRead>>();
  // … 711 lines
  const coverKeeps = {
    // … 1 lines
    forest: new CellKeeps<Candidate>((c) => (coverFree(c.x, c.y) && !clearingsOf(keepNet!.doc)(c.x, c.y) ? terrain.renderedHeightAt(c.x, c.y) : null)),
  // … 9 lines
  };
  // … 5 lines
  const plantedKeeps = new CellKeeps<PlantedTree>((t) => (onCarriageway(keepNet!, t) || buildings.covers(t.x, t.y) ? null : terrain.renderedHeightAt(t.x, t.y)));
  // … 92 lines
  const handle: SceneHandle = {
    // … 56 lines
    get stats(): RenderStats {
      return {
        // … 8 lines
        terrainMs: Math.round(terrainMs),
      };
    },
    // … 54 lines
    openingState() {
      return { roadsIn: roads !== null && mapRoadsIn, coarsePending: buildings.coarsePending, viewPending: buildings.viewPending, treesStarted: natureTreesStarted,
        treesPending: natureTreesPending, programs: openingPrograms, worked: reveal.worked, worldBusy: worldJob !== null, fillPending: openingFillPending, fillCellsLeft: fillLeft?.size ?? 0 };
    },
    // … 25 lines
    roadsSolve() {
      return elevation ? { elevation, terrainRevision } : null;
    },
    surfaceHeightAt(x, y) {
      // … 1 lines
      if (!elevation) return ground;
      // … 2 lines
      for (const set of SURFACE_SETS) {
        if (!elevation.has(set)) continue;
        const road = elevation.roadAt(x, y, set);
        if (road.type < 0 || Math.abs(road.across) > road.half) continue;
        top = Math.max(top, elevation.at(x, y, set));
      }
      return top;
    },
    // … 26 lines
    busy: () => (fx?.busy() ?? false) || natureTreesPending || cameraEasing || (terrain.waiting?.() ?? false),
    // … 23 lines
    setPolePreview(net, preview) {
      const key = preview && elevation
        ? `${net.revision}:${preview.poles.map((p) => `${p.x.toFixed(2)},${p.y.toFixed(2)},${+p.lamp},${+p.standing}`).join(';')}`
        : '';
      if (key === polePreviewKey) return;
      polePreviewKey = key;
      if (polePreview) {
        world.remove(polePreview.group);
        polePreview.dispose();
        polePreview = null;
      }
      if (!preview || !elevation || preview.poles.length === 0) return;
      polePreview = buildPolePreview(net, poleGroundAt(elevation, terrain.renderedHeightAt, net), sceneryKit, preview);
      world.add(polePreview.group);
    },
    // … 24 lines
    elevationAt(x, y, structure) {
      if (!elevation) return terrain.renderedHeightAt(x, y);
      return elevation.at(x, y, structure ? new Set([structure]) : undefined);
    },
    // … 2 lines
    draw(net, sim, alpha, delta, options) {
      // … 20 lines
      const stroking = !!options?.holdRoads && !!elevation && networkRevision === net.revision;
      const groundMoved = terrain.update(net.doc, stroking);
      // … 2 lines
      if (groundMoved) {
        landVersion++;
        // The stroke's dirty region, dab by dab (`landDirty`); a rewrite the terrain cannot place (an undo of the relief), where the diary says it moved.
        const moved = terrain.lastRegion;
        const said = moved === null ? landMovedSince(landEntries) : regionRect(moved);
        landDirty = said === null || landDirty === 'all' ? 'all' : unionRect(landDirty, said);
      }
      // … 2 lines
      if (stroking) {
        if (groundMoved) {
          shapeGround(net, terrain.lastRegion);
          terrainMs = performance.now() - terrainStarted;
        }
      } else {
        const atRebuild = performance.now();
        frameMark('frame:rebuild-start');
        rebuildWorld(net);
        frameMark('frame:rebuild');
        measureHitch('hitch:draw/Rebuild', atRebuild);
        terrain.settle();
      }
      // … 16 lines
      const groundSettled = !stroking && !worldJob;
      // … 1 lines
      if (groundSettled) { placedOn = net; engine.refresh('draw'); }
      // … 4 lines
      if ((groundSettled || (!stroking && groundIn)) && groundChanges.version !== buildingGroundSeen) {
        const moved = buildingGroundSeen < 0 ? null : buildingsOnGroundSince(net.doc, buildingGroundSeen);
        if (moved === null || moved.length > 0) buildingGround = String(groundChanges.version);
        buildingGroundSeen = groundChanges.version;
      }
      // … 1 lines
      if (roads !== null && (reveal.opened || (groundIn && networkRevision === net.revision))) {
        // … 5 lines
        buildings.update(net.doc, terrain.renderedHeightAt, buildingGround, pavedHeightAt, builtGroundAt,
          (since) => (since === '' ? null : buildingsOnGroundSince(net.doc, Number(since))), levelLand ? levelGroundDigest : landGroundDigest);
      // … 3 lines
      }
      // … 13 lines
      if (groundSettled) gardens.follow(net.doc.changes);
      // … 2 lines
      if (!stroking && !worldJob) {
        keepNet = net;
        // What the tests read that changed since the last look, and where (null: the diary cannot say).
        const entries = vegetationRead.read(net.doc.changes);
        let rects: ChangeRect[] | null = entries === null ? null : [];
        for (const entry of entries ?? []) {
          if (entry.rects === null) { rects = null; break; }
          rects!.push(...entry.rects);
        }
        // The stones, the scrub and the painted woods: swept again only in the cells the paint reached (`coverSweep.ts`).
        {
          const at = performance.now();
          const paint = coverPaintRead.read(net.doc.changes);
          if (paint === null || paint.some((e) => e.rects === null)) sweepAllCovers(net.doc);
          else if (paint.length) {
            const cells = new Set<number>();
            for (const e of paint) for (const r of e.rects!) keepCellsOver(r, COVER_SWEEP_PAD, cells);
            resweepCovers(cells);
          }
          shorePlates.follow(terrain.parts);
          measureHitch('hitch:cover/sweep', at);
        }
        // The painted woods' trees are the countryside's (`natureTrees.ts`); the old card trees only if those cannot be grown.
        const standIns = !natureTreeKit && !natureTreesPending;
        let coverWhole = vegetationRedraw || coverStandIns !== standIns;
        // The cells whose places changed, of every kind (the painted woods' are read by the cover's card trees and by the forest).
        const changed = new Set<number>();
        // The stones take the colour of the painted rock under them: drawn again where the geology changed.
        const geology = terrain.geologyChanges.version;
        if (geology !== coverGeologySeen) {
          const painted = coverGeologySeen < 0 ? null : terrain.geologyChanges.rectsSince(coverGeologySeen);
          if (painted === null) coverWhole = true;
          else for (const r of painted) keepCellsOver(r, TERRAIN_CELL, changed);
          coverGeologySeen = geology;
        }
        {
          const at = performance.now();
          if (rects === null) for (const keep of coverKinds) keep.refreshAll();
          else if (rects.length) for (const keep of coverKinds) keep.refresh(rects, KEEP_REACH);
          for (const keep of coverKinds) {
            const cells = keep.takeChangedCells();
            if (cells === null) coverWhole = true; else for (const cell of cells) changed.add(cell);
          }
          if (coverWhole || changed.size) {
            // DO NOT gather every cell of the map unless every cell is drawn again.
            coverCellsOf().sync(coverWhole ? null : changed, coverWhole ? allCoverCells() : []);
            paintedForestOf()?.sync(coverWhole ? null : changed, coverWhole ? coverKeeps.forest.cellKeys() : []);
          }
          coverStandIns = standIns;
          measureHitch('hitch:cover', at);
        }
        // The ecosystem's own trees.
        bringPlateLand(net.doc);
        const natureSweepKey = `${ecologySeen.follow(terrain.parts)}:${landSerial}:${quality.vegetation}`;
        if (natureSweepWant !== natureSweepKey || natureSweepPending) {
          const at = performance.now();
          natureSweepWant = natureSweepKey;
          const swept = natureSweep(natureSweepKey);
          natureSweepPending = (swept.pending ?? 0) > 0;
          // While plates wait their sweep (one a frame), the trees standing stay
          if (!natureSweepPending || !natureSweepState) natureSweepState = swept;
          measureHitch('hitch:nature/sweep', at);
        }
        if (natureSweepState) {
          const sweep = natureSweepState;
          const at = performance.now();
          // A new sweep fills again the cells of the plates swept again only (`fileNature`).
          if (natureFiledFrom !== sweep) {
            natureFiledFrom = sweep;
            fileNature(sweep);
          }
          if (rects === null) natureKeeps.refreshAll();
          else if (rects.length) natureKeeps.refresh(rects, KEEP_REACH);
          const cells = natureKeeps.takeChangedCells();
          const whole = cells === null || vegetationRedraw;
          if (whole || cells!.size) {
            const changed = whole ? null : cells;
            if (natureTreeKit) {
              natureForest ??= (() => {
                const made = createTreeCells(natureTreeKit, 'nature-forest', (cell) => natureKeeps.standingIn(cell).map(({ c, z }) => ({ ...c, z })));
                world.add(made.group);
                return made;
              })();
              natureForest.sync(changed, whole ? natureKeeps.cellKeys() : []);
              // Grown (or nothing to grow): no longer drawing while waiting for them.
              natureTreesPending = false;
            }
            // The old card trees only if the new ones could not be grown
            natureCards ??= (() => {
              const made = createCoverCells(coverKit, 'nature-cards', {
                rocksIn: () => [], scrubIn: () => [],
                treesIn: (cell) => (!natureTreeKit && !natureTreesPending ? natureKeeps.standingIn(cell).map(({ c, z }) => ({ ...c, z })) : []),
              });
              world.add(made.group);
              return made;
            })();
            natureCards.sync(changed, whole ? natureKeeps.cellKeys() : []);
          }
          measureHitch('hitch:nature', at);
        }
        // The trees the player planted (`world/trees.ts`), with the woods' own models: filed again only in the cells planted or cut
        if (natureTreeKit) {
          const planted = plantedRead.read(net.doc.changes);
          const plantedWhole = planted === null || planted.some((e) => e.rects === null);
          if (plantedWhole) plantAll(net.doc);
          else if (planted.length) {
            const cells = new Set<number>();
            for (const e of planted) for (const r of e.rects!) keepCellsOver(r, 0, cells);
            for (const key of cells) plantedKeeps.fill(key, plantedIn(net.doc, key), (a, b) => a === b);
          }
          // DO NOT test every tree again right after `plantAll` tested them on the same roads and buildings: a map just opened reads no rectangle (`rects` null), and every tree of a city of 70,000 buildings was tested twice in one frame, the buildings' cover cells made twice (the first ones let go for their budget): a page task of 3.8 s behind the curtain (trace of step-70k, 2026-10-22).
          if (rects === null) { if (!plantedWhole) plantedKeeps.refreshAll(); } else if (rects.length) plantedKeeps.refresh(rects, KEEP_REACH);
          const cells = plantedKeeps.takeChangedCells();
          if (cells === null || cells.size || vegetationRedraw) {
            const forest = plantedForestOf()!;
            const whole = cells === null || vegetationRedraw;
            forest.sync(whole ? null : cells, whole ? plantedKeeps.cellKeys() : []);
          }
        }
        vegetationRedraw = false;
      }
      // … 21 lines
      {
        // … 26 lines
        if (pendingStandIn.length && elevation && net.revision === net.doc.revision) {
          roadStandIn.show(net, elevation, pendingStandIn.splice(0), net.revision);
        }
      // … 5 lines
      }
      // … 17 lines
      if (pendingRoadFlash.length && elevation) {
        const pos: number[] = [], ring: number[] = [];
        for (const id of pendingRoadFlash.splice(0)) {
          const ribbon = net.ribbons.get(id);
          if (!ribbon) continue;
          const line = ribbon.full, half = sidewalkHalf(ribbon.road);
          const at = (s: number, side: number): [number, number, number] => {
            const f = line.sampleAt(Math.min(s, line.length));
            const x = f.p.x + f.n.x * half * side, y = f.p.y + f.n.y * half * side;
            return [x, elevation!.onSegment(id, f.p.x, f.p.y) + m(0.3), -y];
          };
          // The road itself only: cut where its ends meet a junction (its trims), not running into the road it joins.
          const trims = net.trims.get(id);
          const from = trims ? Math.max(0, ...Object.values(trims.a)) : 0;
          const to = line.length - (trims ? Math.max(0, ...Object.values(trims.b)) : 0);
          const step = m(1.5);
          for (let s = from; s < to; s += step) {
            const e = Math.min(to, s + step);
            const l0 = at(s, 1), r0 = at(s, -1), l1 = at(e, 1), r1 = at(e, -1);
            pos.push(...l0, ...r0, ...r1, ...l0, ...r1, ...l1);
            for (let k = 0; k < 6; k++) ring.push(0);
          }
        }
        if (pos.length) addBlink(pos, ring);
      }
      // … 42 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.cull(crowdFrustum, crowdProjection);
      // … 138 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 26 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 9 lines
      {
        // … 23 lines
        if (roadFocus.frames++ % 6 === 0) {
          // … 6 lines
          if (roadResidency.wantsNetwork && elevation && worldJob === null && !openingBuild && fillLeft === null && !networkFar.pending && networkFar.outdated()) {
            networkFar.settle(farShown());
            requestNetworkFar(net, elevation);
          }
        }
      // … 1 lines
      }
      // … 75 lines
      if (!reveal.opened) {
        // … 9 lines
        const whole = roadsIn && !buildings.coarsePending && (mapView || !buildings.viewPending)
          && natureTreesStarted && !natureTreesPending;
        // … 5 lines
        if (openingPrograms === 'no' && (whole || drawablesIn)) {
          openingPrograms = 'asked';
          performance.mark('reveal:whole');
          programs.compile(scene);
          const ready = programs.ready;
          void ready.then(() => { if (programs.ready === ready) { openingPrograms = 'built'; performance.mark('reveal:programs'); onAssetsReady(); } });
        }
        const next = reveal.step(whole && openingPrograms === 'built', performance.now());
        if (next === 'warm') {
          // One frame drawn unseen first, its programs built: the new meshes go to the GPU
          performance.mark('reveal:warm');
          if (post.target) {
            renderer.setRenderTarget(post.target);
            renderer.render(scene, rig.camera);
            renderer.setRenderTarget(null);
          }
          performance.mark('reveal:warm-drawn');
          programs.compile(scene);
          onAssetsReady();
        } else if (next === 'show') {
          // Shown however far it got (`REVEAL_LIMIT_MS`): its programs compiled all the same.
          programs.compile(scene);
          // The town comes out from behind the curtain: its casters drawn into the sun's stores (what the curtain hid, no signature sees).
          shadowMoved = true;
          performance.mark('opening:shown', { detail: { waitedMs: performance.now() - openingSince, workedMs: reveal.worked, whole } });
        }
      }
    // … 86 lines
    },
    // … 63 lines
    dispose() {
      // … 17 lines
      polePreview?.dispose();
      // … 14 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.dispose();
    // … 16 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:1195-1198 slice of src/render/renderer.ts#createSceneRenderer.requestNetworkFar (shared): the feature's statements; "…" marks code left out
  const requestNetworkFar = (net: Network, heights: RoadElevation): void => {
    networkFarRead.read(net.doc.changes);
    networkFar.request(net, heights, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN, null);
  };

// src/render/renderer.ts:1269-1276 slice of src/render/renderer.ts#createSceneRenderer.levelGroundDigest (shared): the feature's statements; "…" marks code left out
  const levelGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(Math.round(builtGroundAt(minX, minY) * 100)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    // … 1 lines
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    // … 1 lines
    if (pads) out.add(pads.apron).addPlain(pads.pads);
    return out.value();
  };

// src/render/renderer.ts:1278-1283 slice of src/render/renderer.ts#createSceneRenderer.landGroundDigest (shared): the feature's statements; "…" marks code left out
  const landGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(terrain.digest(minX, minY, maxX, maxY)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    // … 1 lines
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    return out.value();
  };

// src/render/renderer.ts:1798-1802 slice of src/render/renderer.ts#createSceneRenderer.readOf (shared): the feature's statements; "…" marks code left out
  const readOf = (solve: RoadElevation): Map<number, number> => {
    let known = groundRead.get(solve);
    if (!known) groundRead.set(solve, known = new Map());
    return known;
  };

// src/render/renderer.ts:1804-1812 slice of src/render/renderer.ts#createSceneRenderer.blockRead (shared): the feature's statements; "…" marks code left out
  const blockRead = (solve: RoadElevation, x0: number, y0: number): number => {
    const known = readOf(solve);
    const key = blockKey(x0, y0);
    const kept = known.get(key);
    if (kept !== undefined) return kept;
    const value = blockDigest(solve, terrain.naturalRenderedHeightAt, x0, y0, SHAPE_BLOCK, TERRAIN_CELL, TERRAIN_HALF);
    known.set(key, value);
    return value;
  };

// src/render/renderer.ts:1825-1898 slice of src/render/renderer.ts#createSceneRenderer.compareSolves (shared): the feature's statements; "…" marks code left out
  const compareSolves = (before: RoadElevation, after: RoadElevation, landBlocks: readonly [number, number, number, number][] | null): BlockCompare => {
    const at = performance.now();
    const out: BlockCompare = { blocks: [], ready: false, ms: 0 };
    const candidates: [number, number][] = [];
    // Only blocks a road that differs reaches can differ (`differences`)
    const where = after.differences?.(before) ?? null;
    if (!where || where.length) {
      // The blocks of the plates the differences reach; not knowing where, of the plates drawn in full (only their ground is shaped, `shapeToRoads`), never of a world's plates.
      for (const plate of platesUnder(where)) {
        for (let x = plate.cx - plate.half; x < plate.cx + plate.half; x += SHAPE_BLOCK) {
          for (let y = plate.cy - plate.half; y < plate.cy + plate.half; y += SHAPE_BLOCK) {
            const x1 = x + SHAPE_BLOCK, y1 = y + SHAPE_BLOCK;
            if (where && !where.some((r) => r.minX <= x1 && r.maxX >= x && r.minY <= y1 && r.maxY >= y)) continue;
            candidates.push([x, y]);
          }
        }
      }
    }
    const finish = (): void => {
      const changed: [number, number, number, number][] = [];
      for (const [x, y] of candidates) if (blockRead(before, x, y) !== blockRead(after, x, y)) changed.push([x, y, x + SHAPE_BLOCK, y + SHAPE_BLOCK]);
      if (landBlocks) {
        const seen = new Set(changed.map((b) => `${b[0]},${b[1]}`));
        for (const b of landBlocks) if (!seen.has(`${b[0]},${b[1]}`)) changed.push(b);
      }
      out.blocks = changed;
      out.ready = true;
    };
    const waits: Promise<void>[] = [];
    const ask: { solve: RoadElevation; blocks: [number, number][] }[] = [];
    for (const solve of [before, after]) {
      const known = readOf(solve);
      const reading = groundReading.get(solve);
      const mine: [number, number][] = [];
      for (const [x, y] of candidates) {
        const key = blockKey(x, y);
        if (known.has(key)) continue;
        const job = reading?.get(key);
        if (job) { if (!waits.includes(job)) waits.push(job); continue; }
        mine.push([x, y]);
      }
      if (mine.length) ask.push({ solve, blocks: mine });
    }
    if (ask.length && groundWorkers.available && ask.every((a) => a.solve.shapeCut)) {
      // The roads near the blocks asked and the ground under them, as plain numbers; a block the worker cannot read
      const rect = (x: number, y: number): [number, number, number, number] => [x - 1, y - 1, x + SHAPE_BLOCK + 1, y + SHAPE_BLOCK + 1];
      const job: BlockDigestAsk = {
        solves: ask.map(({ solve, blocks }) => ({
          roads: solve.shapeCut!(blocks.map(([x, y]) => rect(x, y))),
          blocks: Float64Array.from(blocks.flat()),
        })),
        plates: terrain.naturalCut?.(ask.flatMap(({ blocks }) => blocks.map(([x, y]) => rect(x, y)))) ?? [],
        block: SHAPE_BLOCK, cell: TERRAIN_CELL, half: TERRAIN_HALF,
      };
      const filed = groundWorkers.digest(job).then((answers) => {
        ask.forEach(({ solve, blocks }, k) => {
          const known = readOf(solve);
          blocks.forEach(([x, y], j) => { const v = answers[k]![j]!; if (!Number.isNaN(v)) known.set(blockKey(x, y), v); });
        });
      }, (error: unknown) => { console.warn('[ground] blocks digested on the page', error); }).finally(() => {
        for (const { solve, blocks } of ask) for (const [x, y] of blocks) groundReading.get(solve)?.delete(blockKey(x, y));
      });
      for (const { solve, blocks } of ask) {
        let reading = groundReading.get(solve);
        if (!reading) groundReading.set(solve, reading = new Map());
        for (const [x, y] of blocks) reading.set(blockKey(x, y), filed);
      }
      waits.push(filed);
    }
    if (!waits.length) { finish(); out.ms = performance.now() - at; return out; }
    out.ms = performance.now() - at;
    void Promise.all(waits).then(() => { const t = performance.now(); finish(); out.ms += performance.now() - t; });
    return out;
  };

// src/render/renderer.ts:2006-2052 slice of src/render/renderer.ts#createSceneRenderer.shapeGround (shared): the feature's statements; "…" marks code left out
  const shapeGround = (net: Network, regions: TerrainRegion | readonly TerrainRegion[] | null = null, sites = false, padsReady = false): { readonly landed: boolean; readonly ms: number } => {
    // … 23 lines
    if (groundWorkers.available || shapings.length) {
      // … 1 lines
      const asking = terrain.askShape(region, region ? null : [...(roads?.shapeBounds() ?? []), ...(pads?.shapeBounds() ?? [])]);
      const boxes = cornerBoxes(asking.plates, TERRAIN_CELL, TERRAIN_HALF);
      const ask: GroundShapeAsk = { roads: roads ? roads.shapeCut!(boxes) : null, pads: pads ? pads.cut(boxes) : null, plates: asking.plates };
      const shaping: Shaping = { ask, take: asking.take, then: told, answers: null, failed: false, landed: false, ms: 0 };
    // … 11 lines
    }
    // … 1 lines
    const shape = pads ? { shapeBounds: () => [...(roads?.shapeBounds() ?? []), ...pads.shapeBounds()], shapeAt: shapeBoth(roads, pads) } : roads;
  // … 2 lines
  };

// src/render/renderer.ts:2214-2215 slice of src/render/renderer.ts#createSceneRenderer.stationsAsked (shared): the feature's statements; "…" marks code left out
  const stationsAsked = (net: Network, roads: ReturnType<typeof roadsToRead>): boolean =>
    roadStations.waiting(roads, (terrain.renderedHeightAt as SharedGroundAt).share?.current ?? null, net.doc.terrainRevision);

// src/render/renderer.ts:2225-2248 slice of src/render/renderer.ts#createSceneRenderer.completeSolve (shared): the feature's statements; "…" marks code left out
  const completeSolve = (net: Network): boolean => {
    // … 1 lines
    if (!partial?.pending) { solveLeft = null; return true; }
    // … 3 lines
    left.order ??= pendingOrder(net, partial.pendingRoads!(), left.x, left.y);
    const scope: SolveScope = { roads: left.order.slice(left.at, left.at + SOLVE_SHARE) };
    const groundAt = isLevelLand(net) ? FLAT_GROUND : builtGroundAt;
    // … 1 lines
    if (stationsAsked(net, roadsToRead(net, groundAt, partial, null, scope))) return false;
    // … 1 lines
    elevation = buildRoadElevation(net, groundAt, partial, roadStations.current, null, scope);
    // … 2 lines
    const moved = elevation.differences?.(partial) ?? null;
    derived(net, 'elevation', moved ? moved.map((r): ChangeRect => [r.minX, r.minY, r.maxX, r.maxY]) : null, 'road heights solved past the opening', worldCause,
      { ms: performance.now() - started, detail: `${elevation.pending ?? 0} roads left` });
    // … 2 lines
    if (elevation.pending) return false;
  // … 2 lines
  };

// src/render/renderer.ts:2290-2315 slice of src/render/renderer.ts#createSceneRenderer.takeEditedCells (shared): the feature's statements; "…" marks code left out
  const takeEditedCells = (net: Network): void => {
    // … 5 lines
    const pad = SURFACE_READ_REACH;
    // … 1 lines
    for (const entry of edits) {
      // … 1 lines
      for (const r of entry.rects ?? []) {
        const [i0, j0] = farCellOf(r[0] - pad, r[1] - pad).split(',').map(Number) as [number, number];
        const [i1, j1] = farCellOf(r[2] + pad, r[3] + pad).split(',').map(Number) as [number, number];
        for (let i = i0; i <= i1; i++) {
          for (let j = j0; j <= j1; j++) {
            const key = `${i},${j}`;
            if (!left.delete(key)) continue;
            fillTaken.add(key);
            fillAwaiting.add(key);
            rects.push(farCellRect(key));
          }
        }
      }
    }
  // … 1 lines
  };

// src/render/renderer.ts:2317-2394 slice of src/render/renderer.ts#createSceneRenderer.buildWorld (shared): the feature's statements; "…" marks code left out
  const buildWorld = (net: Network, full = false): void => {
    // … 12 lines
    const solveScope: SolveScope | null = opening ? { region: [centre.x - reach, centre.y - reach, centre.x + reach, centre.y + reach] } : null;
    // … 16 lines
    const groundAt = levelLand ? FLAT_GROUND : builtGroundAt;
    // … 2 lines
    const fillSolved = full && fillRects !== null && elevation !== null && !elevation.pending && net.state === solvedState;
    const solvedAhead = fillSolved || (offeredElevation !== null && offeredElevation.revision === net.revision && offeredElevation.terrain === net.doc.terrainRevision);
    // … 1 lines
    const stationAsk = roadsRead.of(groundMoved ? null : [net.state, from, ...(solveScope?.region ?? [])], () => roadsToRead(net, groundAt, from, groundMoved, solveScope));
    // … 1 lines
    stationsWait = !solvedAhead && stationsAsked(net, stationAsk);
    // … 12 lines
    elevation = fillSolved && elevation ? elevation : offered && offered.revision === net.revision && offered.terrain === terrainRevision
      ? offered.elevation
      // On land that has not moved, from the last solve
      : buildRoadElevation(net, groundAt, from, roadStations.current, groundMoved, solveScope);
  // … 22 lines
  };

// src/render/renderer.ts:2506-2544 slice of src/render/renderer.ts#createSceneRenderer.warmRoadShaders (shared): the feature's statements; "…" marks code left out
  const warmRoadShaders = (): void => {
    // … 2 lines
    try {
      // … 12 lines
      const warmElevation = buildRoadElevation(warmNet, flat);
      // … 1 lines
      const parts = [
        buildRoadSurfaces(warmNet, warmElevation, materials, flat, undefined, terrain.vergeMaterial),
        buildStructureDetails(warmNet, warmElevation, flat, materials),
        buildUtilities(warmNet, poleGroundAt(warmElevation, flat), sceneryKit),
      ];
      for (const part of parts) group.add(part.group);
      // … 8 lines
      void compileAhead(group).then(() => {
        for (const part of parts) part.dispose();
      });
    } catch (error) {
    // … 1 lines
    }
  };
