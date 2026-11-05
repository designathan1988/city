// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 48 lines
  setPolePreview(net: Network, preview: PolePreviewInput | null): void;
  // … 95 lines
  offerElevation(elevation: RoadElevation, networkRevision: number): void;
  // … 1 lines
  roadsSolve(): { readonly elevation: RoadElevation; readonly terrainRevision: number } | null;
  // … 3 lines
  readonly pavedHeightAt: PavingSource;
  // … 2 lines
  draw(net: Network, sim: SimClient, alpha: number, delta: number, options?: DrawOptions): void;
// … 14 lines
}

// src/render/renderer.ts:399-399 slice of src/render/renderer.ts#SURFACE_SETS (shared): the feature's statements; "…" marks code left out
const SURFACE_SETS: readonly ReadonlySet<RoadStructure>[] = [GROUND_ONLY, new Set<RoadStructure>(['elevated', 'bridge'])];

// src/render/renderer.ts:404-404 slice of src/render/renderer.ts#FAR_ROAD_CELL (shared): the feature's statements; "…" marks code left out
const FAR_ROAD_CELL = 8 * TILE;

// src/render/renderer.ts:412-412 slice of src/render/renderer.ts#isLevelLand (shared): the feature's statements; "…" marks code left out
const isLevelLand = (net: Network): boolean => net.doc.terrainRelief === RELIEF_FLAT && net.doc.terrainStamps.length === 0;

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 518 lines
  const roadPaint = paintCatalogue();
  // … 1 lines
  const roadPrograms = roadProgramStandIns(materials, terrain.vergeMaterial, (color) => roadPaint.get(color) ?? roadPaint.get(LANE_LINE) ?? materials.asphalt);
  world.add(roadPrograms.group);
  // … 1 lines
  const roadStandIn = new RoadStandIn();
  world.add(roadStandIn.group);
  // … 1 lines
  const networkFar = new NetworkFarRoads();
  world.add(networkFar.group);
  // … 3 lines
  let roads: RoadSurfaces | null = null;
  // … 3 lines
  const furniture = createStreetFurniture(sceneryKit);
  world.add(furniture.group, furniture.grass);
  // … 47 lines
  let elevation: RoadElevation | null = null;
  // … 28 lines
  const tileReads = new Map<string, Promise<TileBundle | undefined>>();
  // … 27 lines
  const surfaceReuse: SurfaceReuse = {
    farBatches: farBlocks,
    tiles: new Map(),
    depends: new Map(),
    // Each pass as the last build left it: a tile put in visits its own road tiles, not the region's (`roadSurfaces.ts` PassState).
    passes: new Map(),
    withinMoved: [],
    dependsOn: (minX, minY, maxX, maxY) => new Digest()
      .add(elevation?.digest(minX, minY, maxX, maxY, SURFACE_READ_REACH) ?? 0)
      .add(terrain.digest(minX, minY, maxX, maxY))
      .value(),
    paint: roadPaint,
    // A block of road is made at full detail only near the camera (`mesh/roadLod.ts`).
    near: (x, y) => isNear(x, y, roadFocus.x, roadFocus.y, roadFocus.reach),
    // The near roads stop at the opening's view (`openingBuild`): the rest of a large city's roads is filled after the curtain.
    within: (rect) => openingWithin === null || openingWithin === undefined || openingWithin(rect),
    chunks: new Map(),
    // The inputs held round the view (`InputResidency`): derived where a build visits, let go far from the eye; DO NOT derive the whole map's rings (82 MB of the large city's heap, 2026-10-21).
    inputs: new SurfaceInputs(null, ROAD_INPUT_RESIDENCY),
    inputsEye: () => roadFocus,
    retired: [],
    // DO NOT read the whole store at the opening: it grows with every place the camera went (CLAUDE.md, "Streaming world").
    stored: {
      has: (key) => storedKeys.has(key),
      fetch: (keys) => fetchTiles(keys),
    },
    // Kept by the road tile workers that make them (`roadTile.worker.ts`): the page only marks the key.
    workersKeep: tilesPrefix && typeof Worker !== 'undefined' ? tilesPrefix : null,
    kept: (key) => { if (tilesPrefix && !storedKeys.has(key)) noteStored(key); },
    // DO NOT hold the built tiles for idle writes (the ones made here, when the workers failed)
    keep: (key, bundle) => {
      if (!tilesPrefix || storedKeys.has(key)) return;
      // The tile's arrays handed to the writer, not copied: nothing reads them once its blocks are merged (`SurfaceReuse.keep`).
      if (tileWrites) {
        const buffers: ArrayBuffer[] = [];
        for (const tile of bundle.values()) {
          for (const a of [tile.positions, tile.normals, tile.uvs, tile.colors, tile.sections, tile.indices]) buffers.push(a.buffer as ArrayBuffer);
        }
        // DO NOT post past the ceiling: not kept, made again next time.
        if (!tileWrites.post(tilesPrefix, [[key, bundle]], buffers, buffers.reduce((n, b) => n + b.byteLength, 0))) return;
      } else writeDerivedMany(tilesPrefix, new Map([[key, bundle]]));
      noteStored(key);
    },
  };
  // … 13 lines
  let offeredElevation: { elevation: RoadElevation; revision: number; terrain: number } | null = null;
  // … 1 lines
  const roadTiles = new RoadTilePool();
  // … 30 lines
  let openingWithin: SurfaceReuse['within'] | null = null;
  // … 72 lines
  const roadStations = new RoadStations(() => onAssetsReady());
  // … 35 lines
  const pavedHeightAt: PavingSource = Object.assign((x: number, y: number): number => {
    if (!elevation || !elevation.has(GROUND_ONLY)) return NaN;
    return pavedHeightFrom(elevation, GROUND_ONLY, footwayRiseHere, x, y);
  }, {
    pack: (rect: readonly [number, number, number, number]): { data: PavingData; transfer: ArrayBuffer[] } | null => {
      const ground = elevation !== null && elevation.has(GROUND_ONLY);
      if (ground && !elevation!.fieldFor) return null;
      const field = ground ? elevation!.fieldFor!([rect], []) : null;
      const flush = worldNet ? packFlush(worldNet, [rect]) : null;
      return { data: { rect, field: field?.data ?? null, flush: flush?.data ?? null },
        transfer: [...field?.transfer ?? [], ...flush?.transfer ?? []] };
    },
  });
  // … 1 lines
  const agents: AgentMeshes = createAgentMeshes(deckHeight, onAssetsReady,
    // … 1 lines
    (x, y) => {
      // … 2 lines
      const paved = pavedHeightAt(x, y);
      const ground = Number.isFinite(paved) ? paved : terrain.renderedHeightAt(x, y);
      return Number.isFinite(lot) ? Math.max(lot, ground) : ground;
    },
    // … 3 lines
    (x, y) => terrain.renderedHeightAt(x, y));
  // … 15 lines
  const roadResidency: RoadResidency = { view: detailFrustum, budget: Infinity, farBudget: Infinity, network: networkFar };
  // … 148 lines
  {
    // … 1 lines
    renderer.shadowMap.render = (lights, shadowScene, camera) => {
      if (!occludedNow) { cascadesRender(lights, shadowScene, camera); return; }
    // … 6 lines
    };
  }
  // … 1 lines
  renderer.domElement.addEventListener('webglcontextrestored', () => {
    // … 1 lines
    for (const { mesh } of surfaceReuse.chunks?.values() ?? []) refillArrays(mesh);
    // … 3 lines
    wantAllTexels();
  });
  // … 36 lines
  let keepNet: Network | null = null;
  // … 119 lines
  const groundChanges: GroundRecord = {
    get version() {
      const diary = groundDiary;
      if (!diary || diary.version === groundScanned) return groundVersion;
      const fresh = diary.since(groundScanned, GROUND_KINDS);
      if (fresh === null) groundVersion = diary.version;
      else if (fresh.length) groundVersion = fresh[fresh.length - 1]!.serial;
      groundScanned = diary.version;
      return groundVersion;
    },
    touches: (since, area) => groundDiary?.touches(since, area, GROUND_KINDS) ?? false,
  // … 2 lines
  };
  // … 8 lines
  const floorGroundSince: GroundSince = {
    get version() { return groundDiary?.serialOf(...FLOOR_KINDS) ?? 0; },
    rectsSince: (since) => {
      const entries = groundDiary?.since(since, FLOOR_KINDS);
      if (!entries) return null;
      // … 1 lines
      for (const entry of entries) {
        if (entry.rects === null) return null;
        rects.push(...entry.rects);
      }
    // … 1 lines
    },
  };
  // … 29 lines
  const floors = buildingFloors(builtGroundAt, pavedHeightAt, floorBox, FLOOR_READ_REACH);
  // … 2 lines
  const cameraSolidsKept = cameraSolidsKeeper((b) => floors.floorOf(b), floorBox, FLOOR_READ_REACH);
  // … 4 lines
  let roadsWalk: { readonly net: Network; readonly at: Iterator<SegmentRibbon> } | null = null;
  // … 38 lines
  const sitesGraded = siteGrading(builtGroundAt, pavedHeightAt, SITE_APRON, (b) => bankBox(b), floors);
  // … 29 lines
  const groundRead = new WeakMap<RoadElevation, Map<number, number>>();
  // … 1 lines
  const groundReading = new WeakMap<RoadElevation, Map<number, Promise<void>>>();
  // … 291 lines
  let placedOn: Network | null = null;
  engine.register({
    // … 1 lines
    refresh: (): number => {
      if (!placedOn || !elevation) return 0;
      // … 1 lines
      const before = furniture.triangles;
      const work = furniture.sync(placedOn, elevation, terrain.renderedHeightAt, cityMinutes);
      builtTriangles += furniture.triangles - before;
      // … 1 lines
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
      builtTriangles += barriers.triangles - before;
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
      builtTriangles += transit.triangles - before;
      return work;
    },
  }, 'draw');
  // … 52 lines
  let worldNet: Network | null = null;
  // … 16 lines
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
        builtTriangles: builtTriangles + buildings.triangles,
        // … 5 lines
        roadTiles: roads ? { built: roads.built, reused: roads.reused } : { built: 0, reused: 0 },
        // … 1 lines
        terrainMs: Math.round(terrainMs),
      };
    },
    // … 54 lines
    openingState() {
      return { roadsIn: roads !== null && mapRoadsIn, coarsePending: buildings.coarsePending, viewPending: buildings.viewPending, treesStarted: natureTreesStarted,
        treesPending: natureTreesPending, programs: openingPrograms, worked: reveal.worked, worldBusy: worldJob !== null, fillPending: openingFillPending, fillCellsLeft: fillLeft?.size ?? 0 };
    },
    beginLoad() {
      // … 10 lines
      networkFar.clear();
      forgetRoadCells(roadResidency);
    // … 4 lines
    },
    // … 7 lines
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
    pavedHeightAt,
    // … 25 lines
    busy: () => (fx?.busy() ?? false) || natureTreesPending || cameraEasing || (terrain.waiting?.() ?? false),
    // … 2 lines
    drifting: () => ((fogMoving || placedCloudsShown) && post.enabled && (quality.cloudShadows || quality.skyClouds)) || weatherActive,
    // … 5 lines
    setTransitPreview(preview) {
      // … 8 lines
      transitPreview.set(preview?.points ?? [], preview?.mode ?? 'train', terrain.renderedHeightAt, pavedHeightAt);
    },
    // … 4 lines
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
      // … 3 lines
      groundDiary = net.doc.changes;
      // … 16 lines
      const stroking = !!options?.holdRoads && !!elevation && networkRevision === net.revision;
      const groundMoved = terrain.update(net.doc, stroking);
      // … 1 lines
      const landEntries = landRead.read(net.doc.changes);
      if (groundMoved) {
        landVersion++;
        // The stroke's dirty region, dab by dab (`landDirty`); a rewrite the terrain cannot place (an undo of the relief), where the diary says it moved.
        const moved = terrain.lastRegion;
        const said = moved === null ? landMovedSince(landEntries) : regionRect(moved);
        landDirty = said === null || landDirty === 'all' ? 'all' : unionRect(landDirty, said);
      }
      terrain.updatePaint(net.doc);
      // … 1 lines
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
      // … 9 lines
      if (!levelLand && !(worldJob && worldJobGround) && gradedFor !== net.doc.buildings.revision) {
        gradedFor = net.doc.buildings.revision;
        const changed = sitesGraded.changedSites(net.doc.buildings);
        if (changed) shapeGround(net, changed.map(terrainRegion), true);
      }
      // … 2 lines
      const groundSettled = !stroking && !worldJob;
      // … 1 lines
      if (groundSettled) { placedOn = net; engine.refresh('draw'); }
      // … 2 lines
      const groundIn = !worldJob || (levelLand && !worldJobGround && networkRevision === net.revision);
      // … 1 lines
      if ((groundSettled || (!stroking && groundIn)) && groundChanges.version !== buildingGroundSeen) {
        const moved = buildingGroundSeen < 0 ? null : buildingsOnGroundSince(net.doc, buildingGroundSeen);
        if (moved === null || moved.length > 0) buildingGround = String(groundChanges.version);
        buildingGroundSeen = groundChanges.version;
      }
      // … 1 lines
      if (roads !== null && (reveal.opened || (groundIn && networkRevision === net.revision))) {
        buildings.hidden = !reveal.opened;
        // Buildings put together or streamed in (HLOD, `layer.ts`) are new casters: the casters' signature tells the cascades where, by the
        // batches' and the parts' boxes (`shellBatch.ts` castChanged, `PartCell.castInto`). DO NOT throw every store away for each cell landed:
        // a flight lands cells at every few frames, each drew the three cascades whole.
        frameMark('frame:buildings-start');
        buildings.update(net.doc, terrain.renderedHeightAt, buildingGround, pavedHeightAt, builtGroundAt,
          (since) => (since === '' ? null : buildingsOnGroundSince(net.doc, Number(since))), levelLand ? levelGroundDigest : landGroundDigest);
        // Another frame while they land, nothing more: what lands stirs the stores itself (childadded, the door, `update`'s answer).
        if (buildings.pending) askDraw();
        frameMark('frame:buildings-end');
      }
      // … 1 lines
      if (!natureTreesStarted && roads !== null) startNatureTrees();
      // … 1 lines
      if (!fxPreload && !fxLoading && roads !== null && !worldJob && typeof requestIdleCallback === 'function') {
        fxPreload = true;
        requestIdleCallback(() => { void loadEffects(); }, { timeout: 5000 });
      }
      // … 5 lines
      gardens.source(net.doc.buildings, terrain.renderedHeightAt);
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
      // … 6 lines
      if (solidsNeeded) {
        if (cameraSolidsKept.follow(net.doc.buildings, floorGroundSince)) {
          rig.setSolids((x, y) => cameraSolidsKept.solids().floorAt(x, y), (x, y) => cameraSolidsKept.solids().roofAt(x, y));
          townSolids = () => cameraSolidsKept.solids();
        }
      } else if (townSolids !== null) {
        rig.setSolids(null);
        townSolids = null;
      }
      // … 2 lines
      post.followTown(rig.target.x, -rig.target.z, townSolids, canyonChanges(net.doc.changes));
      // … 3 lines
      {
        // … 26 lines
        if (pendingStandIn.length && elevation && net.revision === net.doc.revision) {
          roadStandIn.show(net, elevation, pendingStandIn.splice(0), net.revision);
        }
        roadStandIn.landed(landedRevision);
      // … 4 lines
      }
      if (roads) roads.group.visible = true;
      // … 3 lines
      for (const mesh of furniture.meshes) {
        mesh.visible = quality.detailProps && detailed && (!plantMap || !isLeafCards(mesh));
      }
      // … 4 lines
      if (plantMap !== plantMapFor.map || gardens !== plantMapFor.gardens) {
        // … 2 lines
        furniture.setMap(plantMap);
      }
      // … 1 lines
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
      // … 32 lines
      {
        // … 1 lines
        const props = [...(gardens && detailed ? [gardens.layer] : []), ...(quality.detailProps && detailed ? [furniture.layer] : [])];
        if (props.length) lookProps(props, view);
      // … 1 lines
      }
      furniture.cull(crowdFrustum, crowdProjection);
      // … 3 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.cull(crowdFrustum, crowdProjection);
      // … 36 lines
      const key = cutSpec ? `${cutSpec.level}@${cutSpec.x},${cutSpec.y}:${net.doc.buildings.revision}` : '';
      if (key !== lampsKey) {
        lampsKey = key;
        const lamps = roomLamps.lamps(net.doc, cutSpec, builtGroundAt, pavedHeightAt, ROOM_LIGHTS);
        roomLights.forEach((light, i) => {
          const at = lamps[i];
          light.userData['used'] = !!at;
          if (at) light.position.set(at.x, at.z, -at.y);
        });
      }
      // … 15 lines
      {
        // … 1 lines
        const weather = net.doc.weather;
        // … 1 lines
        const wind = windVector(weather, m(1));
        cloudDrift.x += wind.x * step;
        cloudDrift.y += wind.y * step;
        // … 1 lines
        setWindWeather(wind.x, -wind.y, weather.wind);
        // … 1 lines
        const windLength = Math.hypot(wind.x, wind.y);
        const angle = (weather.windDirection * Math.PI) / 180;
        terrain.setWaterLook({
          waves: Math.min(1, weather.waves + weather.wind / 40),
          foam: weather.foam,
          current: weather.current * m(1),
          windX: windLength > 1e-6 ? wind.x / windLength : Math.cos(angle),
          windZ: windLength > 1e-6 ? -wind.y / windLength : -Math.sin(angle),
          windSpeed: windLength,
        });
        // … 1 lines
        if (weather.lightning > 0 && Math.random() < (step * weather.lightning) / 60) {
          const a = Math.random() * Math.PI * 2, r = Math.sqrt(Math.random()) * span * 0.45;
          strikeAt(target.x + Math.cos(a) * r, -target.z + Math.sin(a) * r);
        }
        // … 2 lines
        const storm = Math.min(1, weather.lightning / 8);
        environment.setWeather(Math.max(weather.rain * 0.85, storm * 0.6), flash);
        rainWind.set(wind.x, -wind.y);
        rain.update(weather.rain, target, span, rainWind, step);
        sceneryKit.setHaze(Math.max(weather.rain, storm));
        weatherActive = weather.rain > 0 || weather.lightning > 0 || flash > 0 || (weather.wind > 0 && placedCloudsShown);
      }
      // … 43 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 4 lines
        const moving = net.doc.weather.wind > 0 || (simRunning && movingCasters());
        if (castersMoved && !moving) shadowMoved = true;
        castersMoved = moving;
        // … 19 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 9 lines
      {
        // … 11 lines
        if (net.doc.gullyRevision !== gulliesFor) {
          gulliesFor = net.doc.gullyRevision;
          terrain.setGullies(net.doc.gullyDabs, net.doc.gullyAuto);
        }
        // … 8 lines
        if (roadFocus.frames++ % 6 === 0) {
          // … 1 lines
          roadResidency.networkFrom = !rig.perspective && rig.viewport.zoom < 1 ? 0 : undefined;
          // … 1 lines
          for (const { mesh } of surfaceReuse.chunks.values()) blocks.push(mesh);
          updateRoadLevels(blocks, roadFocus.x, roadFocus.y, roadFocus.reach, roadResidency);
          // … 1 lines
          if (roadResidency.wantsNetwork && elevation && worldJob === null && !openingBuild && fillLeft === null && !networkFar.pending && networkFar.outdated()) {
            networkFar.settle(farShown());
            requestNetworkFar(net, elevation);
          }
        }
      // … 1 lines
      }
      // … 2 lines
      {
        // … 1 lines
        const doc = net.doc;
        // … 1 lines
        const landHeld = !!options?.holdRoads && fogTerrainSeen !== doc.terrainRevision;
        const fogSerial = doc.changes.serialOf(...FOG_KINDS);
        if ((fogJournal !== doc.changes || fogSerial !== fogSeen) && !landHeld) {
          const startedAt = performance.now();
          // Only where the fog, the land or the ground under it changed (`createFogField`)
          const entries = fogJournal === doc.changes && fogSeen >= 0 ? doc.changes.since(fogSeen, FOG_KINDS) : null;
          const ground = (x: number, y: number): number => terrain.renderedHeightAt(x, y);
          const laid = !entries || entries.some((e) => e.rects === null)
            ? fogField.whole(doc.fogDabs, ground)
            : fogField.update(doc.fogDabs, ground, entries.flatMap((e) => e.rects!));
          fogLayer = laid.any ? laid : null;
          fogJournal = doc.changes;
          fogSeen = fogSerial;
          fogTerrainSeen = doc.terrainRevision;
          measureHitch('hitch:fog', startedAt);
        }
        if (!landHeld) {
          // The fog's window kept round where the view looks (`fogLayer.ts`): only the texels that came in written.
          const followed = fogField.follow(rig.viewport.centre.x, rig.viewport.centre.y, doc.fogDabs, (x, y) => terrain.renderedHeightAt(x, y));
          if (followed) fogLayer = followed.any ? followed : null;
        }
        const f = doc.fogSettings;
        post.setPlacedClouds(doc.clouds);
        // … 1 lines
        if (elementKit && !landHeld) {
          const startedAt = performance.now();
          const layer = elementLayerOf()!;
          const entries = elementRead.read(doc.changes);
          let cells: Set<number> | null = null;
          if (entries !== null && entries.every((e) => e.rects !== null)) {
            cells = new Set();
            // A prop stands on the ground drawn under its point: a terrain cell round a change of the ground.
            for (const e of entries) for (const r of e.rects!) keepCellsOver(r, TERRAIN_CELL, cells);
          }
          if ((cells === null || cells.size > 0) && layer.sync(cells, doc, (x, y) => terrain.renderedHeightAt(x, y))) {
            // What came and went is told by its boxes (`cellInstances.ts` reconcile, `castBoxes`): no store thrown away for it.
            layer.cull(crowdFrustum, crowdProjection);
          }
          measureHitch('hitch:elements', startedAt);
        }
        // … 1 lines
        post.setGroundFog(fogLayer?.any ? { texture: fogLayer.texture, low: fogLayer.low, high: fogLayer.high, density: f.density, window: fogLayer.window } : null);
        fogMoving = !!fogLayer?.any;
        placedCloudsShown = doc.clouds.length > 0 || !!elementLayer?.hasEffects;
      }
      // … 1 lines
      const rainNow = net.doc.weather.rain;
      const air = rainNow > 0 ? { ...atmosphere, fog: Math.max(atmosphere.fog, rainNow * 0.2), fogHeight: Math.max(atmosphere.fogHeight, m(120)) } : atmosphere;
      // … 3 lines
      post.setAtmosphere(air, sunTowards.copy(environment.sun.position).sub(environment.sun.target.position), environment.skyColor,
        sunLight.copy(environment.sun.color).multiplyScalar(environment.sun.intensity), eyeShift);
      // … 15 lines
      roadTiles.unseen = !reveal.opened;
      if (!reveal.opened) {
        // … 8 lines
        const roadsIn = roads !== null && mapRoadsIn && !networkFar.pending;
        const whole = roadsIn && !buildings.coarsePending && (mapView || !buildings.viewPending)
          && natureTreesStarted && !natureTreesPending;
        // … 4 lines
        const drawablesIn = roadsIn && !buildings.coarsePending && !buildings.viewPending;
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
      // … 2 lines
      if (reveal.opened && qualityCompile && performance.now() - qualityCompileSince < QUALITY_COMPILE_GIVE_UP_MS) { /* drawn again once built: `setQuality` asks for the frame */ }
      else if (reveal.opened) {
        // … 3 lines
        if (openingFillPending && worldJob === null && completeSolve(net)) {
          // … 3 lines
          const scope = openingWithin;
          // … 2 lines
          fillLeft = new Set(networkFar.cellKeys());
          // … 1 lines
          fillEditRead.read(net.doc.changes);
          openingWithin = (rect) => (scope?.(rect) ?? false) || fillTaken.has(farCellOf((rect[0] + rect[2]) / 2, (rect[1] + rect[3]) / 2));
        }
        if (fillLeft !== null && fillLeft.size > 0 && worldJob === null && !openingFillPending) fillStep(net);
        // … 12 lines
        if (backend === 'webgl') {
          // … 2 lines
          const wanted = occlusionProbe.enabled && townSolids !== null && rig.perspective && effectSwitches === ALL_EFFECTS_ON && buildingChoices === DEFAULT_BUILDING_CHOICES;
          const occluding = wanted && occlusionIdle === 0;
          if (wanted && occlusionIdle > 0) occlusionIdle--;
          // … 2 lines
          occlusionProbe.stats.active = occluding;
          if (occluding) {
            occluderCells.clear();
            const camera = rig.camera as PerspectiveCamera;
            occlusion.build(camera, camera.aspect, townSolids!(), occluderDrawn, camera.near);
            occlusionProbe.stats.objectsHidden = buildings.hideOccluded(camera, occlusionHidden);
            occludedNow = occlusionProbe.stats.objectsHidden > 0;
            if (!occludedNow) occlusionIdle = OCCLUSION_RETRY_FRAMES;
          } else occlusion.clear();
          // … 1 lines
          try {
          // … 1 lines
          } finally {
            if (occluding) buildings.restoreOccluded();
          // … 1 lines
          }
          // … 1 lines
          if (occluding) Object.assign(occlusionProbe.stats, occlusion.stats);
        // … 1 lines
        } else gpuView?.render(scene, rig.camera);
      // … 9 lines
      } else lookAgain();
    // … 16 lines
    },
    // … 63 lines
    dispose() {
      // … 3 lines
      roadTiles.dispose();
      networkFar.dispose();
      // … 12 lines
      polePreview?.dispose();
      // … 6 lines
      roadPrograms.dispose();
      // … 2 lines
      roads?.dispose();
      disposeSurfaceReuse(surfaceReuse);
      // … 1 lines
      furniture.dispose();
      // … 1 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.dispose();
    // … 16 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:1062-1080 slice of src/render/renderer.ts#createSceneRenderer.fetchTiles (shared): the feature's statements; "…" marks code left out
  const fetchTiles = (keys: readonly string[]): Promise<Map<string, TileBundle>> => {
    if (!tilesPrefix) return Promise.resolve(new Map());
    const unique = [...new Set(keys)];
    const missing = unique.filter((key) => !tileReads.has(key));
    if (missing.length) {
      // Read off the page where there is a writer (`DerivedWriterLane.read`), else here (tests).
      const batch = tileWrites ? tileWrites.read<TileBundle>(tilesPrefix, missing) : readDerivedSome<TileBundle>(tilesPrefix, missing);
      for (const key of missing) {
        const one = batch.then((got) => got.get(key));
        tileReads.set(key, one);
        void one.finally(() => { if (tileReads.get(key) === one) tileReads.delete(key); });
      }
    }
    return Promise.all(unique.map((key) => tileReads.get(key)!.then((bundle) => [key, bundle] as const))).then((pairs) => {
      const out = new Map<string, TileBundle>();
      for (const [key, bundle] of pairs) if (bundle) out.set(key, bundle);
      return out;
    });
  };

// src/render/renderer.ts:1082-1082 slice of src/render/renderer.ts#createSceneRenderer.farShown (shared): the feature's statements; "…" marks code left out
  const farShown = (): string[] => [...(roadResidency.dropped ?? []), ...(roadResidency.parked ?? [])];

// src/render/renderer.ts:1195-1198 slice of src/render/renderer.ts#createSceneRenderer.requestNetworkFar (shared): the feature's statements; "…" marks code left out
  const requestNetworkFar = (net: Network, heights: RoadElevation): void => {
    networkFarRead.read(net.doc.changes);
    networkFar.request(net, heights, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN, null);
  };

// src/render/renderer.ts:1257-1264 slice of src/render/renderer.ts#createSceneRenderer.deckHeight (shared): the feature's statements; "…" marks code left out
  const deckHeight = (
  // … 3 lines
  ): number => {
    // … 1 lines
    return segment === undefined ? elevation.at(x, y) : elevation.onSegment(segment, x, y);
  };

// src/render/renderer.ts:1267-1267 slice of src/render/renderer.ts#createSceneRenderer.footwayRiseHere (shared): the feature's statements; "…" marks code left out
  const footwayRiseHere = (x: number, y: number): number => (worldNet ? footwayRiseAt(worldNet, x, y) : FOOTWAY_RISE);

// src/render/renderer.ts:1269-1276 slice of src/render/renderer.ts#createSceneRenderer.levelGroundDigest (shared): the feature's statements; "…" marks code left out
  const levelGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(Math.round(builtGroundAt(minX, minY) * 100)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    const flush = worldNet ? packFlush(worldNet, [[minX, minY, maxX, maxY]]).data : null;
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    // … 1 lines
    if (pads) out.add(pads.apron).addPlain(pads.pads);
    return out.value();
  };

// src/render/renderer.ts:1278-1283 slice of src/render/renderer.ts#createSceneRenderer.landGroundDigest (shared): the feature's statements; "…" marks code left out
  const landGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(terrain.digest(minX, minY, maxX, maxY)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    const flush = worldNet ? packFlush(worldNet, [[minX, minY, maxX, maxY]]).data : null;
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    return out.value();
  };

// src/render/renderer.ts:1406-1413 slice of src/render/renderer.ts#createSceneRenderer.applyGeometryBudget (shared): the feature's statements; "…" marks code left out
  const applyGeometryBudget = (): void => {
    // … 2 lines
    roadResidency.budget = budget.roadsNearMB * 1048576;
    roadResidency.farBudget = budget.roadsFarMB * 1048576;
  // … 2 lines
  };

// src/render/renderer.ts:1719-1719 slice of src/render/renderer.ts#createSceneRenderer.followFloors (shared): the feature's statements; "…" marks code left out
  const followFloors = (net: Network, shaped: readonly ChangeRect[] | null = null): void => floors.follow(net.doc.buildings, floorGroundSince, shaped);

// src/render/renderer.ts:1726-1732 slice of src/render/renderer.ts#createSceneRenderer.nextWatched (shared): the feature's statements; "…" marks code left out
  const nextWatched = (net: Network): SegmentRibbon | null => {
    let walk = roadsWalk;
    if (!walk || walk.net !== net) roadsWalk = walk = { net, at: net.ribbons.values() };
    let step = walk.at.next();
    if (step.done) { roadsWalk = walk = { net, at: net.ribbons.values() }; step = walk.at.next(); }
    return step.done ? null : step.value;
  };

// src/render/renderer.ts:1733-1759 slice of src/render/renderer.ts#createSceneRenderer.watchRoads (shared): the feature's statements; "…" marks code left out
  const watchRoads = (net: Network): void => {
    const now = performance.now();
    // Not while the world is being rebuilt: until the job lands the new roads have no picture yet by design
    if (now - roadsWatchedAt < 1000 || !reveal.opened || net.doc.segments.size === 0 || worldJob !== null) return;
    roadsWatchedAt = now;
    // DO NOT count every road mesh's vertices: one drawn mesh is the answer.
    const vertices = roads ? firstDrawnVertices(roads.group) : 0;
    let buried: { x: number; y: number; land: number; paved: number } | null = null;
    for (let k = 0; k < 4; k++) {
      const ribbon = nextWatched(net);
      if (!ribbon) break;
      const p = ribbon.full.sampleAt(ribbon.full.length / 2).p;
      const paved = pavedHeightAt(p.x, p.y), land = terrain.renderedHeightAt(p.x, p.y);
      // Under the land on purpose in a tunnel.
      if (net.doc.segment(ribbon.id)?.structure === 'tunnel') continue;
      if (Number.isFinite(paved) && land > paved + m(0.3)) { buried = { x: p.x, y: p.y, land, paved }; break; }
    }
    const missing = !roads || !roads.group.parent || vertices === 0;
    if ((missing || buried) && !roadsReported) {
      roadsReported = true;
      // Once, latched by `roadsReported`; as text, so a headless console log keeps it.
      console.error('roads missing from the picture', JSON.stringify({
        missing, buried, vertices, inWorld: !!roads?.group.parent, worldJob: worldJob !== null,
        terrainRevision: net.doc.terrainRevision, revision: net.revision,
      }));
    } else if (!missing && !buried) roadsReported = false;
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

// src/render/renderer.ts:1907-1918 slice of src/render/renderer.ts#createSceneRenderer.padsAskOf (shared): the feature's statements; "…" marks code left out
  const padsAskOf = (list: readonly Building[]): PadsAsk | null => {
    // … 1 lines
    if (!ground || !pavedHeightAt.pack) return null;
    // … 5 lines
    const paving = pavedHeightAt.pack([x0, y0, x1, y1]);
    if (!paving) return null;
    return { buildings: list.map((b) => ({ record: refOf(b), floor: floors.floorOf(b) })), paving: paving.data, ground, apron: SITE_APRON };
  };

// src/render/renderer.ts:1919-1972 slice of src/render/renderer.ts#createSceneRenderer.shapeBlocksSteps (shared): the feature's statements; "…" marks code left out
  function* shapeBlocksSteps(net: Network, blocks: readonly [number, number, number, number][]): Generator<'wait' | void, void, void> {
    // … 1 lines
    const reached = net.doc.buildings.nearAny(blocks, BANK_REACH, (b, q) => {
      const box = bankBox(b);
      return box[0] <= q[2] && box[2] >= q[0] && box[1] <= q[3] && box[3] >= q[1];
    });
    followFloors(net, blocks);
    // … 1 lines
    const padsAsk = reached.length ? padsAskOf(reached) : null;
    // … 1 lines
    if (padsAsk && groundWorkers.available) {
      let answer: (Pad | null)[] | null = null, failed = false;
      groundWorkers.pads(padsAsk).then((pads) => { answer = pads; }, (error: unknown) => {
        console.warn('[ground] the platforms are worked out on the page', error);
        failed = true;
      });
      while (!answer && !failed) { groundWorkers.check(); yield 'wait'; }
      const pads = answer as (Pad | null)[] | null;
      if (pads) { reached.forEach((b, i) => sitesGraded.take(b, pads[i] ?? null)); padsTaken = true; }
    }
    if (!padsTaken) {
      // No worker (or it could not): a step a square of the ground's blocks (`PADS_SQUARE` of them a side), never a building a step.
      const side = SHAPE_BLOCK * PADS_SQUARE;
      const bySquare = new Map<string, Building[]>();
      for (const b of reached) {
        const box = bankBox(b);
        const k = `${Math.floor((box[0] + box[2]) / 2 / side)},${Math.floor((box[1] + box[3]) / 2 / side)}`;
        let list = bySquare.get(k);
        if (!list) bySquare.set(k, list = []);
        list.push(b);
      }
      for (const list of bySquare.values()) {
        for (const b of list) sitesGraded.refresh(b);
        yield;
      }
    }
    gradedFor = net.doc.buildings.revision;
    // … 1 lines
    const sites = sitesGraded.changedSites(net.doc.buildings);
    sitesGraded.rebuild(net.doc.buildings);
    // … 7 lines
    if (sites) regions.push(...sites.map(terrainRegion));
    // … 1 lines
    const shaping = shapeGround(net, regions, false, true);
    while (!shaping.landed) { landShapes(); if (shaping.landed) break; yield 'wait'; }
    derived(net, 'ground', regions.map(regionRect), 'ground cut and filled up to the roads', worldCause,
      { ms: shaping.ms, detail: `${regions.length} regions` });
  }

// src/render/renderer.ts:2006-2052 slice of src/render/renderer.ts#createSceneRenderer.shapeGround (shared): the feature's statements; "…" marks code left out
  const shapeGround = (net: Network, regions: TerrainRegion | readonly TerrainRegion[] | null = null, sites = false, padsReady = false): { readonly landed: boolean; readonly ms: number } => {
    const at = performance.now();
    const list: readonly TerrainRegion[] | null = regions === null ? null
      : typeof regions[0] === 'number' ? [regions as TerrainRegion] : regions as readonly TerrainRegion[];
    if (list && !list.length) return LANDED;
    const region = list;
    // The ground is cut and filled here, and only here: what stands on it reads where from the diary
    const parent = net.doc.changes.version;
    const told = (): void => {
      if (padsReady) return;
      derived(net, 'ground', list ? list.map(regionRect) : null, sites ? 'lot ground levelled' : 'ground cut and filled', parent);
    };
    const roads = net.doc.segments.size > 0 ? elevation : null;
    followFloors(net, list ? list.map(regionRect) : null);
    if (!padsReady && (!region || !sitesGraded.pads || sites)) {
      gradedFor = net.doc.buildings.revision;
      // The roads or the land moved: the platforms whose banks the diary says they moved under worked out afresh (`siteGrading.ts` `follow`, the floors' own changes); every one only the first time, or where the diary cannot say.
      if (!sites) {
        if (sitesGraded.pads) sitesGraded.follow(net.doc.buildings, floorGroundSince, BANK_REACH);
        else sitesGraded.forget(net.doc.buildings);
      }
      sitesGraded.rebuild(net.doc.buildings);
    }
    const pads = net.doc.buildings.size > 0 ? sitesGraded.pads : null;
    if (groundWorkers.available || shapings.length) {
      // The corners asked, and the roads and platforms they read as they stand now, as plain numbers.
      const asking = terrain.askShape(region, region ? null : [...(roads?.shapeBounds() ?? []), ...(pads?.shapeBounds() ?? [])]);
      const boxes = cornerBoxes(asking.plates, TERRAIN_CELL, TERRAIN_HALF);
      const ask: GroundShapeAsk = { roads: roads ? roads.shapeCut!(boxes) : null, pads: pads ? pads.cut(boxes) : null, plates: asking.plates };
      const shaping: Shaping = { ask, take: asking.take, then: told, answers: null, failed: false, landed: false, ms: 0 };
      shapings.push(shaping);
      if (groundWorkers.available) {
        groundWorkers.shape(ask).then((answers) => { shaping.answers = answers; }, (error: unknown) => {
          console.warn('[ground] a pass is shaped on the page', error);
          shaping.failed = true;
        });
      } else shaping.failed = true;
      shaping.ms = performance.now() - at;
      // No worker: put in place now, after the passes before it.
      if (shaping.failed) landShapes();
      return shaping;
    }
    told();
    const shape = pads ? { shapeBounds: () => [...(roads?.shapeBounds() ?? []), ...pads.shapeBounds()], shapeAt: shapeBoth(roads, pads) } : roads;
    terrain.shapeToRoads(shape, region);
    return { landed: true, ms: performance.now() - at };
  };

// src/render/renderer.ts:2063-2074 slice of src/render/renderer.ts#createSceneRenderer.syncUtilities (shared): the feature's statements; "…" marks code left out
  const syncUtilities = (net: Network, groundAt: (x: number, y: number) => number): void => {
    const serial = net.doc.changes.serialOf(...POLE_KINDS);
    if (utilities) utilities.source(net, groundAt);
    if (utilities && serial === poleSerial) return;
    if (!utilities) {
      utilities = new PoleCells(sceneryKit);
      utilities.source(net, groundAt);
      world.add(utilities.group);
    }
    utilities.follow(net.doc.changes);
    poleSerial = serial;
  };

// src/render/renderer.ts:2076-2082 slice of src/render/renderer.ts#createSceneRenderer.rebuildUtilities (shared): the feature's statements; "…" marks code left out
  const rebuildUtilities = (net: Network): void => {
    if (!elevation) return;
    utilityRevision = net.doc.utilityRevision;
    const at = performance.now();
    syncUtilities(net, poleGroundAt(elevation, terrain.renderedHeightAt, net));
    measureHitch('hitch:utilities/poles', at);
  };

// src/render/renderer.ts:2177-2187 slice of src/render/renderer.ts#createSceneRenderer.networkExtent (shared): the feature's statements; "…" marks code left out
  const networkExtent = (net: Network): Rect => {
    let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
    for (const n of net.doc.nodes.values()) {
      if (n.x < minX) minX = n.x;
      if (n.y < minY) minY = n.y;
      if (n.x > maxX) maxX = n.x;
      if (n.y > maxY) maxY = n.y;
    }
    if (!Number.isFinite(minX)) return [0, 0, 0, 0];
    return [minX - SHAPE_BLOCK, minY - SHAPE_BLOCK, maxX + SHAPE_BLOCK, maxY + SHAPE_BLOCK];
  };

// src/render/renderer.ts:2191-2193 slice of src/render/renderer.ts#createSceneRenderer.derived (shared): the feature's statements; "…" marks code left out
  const derived = (net: Network, kind: DerivedChangeKind, rects: readonly ChangeRect[] | null, cause: string, parent: number,
    extra: { readonly ms?: number; readonly detail?: string } = {}): number =>
    net.doc.changes.record(kind, rects, { cause, ...(parent ? { parent } : {}), ...extra });

// src/render/renderer.ts:2196-2210 slice of src/render/renderer.ts#createSceneRenderer.rebuildWorld (shared): the feature's statements; "…" marks code left out
  const rebuildWorld = (net: Network): void => {
    worldNet = net;
    // A map opened always builds its own world (`mapRoadsIn`), whatever its revision says.
    if (!mapOpened && networkRevision === net.revision && terrainRevision === net.doc.terrainRevision) {
      stationsWait = false;
      // A world being built builds the poles too (`worldSteps`)
      if (worldJob === null && (utilityRevision !== net.doc.utilityRevision || poleSerial !== net.doc.changes.serialOf(...POLE_KINDS))) {
        const atUtilities = performance.now();
        rebuildUtilities(net);
        measureHitch('hitch:road-edit/utilities alone', atUtilities);
      }
      return;
    }
    buildWorld(net);
  };

// src/render/renderer.ts:2214-2215 slice of src/render/renderer.ts#createSceneRenderer.stationsAsked (shared): the feature's statements; "…" marks code left out
  const stationsAsked = (net: Network, roads: ReturnType<typeof roadsToRead>): boolean =>
    roadStations.waiting(roads, (terrain.renderedHeightAt as SharedGroundAt).share?.current ?? null, net.doc.terrainRevision);

// src/render/renderer.ts:2225-2248 slice of src/render/renderer.ts#createSceneRenderer.completeSolve (shared): the feature's statements; "…" marks code left out
  const completeSolve = (net: Network): boolean => {
    const partial = elevation;
    if (!partial?.pending) { solveLeft = null; return true; }
    // The network moved since the solve standing and its world is not built yet: that build's solve (no scope) takes every road left. DO NOT take a share of it here: the edit's roads would be solved in it, and the build's comparison of the two solves would find none of them moved (`compareSolves`).
    if (net.state !== solvedState) return false;
    const left = solveLeft ??= { x: rig.viewport.centre.x, y: rig.viewport.centre.y, order: null, at: 0 };
    left.order ??= pendingOrder(net, partial.pendingRoads!(), left.x, left.y);
    const scope: SolveScope = { roads: left.order.slice(left.at, left.at + SOLVE_SHARE) };
    const groundAt = isLevelLand(net) ? FLAT_GROUND : builtGroundAt;
    // Off level ground the share's stations are read off the page first, as a world build's (`roadStations.ts`).
    if (stationsAsked(net, roadsToRead(net, groundAt, partial, null, scope))) return false;
    const started = performance.now();
    elevation = buildRoadElevation(net, groundAt, partial, roadStations.current, null, scope);
    roadStations.used();
    left.at += SOLVE_SHARE;
    const moved = elevation.differences?.(partial) ?? null;
    derived(net, 'elevation', moved ? moved.map((r): ChangeRect => [r.minX, r.minY, r.maxX, r.maxY]) : null, 'road heights solved past the opening', worldCause,
      { ms: performance.now() - started, detail: `${elevation.pending ?? 0} roads left` });
    measureHitch('hitch:opening/solve share', started);
    onAssetsReady();
    if (elevation.pending) return false;
    solveLeft = null;
    return true;
  };

// src/render/renderer.ts:2250-2250 slice of src/render/renderer.ts#createSceneRenderer.farCellOf (shared): the feature's statements; "…" marks code left out
  const farCellOf = (x: number, y: number): string => `${Math.floor((x - TILE_ORIGIN) / FAR_ROAD_CELL)},${Math.floor((y - TILE_ORIGIN) / FAR_ROAD_CELL)}`;

// src/render/renderer.ts:2252-2256 slice of src/render/renderer.ts#createSceneRenderer.farCellRect (shared): the feature's statements; "…" marks code left out
  const farCellRect = (key: string): Rect => {
    // … 1 lines
    const x = TILE_ORIGIN + i * FAR_ROAD_CELL, y = TILE_ORIGIN + j * FAR_ROAD_CELL;
    return [x, y, x + FAR_ROAD_CELL, y + FAR_ROAD_CELL];
  };

// src/render/renderer.ts:2258-2282 slice of src/render/renderer.ts#createSceneRenderer.fillStep (shared): the feature's statements; "…" marks code left out
  const fillStep = (net: Network): void => {
    const left = fillLeft!;
    const { x, y } = rig.viewport.centre;
    const gap = (key: string): number => {
      const r = farCellRect(key);
      return Math.hypot(Math.max(r[0] - x, 0, x - r[2]), Math.max(r[1] - y, 0, y - r[3]));
    };
    const ranked = [...left].map((key) => ({ key, gap: gap(key) })).sort((p, q) => p.gap - q.gap);
    // The cells the view stands over in one step, however many; past them a few, the step's fixed cost shared.
    const near = ranked.filter((c) => c.gap === 0).length;
    const taken = ranked.slice(0, Math.max(near, FILL_CELLS)).map((c) => c.key);
    fillRects = taken.map(farCellRect);
    for (const key of taken) {
      left.delete(key);
      fillTaken.add(key);
      fillAwaiting.add(key);
    }
    // Every build visits the tiles that came into the scope (`roadSurfaces.ts` withinMoved), whichever lands first.
    surfaceReuse.withinMoved?.push(...fillRects);
    try {
      buildWorld(net, true);
    } finally {
      fillRects = null;
    }
  };

// src/render/renderer.ts:2290-2315 slice of src/render/renderer.ts#createSceneRenderer.takeEditedCells (shared): the feature's statements; "…" marks code left out
  const takeEditedCells = (net: Network): void => {
    const left = fillLeft;
    if (left === null || left.size === 0) return;
    // Null: the first read, or a diary that cannot say where (the whole map: left to the fill's own order).
    const edits = fillEditRead.read(net.doc.changes);
    if (!edits) return;
    const pad = SURFACE_READ_REACH;
    const rects: Rect[] = [];
    for (const entry of edits) {
      if (isStreaming(entry)) continue;
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
    if (rects.length) surfaceReuse.withinMoved?.push(...rects);
  };

// src/render/renderer.ts:2317-2394 slice of src/render/renderer.ts#createSceneRenderer.buildWorld (shared): the feature's statements; "…" marks code left out
  const buildWorld = (net: Network, full = false): void => {
    const started = performance.now();
    const opening = openingBuild && !full;
    // A player's edit while the fill is under way: the cells it reached built by this build, before any other (`takeEditedCells`).
    if (!full) takeEditedCells(net);
    // The opening's first world builds only the tiles and the ground round the view; the rest is filled after the curtain.
    const centre = rig.viewport.centre;
    const openingHalf = m(700);
    openingWithin = opening ? (rect) => rect[0] <= centre.x + openingHalf && rect[2] >= centre.x - openingHalf
      && rect[1] <= centre.y + openingHalf && rect[3] >= centre.y - openingHalf : null;
    const openingRegion = opening ? terrainRegion([centre.x - openingHalf, centre.y - openingHalf, centre.x + openingHalf, centre.y + openingHalf]) : null;
    // The opening's solve: the roads round its view and a margin (`OPENING_SOLVE_MARGIN`), the rest solved once the town is shown (`completeSolve`). DO NOT solve a large city's whole network before its first frame: no pixel of the opening reads a road past the margin.
    const reach = openingHalf + OPENING_SOLVE_MARGIN;
    const solveScope: SolveScope | null = opening ? { region: [centre.x - reach, centre.y - reach, centre.x + reach, centre.y + reach] } : null;
    if (opening) solveLeft = { x: centre.x, y: centre.y, order: null, at: 0 };
    // The fill after the reveal (`full`): the solve stands and only the road tiles the opening's scope left out are built, in place - DO NOT rebuild every tile of the city and hold the old and the new surfaces at once (the 3.4 GB peak of the metropolis).
    // Over the whole network's extent: DO NOT take one plate's (`TERRAIN_HALF`) - a metropolis spans several, and every street past the middle plate was left unbuilt (the player, 2026-10-20: "não gera vias").
    const fillBlocks = full ? (fillRects ? fillRects.flatMap((r) => blocksOver(r)) : blocksOver(networkExtent(net))) : null;
    // Only the roads changed (the land did not): the ground is cut and filled again only where the solve of the roads differs.
    // A map just opened (`mapOpened`): the last solve is another city's. DO NOT start from it or compare the two: every block of the map differed, and the whole-map digest held the opening's world 1.7 s on a worker (2026-10-21).
    const newMap = mapOpened && !full;
    const landStill = full || (!newMap && terrainRevision === net.doc.terrainRevision && elevation !== null);
    // Where the land moved (a stroke's region), when it can be said: the solve and the surfaces follow it there only. Kept until the solve
    // runs: cleared before a wait for the roads' stations, the next frame found no region and solved the whole map (TER-01, VIA-01).
    const land = full || newMap ? null : landDirty;
    const landBlocks = full ? fillBlocks : (!landStill && land !== null && land !== 'all' ? blocksOver(land) : null);
    const groundMoved = full ? null : (landBlocks && land !== null && land !== 'all' ? [land] : null);
    const from = landStill || groundMoved ? elevation : null;
    // A new map on level ground (`RELIEF_FLAT`, no stamp): every station's ground is the same, and reading it costs most of a large city's solve. A stroke or a height stamp makes it real ground again.
    const levelLand = isLevelLand(net);
    const groundAt = levelLand ? FLAT_GROUND : builtGroundAt;
    // DO NOT read the ground under a tile's hundreds of new roads in this frame: a job reads it off the shared plates, the solve waits for it.
    // A step of the opening's fill on the network solved whole (`completeSolve`): the solve stands, not made again for each step.
    const fillSolved = full && fillRects !== null && elevation !== null && !elevation.pending && net.state === solvedState;
    const solvedAhead = fillSolved || (offeredElevation !== null && offeredElevation.revision === net.revision && offeredElevation.terrain === net.doc.terrainRevision);
    performance.mark('world:stations-start');
    const stationAsk = roadsRead.of(groundMoved ? null : [net.state, from, ...(solveScope?.region ?? [])], () => roadsToRead(net, groundAt, from, groundMoved, solveScope));
    performance.mark('world:stations-read');
    stationsWait = !solvedAhead && stationsAsked(net, stationAsk);
    performance.mark('world:stations-done');
    if (stationsWait) return;
    landDirty = null;
    const previousElevation = elevation;
    mapOpened = false;
    networkRevision = net.revision;
    terrainRevision = net.doc.terrainRevision;

    // ONE solve for the whole network, shared by every consumer below.
    const offered = offeredElevation;
    offeredElevation = null;
    performance.mark('world:solve');
    elevation = fillSolved && elevation ? elevation : offered && offered.revision === net.revision && offered.terrain === terrainRevision
      ? offered.elevation
      // On land that has not moved, from the last solve
      : buildRoadElevation(net, groundAt, from, roadStations.current, groundMoved, solveScope);
    performance.mark('world:solved');
    // A scoped opening: every road of the map drawn at once at its far level, from the network, the tiles the opening builds in detail cut out (`openingWithin`: every tile meeting the view's square).
    if (opening) {
      const lo = (v: number): number => Math.ceil((v - TILE_ORIGIN) / TILE - 1) * TILE + TILE_ORIGIN;
      const hi = (v: number): number => (Math.floor((v - TILE_ORIGIN) / TILE) + 1) * TILE + TILE_ORIGIN;
      networkFarRead.read(net.doc.changes);
      networkFar.request(net, elevation, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN,
        [lo(centre.x - openingHalf), lo(centre.y - openingHalf), hi(centre.x + openingHalf), hi(centre.y + openingHalf)]);
    }
    solvedState = net.state;
    roadStations.used();
    measureHitch('hitch:road-edit/elevation', started);
    // Now the ground comes to meet the roads
    // The blocks the two solves differ in, digested off the page (`compareSolves`) and read by the job, which waits for them. A fill compares none: its blocks are the surfaces' visit list, not a ground change (`worldSteps` shapes none on level land).
    const compare = full || newMap ? null : (previousElevation && (landStill || landBlocks) ? compareSolves(previousElevation, elevation, landBlocks) : null);
    pendingBlocks = full ? [{ blocks: fillBlocks!, ready: true, ms: 0, fill: true }] : (compare && !pendingBlocks.includes(null) ? [...pendingBlocks, compare] : [null]);
    const elevationMs = performance.now() - started;
    const cause = net.doc.changes.version;
    // DO NOT build it in one go: the roads of a generated city were built in the frame of the change, on the page
    worldJob = worldSteps(net, pendingBlocks, { cause, elevationMs }, started, true, openingRegion, levelLand);
    worldJobFresh = true;
    worldJobGround = true;
  };

// src/render/renderer.ts:2397-2501 slice of src/render/renderer.ts#createSceneRenderer.worldSteps (shared): the feature's statements; "…" marks code left out
  function* worldSteps(net: Network, compares: readonly (BlockCompare | null)[], solved: { readonly cause: number; readonly elevationMs: number },
    started: number, sliced: boolean, region: TerrainRegion | null, levelLand: boolean): Generator<'wait' | void, void, void> {
    // … 11 lines
    worldCause = derived(net, 'elevation', changed, 'road heights solved', solved.cause,
      { ms: solved.elevationMs + (own?.ms ?? 0), detail: changed ? `${changed.length} blocks of ${SHAPE_BLOCK} u` : 'whole map' }) || solved.cause;
    // … 4 lines
    if (levelLand) {
      // Level ground (a generated city's new map): nothing to cut or fill, and its buildings were grown on this ground. DO NOT shape 29 thousand pads and follow every floor for a land that is already level (12.5 s of the opening).
      gradedFor = net.doc.buildings.revision;
    } else if (blocks === null) {
      const shaping = shapeGround(net, region);
      while (!shaping.landed) { landShapes(); if (shaping.landed) break; yield 'wait'; }
    } else if (blocks.length || gradedFor !== net.doc.buildings.revision) yield* shapeBlocksSteps(net, blocks);
    // … 14 lines
    const freshRoads = yield* roadSurfaceSteps(net, solve, materials, terrain.renderedHeightAt, surfaceReuse, terrain.vergeMaterial,
      sliced ? roadTiles : null, surfaceBlocks);
    // … 2 lines
    derived(net, 'surfaces', freshRoads.rebuilt, 'road surfaces rebuilt', worldCause,
      { ms: freshRoads.workMs, detail: `${freshRoads.built} tiles built, ${freshRoads.reused} reused` });
    // … 2 lines
    const freshDetails = (details ??= createStructureCells(materials, terrain.vergeMaterial)).prepare(net, solve, terrain.renderedHeightAt, blocks);
    // … 3 lines
    const utilityBuilt = net.doc.utilityRevision;
    // … 3 lines
    if (sliced) {
      // … 1 lines
      for (const piece of [freshRoads.group, ...freshDetails.fresh]) {
        if (piece.parent === null) stage.add(piece);
      }
    // … 5 lines
    }
    // … 3 lines
    if (roads && roads !== freshRoads) { triangles -= roads.triangles; world.remove(roads.group); roads.dispose(); }
    triangles -= details?.triangles ?? 0;
    for (const mesh of surfaceReuse.retired?.splice(0) ?? []) disposeMesh(mesh);
    roads = freshRoads;
    // … 2 lines
    const roadEdits = networkFarRead.read(net.doc.changes);
    if (roadEdits === null || roadEdits.some((entry) => !isStreaming(entry))) networkFar.outdate();
    // … 1 lines
    for (const key of fillLanding) { networkFar.hide(key); fillAwaiting.delete(key); }
    // … 1 lines
    if (fillLeft !== null && fillLeft.size === 0 && fillAwaiting.size === 0) {
      fillLeft = null;
      openingWithin = null;
      networkFar.settle(farShown());
      requestNetworkFar(net, solve);
    } else if (fillLeft === null && !openingBuild && openingWithin === null && !networkFar.settled) networkFar.settle(farShown());
    // … 1 lines
    roads.adopt();
    world.add(roads.group);
    freshDetails.apply();
    // … 1 lines
    utilityRevision = utilityBuilt;
    builtTriangles = triangles + roads.triangles + (details?.triangles ?? 0);
    // … 2 lines
    syncUtilities(net, poleGroundAt(solve, terrain.renderedHeightAt, net));
  // … 7 lines
  }

// src/render/renderer.ts:2506-2544 slice of src/render/renderer.ts#createSceneRenderer.warmRoadShaders (shared): the feature's statements; "…" marks code left out
  const warmRoadShaders = (): void => {
    // … 2 lines
    try {
      // … 9 lines
      const warmNet = new Network(doc);
      warmNet.rebuild();
      // … 1 lines
      const warmElevation = buildRoadElevation(warmNet, flat);
      // … 1 lines
      const parts = [
        buildRoadSurfaces(warmNet, warmElevation, materials, flat, undefined, terrain.vergeMaterial),
        buildStructureDetails(warmNet, warmElevation, flat, materials),
        buildUtilities(warmNet, poleGroundAt(warmElevation, flat), sceneryKit),
      ];
      for (const part of parts) group.add(part.group);
      // … 2 lines
      for (const mesh of [...furniture.warm(), ...signs.warm()]) {
        group.add(mesh);
        const standIn = shadowStandIn(mesh);
        if (standIn) placedShadows.add(standIn);
      }
      // … 1 lines
      void compileAhead(group).then(() => {
        for (const part of parts) part.dispose();
      });
    } catch (error) {
    // … 1 lines
    }
  };

// src/render/renderer.ts:2642-2657 slice of src/render/renderer.ts#createSceneRenderer.switchedOff (shared): the feature's statements; "…" marks code left out
  const switchedOff = (on: EffectSwitches): (Object3D | null | undefined)[] => [
    ...(on.terrain ? [] : terrain.meshes),
    ...(on.water || !on.terrain ? [] : waterMeshes()),
    ...(on.roads ? [] : [roads?.group, details?.group, roadStandIn.group, networkFar.group]),
    ...(on.buildings ? [] : [buildings.group]),
    ...(on.vegetation ? [] : [...[coverCells, natureCards, natureForest, paintedForest, plantedForest].map((plants) => plants?.group), ...(gardens?.meshes ?? [])]),
    ...(on.furniture ? [] : [furniture.group, furniture.grass]),
    ...(on.props ? [] : [signs.group, barriers.group, utilities?.group, elementLayer?.group]),
    ...(on.transit ? [] : [transit.group]),
    ...(on.people ? [] : [agentParts]),
    ...(on.farTraffic ? [] : [densityCars.object]),
    ...(on.signals ? [] : [signals.group]),
    ...(on.particles ? [] : [exhaust.points]),
    ...(on.weather ? [] : [rain.object, lightning.group]),
    ...(on.pointLights ? [] : [fxLights.muzzle, fxLights.blast, ...roomLights]),
  ];

// src/render/renderer.ts:2871-2881 slice of src/render/renderer.ts#createSceneRenderer.natureOpen (shared): the feature's statements; "…" marks code left out
  const natureOpen = (net: Network, x: number, y: number, z: number): boolean => {
    // Off its plate's edge strip (the plate's centre worked out, no object a candidate).
    const cx = Math.floor(x / PLATE_SIZE + 0.5) * PLATE_SIZE, cy = Math.floor(y / PLATE_SIZE + 0.5) * PLATE_SIZE;
    if (Math.abs(x - cx) > PLATE_SIZE / 2 - m(2) || Math.abs(y - cy) > PLATE_SIZE / 2 - m(2)) return false;
    const level = terrain.shoreLevelAt(x, y);
    if (level !== null && level > z - m(0.3)) return false;
    const g = m(2);
    const grade = Math.hypot(terrain.renderedHeightAt(x + g, y) - terrain.renderedHeightAt(x - g, y), terrain.renderedHeightAt(x, y + g) - terrain.renderedHeightAt(x, y - g)) / (2 * g);
    if (grade > 0.75) return false;
    return !onRoadGround(net, { x, y }) && !buildings.covers(x, y);
  };

// src/render/renderer.ts:2885-2889 slice of src/render/renderer.ts#createSceneRenderer.natureStands (shared): the feature's statements; "…" marks code left out
  const natureStands = (c: Candidate): number | null => {
    // … 2 lines
    return natureOpen(net, c.x, c.y, at) && !clearingsOf(net.doc)(c.x, c.y) ? at : null;
  };

// src/render/renderer.ts:2921-2921 slice of src/render/renderer.ts#createSceneRenderer.coverFree (shared): the feature's statements; "…" marks code left out
  const coverFree = (x: number, y: number): boolean => !onRoadGround(keepNet!, { x, y }) && !buildings.covers(x, y);
