// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:199-217 slice of src/render/renderer.ts#RenderStats (shared): the feature's statements; "…" marks code left out
interface RenderStats {
  // … 6 lines
  readonly quality: QualityLevel;
// … 10 lines
}

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 20 lines
  setGeometryBudget(budget: GeometryBudget | null): void;
  // … 12 lines
  readonly viewport: Viewport;
  // … 8 lines
  readonly occlusion: { enabled: boolean; readonly stats: Readonly<OcclusionStats & { objectsHidden: number; active: boolean }> };
  // … 112 lines
  setQuality(level: QualityLevel | 'auto', settings?: QualitySettings): void;
  readonly quality: QualityLevel | 'auto';
  // … 1 lines
  setEffects(switches: EffectSwitches, buildings?: BuildingChoices): void;
  // … 1 lines
  setOptimizations(values: Optimizations): void;
// … 7 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
  // … 3 lines
  initialQuality: QualityLevel | 'auto' = 'auto',
  // … 4 lines
  initialSettings?: QualitySettings,
// … 2 lines
): SceneHandle {
  // … 15 lines
  const canvasForChain = (initialSettings ?? QUALITY[initialQuality === 'auto' ? 'high' : initialQuality]).postProcessing;
  // … 21 lines
  const renderer = makeRenderer({
    // … 2 lines
    antialias: !canvasForChain,
    depth: !canvasForChain,
  // … 5 lines
  });
  // … 366 lines
  let requested: QualityLevel | 'auto' = initialQuality;
  // … 1 lines
  let tier: QualityLevel = requested === 'auto' ? 'high' : requested;
  let quality: QualitySettings = initialSettings ?? QUALITY[tier];
  // … 2 lines
  const anisotropy = Math.min(quality.anisotropy, maxAnisotropy);
  // … 62 lines
  const rig = createIsoRig(initialCentre, initialHeight / Math.max(0.001, initialZoom * 2));
  // … 1 lines
  const environment = createEnvironment(scene, renderer, {
    shadows: quality.shadows,
    shadowMapSize: quality.shadowMapSize,
  });
  const materials: SceneMaterials = createMaterials(anisotropy);
  // … 3 lines
  applyWear(materials.asphalt, wear, 0, 'asphalt');
  applyWear(materials.footway, wear, 1, 'footway');
  materials.setDetail(quality.surfaceDetail);
  // … 3 lines
  const terrain: TerrainSurface = createFlatAtlas(anisotropy);
  // … 1 lines
  terrain.onLevels = () => { landTopFor = -1; };
  // … 3 lines
  rig.setGround((x, y) => terrain.renderedHeightAt(x, y));
  // … 2 lines
  scene.add(...terrain.meshes);
  // … 11 lines
  const roadPrograms = roadProgramStandIns(materials, terrain.vergeMaterial, (color) => roadPaint.get(color) ?? roadPaint.get(LANE_LINE) ?? materials.asphalt);
  world.add(roadPrograms.group);
  // … 119 lines
  const surfaceReuse: SurfaceReuse = {
    // … 6 lines
    dependsOn: (minX, minY, maxX, maxY) => new Digest()
      .add(elevation?.digest(minX, minY, maxX, maxY, SURFACE_READ_REACH) ?? 0)
      .add(terrain.digest(minX, minY, maxX, maxY))
      .value(),
    // … 4 lines
    within: (rect) => openingWithin === null || openingWithin === undefined || openingWithin(rect),
  // … 27 lines
  };
  // … 23 lines
  const reveal = new RevealGate();
  // … 145 lines
  const agents: AgentMeshes = createAgentMeshes(deckHeight, onAssetsReady,
    // A walker off the streets stands on what is drawn there
    (x, y) => {
      const lot = buildings.anyLotHeightAt(x, y);
      // On a road's footway or carriageway, its paving; the terrain elsewhere.
      const paved = pavedHeightAt(x, y);
      const ground = Number.isFinite(paved) ? paved : terrain.renderedHeightAt(x, y);
      return Number.isFinite(lot) ? Math.max(lot, ground) : ground;
    },
    // A resident's parked car stands on its lot as the lot is drawn.
    (building, x, y) => buildings.lotHeightAt(building as BuildingId, x, y),
    // The land alone: whether a walker off the streets is on screen at all, before the yards are asked.
    (x, y) => terrain.renderedHeightAt(x, y));
  // … 1 lines
  agents.keepPeople();
  // … 64 lines
  agentParts.add(...agents.meshes);
  // … 6 lines
  environment.setMovers([
    // … 1 lines
    { roots: agents.meshes, moving: () => simRunning && agents.casting, maxCascade: 1 },
    { roots: [transit.group], moving: () => simRunning && transit.moving },
  // … 1 lines
  ]);
  for (const root of [...agents.meshes, transit.group]) moverRoots.add(root);
  // … 5 lines
  const buildings = createBuildingLayer(anisotropy);
  // … 1 lines
  buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
  // … 3 lines
  let geometryBudget: GeometryBudget | null = null;
  // … 38 lines
  holdStill(buildings.group);
  scene.add(buildings.group);
  // … 3 lines
  environment.setCasters((texel, cascade, half) => {
    // … 1 lines
    buildingSwitcher.casters(effectSwitches, texel);
    // … 2 lines
    buildings.castInto(texel === null ? null : texel * optimizer.castTexel, cascade >= optimizer.coarseFrom, cascade > 0 ? half : 0);
  // … 1 lines
  });
  // … 5 lines
  const occlusion = new OcclusionBuffer();
  const occlusionProbe = { enabled: true, stats: { ...occlusion.stats, objectsHidden: 0, active: false } };
  // … 14 lines
  {
    // … 1 lines
    renderer.shadowMap.render = (lights, shadowScene, camera) => {
      if (!occludedNow) { cascadesRender(lights, shadowScene, camera); return; }
      buildings.revealOccluded(true);
      try {
      // … 1 lines
      } finally {
        buildings.revealOccluded(false);
      }
    };
  }
  // … 1 lines
  renderer.domElement.addEventListener('webglcontextrestored', () => {
    // … 3 lines
    buildings.refill();
  // … 2 lines
  });
  // … 51 lines
  void loadElementKit(anisotropy).then((kit) => { elementKit = kit; }, (error: unknown) => {
    console.warn('[elements] models not loaded', error);
  });
  // … 538 lines
  engine.register({
    // … 1 lines
    refresh: (): number => {
      // … 3 lines
      const work = furniture.sync(placedOn, elevation, terrain.renderedHeightAt, cityMinutes);
      // … 2 lines
      return work;
    },
  }, 'draw');
  // … 10 lines
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 3 lines
      const work = barriers.sync(placedOn.doc, terrain.renderedHeightAt);
      // … 1 lines
      return work;
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 5 lines
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
  // … 484 lines
  let effectSwitches: EffectSwitches = ALL_EFFECTS_ON;
  // … 1 lines
  const frameHider = createFrameHider();
  let buildingChoices: BuildingChoices = DEFAULT_BUILDING_CHOICES;
  const buildingSwitcher = createBuildingSwitcher(buildings);
  // … 1 lines
  const optimizer = createOptimizer({ renderer, scene, environment, waterMeshes: () => waterMeshes() });
  let baseQuality: QualitySettings = quality;
  // … 41 lines
  let post: PostChain = createPostChain(renderer, scene, rig.camera, quality, tier);
  // … 10 lines
  renderer.setPixelRatio(Math.min(WEAK_MACHINE.pixelRatio ?? (window.devicePixelRatio || 1), quality.pixelRatio));
  // … 2 lines
  const sizeScreen = screenSizer(renderer, () => Math.min(WEAK_MACHINE.pixelRatio ?? (window.devicePixelRatio || 1), quality.pixelRatio),
    (width, height, ratio, wholePixels) => {
      rig.resize(width, height);
      post.setSize(width, height, ratio);
      drawingSize.width = width; drawingSize.height = height; drawingSize.ratio = ratio;
      gpuView?.setSize(width, height, ratio);
      // The GPU's cap from the device and the pixels at the tier's whole ratio, not the dynamic resolution's share: the buildings' detail does not follow it. And the frame's targets in the ledger (`gpuLedger.ts`).
      gpuLedger.cap = deviceGpuCap(readGpuDevice(renderer.getContext(), wholePixels));
      targetsTold = -1;
      tellTargets();
    });
  // … 223 lines
  const coverKeeps = {
    // … 1 lines
    forest: new CellKeeps<Candidate>((c) => (coverFree(c.x, c.y) && !clearingsOf(keepNet!.doc)(c.x, c.y) ? terrain.renderedHeightAt(c.x, c.y) : null)),
    // … 3 lines
    shore: new CellKeeps<ShoreCandidate>((c) => {
      const ground = terrain.renderedHeightAt(c.x, c.y);
      const above = ground - c.level;
      const odds = above < -m(2.5) ? 0.06 : above < -m(0.3) ? 0.3 : above < m(0.6) ? 0.45 : above < m(2) ? 0.22 : above < m(4) ? 0.07 : 0;
      return c.draw <= odds && coverFree(c.x, c.y) ? ground : null;
    }),
  };
  // … 5 lines
  const plantedKeeps = new CellKeeps<PlantedTree>((t) => (onCarriageway(keepNet!, t) || buildings.covers(t.x, t.y) ? null : terrain.renderedHeightAt(t.x, t.y)));
  // … 25 lines
  const coverField: CoverField = {
    forestAt: (x, y) => terrain.forestAt(x, y),
    coverAt: (kind, x, y) => terrain.coverAt(kind, x, y),
    shoreLevelAt: (x, y) => terrain.shoreLevelAt(x, y),
  };
  // … 62 lines
  const handle: SceneHandle = {
    // … 2 lines
    flashGrid(cells) {
      // … 3 lines
      for (const c of cells) {
        // … 1 lines
        for (let i = 0; i < sub; i++) for (let j = 0; j < sub; j++) {
          // … 1 lines
          const v = (x: number, y: number): void => { pos.push(x, terrain.renderedHeightAt(x, y) + lift, -y); ring.push(c.ring); };
        // … 1 lines
        }
      }
    // … 1 lines
    },
    // … 17 lines
    meshProbe: () => agents.meshProbe(),
    animProbe: (id) => agents.animProbe(id),
    forceClip: (id, clip) => agents.forceClip(id, clip as never),
    driverBody: (vehicle, x, y) => agents.driverBody(vehicle, x, y),
    // … 5 lines
    census: () => agents.census(),
    // … 1 lines
    async compare(options) {
      // … 3 lines
      return view.compare(renderer, scene, rig.camera, options);
    },
    // … 2 lines
    viewport: rig.viewport,
    // … 3 lines
    get stats(): RenderStats {
      return {
        builtTriangles: builtTriangles + buildings.triangles,
        // … 2 lines
        quality: tier,
        // … 4 lines
        terrainMs: Math.round(terrainMs),
      };
    },
    terrainHeightAt(x, y) {
      return terrain.renderedHeightAt(x, y);
    },
    landTop() {
      if (landTopFor !== landVersion) {
        landTopFor = landVersion;
        // Kept by the atlas as its plates' ground moves
        let top = terrain.highest?.() ?? -Infinity;
        const corners = Math.round((TERRAIN_HALF * 2) / TERRAIN_CELL);
        if (!terrain.highest) for (const part of terrain.parts) {
          for (let j = 0; j <= corners; j++) {
            for (let i = 0; i <= corners; i++) top = Math.max(top, part.surface.renderedHeightAt(-TERRAIN_HALF + i * TERRAIN_CELL, TERRAIN_HALF - j * TERRAIN_CELL));
          }
        }
        landTopValue = top;
      }
      return landTopValue;
    },
    // … 6 lines
    get opening() {
      return !reveal.opened;
    },
    // … 14 lines
    setGeometryBudget(budget) {
      // … 2 lines
      if (buildingChoices.detailMemoryMB < 0) buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
    },
    // … 1 lines
    viewCamera: () => rig.camera,
    buildingLevels() {
      return buildings.levels();
    },
    buildingCensus() {
      return buildings.census();
    },
    openingState() {
      return { roadsIn: roads !== null && mapRoadsIn, coarsePending: buildings.coarsePending, viewPending: buildings.viewPending, treesStarted: natureTreesStarted,
        treesPending: natureTreesPending, programs: openingPrograms, worked: reveal.worked, worldBusy: worldJob !== null, fillPending: openingFillPending, fillCellsLeft: fillLeft?.size ?? 0 };
    },
    beginLoad() {
      reveal.begin();
      // … 5 lines
      openingFillPending = openingBuild;
    // … 9 lines
    },
    // … 10 lines
    surfaceHeightAt(x, y) {
      const ground = terrain.renderedHeightAt(x, y);
      if (!elevation) return ground;
      let top = ground;
      // … 1 lines
      for (const set of SURFACE_SETS) {
        // … 3 lines
        top = Math.max(top, elevation.at(x, y, set));
      }
      return top;
    },
    // … 1 lines
    setBuildingPreview(preview) {
      buildings.setPreview(preview);
    },
    // … 1 lines
    strikeGround(x, y, strength) {
      // … 5 lines
      exhaust.burst(x, y, terrain.renderedHeightAt(x, y), 20 + strength * 6, 1, m(1 + strength * 0.4), m(3), 5);
    },
    // … 13 lines
    busy: () => (fx?.busy() ?? false) || natureTreesPending || cameraEasing || (terrain.waiting?.() ?? false),
    targetBytes: () => renderTargetsBytes([...post.targets(), ...environment.shadowMaps()]),
    buildingDetailBytes: () => buildings.residentBytes,
    drifting: () => ((fogMoving || placedCloudsShown) && post.enabled && (quality.cloudShadows || quality.skyClouds)) || weatherActive,
    // … 5 lines
    setTransitPreview(preview) {
      // … 8 lines
      transitPreview.set(preview?.points ?? [], preview?.mode ?? 'train', terrain.renderedHeightAt, pavedHeightAt);
    },
    // … 4 lines
    setPolePreview(net, preview) {
      // … 5 lines
      if (polePreview) {
        world.remove(polePreview.group);
        polePreview.dispose();
        polePreview = null;
      }
      // … 1 lines
      polePreview = buildPolePreview(net, poleGroundAt(elevation, terrain.renderedHeightAt, net), sceneryKit, preview);
      world.add(polePreview.group);
    },
    setPerspective(on) {
      if (on === rig.perspective) return;
      rig.setPerspective(on);
      // … 1 lines
      post.dispose();
      post = createPostChain(renderer, scene, rig.camera, quality, tier);
      post.setNight(Math.max(0, lastDark));
    // … 2 lines
    },
    // … 7 lines
    setBuildingCutaway(spec) {
      buildings.setCutaway(spec);
    // … 1 lines
    },
    setBuildingsDimmed(except) {
      buildings.setDimmed(except === undefined ? undefined : (except as BuildingId | null));
    },
    elevationAt(x, y, structure) {
      if (!elevation) return terrain.renderedHeightAt(x, y);
    // … 1 lines
    },
    // … 2 lines
    draw(net, sim, alpha, delta, options) {
      // … 11 lines
      cameraEasing = rig.settle(wallDt);
      // … 9 lines
      const groundMoved = terrain.update(net.doc, stroking);
      // … 2 lines
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
        // … 5 lines
        terrain.settle();
      }
      // … 29 lines
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
      // … 12 lines
      gardens.source(net.doc.buildings, terrain.renderedHeightAt);
      // … 3 lines
      if (!stroking && !worldJob) {
        // … 9 lines
        {
          // … 8 lines
          shorePlates.follow(terrain.parts);
        // … 1 lines
        }
        // … 1 lines
        const standIns = !natureTreeKit && !natureTreesPending;
        let coverWhole = vegetationRedraw || coverStandIns !== standIns;
        // … 3 lines
        const geology = terrain.geologyChanges.version;
        if (geology !== coverGeologySeen) {
          const painted = coverGeologySeen < 0 ? null : terrain.geologyChanges.rectsSince(coverGeologySeen);
          if (painted === null) coverWhole = true;
          else for (const r of painted) keepCellsOver(r, TERRAIN_CELL, changed);
          coverGeologySeen = geology;
        }
        {
          // … 7 lines
          if (coverWhole || changed.size) {
            // DO NOT gather every cell of the map unless every cell is drawn again.
            coverCellsOf().sync(coverWhole ? null : changed, coverWhole ? allCoverCells() : []);
            paintedForestOf()?.sync(coverWhole ? null : changed, coverWhole ? coverKeeps.forest.cellKeys() : []);
          }
          coverStandIns = standIns;
        // … 1 lines
        }
        // … 2 lines
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
        // … 1 lines
        if (natureTreeKit) {
          // … 2 lines
          if (plantedWhole) plantAll(net.doc);
          else if (planted.length) {
            // … 2 lines
            for (const key of cells) plantedKeeps.fill(key, plantedIn(net.doc, key), (a, b) => a === b);
          }
          // … 1 lines
          if (rects === null) { if (!plantedWhole) plantedKeeps.refreshAll(); } else if (rects.length) plantedKeeps.refresh(rects, KEEP_REACH);
          const cells = plantedKeeps.takeChangedCells();
          if (cells === null || cells.size || vegetationRedraw) {
            const forest = plantedForestOf()!;
            const whole = cells === null || vegetationRedraw;
            forest.sync(whole ? null : cells, whole ? plantedKeeps.cellKeys() : []);
          }
        }
      // … 1 lines
      }
      // … 5 lines
      const solidsNeeded = rig.perspective && rig.viewport.elevation < 1.2;
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
      const detailed = rig.viewport.zoom >= quality.detailCutoffZoom;
      const plantMap = rig.viewport.zoom < PLANT_MAP_ZOOM;
      // … 1 lines
      {
        const eye = rig.camera.position;
        // … 1 lines
        if (!rig.perspective) {
          const halfH = css.h / Math.max(1e-3, 2 * rig.viewport.zoom);
          k = halfH / Math.tan((PERSPECTIVE_FOV * Math.PI) / 360) / Math.max(1e-3, eye.distanceTo(rig.target));
        }
        const t = rig.target;
        const ex = t.x + (eye.x - t.x) * k, ey = -(t.z + (eye.z - t.z) * k), ez = t.y + (eye.y - t.y) * k;
        // … 1 lines
        const pxAtMetre = css.h / (2 * Math.tan(((rig.perspective ? (rig.camera as PerspectiveCamera).fov : PERSPECTIVE_FOV) * Math.PI) / 360));
        // … 1 lines
        let reach = rig.perspective ? (rig.camera as PerspectiveCamera).far : Infinity;
        if (!rig.perspective) {
          const halfH = css.h / Math.max(1e-3, 2 * rig.viewport.zoom);
          const halfW = (halfH * css.w) / Math.max(1, css.h);
          reach = Math.hypot(halfW, halfH / Math.max(0.3, Math.sin(rig.viewport.elevation))) * 1.25 + m(120);
        }
        rig.camera.updateMatrixWorld();
        detailProjection.multiplyMatrices(rig.camera.projectionMatrix, rig.camera.matrixWorldInverse);
        detailFrustum.setFromProjectionMatrix(detailProjection, rig.camera.coordinateSystem, rig.camera.reversedDepth);
        // … 3 lines
        buildings.frame.budget = buildingChoices.triangles === 0 ? Infinity : VIEW_BUDGETS[viewOf(rig.viewport.zoom)].triangles * buildingChoices.triangles * quality.triangleScale;
        buildings.levelOfDetail(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, reach, detailFrustum);
        // … 8 lines
        buildings.cullDetails(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, texel * optimizer.castTexel);
      }
      // … 4 lines
      for (const mesh of furniture.meshes) {
        mesh.visible = quality.detailProps && detailed && (!plantMap || !isLeafCards(mesh));
      }
      for (const mesh of gardens?.meshes ?? []) {
        mesh.visible = detailed && (!plantMap || !isLeafCards(mesh));
      }
      // … 1 lines
      if (plantMap !== plantMapFor.map || gardens !== plantMapFor.gardens) {
        plantMapFor = { map: plantMap, gardens };
        gardens?.setMap(plantMap);
        furniture.setMap(plantMap);
      }
      // … 38 lines
      const cullCamera = rig.camera;
      crowdProjection.multiplyMatrices(cullCamera.projectionMatrix, cullCamera.matrixWorldInverse);
      crowdFrustum.setFromProjectionMatrix(crowdProjection, cullCamera.coordinateSystem, cullCamera.reversedDepth);
      // … 9 lines
      cameraRight.setFromMatrixColumn(cullCamera.matrixWorld, 0).normalize();
      // … 7 lines
      {
        const view = propView(cullCamera, crowdFrustum, screenHalf(), rig.target.y);
        const props = [...(gardens && detailed ? [gardens.layer] : []), ...(quality.detailProps && detailed ? [furniture.layer] : [])];
        if (props.length) lookProps(props, view);
        utilities?.look(view, performance.now() + POLE_FRAME_MS);
      }
      // … 4 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.cull(crowdFrustum, crowdProjection);
      // … 2 lines
      transit.update(sim.trains, terrain.renderedHeightAt);
      // … 1 lines
      agents.sync(sim, net, alpha, detailed, rig.viewport.zoom, {
        pedestrianDetail: quality.pedestrianDetail,
        pedestrianVisible,
        personPixels,
        screenScale,
        pixelRatio: renderer.getPixelRatio(),
        shadows: renderer.shadowMap.enabled && optimizer.agentShadows,
        eye: cullCamera.position,
        vehicleVisible,
        vehicleInside,
        vehicleBox,
        occupantZoom: quality.occupantZoom,
        // The tier's lighter vehicles and people (`quality.ts`; only the low tier draws them lighter).
        tier: quality,
        exhaust: (x, y, z, angle, length, speed, dusty) => {
          if (optimizer.exhaust) exhaust.emit(x, y, z, angle, length, speed, dusty);
          if (optimizer.wear && !dusty && Math.abs(speed) > 0.5) wear.wheels(x, y, angle, Math.min(length * 0.42, m(1.7)), wallDt);
        },
        // The bodies of shots and blows (`playEffects.ts`), once anybody has shot or struck.
        ...(fx ? { ragdolls: (citizens: Parameters<PlayEffects['frame']>[0]) => fx!.frame(citizens, sim, wallDt, fxDt) } : {}),
        hiddenPed: (id) => fx?.hides(id) ?? false,
      });
      // … 1 lines
      densityCars.update(net, sim, rig.target.x, -rig.target.z);
      if (optimizer.exhaust) exhaust.tick(windClock, renderer.domElement.height / 2);
      // … 1 lines
      if (optimizer.wear) {
        for (const ped of sim.pedViews) if (ped.v > 0.05) wear.feet(ped.x, ped.y, wallDt);
        wear.follow(rig.viewport.centre.x, rig.viewport.centre.y);
        wear.tick(wallDt);
      }
      // … 13 lines
      signals.sync(sim.signals, net, detailed, crowdFrustum);
      // … 1 lines
      target.copy(rig.target);
      const halfHeight = css.h / Math.max(0.001, rig.viewport.zoom * 2);
      const halfWidth = (halfHeight * css.w) / Math.max(1, css.h);
      // … 1 lines
      const reach = rig.perspective ? 1.6 : 1;
      const groundHalfDepth = (reach * halfHeight) / Math.max(0.3, Math.sin(rig.viewport.elevation));
      // … 1 lines
      const tallestTop = Math.max(0, buildings.top);
      // … 3 lines
      {
        // … 6 lines
        post.setCloudDrift(cloudDrift.x, cloudDrift.y);
        // … 4 lines
        terrain.setWaterLook({
          waves: Math.min(1, weather.waves + weather.wind / 40),
          foam: weather.foam,
          current: weather.current * m(1),
          windX: windLength > 1e-6 ? wind.x / windLength : Math.cos(angle),
          windZ: windLength > 1e-6 ? -wind.y / windLength : -Math.sin(angle),
          windSpeed: windLength,
        });
        const span = 2 * Math.max(halfWidth, halfHeight);
        if (weather.lightning > 0 && Math.random() < (step * weather.lightning) / 60) {
          const a = Math.random() * Math.PI * 2, r = Math.sqrt(Math.random()) * span * 0.45;
          strikeAt(target.x + Math.cos(a) * r, -target.z + Math.sin(a) * r);
        }
        // … 5 lines
        rain.update(weather.rain, target, span, rainWind, step);
      // … 2 lines
      }
      // … 2 lines
      post.setDirectShare(environment.directShare());
      if (Math.abs(dark - lastDark) > 0.01) {
        // … 1 lines
        post.setNight(dark);
        // … 1 lines
        agents.setNight(dark);
      }
      // … 1 lines
      buildings.setNight(effectSwitches.bNightWindows ? lastDark : 0, clock / 60);
      // … 1 lines
      environment.follow(rig.camera, target, tallestTop, builtGroundAt);
      // … 1 lines
      {
        // … 2 lines
        if (rig.perspective) {
          // The cone on the plane, as the simulation sees it.
          const coneDirection = rig.camera.getWorldDirection(flatDirection);
          const flat = Math.hypot(coneDirection.x, coneDirection.z);
          if (flat > 0.05) {
            const aspect = css.w / Math.max(1, css.h);
            const fov = rig.camera instanceof PerspectiveCamera ? rig.camera.fov : PERSPECTIVE_FOV;
            const halfAcross = Math.atan(Math.tan((fov * Math.PI) / 360) * aspect);
            view = {
              ex: rig.camera.position.x, ey: -rig.camera.position.z,
              dx: coneDirection.x / flat, dy: -coneDirection.z / flat,
              cos: Math.cos(Math.min(Math.PI, halfAcross + VIEW_MARGIN)), far: VIEW_SEEN,
            };
          }
        }
        sim.focus = {
          x: target.x,
          y: -target.z,
          r: Math.hypot(halfWidth, groundHalfDepth) + m(20),
          detail: optimizer.focusDetail && rig.viewport.zoom * m(1.7) >= 10,
          view,
        };
      }
      // … 3 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // The two-wheelers changed between casting and a soft disc: their shadows come or go now.
        if (agents.castersChanged) shadowMoved = true;
        // Moving only when something that moves is in the pass (`movingCasters`) or the wind bends the trees
        simRunning = !sim.paused;
        const moving = net.doc.weather.wind > 0 || (simRunning && movingCasters());
        if (castersMoved && !moving) shadowMoved = true;
        castersMoved = moving;
        // Each cascade drawn when what it holds can have changed (`sunCascades.ts` `pass`)
        // The still casters as they stood at the last pass: the same signature (`casterSignature`: what casts, where, with what it draws) and nothing landed it cannot see (`castersStirred`).
        // DO NOT take a view that moved for casters that changed: the camera turning redrew every cascade whole at nearly every frame
        // (the probe, 2026-10-19: 309 of 533 passes in the district turning, 369 of 592 in the street), the stores thrown away for a view
        // whose still casters were the same. What the layers cull by the view shows in what they draw.
        // DO NOT skip the signature while the view stands still: the layers change what they draw with no view moved and nothing added (a level
        // swapped a frame after its data came, the poles' levels by the shadow's texel, `poleLayer.ts`); the street view 12 s after opening, still,
        // had another signature than the one last taken (2026-10-20). It costs 0.15-0.2 ms a pass in the large city.
        // DO NOT take the buildings' error scale moving for changed casters either (`frameBudget.ts`): it follows the frame's time and wanders under load
        // while a level changes only past a threshold, which the signature sees; every cascade was drawn whole at every frame for ~20 s after each move.
        // What changed among the still casters, and the world box holding it: a cascade's store is drawn again only there (`sunCascades.ts` patches) - DO NOT throw the stores away for a level swapped or a run come or gone as the view moves: cascades 0 and 1 were drawn whole in 121 of 121 frames of a street pan (shadow stats, 2026-10-22).
        const changes = casterChanges();
        const still = !changes.any;
        // Where the view stands, a cascade drawn whole is kept for the frames after (`ShadowPassInput.keep`).
        // DO NOT keep one behind the curtain: the town is hidden there (`buildings.hidden`), which no caster's signature says, and a cascade drawn then without the buildings was kept past the reveal - a reopened city, its cells in from the store before the curtain fell, had no building's shadow at all, its streets lit bright (2026-10-21).
        const viewStill = reveal.opened && shadowView.equals(rig.camera.matrixWorld) && shadowProjection.equals(rig.camera.projectionMatrix);
        shadowView.copy(rig.camera.matrixWorld);
        shadowProjection.copy(rig.camera.projectionMatrix);
        const patch = changes.any ? changes.boxes : undefined;
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
          renderer.shadowMap.needsUpdate = true;
        }
        shadowMoved = false;
      }
      // … 2 lines
      if (shake > 0) {
        // … 1 lines
        rig.camera.position.add(shakeOffset);
        rig.camera.updateMatrixWorld();
      }
      // … 1 lines
      terrain.setSun(sunTowards.copy(environment.sun.position).sub(environment.sun.target.position));
      {
        // … 1 lines
        const centre = rig.viewport.centre;
        const cam = rig.camera.position;
        const dx = cam.x - centre.x, dz = cam.z + centre.y;
        const across = Math.hypot(dx, dz);
        const pull = across > 1 ? Math.min(150, across * 0.3) / across : 0;
        reliefFocus.x = centre.x + dx * pull;
        reliefFocus.z = -centre.y + dz * pull;
        // … 1 lines
        const halfHeight = css.h / Math.max(1e-3, 2 * rig.viewport.zoom);
        const close = halfHeight < 900;
        if (net.doc.gullyRevision !== gulliesFor) {
          // … 1 lines
          terrain.setGullies(net.doc.gullyDabs, net.doc.gullyAuto);
        }
        // … 1 lines
        terrain.focus?.(centre.x, centre.y, halfHeight);
        // … 1 lines
        terrain.lookFrom?.(rig.camera, renderer.domElement.height * optimizer.terrainPixels);
        // … 1 lines
        roadFocus.x = centre.x; roadFocus.y = centre.y;
        // … 1 lines
        roadFocus.reach = Math.min(900, Math.max(320, halfHeight * 1.8)) / buildings.frame.scale;
        if (roadFocus.frames++ % 6 === 0) {
          // … 1 lines
          roadResidency.networkFrom = !rig.perspective && rig.viewport.zoom < 1 ? 0 : undefined;
          // … 4 lines
          if (roadResidency.wantsNetwork && elevation && worldJob === null && !openingBuild && fillLeft === null && !networkFar.pending && networkFar.outdated()) {
            networkFar.settle(farShown());
            requestNetworkFar(net, elevation);
          }
        }
        if (optimizer.reliefDue()) terrain.bakeRelief(renderer, close ? reliefFocus : null);
      }
      // … 2 lines
      {
        // … 5 lines
        if ((fogJournal !== doc.changes || fogSerial !== fogSeen) && !landHeld) {
          // … 3 lines
          const ground = (x: number, y: number): number => terrain.renderedHeightAt(x, y);
          const laid = !entries || entries.some((e) => e.rects === null)
            ? fogField.whole(doc.fogDabs, ground)
            : fogField.update(doc.fogDabs, ground, entries.flatMap((e) => e.rects!));
          fogLayer = laid.any ? laid : null;
        // … 4 lines
        }
        if (!landHeld) {
          // … 1 lines
          const followed = fogField.follow(rig.viewport.centre.x, rig.viewport.centre.y, doc.fogDabs, (x, y) => terrain.renderedHeightAt(x, y));
          if (followed) fogLayer = followed.any ? followed : null;
        }
        // … 1 lines
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
        elementLayer?.emit(Math.min(0.1, Math.max(0, delta)), rig.camera.position, exhaust);
        post.setGroundFog(fogLayer?.any ? { texture: fogLayer.texture, low: fogLayer.low, high: fogLayer.high, density: f.density, window: fogLayer.window } : null);
        fogMoving = !!fogLayer?.any;
      // … 1 lines
      }
      // … 4 lines
      const eyeShift = rig.perspective ? 0
        : rig.camera.position.distanceTo(rig.target) - halfHeight / Math.tan((PERSPECTIVE_FOV * Math.PI) / 360);
      post.setAtmosphere(air, sunTowards.copy(environment.sun.position).sub(environment.sun.target.position), environment.skyColor,
        sunLight.copy(environment.sun.color).multiplyScalar(environment.sun.intensity), eyeShift);
      // … 2 lines
      agents.renderPalettes(renderer);
      // … 12 lines
      roadTiles.unseen = !reveal.opened;
      if (!reveal.opened) {
        // THE OPENING IN TWO STEPS: shown once the roads built so far and the masses are in; the near cells' detail and the world's later builds land after, as they do when the camera moves (DO NOT wait for `buildings.pending` or `worldJob`: the whole city waited 2.5-4.5 min for them, 2026-10-16).
        // OPEN = WHAT THE VIEW SEES IN ITS FINAL DETAIL (CLAUDE.md; Cesium3DTileset tilesLoaded: every tile meeting the screen's error this frame loaded): the near cells, the near roads and the tiles' near products in the frustum too, not the masses alone - the curtain fell with the district in masses and the street in a smooth mass (conf130/131/133).
        // A FULL-MAP OPENING (the fit on a rounded city, zoom under 1: a metre under a pixel at every distance): the masses and the far roads
        // ARE the final detail there. The near levels were still waited for and took seconds of the curtain (2026-10-20); they stream after it,
        // where a pixel cannot tell them from what stands.
        const mapView = !rig.perspective && rig.viewport.zoom < 1;
        // THE MAP'S OWN ROADS (`mapRoadsIn`), not whatever stands: DO NOT take the last city's for this one's - the curtain fell on a town of buildings over grass, no street drawn (2026-10-21).
        // And every road past the opening's view at its far level (`networkFar`): DO NOT show a map whose streets stop at the view's edge.
        const roadsIn = roads !== null && mapRoadsIn && !networkFar.pending;
        const whole = roadsIn && !buildings.coarsePending && (mapView || !buildings.viewPending)
          && natureTreesStarted && !natureTreesPending;
        // DO NOT draw the unseen frame before the town's programs are built: drawn first, each program was built at its first use, one after the other, in that one frame (4.5 s of the opening's 62, 2026-10-16); asked together, the driver builds them in parallel (KHR_parallel_shader_compile).
        // The programs are asked once the BUILDINGS are in, not when every road tile and tree is: the town's cells are what bring every
        // material the reveal draws, and the driver builds the shaders while the last tiles stream (the compile was on the critical path
        // after everything else was ready: 2.4-7.4 s of the opening, measured 2026-10-20).
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
        if (!revealedPrograms) { revealedPrograms = true; programs.afterReveal(); }
        // The rest of a large city's roads and ground, now that its view is shown: built in slices while the game runs (`openingBuild`).
        // The roads' heights past the opening's view first, a share a frame (`completeSolve`): the fill builds on the whole solve.
        if (openingFillPending && worldJob === null && completeSolve(net)) {
          openingFillPending = false;
          openingBuild = false;
          // The opening's tiles stay built; the far level's cells are taken a few at a time (`fillStep`).
          const scope = openingWithin;
          fillTaken.clear();
          fillAwaiting.clear();
          fillLeft = new Set(networkFar.cellKeys());
          fillEditRead.reset();
          fillEditRead.read(net.doc.changes);
          openingWithin = (rect) => (scope?.(rect) ?? false) || fillTaken.has(farCellOf((rect[0] + rect[2]) / 2, (rect[1] + rect[3]) / 2));
        }
        if (fillLeft !== null && fillLeft.size > 0 && worldJob === null && !openingFillPending) fillStep(net);
        // The tier's geometry budget kept (`governGeometry`), a read of the scene now and then.
        if (++governFrames >= GOVERN_FRAMES) { governFrames = 0; governGeometry(); }
        // Round the frame's whole drawing: the shadow, the scene, every pass.
        gpuTimer.poll();
        // The effects panel's switches: the passes skipped, the layers hidden for this drawing only, the shadows kept as they stand.
        post.setSwitches(optimizer.postSwitches(effectSwitches));
        if (effectSwitches !== ALL_EFFECTS_ON || buildingChoices !== DEFAULT_BUILDING_CHOICES) {
          frameHider.hide(switchedOff(effectSwitches));
          buildingSwitcher.beforeRender(effectSwitches, buildingChoices, frameHider);
          if (!effectSwitches.shadowUpdate) renderer.shadowMap.needsUpdate = false;
        }
        optimizer.beforeRender();
        if (backend === 'webgl') {
          // The walls round the eye laid from the camera as this frame draws it (its shake included), the buildings behind them out of the colour pass.
          // Only where the town is drawn as it stands (no effect switched off, the camera low enough for the solids to be filed: `solidsNeeded`).
          const wanted = occlusionProbe.enabled && townSolids !== null && rig.perspective && effectSwitches === ALL_EFFECTS_ON && buildingChoices === DEFAULT_BUILDING_CHOICES;
          const occluding = wanted && occlusionIdle === 0;
          if (wanted && occlusionIdle > 0) occlusionIdle--;
          occludedNow = false;
          occlusionProbe.stats.objectsHidden = 0;
          occlusionProbe.stats.active = occluding;
          if (occluding) {
            occluderCells.clear();
            const camera = rig.camera as PerspectiveCamera;
            occlusion.build(camera, camera.aspect, townSolids!(), occluderDrawn, camera.near);
            occlusionProbe.stats.objectsHidden = buildings.hideOccluded(camera, occlusionHidden);
            occludedNow = occlusionProbe.stats.objectsHidden > 0;
            if (!occludedNow) occlusionIdle = OCCLUSION_RETRY_FRAMES;
          } else occlusion.clear();
          gpuTimer.begin();
          try {
            post.render(delta);
          } finally {
            if (occluding) buildings.restoreOccluded();
            occludedNow = false;
          }
          gpuTimer.end();
          if (occluding) Object.assign(occlusionProbe.stats, occlusion.stats);
        // Under WebGPU, the frame its renderer can draw so far: the scene without the post chain nor the shadows (stages 3 and 4); nothing until its device stands.
        } else gpuView?.render(scene, rig.camera);
        // What came into the scene given the optimisations on, a few times a second: built off the draw when that changed programs.
        if (optimizer.afterRender()) compileChanged();
        buildingSwitcher.afterRender();
        frameHider.restore();
        tellTargets();
        // What this frame drew, before anything after it draws more (the compiles and warm-ups below render too).
        frameDrawn.calls = renderer.info.render.calls;
        frameDrawn.triangles = renderer.info.render.triangles;
      // Not open yet: looked at again in `WAIT_LOOK_MS`, the programs and workers it waits on waking it at once when they land (`onAssetsReady`). DO NOT ask the next frame straight away: thousands of empty frames a second (a MessageChannel, no vsync while loading) starved the very workers and driver threads the opening waits for (UI-02).
      } else lookAgain();
      // … 1 lines
      if (shake > 0) { rig.camera.position.sub(shakeOffset); rig.camera.updateMatrixWorld(); }
      // … 3 lines
      if (backend === 'webgl') drainCompiles(renderer, rig.camera, scene, post.target);
      // … 5 lines
      if (optimizer.uploadsDue()) drainWarm(renderer);
    // … 4 lines
    },
    setQuality(level, settings) {
      // … 1 lines
      const resolved = level === 'auto' ? tier : level;
      tier = resolved;
      baseQuality = settings ?? QUALITY[resolved];
      // … 1 lines
      applyQuality(resolved, withOptimizations(baseQuality, optimizer.values));
    // … 1 lines
    },
    setOptimizations(next) {
      const before = optimizer.values;
      const values = (Object.keys(DEFAULT_OPTIMIZATIONS) as (keyof Optimizations)[]).every((k) => next[k] === DEFAULT_OPTIMIZATIONS[k]) ? DEFAULT_OPTIMIZATIONS : next;
      const { programs, shadows } = optimizer.set(values);
      let compile = programs;
      if (qualityOptionsDiffer(before, values)) {
        applyQuality(tier, withOptimizations(baseQuality, values));
        compile = true;
      }
      if (shadows) shadowMoved = true;
      if (compile) compileChanged();
    // … 1 lines
    },
    optimizationReport: () => optimizer.report(),
    setEffects(next, nextBuildings = DEFAULT_BUILDING_CHOICES) {
      const before = effectSwitches;
      effectSwitches = Object.values(next).every(Boolean) ? ALL_EFFECTS_ON : next;
      if (nextBuildings.detailMemoryMB !== buildingChoices.detailMemoryMB) {
        buildings.setDetailBudget((nextBuildings.detailMemoryMB < 0 ? quality.detailMemoryMB : nextBuildings.detailMemoryMB) * 1048576);
      }
      const plain = (Object.keys(DEFAULT_BUILDING_CHOICES) as (keyof BuildingChoices)[]).every((key) => nextBuildings[key] === DEFAULT_BUILDING_CHOICES[key]);
      buildingChoices = plain ? DEFAULT_BUILDING_CHOICES : nextBuildings;
      if (before.bShadows !== next.bShadows) shadowMoved = true;
      // The shadows and the lights change every lit material's program: built off the draw, as a quality change's.
      let programs = false;
      if (before.shadows !== next.shadows) {
        renderer.shadowMap.enabled = quality.shadows && next.shadows;
        environment.setQuality({ shadows: quality.shadows && next.shadows, shadowMapSize: quality.shadowMapSize });
        programs = true;
      }
      if (before.shadows !== next.shadows || before.shadowUpdate !== next.shadowUpdate) shadowMoved = true;
      if (before.surfaceDetail !== next.surfaceDetail) materials.setDetail(quality.surfaceDetail && next.surfaceDetail);
      if (before.environmentMap !== next.environmentMap) {
        if (!next.environmentMap) {
          keptEnvironment = scene.environment;
          scene.environment = null;
        } else {
          scene.environment = keptEnvironment;
          keptEnvironment = null;
        }
        programs = true;
      }
      for (const [name, key] of [['sky-fill', 'skyFill'], ['ambient-floor', 'ambient'], ['fill', 'fillLight']] as const) {
        if (before[key] === next[key]) continue;
        const light = scene.getObjectByName(name);
        if (light) light.visible = next[key];
        programs = true;
      }
      if (programs) compileChanged();
      askDraw();
    },
    get quality() {
      return requested;
    },
    dispose() {
      // … 9 lines
      buildingSwitcher.dispose();
      optimizer.dispose();
      // … 6 lines
      polePreview?.dispose();
      // … 3 lines
      agents.dispose();
      // … 1 lines
      buildings.dispose();
      roadPrograms.dispose();
      // … 7 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.dispose();
      // … 3 lines
      elementKit?.dispose();
      // … 6 lines
      terrain.dispose();
      materials.dispose();
      // … 1 lines
      post.dispose();
    // … 2 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:1001-1018 slice of src/render/renderer.ts#createSceneRenderer.loadEffects (shared): the feature's statements; "…" marks code left out
  const loadEffects = (): Promise<void> => fxLoading ??= import('./playEffects').then((mod) => {
    fx = mod.createPlayEffects({
      // … 9 lines
      camera: () => rig.camera,
    // … 2 lines
    });
  // … 2 lines
  });

// src/render/renderer.ts:1225-1243 slice of src/render/renderer.ts#createSceneRenderer.pumpWorld (shared): the feature's statements; "…" marks code left out
  const pumpWorld = (): void => {
    // … 6 lines
    const until = reveal.opened ? -Infinity : performance.now() + HIDDEN_RUN_MS;
    for (;;) {
      // … 7 lines
      if (performance.now() >= until) { askDraw(); return; }
    }
  };

// src/render/renderer.ts:1404-1404 slice of src/render/renderer.ts#createSceneRenderer.budgetNow (shared): the feature's statements; "…" marks code left out
  const budgetNow = (): GeometryBudget => geometryBudget ?? GEOMETRY_BUDGETS[tier];

// src/render/renderer.ts:1406-1413 slice of src/render/renderer.ts#createSceneRenderer.applyGeometryBudget (shared): the feature's statements; "…" marks code left out
  const applyGeometryBudget = (): void => {
    // … 1 lines
    buildings.setMassBudget(budget.massesMB * 1048576);
    roadResidency.budget = budget.roadsNearMB * 1048576;
    roadResidency.farBudget = budget.roadsFarMB * 1048576;
    ViewCells.gpuShare = budget.viewCellsShare;
  // … 1 lines
  };

// src/render/renderer.ts:1422-1439 slice of src/render/renderer.ts#createSceneRenderer.governGeometry (shared): the feature's statements; "…" marks code left out
  const governGeometry = (): number => {
    // … 1 lines
    if (buildingChoices.detailMemoryMB >= 0) return geometry;
    const total = budgetNow().totalMB * 1048576;
    const tierDetail = quality.detailMemoryMB * 1048576;
    // … 5 lines
    if (geometry > total) next = Math.max(total / 16, Math.min(tierDetail, detail - (geometry - total)));
    else if (geometry < total * 0.9) next = Infinity;
    if (next !== governedDetail) {
      governedDetail = next;
      buildings.setDetailBudget(Number.isFinite(next) ? next : tierDetail);
    }
  // … 1 lines
  };

// src/render/renderer.ts:1471-1471 slice of src/render/renderer.ts#createSceneRenderer.occlusionHidden (shared): the feature's statements; "…" marks code left out
  const occlusionHidden = (x0: number, y0: number, x1: number, y1: number, z0: number, z1: number): boolean => occlusion.hidden(x0, y0, x1, y1, z0, z1);

// src/render/renderer.ts:1561-1567 slice of src/render/renderer.ts#createSceneRenderer.strikeAt (shared): the feature's statements; "…" marks code left out
  const strikeAt = (x: number, y: number): void => {
    // … 3 lines
    const distance = Math.hypot(x - rig.target.x, y + rig.target.z);
    for (const listener of strikeListeners) listener(x, y, distance);
  };

// src/render/renderer.ts:1733-1759 slice of src/render/renderer.ts#createSceneRenderer.watchRoads (shared): the feature's statements; "…" marks code left out
  const watchRoads = (net: Network): void => {
    // … 2 lines
    if (now - roadsWatchedAt < 1000 || !reveal.opened || net.doc.segments.size === 0 || worldJob !== null) return;
  // … 22 lines
  };

// src/render/renderer.ts:2225-2248 slice of src/render/renderer.ts#createSceneRenderer.completeSolve (shared): the feature's statements; "…" marks code left out
  const completeSolve = (net: Network): boolean => {
    // … 4 lines
    const left = solveLeft ??= { x: rig.viewport.centre.x, y: rig.viewport.centre.y, order: null, at: 0 };
  // … 17 lines
  };

// src/render/renderer.ts:2258-2282 slice of src/render/renderer.ts#createSceneRenderer.fillStep (shared): the feature's statements; "…" marks code left out
  const fillStep = (net: Network): void => {
    // … 1 lines
    const { x, y } = rig.viewport.centre;
  // … 21 lines
  };

// src/render/renderer.ts:2317-2394 slice of src/render/renderer.ts#createSceneRenderer.buildWorld (shared): the feature's statements; "…" marks code left out
  const buildWorld = (net: Network, full = false): void => {
    // … 5 lines
    const centre = rig.viewport.centre;
    // … 1 lines
    openingWithin = opening ? (rect) => rect[0] <= centre.x + openingHalf && rect[2] >= centre.x - openingHalf
      && rect[1] <= centre.y + openingHalf && rect[3] >= centre.y - openingHalf : null;
    const openingRegion = opening ? terrainRegion([centre.x - openingHalf, centre.y - openingHalf, centre.x + openingHalf, centre.y + openingHalf]) : null;
    // … 2 lines
    const solveScope: SolveScope | null = opening ? { region: [centre.x - reach, centre.y - reach, centre.x + reach, centre.y + reach] } : null;
    if (opening) solveLeft = { x: centre.x, y: centre.y, order: null, at: 0 };
    // … 42 lines
    if (opening) {
      // … 3 lines
      networkFar.request(net, elevation, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN,
        [lo(centre.x - openingHalf), lo(centre.y - openingHalf), hi(centre.x + openingHalf), hi(centre.y + openingHalf)]);
    }
    // … 10 lines
    worldJob = worldSteps(net, pendingBlocks, { cause, elevationMs }, started, true, openingRegion, levelLand);
  // … 2 lines
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

// src/render/renderer.ts:2658-2674 slice of src/render/renderer.ts#createSceneRenderer.applyQuality (shared): the feature's statements; "…" marks code left out
  const applyQuality = (level: QualityLevel, settings: QualitySettings = QUALITY[level]): void => {
    quality = settings;
    // The far woods as dense as the near ones, on the same budget (`farWoods.ts`).
    terrain.setVegetation?.(quality.vegetation);
    renderer.setPixelRatio(Math.min(WEAK_MACHINE.pixelRatio ?? (window.devicePixelRatio || 1), quality.pixelRatio));
    renderer.shadowMap.enabled = quality.shadows && effectSwitches.shadows;
    materials.setDetail(quality.surfaceDetail && effectSwitches.surfaceDetail);
    environment.setQuality({ shadows: quality.shadows && effectSwitches.shadows, shadowMapSize: quality.shadowMapSize });
    if (buildingChoices.detailMemoryMB < 0) buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
    applyGeometryBudget();
    post?.dispose();
    post = createPostChain(renderer, scene, rig.camera, quality, level);
    post.setNight(Math.max(0, lastDark));
    resize();
    // DO NOT force one with `networkRevision = -1`
    shadowMoved = true;
  };

// src/render/renderer.ts:2767-2855 slice of src/render/renderer.ts#createSceneRenderer.natureSweep (shared): the feature's statements; "…" marks code left out
  const natureSweep = (key: string): NatureSweep => {
    if (quality.vegetation <= 0 || !terrain.parts.some((p) => p.surface.ecology())) return { key };
    // … 7 lines
    const vegetation = quality.vegetation;
    for (const part of terrain.parts) {
      // … 3 lines
      const plateKey = `${part.surface.ecologyRevision}:${part.surface.woodsRevision ?? 0}:${plateLand.get(part.surface) ?? 0}:${vegetation}`;
      // … 1 lines
      if (known && known.key === plateKey) {
        plates.push(known);
        treeSum += known.sum;
        continue;
      }
      // … 26 lines
      const plate: NaturePlate = { key: plateKey, part, odds, band, di, dj, sum: plateSum, picked: null, kept: null };
      // … 2 lines
      treeSum += plateSum;
    }
    // … 2 lines
    const base = NATURE_TREES * Math.min(1, quality.vegetation / 2_600);
    const plannedSum = (treeSum / Math.max(1, plates.length)) * (m(1_920) ** 2 / plateArea);
    // … 1 lines
    const treeScale = Math.min(1, Math.exp(Math.round(Math.log(base / Math.max(1, plannedSum)) / 0.02) * 0.02));
    // … 1 lines
    for (const plate of plates) {
      if (plate.picked && plate.picked.scale === treeScale) {
        slices.push({ plate });
        continue;
      }
      // … 2 lines
      for (let j = 0; j < cells; j++) {
        for (let i = 0; i < cells; i++) {
          // … 2 lines
          if (cellHash(gi, gj, 41) >= odds[o]! * treeScale) continue;
        // … 11 lines
        }
      }
      plate.picked = { scale: treeScale, trees: mine };
    // … 1 lines
    }
  // … 1 lines
  };

// src/render/renderer.ts:3014-3031 slice of src/render/renderer.ts#createSceneRenderer.compileChanged (shared): the feature's statements; "…" marks code left out
  function compileChanged(): void {
    // … 7 lines
    const scenePrograms = renderer.compileAsync(scene, rig.camera, scene);
    // … 2 lines
    const compiling: Promise<void> = Promise.all([scenePrograms, post.ready]).then(() => undefined, () => undefined).then(() => {
      if (qualityCompile === compiling) qualityCompile = null;
      askDraw();
    });
    qualityCompile = compiling;
  // … 1 lines
  }
