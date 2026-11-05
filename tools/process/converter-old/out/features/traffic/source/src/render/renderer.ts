// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:187-197 slice of src/render/renderer.ts#BlastHit (shared): the feature's statements; "…" marks code left out
export interface BlastHit {
  // … 7 lines
  readonly vehicles: readonly { x: number; y: number; angle: number; length: number; width: number; height: number; color: number; id?: number; archetype?: Archetype }[];
// … 1 lines
}

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 120 lines
  animProbe(id: number): ReturnType<AgentMeshes['animProbe']>;
  // … 3 lines
  meshProbe(): ReturnType<AgentMeshes['meshProbe']>;
  // … 5 lines
  dropVehicle(v: { id: number; archetype: Archetype; x: number; y: number; angle: number; color: number; dirX: number; dirY: number }): void;
  // … 35 lines
  census(): ReturnType<AgentMeshes['census']>;
// … 1 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 541 lines
  const exhaust = createExhaust();
  scene.add(exhaust.points);
  // … 311 lines
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
  // … 59 lines
  const VEHICLE_LOW = TERRAIN_MIN_HEIGHT - m(16), VEHICLE_HIGH = TERRAIN_MAX_HEIGHT + m(64) + VEHICLE_REACH * 0.3;
  // … 4 lines
  agentParts.add(...agents.meshes);
  // … 3 lines
  const densityCars = createDensityCars(deckHeight);
  scene.add(densityCars.object);
  // … 1 lines
  environment.setMovers([
    // … 1 lines
    { roots: agents.meshes, moving: () => simRunning && agents.casting, maxCascade: 1 },
  // … 2 lines
  ]);
  for (const root of [...agents.meshes, transit.group]) moverRoots.add(root);
  // … 1643 lines
  const handle: SceneHandle = {
    // … 33 lines
    meshProbe: () => agents.meshProbe(),
    animProbe: (id) => agents.animProbe(id),
    forceClip: (id, clip) => agents.forceClip(id, clip as never),
    driverBody: (vehicle, x, y) => agents.driverBody(vehicle, x, y),
    // … 5 lines
    census: () => agents.census(),
    // … 130 lines
    strikeGround(x, y, strength) {
      // … 5 lines
      exhaust.burst(x, y, terrain.renderedHeightAt(x, y), 20 + strength * 6, 1, m(1 + strength * 0.4), m(3), 5);
    },
    // … 82 lines
    draw(net, sim, alpha, delta, options) {
      // … 369 lines
      frustumGroundBox(crowdFrustum, VEHICLE_REACH, VEHICLE_LOW, VEHICLE_HIGH, vehicleBox);
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
      // … 69 lines
      if (Math.abs(dark - lastDark) > 0.01) {
        // … 3 lines
        agents.setNight(dark);
      }
      // … 34 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 1 lines
        if (agents.castersChanged) shadowMoved = true;
        // … 24 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 49 lines
      {
        // … 43 lines
        elementLayer?.emit(Math.min(0.1, Math.max(0, delta)), rig.camera.position, exhaust);
      // … 3 lines
      }
      // … 10 lines
      agents.renderPalettes(renderer);
    // … 143 lines
    },
    // … 63 lines
    dispose() {
      // … 11 lines
      densityCars.dispose();
      exhaust.dispose();
      // … 8 lines
      agents.dispose();
    // … 27 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:456-456 slice of src/render/renderer.ts#createSceneRenderer.movingCasters (shared): the feature's statements; "…" marks code left out
  const movingCasters = (): boolean => agents.casting || transit.moving || (fx?.castersMoving() ?? false);

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
