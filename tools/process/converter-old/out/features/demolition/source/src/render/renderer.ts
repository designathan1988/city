// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 62 lines
  flingOccupants(list: readonly Occupant[]): void;
  // … 55 lines
  ragdollProbe(): RagdollProbe[];
// … 49 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 541 lines
  const exhaust = createExhaust();
  scene.add(exhaust.points);
  // … 2 lines
  let fx: PlayEffects | null = null;
  // … 2044 lines
  const handle: SceneHandle = {
    // … 1 lines
    effects: () => loadEffects().then(() => import('./ragdoll')).then((mod) => mod.warmPhysics()),
    // … 25 lines
    shootBody: (a, b) => fx?.shootBody(a, b) ?? null,
    isDown: (id) => fx?.hides(id) ?? false,
    setEffectsSpeed(speed) { fx?.setEffectsSpeed(speed); },
    stepEffects(seconds) { fx?.stepEffects(seconds); },
    ragdollProbe: () => fx?.ragdollProbe() ?? [],
    clearCasualties() { fx?.clearCasualties(); },
    // … 4 lines
    vehicleHit(x, y, z, dirX, dirY, glass, blood) { fx?.vehicleHit(x, y, z, dirX, dirY, glass, blood); },
    dropVehicle(v) { fx?.dropVehicle(v); },
    wound(x, y, z, dirX, dirY, severed) { fx?.wound(x, y, z, dirX, dirY, severed); },
    shot(from, to) { fx?.shot(from, to); },
    // … 131 lines
    strikeBuilding: (b, x, y, z, strength) => fx?.strikeBuilding(b, x, y, z, strength) ?? false,
    strikeGround(x, y, strength) {
      // … 5 lines
      exhaust.burst(x, y, terrain.renderedHeightAt(x, y), 20 + strength * 6, 1, m(1 + strength * 0.4), m(3), 5);
    },
    explode(x, y, z, radius, hit) { fx?.explode(x, y, z, radius, hit); },
    burn: (x, y, z, size, seconds) => fx?.burn(x, y, z, size, seconds),
    soot: (x, y, z, r) => fx?.soot(x, y, z, r),
    geyser: (x, y, z, seconds) => fx?.geyser(x, y, z, seconds),
    sparkAt: (x, y, z, seconds) => fx?.sparkAt(x, y, z, seconds),
    leak: (x, y, z, seconds) => fx?.leak(x, y, z, seconds),
    // … 1 lines
    flingOccupants: (list) => { fx?.flingOccupants(list); },
    // … 5 lines
    busy: () => (fx?.busy() ?? false) || natureTreesPending || cameraEasing || (terrain.waiting?.() ?? false),
    // … 3 lines
    forgetRuin(id) { fx?.forgetRuin(id); },
    // … 64 lines
    draw(net, sim, alpha, delta, options) {
      // … 9 lines
      const fxDt = fx ? fx.clock(wallDt) : 0;
      // … 360 lines
      agents.sync(sim, net, alpha, detailed, rig.viewport.zoom, {
        // … 13 lines
        exhaust: (x, y, z, angle, length, speed, dusty) => {
          if (optimizer.exhaust) exhaust.emit(x, y, z, angle, length, speed, dusty);
        // … 1 lines
        },
        // … 1 lines
        ...(fx ? { ragdolls: (citizens: Parameters<PlayEffects['frame']>[0]) => fx!.frame(citizens, sim, wallDt, fxDt) } : {}),
        hiddenPed: (id) => fx?.hides(id) ?? false,
      });
      // … 2 lines
      if (optimizer.exhaust) exhaust.tick(windClock, renderer.domElement.height / 2);
      fx?.update(wallDt, fxDt);
      // … 108 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 26 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 1 lines
      const shake = fx?.shake() ?? 0;
      if (shake > 0) {
        shakeOffset.set((Math.random() - 0.5) * shake, (Math.random() - 0.5) * shake, (Math.random() - 0.5) * shake);
        rig.camera.position.add(shakeOffset);
        rig.camera.updateMatrixWorld();
      }
      // … 42 lines
      {
        // … 43 lines
        elementLayer?.emit(Math.min(0.1, Math.max(0, delta)), rig.camera.position, exhaust);
      // … 3 lines
      }
      // … 139 lines
      if (shake > 0) { rig.camera.position.sub(shakeOffset); rig.camera.updateMatrixWorld(); }
    // … 14 lines
    },
    // … 63 lines
    dispose() {
      // … 8 lines
      fx?.dispose();
      // … 3 lines
      exhaust.dispose();
    // … 36 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:456-456 slice of src/render/renderer.ts#createSceneRenderer.movingCasters (shared): the feature's statements; "…" marks code left out
  const movingCasters = (): boolean => agents.casting || transit.moving || (fx?.castersMoving() ?? false);

// src/render/renderer.ts:1001-1018 slice of src/render/renderer.ts#createSceneRenderer.loadEffects (shared): the feature's statements; "…" marks code left out
  const loadEffects = (): Promise<void> => fxLoading ??= import('./playEffects').then((mod) => {
    fx = mod.createPlayEffects({
      scene, exhaust, agents,
      buildings: {
        chunkOf: (b) => buildings.chunkOf(b),
        setRuined: (ruined) => buildings.setRuined(ruined),
        lotHeightAt: (id, x, y) => buildings.lotHeightAt(id, x, y),
      },
      renderedHeightAt: (x, y) => terrain.renderedHeightAt(x, y),
      naturalRenderedHeightAt: (x, y) => builtGroundAt(x, y),
      pavedHeightAt: (x, y) => pavedHeightAt(x, y),
      camera: () => rig.camera,
      onAssetsReady,
      lights: fxLights,
    });
    fx.onBuildingDown((id) => { for (const listener of buildingDownListeners) listener(id); });
  // … 1 lines
  });

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
