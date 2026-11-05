// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 583 lines
  const transit = createTransit();
  // … 356 lines
  environment.setMovers([
    // … 2 lines
    { roots: [transit.group], moving: () => simRunning && transit.moving },
  // … 1 lines
  ]);
  for (const root of [...agents.meshes, transit.group]) moverRoots.add(root);
  // … 697 lines
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
      // … 4 lines
      builtTriangles += barriers.triangles - before;
    // … 1 lines
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      // … 2 lines
      if (!transit.group.parent) { world.add(transit.group); transit.setXray(transitXray); }
      const before = transit.triangles;
      // … 1 lines
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
  // … 581 lines
  let transitPreview: TrackPreview | null = null;
  // … 310 lines
  const handle: SceneHandle = {
    // … 56 lines
    get stats(): RenderStats {
      return {
        builtTriangles: builtTriangles + buildings.triangles,
      // … 8 lines
      };
    },
    // … 130 lines
    setTransitXray(on) {
      // … 1 lines
      transit.setXray(on);
    },
    setTransitPreview(preview) {
      // … 4 lines
      if (!transitPreview) {
        transitPreview = createTrackPreview();
        world.add(transitPreview.group);
      }
      transitPreview.set(preview?.points ?? [], preview?.mode ?? 'train', terrain.renderedHeightAt, pavedHeightAt);
    },
    // … 49 lines
    draw(net, sim, alpha, delta, options) {
      // … 364 lines
      transit.cull(crowdFrustum, crowdProjection);
      // … 3 lines
      transit.update(sim.trains, terrain.renderedHeightAt);
    // … 419 lines
    },
    // … 63 lines
    dispose() {
      // … 15 lines
      transit.dispose();
      transitPreview?.dispose();
    // … 32 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:456-456 slice of src/render/renderer.ts#createSceneRenderer.movingCasters (shared): the feature's statements; "…" marks code left out
  const movingCasters = (): boolean => agents.casting || transit.moving || (fx?.castersMoving() ?? false);

// src/render/renderer.ts:2587-2624 slice of src/render/renderer.ts#createSceneRenderer.warmPreviewShaders (shared): the feature's statements; "…" marks code left out
  const warmPreviewShaders = (): void => {
    // … 2 lines
    try {
      // … 23 lines
      for (const mode of ['train', 'metro'] as const) {
        const track = buildTrackPreview([{ x: 0, y: 0 }, { x: m(60), y: 0 }], mode, () => 0);
        group.add(track.group);
        done.push(() => track.dispose());
      }
    // … 2 lines
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
