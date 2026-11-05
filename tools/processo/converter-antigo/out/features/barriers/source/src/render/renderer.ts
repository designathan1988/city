// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 581 lines
  const barriers = createBarriers();
  // … 1062 lines
  engine.register({
    // … 1 lines
    refresh: (): number => {
      // … 4 lines
      builtTriangles += furniture.triangles - before;
    // … 2 lines
    },
  }, 'draw');
  // … 10 lines
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 1 lines
      if (!barriers.group.parent) world.add(barriers.group);
      const before = barriers.triangles;
      const work = barriers.sync(placedOn.doc, terrain.renderedHeightAt);
      builtTriangles += barriers.triangles - before;
      return work;
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 12 lines
      builtTriangles += transit.triangles - before;
    // … 1 lines
    },
  }, 'draw');
  // … 892 lines
  const handle: SceneHandle = {
    // … 56 lines
    get stats(): RenderStats {
      return {
        builtTriangles: builtTriangles + buildings.triangles,
      // … 8 lines
      };
    },
    // … 194 lines
    draw(net, sim, alpha, delta, options) {
      // … 363 lines
      barriers.cull(crowdFrustum, crowdProjection);
    // … 424 lines
    },
    // … 63 lines
    dispose() {
      // … 14 lines
      barriers.dispose();
    // … 34 lines
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
