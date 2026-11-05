// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 46 lines
  setBuildingPreview(preview: BuildingPreviewInput | null): void;
  // … 49 lines
  setBuildingCutaway(spec: CutawaySpec | null): void;
// … 71 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 854 lines
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
  // … 5 lines
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
      // … 1 lines
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
  // … 114 lines
  let cutSpec: CutawaySpec | null = null;
  // … 8 lines
  const roomLamps = new RoomLamps();
  // … 1010 lines
  const buildingSwitcher = createBuildingSwitcher(buildings);
  // … 310 lines
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
    // … 24 lines
    buildingDetailBytes: () => buildings.residentBytes,
    // … 53 lines
    setBuildingCutaway(spec) {
      buildings.setCutaway(spec);
    // … 1 lines
    },
    setBuildingsDimmed(except) {
      buildings.setDimmed(except === undefined ? undefined : (except as BuildingId | null));
    },
    // … 6 lines
    draw(net, sim, alpha, delta, options) {
      // … 75 lines
      if (roads !== null && (reveal.opened || (groundIn && networkRevision === net.revision))) {
        buildings.hidden = !reveal.opened;
        // … 4 lines
        buildings.update(net.doc, terrain.renderedHeightAt, buildingGround, pavedHeightAt, builtGroundAt,
          (since) => (since === '' ? null : buildingsOnGroundSince(net.doc, Number(since))), levelLand ? levelGroundDigest : landGroundDigest);
        // … 1 lines
        if (buildings.pending) askDraw();
      // … 1 lines
      }
      // … 16 lines
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
      // … 21 lines
      {
        // … 23 lines
        buildings.frame.budget = buildingChoices.triangles === 0 ? Infinity : VIEW_BUDGETS[viewOf(rig.viewport.zoom)].triangles * buildingChoices.triangles * quality.triangleScale;
        buildings.levelOfDetail(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, reach, detailFrustum);
        // … 8 lines
        buildings.cullDetails(ex, ey, ez, pxAtMetre * optimizer.buildingPx, rig.perspective ? null : rig.viewport.zoom, texel * optimizer.castTexel);
      }
      // … 89 lines
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
      // … 10 lines
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
      // … 61 lines
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
          // … 8 lines
          if (occluding) {
            // … 3 lines
            occlusionProbe.stats.objectsHidden = buildings.hideOccluded(camera, occlusionHidden);
          // … 2 lines
          } else occlusion.clear();
          // … 1 lines
          try {
          // … 1 lines
          } finally {
            if (occluding) buildings.restoreOccluded();
          // … 1 lines
          }
        // … 3 lines
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
      // … 11 lines
      agents.dispose();
      // … 1 lines
      buildings.dispose();
    // … 25 lines
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
      // … 3 lines
      if (blueprint) {
        // … 1 lines
        const ghost = buildBuildingMeshes([sample], () => 0, buildings.kit, true);
        group.add(ghost.group);
        done.push(() => ghost.dispose());
        // … 1 lines
        const real = buildBuildingMeshes([sample], () => 0, buildings.kit, false);
        group.add(real.group);
        done.push(() => real.dispose());
        // … 1 lines
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
