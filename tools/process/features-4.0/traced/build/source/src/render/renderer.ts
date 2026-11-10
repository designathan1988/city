// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 46 lines
  setBuildingPreview(preview: BuildingPreviewInput | null): void;
  // … 7 lines
  setLotOverlay(input: LotOverlayInput | null, changes?: ChangeJournal): void;
  // … 1 lines
  strikeBuilding(b: Building, x: number, y: number, z: number, strength: number): boolean;
  // … 39 lines
  setBuildingCutaway(spec: CutawaySpec | null): void;
// … 71 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 491 lines
  const wear = createWearField();
  // … 1 lines
  applyWear(materials.asphalt, wear, 0, 'asphalt');
  applyWear(materials.footway, wear, 1, 'footway');
  // … 144 lines
  const farBlocks = new FarBlockBatches();
  world.add(farBlocks.group);
  const surfaceReuse: SurfaceReuse = {
    farBatches: farBlocks,
  // … 41 lines
  };
  // … 169 lines
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
  // … 2 lines
  ]);
  for (const root of [...agents.meshes, transit.group]) moverRoots.add(root);
  // … 4 lines
  gpuBatchContext(renderer.getContext() as WebGL2RenderingContext);
  const buildings = createBuildingLayer(anisotropy);
  // … 1 lines
  buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
  // … 42 lines
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
  // … 21 lines
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
    // … 2 lines
    farBlocks.refill();
    buildings.refill();
  // … 2 lines
  });
  // … 1 lines
  let gardens: GardenCells | null = null;
  // … 13 lines
  let townSolids: (() => CameraSolids) | null = null;
  // … 98 lines
  let cutSpec: CutawaySpec | null = null;
  // … 8 lines
  const roomLamps = new RoomLamps();
  // … 47 lines
  const groundSince: GroundSince = {
    get version() { return groundChanges.version; },
    rectsSince: (since) => groundRectsSince(since),
  };
  // … 3 lines
  const floorGroundSince: GroundSince = {
    get version() { return groundDiary?.serialOf(...FLOOR_KINDS) ?? 0; },
    rectsSince: (since) => {
      const entries = groundDiary?.since(since, FLOOR_KINDS);
      if (!entries) return null;
      const rects: ChangeRect[] = [];
      for (const entry of entries) {
        if (entry.rects === null) return null;
        rects.push(...entry.rects);
      }
      return rects;
    },
  };
  // … 1 lines
  const floorBoxes = new WeakMap<Building, ChangeRect>();
  // … 11 lines
  const CANYON_GROUND_REACH = FLOOR_READ_REACH + m(64);
  // … 15 lines
  const floors = buildingFloors(builtGroundAt, pavedHeightAt, floorBox, FLOOR_READ_REACH);
  // … 2 lines
  const cameraSolidsKept = cameraSolidsKeeper((b) => floors.floorOf(b), floorBox, FLOOR_READ_REACH);
  // … 43 lines
  const sitesGraded = siteGrading(builtGroundAt, pavedHeightAt, SITE_APRON, (b) => bankBox(b), floors);
  // … 1 lines
  const bankBoxes = new WeakMap<Building, readonly [number, number, number, number]>();
  // … 356 lines
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 5 lines
      const work = transit.sync(net.doc, terrain.renderedHeightAt, pavedHeightAt, (p) => {
        // … 1 lines
        for (const b of net.doc.buildings.near(p.x, p.y, p.x, p.y)) {
          if (solidFootprints(b).some((ring) => pointInPolygon(p, ring))) return true;
        }
      // … 1 lines
      });
    // … 2 lines
    },
  }, 'draw');
  // … 488 lines
  const buildingSwitcher = createBuildingSwitcher(buildings);
  // … 90 lines
  let lotOverlay: ReturnType<typeof createLotOverlay> | null = null;
  // … 5 lines
  let plantMapFor: { map: boolean | null; gardens: GardenCells | null } = { map: null, gardens: null };
  // … 213 lines
  const plantedKeeps = new CellKeeps<PlantedTree>((t) => (onCarriageway(keepNet!, t) || buildings.covers(t.x, t.y) ? null : terrain.renderedHeightAt(t.x, t.y)));
  // … 92 lines
  const handle: SceneHandle = {
    // … 33 lines
    meshProbe: () => agents.meshProbe(),
    animProbe: (id) => agents.animProbe(id),
    forceClip: (id, clip) => agents.forceClip(id, clip as never),
    driverBody: (vehicle, x, y) => agents.driverBody(vehicle, x, y),
    // … 5 lines
    census: () => agents.census(),
    // … 13 lines
    get stats(): RenderStats {
      return {
        builtTriangles: builtTriangles + buildings.triangles,
      // … 8 lines
      };
    },
    // … 41 lines
    setGeometryBudget(budget) {
      // … 2 lines
      if (buildingChoices.detailMemoryMB < 0) buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
    },
    // … 2 lines
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
    // … 42 lines
    setBuildingPreview(preview) {
      buildings.setPreview(preview);
    },
    // … 1 lines
    strikeGround(x, y, strength) {
      for (let k = 0; k < 6 + strength * 2; k++) {
        // … 1 lines
        wear.wheels(x + Math.cos(a) * r, y + Math.sin(a) * r, a, 0.01, 30);
      }
      wear.tick(10);
    // … 1 lines
    },
    // … 15 lines
    buildingDetailBytes: () => buildings.residentBytes,
    // … 17 lines
    setLotOverlay(input, changes) {
      lotOverlay ??= createLotOverlay((x, y) => handle.surfaceHeightAt(x, y));
      lotOverlay.set(input, changes);
    },
    // … 32 lines
    setBuildingCutaway(spec) {
      buildings.setCutaway(spec);
    // … 1 lines
    },
    setBuildingsDimmed(except) {
      buildings.setDimmed(except === undefined ? undefined : (except as BuildingId | null));
    },
    // … 6 lines
    draw(net, sim, alpha, delta, options) {
      // … 55 lines
      if (!levelLand && !(worldJob && worldJobGround) && gradedFor !== net.doc.buildings.revision) {
        gradedFor = net.doc.buildings.revision;
        const changed = sitesGraded.changedSites(net.doc.buildings);
        if (changed) shapeGround(net, changed.map(terrainRegion), true);
      }
      // … 15 lines
      if (roads !== null && (reveal.opened || (groundIn && networkRevision === net.revision))) {
        buildings.hidden = !reveal.opened;
        // … 4 lines
        buildings.update(net.doc, terrain.renderedHeightAt, buildingGround, pavedHeightAt, builtGroundAt,
          (since) => (since === '' ? null : buildingsOnGroundSince(net.doc, Number(since))), levelLand ? levelGroundDigest : landGroundDigest);
        // … 1 lines
        if (buildings.pending) askDraw();
      // … 1 lines
      }
      // … 8 lines
      if (!gardens) {
        gardens = createGardenCells(sceneryKit);
        world.add(gardens.layer.group);
      }
      gardens.source(net.doc.buildings, terrain.renderedHeightAt);
      if (groundSettled) gardens.follow(net.doc.changes);
      // … 2 lines
      if (!stroking && !worldJob) {
        // … 100 lines
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
        // … 23 lines
        buildings.frame.budget = buildingChoices.triangles === 0 ? Infinity : VIEW_BUDGETS[viewOf(rig.viewport.zoom)].triangles * buildingChoices.triangles * quality.triangleScale;
        buildings.levelOfDetail(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, reach, detailFrustum);
        // … 8 lines
        buildings.cullDetails(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, texel * optimizer.castTexel);
      }
      // … 7 lines
      for (const mesh of gardens?.meshes ?? []) {
        mesh.visible = detailed && (!plantMap || !isLeafCards(mesh));
      }
      // … 1 lines
      if (plantMap !== plantMapFor.map || gardens !== plantMapFor.gardens) {
        plantMapFor = { map: plantMap, gardens };
        gardens?.setMap(plantMap);
        furniture.setMap(plantMap);
      }
      // … 58 lines
      {
        // … 1 lines
        const props = [...(gardens && detailed ? [gardens.layer] : []), ...(quality.detailProps && detailed ? [furniture.layer] : [])];
        if (props.length) lookProps(props, view);
      // … 1 lines
      }
      // … 6 lines
      gardens?.cull(crowdFrustum, crowdProjection);
      // … 2 lines
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
      // … 4 lines
      if (optimizer.wear) {
        for (const ped of sim.pedViews) if (ped.v > 0.05) wear.feet(ped.x, ped.y, wallDt);
        wear.follow(rig.viewport.centre.x, rig.viewport.centre.y);
        wear.tick(wallDt);
      }
      // … 1 lines
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
      // … 1 lines
      for (const light of roomLights) light.intensity = cutSpec && light.userData['used'] ? 55 * TUNED_UNIT ** light.decay * (0.25 + 0.75 * lastDark) : 0;
      // … 9 lines
      const tallestTop = Math.max(0, buildings.top);
      // … 40 lines
      if (Math.abs(dark - lastDark) > 0.01) {
        // … 3 lines
        agents.setNight(dark);
      }
      // … 1 lines
      buildings.setNight(effectSwitches.bNightWindows ? lastDark : 0, clock / 60);
      // … 1 lines
      environment.follow(rig.camera, target, tallestTop, builtGroundAt);
      // … 30 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 1 lines
        if (agents.castersChanged) shadowMoved = true;
        // … 24 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 9 lines
      {
        // … 22 lines
        roadFocus.reach = Math.min(900, Math.max(320, halfHeight * 1.8)) / buildings.frame.scale;
      // … 13 lines
      }
      // … 1 lines
      farBlocks.sync();
      // … 59 lines
      agents.renderPalettes(renderer);
      // … 13 lines
      if (!reveal.opened) {
        // … 9 lines
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
        // … 22 lines
        if (effectSwitches !== ALL_EFFECTS_ON || buildingChoices !== DEFAULT_BUILDING_CHOICES) {
          // … 1 lines
          buildingSwitcher.beforeRender(effectSwitches, buildingChoices, frameHider);
        // … 1 lines
        }
        // … 1 lines
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
        // … 2 lines
        buildingSwitcher.afterRender();
      // … 6 lines
      } else lookAgain();
    // … 16 lines
    },
    // … 23 lines
    setEffects(next, nextBuildings = DEFAULT_BUILDING_CHOICES) {
      // … 2 lines
      if (nextBuildings.detailMemoryMB !== buildingChoices.detailMemoryMB) {
        buildings.setDetailBudget((nextBuildings.detailMemoryMB < 0 ? quality.detailMemoryMB : nextBuildings.detailMemoryMB) * 1048576);
      }
    // … 30 lines
    },
    // … 3 lines
    dispose() {
      // … 9 lines
      buildingSwitcher.dispose();
      // … 3 lines
      wear.dispose();
      // … 7 lines
      agents.dispose();
      // … 1 lines
      buildings.dispose();
      // … 7 lines
      gardens?.dispose();
      // … 15 lines
      lotOverlay?.dispose();
    // … 1 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:1001-1018 slice of src/render/renderer.ts#createSceneRenderer.loadEffects (shared): the feature's statements; "…" marks code left out
  const loadEffects = (): Promise<void> => fxLoading ??= import('./playEffects').then((mod) => {
    fx = mod.createPlayEffects({
      // … 1 lines
      buildings: {
        chunkOf: (b) => buildings.chunkOf(b),
        setRuined: (ruined) => buildings.setRuined(ruined),
        lotHeightAt: (id, x, y) => buildings.lotHeightAt(id, x, y),
      },
    // … 6 lines
    });
  // … 2 lines
  });

// src/render/renderer.ts:1269-1276 slice of src/render/renderer.ts#createSceneRenderer.levelGroundDigest (shared): the feature's statements; "…" marks code left out
  const levelGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    // … 3 lines
    const pads = sitesGraded.pads?.cut([[minX, minY, maxX, maxY]]);
    if (pads) out.add(pads.apron).addPlain(pads.pads);
  // … 1 lines
  };

// src/render/renderer.ts:1406-1413 slice of src/render/renderer.ts#createSceneRenderer.applyGeometryBudget (shared): the feature's statements; "…" marks code left out
  const applyGeometryBudget = (): void => {
    // … 1 lines
    buildings.setMassBudget(budget.massesMB * 1048576);
  // … 4 lines
  };

// src/render/renderer.ts:1422-1439 slice of src/render/renderer.ts#createSceneRenderer.governGeometry (shared): the feature's statements; "…" marks code left out
  const governGeometry = (): number => {
    // … 4 lines
    const detail = buildings.residentBytes;
    // … 4 lines
    if (geometry > total) next = Math.max(total / 16, Math.min(tierDetail, detail - (geometry - total)));
    else if (geometry < total * 0.9) next = Infinity;
    if (next !== governedDetail) {
      governedDetail = next;
      buildings.setDetailBudget(Number.isFinite(next) ? next : tierDetail);
    }
  // … 1 lines
  };

// src/render/renderer.ts:1464-1470 slice of src/render/renderer.ts#createSceneRenderer.occluderDrawn (shared): the feature's statements; "…" marks code left out
  const occluderDrawn = (x: number, y: number): boolean => {
    const i = Math.floor(x / BATCH_CELL), j = Math.floor(y / BATCH_CELL);
    const key = (i + 32768) * 65536 + (j + 32768);
    let drawn = occluderCells.get(key);
    if (drawn === undefined) occluderCells.set(key, drawn = buildings.occluderCell(i, j));
    return drawn;
  };

// src/render/renderer.ts:1690-1697 slice of src/render/renderer.ts#createSceneRenderer.floorBox (shared): the feature's statements; "…" marks code left out
  const floorBox = (b: Building): ChangeRect => {
    let box = floorBoxes.get(b);
    if (!box) {
      const r = storedBounds(b, FLOOR_READ_REACH);
      floorBoxes.set(b, box = [r.minX, r.minY, r.maxX, r.maxY]);
    }
    return box;
  };

// src/render/renderer.ts:1719-1719 slice of src/render/renderer.ts#createSceneRenderer.followFloors (shared): the feature's statements; "…" marks code left out
  const followFloors = (net: Network, shaped: readonly ChangeRect[] | null = null): void => floors.follow(net.doc.buildings, floorGroundSince, shaped);

// src/render/renderer.ts:1769-1776 slice of src/render/renderer.ts#createSceneRenderer.bankBox (shared): the feature's statements; "…" marks code left out
  const bankBox = (b: Building): readonly [number, number, number, number] => {
    let box = bankBoxes.get(b);
    if (!box) {
      const r = storedBounds(b, BANK_REACH);
      bankBoxes.set(b, box = [r.minX, r.minY, r.maxX, r.maxY]);
    }
    return box;
  };

// src/render/renderer.ts:1778-1786 slice of src/render/renderer.ts#createSceneRenderer.buildingsOnGroundSince (shared): the feature's statements; "…" marks code left out
  const buildingsOnGroundSince = (doc: RoadDoc, serial: number): Building[] | null => {
    const rects = groundSince.rectsSince(serial);
    if (!rects) return null;
    // Every change in one look (`nearAny`): a change at a time read the cells they share again for each.
    return doc.buildings.nearAny(rects, BANK_REACH, (b, r) => {
      const box = bankBox(b);
      return box[0] <= r[2] && box[2] >= r[0] && box[1] <= r[3] && box[3] >= r[1];
    });
  };

// src/render/renderer.ts:1907-1918 slice of src/render/renderer.ts#createSceneRenderer.padsAskOf (shared): the feature's statements; "…" marks code left out
  const padsAskOf = (list: readonly Building[]): PadsAsk | null => {
    const ground = (terrain.renderedHeightAt as SharedGroundAt).share?.current;
    if (!ground || !pavedHeightAt.pack) return null;
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const b of list) {
      const box = bankBox(b);
      x0 = Math.min(x0, box[0]); y0 = Math.min(y0, box[1]); x1 = Math.max(x1, box[2]); y1 = Math.max(y1, box[3]);
    }
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
    // … 2 lines
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
  // … 5 lines
  }

// src/render/renderer.ts:2006-2052 slice of src/render/renderer.ts#createSceneRenderer.shapeGround (shared): the feature's statements; "…" marks code left out
  const shapeGround = (net: Network, regions: TerrainRegion | readonly TerrainRegion[] | null = null, sites = false, padsReady = false): { readonly landed: boolean; readonly ms: number } => {
    // … 13 lines
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
      // … 1 lines
      const asking = terrain.askShape(region, region ? null : [...(roads?.shapeBounds() ?? []), ...(pads?.shapeBounds() ?? [])]);
      const boxes = cornerBoxes(asking.plates, TERRAIN_CELL, TERRAIN_HALF);
      const ask: GroundShapeAsk = { roads: roads ? roads.shapeCut!(boxes) : null, pads: pads ? pads.cut(boxes) : null, plates: asking.plates };
      const shaping: Shaping = { ask, take: asking.take, then: told, answers: null, failed: false, landed: false, ms: 0 };
    // … 11 lines
    }
    // … 1 lines
    const shape = pads ? { shapeBounds: () => [...(roads?.shapeBounds() ?? []), ...pads.shapeBounds()], shapeAt: shapeBoth(roads, pads) } : roads;
    terrain.shapeToRoads(shape, region);
  // … 1 lines
  };

// src/render/renderer.ts:2397-2501 slice of src/render/renderer.ts#createSceneRenderer.worldSteps (shared): the feature's statements; "…" marks code left out
  function* worldSteps(net: Network, compares: readonly (BlockCompare | null)[], solved: { readonly cause: number; readonly elevationMs: number },
    started: number, sliced: boolean, region: TerrainRegion | null, levelLand: boolean): Generator<'wait' | void, void, void> {
    // … 17 lines
    if (levelLand) {
      // Level ground (a generated city's new map): nothing to cut or fill, and its buildings were grown on this ground. DO NOT shape 29 thousand pads and follow every floor for a land that is already level (12.5 s of the opening).
      gradedFor = net.doc.buildings.revision;
    } else if (blocks === null) {
      const shaping = shapeGround(net, region);
      while (!shaping.landed) { landShapes(); if (shaping.landed) break; yield 'wait'; }
    } else if (blocks.length || gradedFor !== net.doc.buildings.revision) yield* shapeBlocksSteps(net, blocks);
  // … 78 lines
  }

// src/render/renderer.ts:2549-2582 slice of src/render/renderer.ts#createSceneRenderer.warmBlastShaders (shared): the feature's statements; "…" marks code left out
  const warmBlastShaders = (): void => {
    // … 2 lines
    const kit = buildings.kit;
    // … 7 lines
    const mats = [kit.shellMaterial, kit.ruinShell, kit.facade, ...Object.values(kit.material), kit.furniture().material, kit.massing];
    for (const mat of mats) {
      const mesh = new Mesh(tiny, mat);
      mesh.castShadow = true; mesh.receiveShadow = true; mesh.frustumCulled = false;
      group.add(mesh);
    }
    // … 7 lines
    for (const mesh of partWarmMeshes(kit)) {
      parts.add(mesh);
      const standIn = shadowStandIn(mesh);
      if (standIn) partShadows.add(standIn);
    }
  // … 4 lines
  };

// src/render/renderer.ts:2587-2624 slice of src/render/renderer.ts#createSceneRenderer.warmPreviewShaders (shared): the feature's statements; "…" marks code left out
  const warmPreviewShaders = (): void => {
    // … 2 lines
    try {
      // … 2 lines
      const blueprint = BLUEPRINTS[0];
      if (blueprint) {
        const sample = { ...instantiate(blueprint.body, { x: 0, y: 0 }, 0), id: -1 } as Building;
        const ghost = buildBuildingMeshes([sample], () => 0, buildings.kit, true);
        group.add(ghost.group);
        done.push(() => ghost.dispose());
        // And as a building grown on a lot draws it: its window panes carry their rooms' light slots
        const real = buildBuildingMeshes([sample], () => 0, buildings.kit, false);
        group.add(real.group);
        done.push(() => real.dispose());
        // And faded ("ocultar outros", `fade.ts`): the faded materials on parts with and without instance colours.
        const faded = buildBuildingMeshes([sample], () => 0, buildings.kit, false);
        fadeMeshes(faded.group, buildings.kit, true);
        group.add(faded.group);
        done.push(() => faded.dispose());
      }
    // … 12 lines
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

// src/render/renderer.ts:2658-2674 slice of src/render/renderer.ts#createSceneRenderer.applyQuality (shared): the feature's statements; "…" marks code left out
  const applyQuality = (level: QualityLevel, settings: QualitySettings = QUALITY[level]): void => {
    // … 7 lines
    if (buildingChoices.detailMemoryMB < 0) buildings.setDetailBudget(quality.detailMemoryMB * 1048576);
  // … 7 lines
  };

// src/render/renderer.ts:2871-2881 slice of src/render/renderer.ts#createSceneRenderer.natureOpen (shared): the feature's statements; "…" marks code left out
  const natureOpen = (net: Network, x: number, y: number, z: number): boolean => {
    // … 8 lines
    return !onRoadGround(net, { x, y }) && !buildings.covers(x, y);
  };

// src/render/renderer.ts:2921-2921 slice of src/render/renderer.ts#createSceneRenderer.coverFree (shared): the feature's statements; "…" marks code left out
  const coverFree = (x: number, y: number): boolean => !onRoadGround(keepNet!, { x, y }) && !buildings.covers(x, y);
