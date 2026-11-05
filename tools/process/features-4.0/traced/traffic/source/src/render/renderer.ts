// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:187-197 slice of src/render/renderer.ts#BlastHit (shared): the feature's statements; "…" marks code left out
export interface BlastHit {
  // … 7 lines
  readonly vehicles: readonly { x: number; y: number; angle: number; length: number; width: number; height: number; color: number; id?: number; archetype?: Archetype }[];
// … 1 lines
}

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 130 lines
  dropVehicle(v: { id: number; archetype: Archetype; x: number; y: number; angle: number; color: number; dirX: number; dirY: number }): void;
// … 37 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 541 lines
  const exhaust = createExhaust();
  scene.add(exhaust.points);
  // … 394 lines
  const densityCars = createDensityCars(deckHeight);
  scene.add(densityCars.object);
  // … 1651 lines
  const handle: SceneHandle = {
    // … 173 lines
    strikeGround(x, y, strength) {
      // … 5 lines
      exhaust.burst(x, y, terrain.renderedHeightAt(x, y), 20 + strength * 6, 1, m(1 + strength * 0.4), m(3), 5);
    },
    // … 82 lines
    draw(net, sim, alpha, delta, options) {
      // … 370 lines
      agents.sync(sim, net, alpha, detailed, rig.viewport.zoom, {
        // … 13 lines
        exhaust: (x, y, z, angle, length, speed, dusty) => {
          if (optimizer.exhaust) exhaust.emit(x, y, z, angle, length, speed, dusty);
        // … 1 lines
        },
      // … 3 lines
      });
      // … 1 lines
      densityCars.update(net, sim, rig.target.x, -rig.target.z);
      if (optimizer.exhaust) exhaust.tick(windClock, renderer.domElement.height / 2);
      // … 190 lines
      {
        // … 43 lines
        elementLayer?.emit(Math.min(0.1, Math.max(0, delta)), rig.camera.position, exhaust);
      // … 3 lines
      }
    // … 154 lines
    },
    // … 63 lines
    dispose() {
      // … 11 lines
      densityCars.dispose();
      exhaust.dispose();
    // … 36 lines
    },
  };
// … 1 lines
}

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
