// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 48 lines
  setPolePreview(net: Network, preview: PolePreviewInput | null): void;
// … 119 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 535 lines
  const furniture = createStreetFurniture(sceneryKit);
  world.add(furniture.group, furniture.grass);
  // … 1 lines
  let utilities: PoleCells | null = null;
  // … 35 lines
  let polePreview: Utilities | null = null;
  // … 1 lines
  const signs = createSignLayer();
  // … 1 lines
  signs.useCopier((picture, sheet, x, y) => renderer.copyTextureToTexture(picture, sheet, null, new Vector2(x, y)));
  // … 2 lines
  const barriers = createBarriers();
  // … 365 lines
  const signals: SignalHeads = createSignalHeads(scene, deckHeight);
  // … 96 lines
  renderer.domElement.addEventListener('webglcontextrestored', () => {
    // … 4 lines
    signs.refill();
  // … 1 lines
  });
  // … 592 lines
  engine.register({
    // … 1 lines
    refresh: (): number => {
      // … 2 lines
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
      // … 1 lines
      if (!signs.group.parent) world.add(signs.group);
      return signs.sync(placedOn, elevation);
    },
  }, 'draw');
  // … 1 lines
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
    // … 149 lines
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
    // … 30 lines
    draw(net, sim, alpha, delta, options) {
      // … 95 lines
      if (!gardens) {
        // … 1 lines
        world.add(gardens.layer.group);
      }
      // … 186 lines
      for (const mesh of furniture.meshes) {
        mesh.visible = quality.detailProps && detailed && (!plantMap || !isLeafCards(mesh));
      }
      // … 4 lines
      if (plantMap !== plantMapFor.map || gardens !== plantMapFor.gardens) {
        // … 2 lines
        furniture.setMap(plantMap);
      }
      // … 58 lines
      {
        const view = propView(cullCamera, crowdFrustum, screenHalf(), rig.target.y);
        const props = [...(gardens && detailed ? [gardens.layer] : []), ...(quality.detailProps && detailed ? [furniture.layer] : [])];
        if (props.length) lookProps(props, view);
        utilities?.look(view, performance.now() + POLE_FRAME_MS);
      }
      furniture.cull(crowdFrustum, crowdProjection);
      signs.cull(crowdFrustum, crowdProjection);
      barriers.cull(crowdFrustum, crowdProjection);
      // … 50 lines
      signals.sync(sim.signals, net, detailed, crowdFrustum);
    // … 373 lines
    },
    // … 63 lines
    dispose() {
      // … 14 lines
      barriers.dispose();
      // … 2 lines
      polePreview?.dispose();
      utilities?.dispose();
      signs.dispose();
      // … 2 lines
      signals.dispose();
      // … 7 lines
      furniture.dispose();
    // … 18 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:2063-2074 slice of src/render/renderer.ts#createSceneRenderer.syncUtilities (shared): the feature's statements; "…" marks code left out
  const syncUtilities = (net: Network, groundAt: (x: number, y: number) => number): void => {
    // … 1 lines
    if (utilities) utilities.source(net, groundAt);
    // … 1 lines
    if (!utilities) {
      utilities = new PoleCells(sceneryKit);
      utilities.source(net, groundAt);
      world.add(utilities.group);
    }
    utilities.follow(net.doc.changes);
  // … 1 lines
  };

// src/render/renderer.ts:2076-2082 slice of src/render/renderer.ts#createSceneRenderer.rebuildUtilities (shared): the feature's statements; "…" marks code left out
  const rebuildUtilities = (net: Network): void => {
    // … 3 lines
    syncUtilities(net, poleGroundAt(elevation, terrain.renderedHeightAt, net));
  // … 1 lines
  };

// src/render/renderer.ts:2397-2501 slice of src/render/renderer.ts#createSceneRenderer.worldSteps (shared): the feature's statements; "…" marks code left out
  function* worldSteps(net: Network, compares: readonly (BlockCompare | null)[], solved: { readonly cause: number; readonly elevationMs: number },
    started: number, sliced: boolean, region: TerrainRegion | null, levelLand: boolean): Generator<'wait' | void, void, void> {
    // … 94 lines
    syncUtilities(net, poleGroundAt(solve, terrain.renderedHeightAt, net));
  // … 7 lines
  }

// src/render/renderer.ts:2506-2544 slice of src/render/renderer.ts#createSceneRenderer.warmRoadShaders (shared): the feature's statements; "…" marks code left out
  const warmRoadShaders = (): void => {
    // … 2 lines
    try {
      // … 8 lines
      doc.addPoleSpan(pole1.id, pole2.id);
      // … 5 lines
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
