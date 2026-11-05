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
  // … 2590 lines
  const handle: SceneHandle = {
    // … 218 lines
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
    // … 883 lines
    dispose() {
      // … 17 lines
      polePreview?.dispose();
    // … 31 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:2063-2074 slice of src/render/renderer.ts#createSceneRenderer.syncUtilities (shared): the feature's statements; "…" marks code left out
  const syncUtilities = (net: Network, groundAt: (x: number, y: number) => number): void => {
    // … 3 lines
    if (!utilities) {
      // … 2 lines
      world.add(utilities.group);
    }
  // … 2 lines
  };

// src/render/renderer.ts:2506-2544 slice of src/render/renderer.ts#createSceneRenderer.warmRoadShaders (shared): the feature's statements; "…" marks code left out
  const warmRoadShaders = (): void => {
    // … 2 lines
    try {
      // … 14 lines
      const parts = [
        buildRoadSurfaces(warmNet, warmElevation, materials, flat, undefined, terrain.vergeMaterial),
        buildStructureDetails(warmNet, warmElevation, flat, materials),
        buildUtilities(warmNet, poleGroundAt(warmElevation, flat), sceneryKit),
      ];
      for (const part of parts) group.add(part.group);
      // … 8 lines
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
