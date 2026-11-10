// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 14 lines
  shadowStats(): (ReturnType<SunCascades['takeStats']> & { readonly unknown: readonly string[]; readonly widest: readonly string[]; readonly changers: readonly string[]; readonly near: readonly string[] }) | null;
  // … 129 lines
  offerElevation(elevation: RoadElevation, networkRevision: number): void;
  // … 1 lines
  roadsSolve(): { readonly elevation: RoadElevation; readonly terrainRevision: number } | null;
// … 21 lines
}

// src/render/renderer.ts:399-399 slice of src/render/renderer.ts#SURFACE_SETS (shared): the feature's statements; "…" marks code left out
const SURFACE_SETS: readonly ReadonlySet<RoadStructure>[] = [GROUND_ONLY, new Set<RoadStructure>(['elevated', 'bridge'])];

// src/render/renderer.ts:412-412 slice of src/render/renderer.ts#isLevelLand (shared): the feature's statements; "…" marks code left out
const isLevelLand = (net: Network): boolean => net.doc.terrainRelief === RELIEF_FLAT && net.doc.terrainStamps.length === 0;

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 4 lines
  onEcologyRead(onAssetsReady);
  // … 57 lines
  sunShadowChunks();
  // … 339 lines
  const swaying: Movers = {
    maxCascade: 0,
    keptPast: true,
    get roots() {
      if (swayChanged) { swayList = [...swayMeshes]; swayChanged = false; }
      return swayList;
    },
    // The wind is never still (`wind.ts` uWindStrength): what it sways moves every frame.
    moving: () => true,
    fixedSize: true,
  };
  // … 72 lines
  const environment = createEnvironment(scene, renderer, {
    shadows: quality.shadows,
    shadowMapSize: quality.shadowMapSize,
  });
  // … 8 lines
  const sceneryKit: SceneryKit = createSceneryKit();
  // … 1 lines
  const terrain: TerrainSurface = createFlatAtlas(anisotropy);
  // … 1 lines
  terrain.onLevels = () => { landTopFor = -1; };
  // … 3 lines
  rig.setGround((x, y) => terrain.renderedHeightAt(x, y));
  // … 2 lines
  scene.add(...terrain.meshes);
  // … 6 lines
  holdStill(world);
  // … 4 lines
  const roadPrograms = roadProgramStandIns(materials, terrain.vergeMaterial, (color) => roadPaint.get(color) ?? roadPaint.get(LANE_LINE) ?? materials.asphalt);
  world.add(roadPrograms.group);
  // … 13 lines
  const furniture = createStreetFurniture(sceneryKit);
  world.add(furniture.group, furniture.grass);
  // … 47 lines
  let elevation: RoadElevation | null = null;
  // … 56 lines
  const surfaceReuse: SurfaceReuse = {
    // … 6 lines
    dependsOn: (minX, minY, maxX, maxY) => new Digest()
      .add(elevation?.digest(minX, minY, maxX, maxY, SURFACE_READ_REACH) ?? 0)
      .add(terrain.digest(minX, minY, maxX, maxY))
      .value(),
  // … 32 lines
  };
  // … 13 lines
  let offeredElevation: { elevation: RoadElevation; revision: number; terrain: number } | null = null;
  // … 56 lines
  let fillRects: Rect[] | null = null;
  // … 84 lines
  const pavedHeightAt: PavingSource = Object.assign((x: number, y: number): number => {
    if (!elevation || !elevation.has(GROUND_ONLY)) return NaN;
    return pavedHeightFrom(elevation, GROUND_ONLY, footwayRiseHere, x, y);
  }, {
    pack: (rect: readonly [number, number, number, number]): { data: PavingData; transfer: ArrayBuffer[] } | null => {
      const ground = elevation !== null && elevation.has(GROUND_ONLY);
      if (ground && !elevation!.fieldFor) return null;
      const field = ground ? elevation!.fieldFor!([rect], []) : null;
      // … 1 lines
      return { data: { rect, field: field?.data ?? null, flush: flush?.data ?? null },
        transfer: [...field?.transfer ?? [], ...flush?.transfer ?? []] };
    },
  });
  // … 1 lines
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
  holdStill(agentParts);
  // … 5 lines
  environment.setMovers([
    // The people and the vehicles cast into the two nearest cascades: in the far one their shadows are under a texel's spread and redrew a million triangles a frame (the lab's shadow pass, 2026-10-20).
    { roots: agents.meshes, moving: () => simRunning && agents.casting, maxCascade: 1 },
    { roots: [transit.group], moving: () => simRunning && transit.moving },
    swaying,
  ]);
  for (const root of [...agents.meshes, transit.group]) moverRoots.add(root);
  // … 50 lines
  holdStill(buildings.group);
  // … 4 lines
  environment.setCasters((texel, cascade, half) => {
    // The effects panel's buildings without shadows: out of every cascade's pass (`buildingSwitches.ts`).
    buildingSwitcher.casters(effectSwitches, texel);
    // The lab's optimisations: the parts cast by a wider texel, the masses from another cascade (`optimizationsApply.ts`).
    // Past the first cascade, the parts also by their size in it (`CAST_RADIUS_SHARE`); the first keeps every part as wide as its texel.
    buildings.castInto(texel === null ? null : texel * optimizer.castTexel, cascade >= optimizer.coarseFrom, cascade > 0 ? half : 0);
    castCastersInto(texel);
  });
  // … 44 lines
  const coverKit = createGroundCoverKit();
  // … 1 lines
  let natureTreeKit: NatureTreeKit | null = null;
  // … 1 lines
  let coverCells: PlantCells | null = null;
  let natureCards: PlantCells | null = null;
  let natureForest: PlantCells | null = null;
  let paintedForest: PlantCells | null = null;
  let plantedForest: PlantCells | null = null;
  // … 44 lines
  const rain = createRain();
  const lightning = createLightning();
  // … 4 lines
  scene.add(rain.object, lightning.group);
  // … 12 lines
  const fogTexture = createFogTexture();
  let fogLayer: FogLayer | null = null;
  const fogField = createFogField(fogTexture);
  // … 79 lines
  const groundChanges: GroundRecord = {
    get version() {
      const diary = groundDiary;
      if (!diary || diary.version === groundScanned) return groundVersion;
      const fresh = diary.since(groundScanned, GROUND_KINDS);
      if (fresh === null) groundVersion = diary.version;
      else if (fresh.length) groundVersion = fresh[fresh.length - 1]!.serial;
      groundScanned = diary.version;
      return groundVersion;
    },
    touches: (since, area) => groundDiary?.touches(since, area, GROUND_KINDS) ?? false,
    // DO NOT leave it out: without it the gardens asked every planted building of the city whether the ground moved under it
    rectsSince: (since) => groundRectsSince(since),
  };
  // … 1 lines
  const groundSince: GroundSince = {
    get version() { return groundChanges.version; },
  // … 1 lines
  };
  // … 91 lines
  const SITE_APRON = TERRAIN_CELL * 1.5;
  const sitesGraded = siteGrading(builtGroundAt, pavedHeightAt, SITE_APRON, (b) => bankBox(b), floors);
  // … 3 lines
  const BANK_REACH = TERRAIN_CELL * 1.5 + m(40) + TERRAIN_CELL;
  // … 25 lines
  const groundRead = new WeakMap<RoadElevation, Map<number, number>>();
  // … 1 lines
  const groundReading = new WeakMap<RoadElevation, Map<number, Promise<void>>>();
  // … 181 lines
  interface Shaping {
    readonly ask: GroundShapeAsk;
    readonly take: (answers: readonly PlateShapeAnswer[]) => boolean;
    /** What the ground's readers are told, once the heights are in place (the diary's 'ground'). */
    readonly then: () => void;
    answers: PlateShapeAnswer[] | null;
    failed: boolean;
    landed: boolean;
    /** The page's time: the ask put together and the answer put in place. */
    ms: number;
  }
  // … 100 lines
  engine.register({
    // … 1 lines
    refresh: (): number => {
      if (!placedOn || !elevation) return 0;
      // … 1 lines
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
      if (!placedOn || !elevation) return 0;
      // … 1 lines
      return signs.sync(placedOn, elevation);
    },
  }, 'draw');
  // … 1 lines
  engine.register({
    // … 2 lines
    refresh: (): number => {
      if (!placedOn) return 0;
      // … 2 lines
      const work = barriers.sync(placedOn.doc, terrain.renderedHeightAt);
      builtTriangles += barriers.triangles - before;
      return work;
    },
  }, 'draw');
  engine.register({
    // … 2 lines
    refresh: (): number => {
      const net = placedOn;
      if (!net) return 0;
      // … 3 lines
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
  // … 4 lines
  let landDirty: Rect | 'all' | null = null;
  // … 64 lines
  const roadsRead = new RoadsRead<ReturnType<typeof roadsToRead>>();
  // … 420 lines
  const optimizer = createOptimizer({ renderer, scene, environment, waterMeshes: () => waterMeshes() });
  // … 100 lines
  const NATURE_SPACING = WOODS_SPACING;
  // … 11 lines
  interface NaturePlate {
    readonly key: string; readonly part: TerrainPart; readonly odds: Float32Array; readonly band: Uint8Array; readonly di: number; readonly dj: number; readonly sum: number;
    /** Its trees as last picked, and the scale they were picked at: picked again only when either moved. */
    picked: { readonly scale: number; readonly trees: readonly Candidate[] } | null;
    /** The trees of `picked` kept on open ground (its own indices, their ground), and what they were kept for. */
    kept: { readonly key: string; readonly of: readonly Candidate[]; readonly idx: readonly number[]; readonly z: readonly number[] } | null;
  }
  // … 171 lines
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
    // … 6 lines
    setGrid(on) {
      // … 1 lines
      TERRAIN_GRID.value = [GRID_CELL, on ? 0.08 : 0];
    // … 1 lines
    },
    // … 6 lines
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
        // … 7 lines
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
    // … 12 lines
    shadowStats() {
      const stats = environment.shadowStats();
      // … 7 lines
      return stats ? { ...stats, unknown: [...casterUnknown], widest, changers, near } : null;
    },
    // … 13 lines
    openingState() {
      return { roadsIn: roads !== null && mapRoadsIn, coarsePending: buildings.coarsePending, viewPending: buildings.viewPending, treesStarted: natureTreesStarted,
        treesPending: natureTreesPending, programs: openingPrograms, worked: reveal.worked, worldBusy: worldJob !== null, fillPending: openingFillPending, fillCellsLeft: fillLeft?.size ?? 0 };
    },
    // … 25 lines
    roadsSolve() {
      return elevation ? { elevation, terrainRevision } : null;
    },
    surfaceHeightAt(x, y) {
      const ground = terrain.renderedHeightAt(x, y);
      if (!elevation) return ground;
      let top = ground;
      // … 1 lines
      for (const set of SURFACE_SETS) {
        if (!elevation.has(set)) continue;
        const road = elevation.roadAt(x, y, set);
        if (road.type < 0 || Math.abs(road.across) > road.half) continue;
        top = Math.max(top, elevation.at(x, y, set));
      }
      return top;
    },
    // … 5 lines
    strikeGround(x, y, strength) {
      // … 5 lines
      exhaust.burst(x, y, terrain.renderedHeightAt(x, y), 20 + strength * 6, 1, m(1 + strength * 0.4), m(3), 5);
    },
    // … 8 lines
    setSmog: (k) => environment.setSmog(k),
    // … 4 lines
    busy: () => (fx?.busy() ?? false) || natureTreesPending || cameraEasing || (terrain.waiting?.() ?? false),
    targetBytes: () => renderTargetsBytes([...post.targets(), ...environment.shadowMaps()]),
    // … 1 lines
    drifting: () => ((fogMoving || placedCloudsShown) && post.enabled && (quality.cloudShadows || quality.skyClouds)) || weatherActive,
    // … 5 lines
    setTransitPreview(preview) {
      // … 8 lines
      transitPreview.set(preview?.points ?? [], preview?.mode ?? 'train', terrain.renderedHeightAt, pavedHeightAt);
    },
    // … 4 lines
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
    setPerspective(on) {
      // … 5 lines
      post.setNight(Math.max(0, lastDark));
    // … 2 lines
    },
    // … 14 lines
    elevationAt(x, y, structure) {
      if (!elevation) return terrain.renderedHeightAt(x, y);
      return elevation.at(x, y, structure ? new Set([structure]) : undefined);
    },
    // … 2 lines
    draw(net, sim, alpha, delta, options) {
      // … 20 lines
      const stroking = !!options?.holdRoads && !!elevation && networkRevision === net.revision;
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
        const atRebuild = performance.now();
        frameMark('frame:rebuild-start');
        rebuildWorld(net);
        frameMark('frame:rebuild');
        measureHitch('hitch:draw/Rebuild', atRebuild);
        terrain.settle();
      }
      // … 9 lines
      if (!levelLand && !(worldJob && worldJobGround) && gradedFor !== net.doc.buildings.revision) {
        // … 1 lines
        const changed = sitesGraded.changedSites(net.doc.buildings);
        if (changed) shapeGround(net, changed.map(terrainRegion), true);
      }
      // … 2 lines
      const groundSettled = !stroking && !worldJob;
      // … 1 lines
      if (groundSettled) { placedOn = net; engine.refresh('draw'); }
      // … 4 lines
      if ((groundSettled || (!stroking && groundIn)) && groundChanges.version !== buildingGroundSeen) {
        const moved = buildingGroundSeen < 0 ? null : buildingsOnGroundSince(net.doc, buildingGroundSeen);
        if (moved === null || moved.length > 0) buildingGround = String(groundChanges.version);
        buildingGroundSeen = groundChanges.version;
      }
      // … 1 lines
      if (roads !== null && (reveal.opened || (groundIn && networkRevision === net.revision))) {
        // … 5 lines
        buildings.update(net.doc, terrain.renderedHeightAt, buildingGround, pavedHeightAt, builtGroundAt,
          (since) => (since === '' ? null : buildingsOnGroundSince(net.doc, Number(since))), levelLand ? levelGroundDigest : landGroundDigest);
      // … 3 lines
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
        keepNet = net;
        // What the tests read that changed since the last look, and where (null: the diary cannot say).
        const entries = vegetationRead.read(net.doc.changes);
        let rects: ChangeRect[] | null = entries === null ? null : [];
        for (const entry of entries ?? []) {
          if (entry.rects === null) { rects = null; break; }
          rects!.push(...entry.rects);
        }
        // The stones, the scrub and the painted woods: swept again only in the cells the paint reached (`coverSweep.ts`).
        {
          const at = performance.now();
          const paint = coverPaintRead.read(net.doc.changes);
          if (paint === null || paint.some((e) => e.rects === null)) sweepAllCovers(net.doc);
          else if (paint.length) {
            const cells = new Set<number>();
            for (const e of paint) for (const r of e.rects!) keepCellsOver(r, COVER_SWEEP_PAD, cells);
            resweepCovers(cells);
          }
          shorePlates.follow(terrain.parts);
          measureHitch('hitch:cover/sweep', at);
        }
        // The painted woods' trees are the countryside's (`natureTrees.ts`); the old card trees only if those cannot be grown.
        const standIns = !natureTreeKit && !natureTreesPending;
        let coverWhole = vegetationRedraw || coverStandIns !== standIns;
        // The cells whose places changed, of every kind (the painted woods' are read by the cover's card trees and by the forest).
        const changed = new Set<number>();
        // The stones take the colour of the painted rock under them: drawn again where the geology changed.
        const geology = terrain.geologyChanges.version;
        if (geology !== coverGeologySeen) {
          const painted = coverGeologySeen < 0 ? null : terrain.geologyChanges.rectsSince(coverGeologySeen);
          if (painted === null) coverWhole = true;
          else for (const r of painted) keepCellsOver(r, TERRAIN_CELL, changed);
          coverGeologySeen = geology;
        }
        {
          const at = performance.now();
          if (rects === null) for (const keep of coverKinds) keep.refreshAll();
          else if (rects.length) for (const keep of coverKinds) keep.refresh(rects, KEEP_REACH);
          for (const keep of coverKinds) {
            const cells = keep.takeChangedCells();
            if (cells === null) coverWhole = true; else for (const cell of cells) changed.add(cell);
          }
          if (coverWhole || changed.size) {
            // DO NOT gather every cell of the map unless every cell is drawn again.
            coverCellsOf().sync(coverWhole ? null : changed, coverWhole ? allCoverCells() : []);
            paintedForestOf()?.sync(coverWhole ? null : changed, coverWhole ? coverKeeps.forest.cellKeys() : []);
          }
          coverStandIns = standIns;
          measureHitch('hitch:cover', at);
        }
        // The ecosystem's own trees.
        bringPlateLand(net.doc);
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
        // The trees the player planted (`world/trees.ts`), with the woods' own models: filed again only in the cells planted or cut
        if (natureTreeKit) {
          const planted = plantedRead.read(net.doc.changes);
          const plantedWhole = planted === null || planted.some((e) => e.rects === null);
          if (plantedWhole) plantAll(net.doc);
          else if (planted.length) {
            const cells = new Set<number>();
            for (const e of planted) for (const r of e.rects!) keepCellsOver(r, 0, cells);
            for (const key of cells) plantedKeeps.fill(key, plantedIn(net.doc, key), (a, b) => a === b);
          }
          // DO NOT test every tree again right after `plantAll` tested them on the same roads and buildings: a map just opened reads no rectangle (`rects` null), and every tree of a city of 70,000 buildings was tested twice in one frame, the buildings' cover cells made twice (the first ones let go for their budget): a page task of 3.8 s behind the curtain (trace of step-70k, 2026-10-22).
          if (rects === null) { if (!plantedWhole) plantedKeeps.refreshAll(); } else if (rects.length) plantedKeeps.refresh(rects, KEEP_REACH);
          const cells = plantedKeeps.takeChangedCells();
          if (cells === null || cells.size || vegetationRedraw) {
            const forest = plantedForestOf()!;
            const whole = cells === null || vegetationRedraw;
            forest.sync(whole ? null : cells, whole ? plantedKeeps.cellKeys() : []);
          }
        }
        vegetationRedraw = false;
      }
      // … 19 lines
      const plantMap = rig.viewport.zoom < PLANT_MAP_ZOOM;
      // … 1 lines
      {
        // … 21 lines
        gpuLedger.set('shadow maps', 'shadow maps', renderTargetsBytes(environment.shadowMaps()));
        // … 4 lines
        if (pendingStandIn.length && elevation && net.revision === net.doc.revision) {
          roadStandIn.show(net, elevation, pendingStandIn.splice(0), net.revision);
        }
        // … 2 lines
        const shadow = environment.sun.shadow;
        const texel = environment.sun.castShadow ? (shadow.camera.right - shadow.camera.left) / shadow.camera.zoom / shadow.mapSize.x : Infinity;
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
      // … 1 lines
      if (pendingRoadFlash.length && elevation) {
        const pos: number[] = [], ring: number[] = [];
        for (const id of pendingRoadFlash.splice(0)) {
          const ribbon = net.ribbons.get(id);
          if (!ribbon) continue;
          const line = ribbon.full, half = sidewalkHalf(ribbon.road);
          const at = (s: number, side: number): [number, number, number] => {
            const f = line.sampleAt(Math.min(s, line.length));
            const x = f.p.x + f.n.x * half * side, y = f.p.y + f.n.y * half * side;
            return [x, elevation!.onSegment(id, f.p.x, f.p.y) + m(0.3), -y];
          };
          // The road itself only: cut where its ends meet a junction (its trims), not running into the road it joins.
          const trims = net.trims.get(id);
          const from = trims ? Math.max(0, ...Object.values(trims.a)) : 0;
          const to = line.length - (trims ? Math.max(0, ...Object.values(trims.b)) : 0);
          const step = m(1.5);
          for (let s = from; s < to; s += step) {
            const e = Math.min(to, s + step);
            const l0 = at(s, 1), r0 = at(s, -1), l1 = at(e, 1), r1 = at(e, -1);
            pos.push(...l0, ...r0, ...r1, ...l0, ...r1, ...l1);
            for (let k = 0; k < 6; k++) ring.push(0);
          }
        }
        if (pos.length) addBlink(pos, ring);
      }
      // … 10 lines
      advanceWind(windClock);
      // … 8 lines
      const covered = renderer.shadowMap.enabled && environment.shadowCover(cascadeCover, CASCADE_KEEP_MARGIN)
        && Math.max(cascadeCover.max.x - cascadeCover.min.x, cascadeCover.max.z - cascadeCover.min.z) <= CASCADE_KEEP_MOST;
      crowdFrustum.keep = covered ? cascadeCover : null;
      crowdFrustum.keepTexel = environment.shadowNearTexel();
      // … 7 lines
      setShadowSun(environment.toSun.x, environment.toSun.y, environment.toSun.z);
      // … 1 lines
      {
        // … 1 lines
        const props = [...(gardens && detailed ? [gardens.layer] : []), ...(quality.detailProps && detailed ? [furniture.layer] : [])];
        if (props.length) lookProps(props, view);
      // … 1 lines
      }
      furniture.cull(crowdFrustum, crowdProjection);
      // … 3 lines
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.cull(crowdFrustum, crowdProjection);
      // … 1 lines
      gardens?.cull(crowdFrustum, crowdProjection);
      transit.update(sim.trains, terrain.renderedHeightAt);
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
      // … 21 lines
      for (const light of roomLights) light.intensity = cutSpec && light.userData['used'] ? 55 * TUNED_UNIT ** light.decay * (0.25 + 0.75 * lastDark) : 0;
      // … 13 lines
      {
        // … 1 lines
        const weather = net.doc.weather;
        // … 1 lines
        const wind = windVector(weather, m(1));
        cloudDrift.x += wind.x * step;
        cloudDrift.y += wind.y * step;
        // … 1 lines
        setWindWeather(wind.x, -wind.y, weather.wind);
        // … 1 lines
        const windLength = Math.hypot(wind.x, wind.y);
        const angle = (weather.windDirection * Math.PI) / 180;
        terrain.setWaterLook({
          waves: Math.min(1, weather.waves + weather.wind / 40),
          foam: weather.foam,
          current: weather.current * m(1),
          windX: windLength > 1e-6 ? wind.x / windLength : Math.cos(angle),
          windZ: windLength > 1e-6 ? -wind.y / windLength : -Math.sin(angle),
          windSpeed: windLength,
        });
        // … 1 lines
        if (weather.lightning > 0 && Math.random() < (step * weather.lightning) / 60) {
          const a = Math.random() * Math.PI * 2, r = Math.sqrt(Math.random()) * span * 0.45;
          strikeAt(target.x + Math.cos(a) * r, -target.z + Math.sin(a) * r);
        }
        // … 1 lines
        const flash = lightning.update(step, canvasSize);
        const storm = Math.min(1, weather.lightning / 8);
        environment.setWeather(Math.max(weather.rain * 0.85, storm * 0.6), flash);
        rainWind.set(wind.x, -wind.y);
        rain.update(weather.rain, target, span, rainWind, step);
        sceneryKit.setHaze(Math.max(weather.rain, storm));
        weatherActive = weather.rain > 0 || weather.lightning > 0 || flash > 0 || (weather.wind > 0 && placedCloudsShown);
      }
      const dark = environment.setTimeOfDay(clock);
      // … 1 lines
      post.setDirectShare(environment.directShare());
      if (Math.abs(dark - lastDark) > 0.01) {
        lastDark = dark;
        post.setNight(dark);
        sceneryKit.setNight(dark);
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
        // … 2 lines
        const moving = net.doc.weather.wind > 0 || (simRunning && movingCasters());
        if (castersMoved && !moving) shadowMoved = true;
        castersMoved = moving;
        // … 19 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
          renderer.shadowMap.needsUpdate = true;
        }
      // … 1 lines
      }
      // … 8 lines
      terrain.setSun(sunTowards.copy(environment.sun.position).sub(environment.sun.target.position));
      {
        // … 11 lines
        if (net.doc.gullyRevision !== gulliesFor) {
          // … 1 lines
          terrain.setGullies(net.doc.gullyDabs, net.doc.gullyAuto);
        }
        // … 1 lines
        terrain.focus?.(centre.x, centre.y, halfHeight);
        // … 1 lines
        terrain.lookFrom?.(rig.camera, renderer.domElement.height * optimizer.terrainPixels);
        // … 4 lines
        if (roadFocus.frames++ % 6 === 0) {
          // … 6 lines
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
        const f = doc.fogSettings;
        // … 2 lines
        if (elementKit && !landHeld) {
          // … 4 lines
          if (entries !== null && entries.every((e) => e.rects !== null)) {
            // … 2 lines
            for (const e of entries) for (const r of e.rects!) keepCellsOver(r, TERRAIN_CELL, cells);
          }
          if ((cells === null || cells.size > 0) && layer.sync(cells, doc, (x, y) => terrain.renderedHeightAt(x, y))) {
            // What came and went is told by its boxes (`cellInstances.ts` reconcile, `castBoxes`): no store thrown away for it.
            layer.cull(crowdFrustum, crowdProjection);
          }
        // … 1 lines
        }
        // … 1 lines
        post.setGroundFog(fogLayer?.any ? { texture: fogLayer.texture, low: fogLayer.low, high: fogLayer.high, density: f.density, window: fogLayer.window } : null);
        fogMoving = !!fogLayer?.any;
      // … 1 lines
      }
      // … 1 lines
      const rainNow = net.doc.weather.rain;
      const air = rainNow > 0 ? { ...atmosphere, fog: Math.max(atmosphere.fog, rainNow * 0.2), fogHeight: Math.max(atmosphere.fogHeight, m(120)) } : atmosphere;
      // … 3 lines
      post.setAtmosphere(air, sunTowards.copy(environment.sun.position).sub(environment.sun.target.position), environment.skyColor,
        sunLight.copy(environment.sun.color).multiplyScalar(environment.sun.intensity), eyeShift);
      // … 2 lines
      agents.renderPalettes(renderer);
      // … 7 lines
      updateMoving(scene);
      // … 5 lines
      if (!reveal.opened) {
        // … 9 lines
        const whole = roadsIn && !buildings.coarsePending && (mapView || !buildings.viewPending)
          && natureTreesStarted && !natureTreesPending;
        // … 5 lines
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
    // … 86 lines
    },
    // … 23 lines
    setEffects(next, nextBuildings = DEFAULT_BUILDING_CHOICES) {
      // … 10 lines
      if (before.shadows !== next.shadows) {
        // … 1 lines
        environment.setQuality({ shadows: quality.shadows && next.shadows, shadowMapSize: quality.shadowMapSize });
      // … 1 lines
      }
    // … 20 lines
    },
    // … 3 lines
    dispose() {
      // … 17 lines
      polePreview?.dispose();
      // … 3 lines
      agents.dispose();
      // … 2 lines
      roadPrograms.dispose();
      // … 5 lines
      furniture.dispose();
      gardens?.dispose();
      for (const plants of [coverCells, natureCards, natureForest, paintedForest, plantedForest]) plants?.dispose();
      coverKit.dispose();
      fogTexture.dispose();
      // … 2 lines
      paintedForest?.dispose();
      plantedForest?.dispose();
      rain.dispose();
      lightning.dispose();
      natureTreeKit?.dispose();
      sceneryKit.dispose();
      terrain.dispose();
      // … 1 lines
      environment.dispose();
    // … 3 lines
    },
  };
// … 1 lines
}

// src/render/renderer.ts:573-583 slice of src/render/renderer.ts#createSceneRenderer.noteSway (shared): the feature's statements; "…" marks code left out
  const noteSway = (object: Object3D, on: boolean): void => {
    // … 2 lines
    if (on) {
      // … 1 lines
      if (!mesh.customDepthMaterial || !sways(mesh.customDepthMaterial)) return;
    // … 2 lines
    } else if (!swayMeshes.delete(mesh)) return;
  // … 1 lines
  };

// src/render/renderer.ts:948-948 slice of src/render/renderer.ts#createSceneRenderer.builtGroundAt (shared): the feature's statements; "…" marks code left out
  const builtGroundAt = (x: number, y: number): number => terrain.naturalRenderedHeightAt(x, y);

// src/render/renderer.ts:1001-1018 slice of src/render/renderer.ts#createSceneRenderer.loadEffects (shared): the feature's statements; "…" marks code left out
  const loadEffects = (): Promise<void> => fxLoading ??= import('./playEffects').then((mod) => {
    fx = mod.createPlayEffects({
      // … 6 lines
      renderedHeightAt: (x, y) => terrain.renderedHeightAt(x, y),
    // … 5 lines
    });
  // … 2 lines
  });

// src/render/renderer.ts:1195-1198 slice of src/render/renderer.ts#createSceneRenderer.requestNetworkFar (shared): the feature's statements; "…" marks code left out
  const requestNetworkFar = (net: Network, heights: RoadElevation): void => {
    networkFarRead.read(net.doc.changes);
    networkFar.request(net, heights, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN, null);
  };

// src/render/renderer.ts:1257-1264 slice of src/render/renderer.ts#createSceneRenderer.deckHeight (shared): the feature's statements; "…" marks code left out
  const deckHeight = (
  // … 3 lines
  ): number => {
    if (!elevation) return terrain.renderedHeightAt(x, y);
  // … 1 lines
  };

// src/render/renderer.ts:1269-1276 slice of src/render/renderer.ts#createSceneRenderer.levelGroundDigest (shared): the feature's statements; "…" marks code left out
  const levelGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(Math.round(builtGroundAt(minX, minY) * 100)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    // … 1 lines
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    // … 1 lines
    if (pads) out.add(pads.apron).addPlain(pads.pads);
    return out.value();
  };

// src/render/renderer.ts:1278-1283 slice of src/render/renderer.ts#createSceneRenderer.landGroundDigest (shared): the feature's statements; "…" marks code left out
  const landGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    const out = new Digest().add(terrain.digest(minX, minY, maxX, maxY)).add(elevation?.digest(minX, minY, maxX, maxY) ?? 0);
    // … 1 lines
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
    return out.value();
  };

// src/render/renderer.ts:1561-1567 slice of src/render/renderer.ts#createSceneRenderer.strikeAt (shared): the feature's statements; "…" marks code left out
  const strikeAt = (x: number, y: number): void => {
    const ground = terrain.renderedHeightAt(x, y);
    // … 1 lines
    lightning.strike(new Vector3(x + (Math.random() - 0.5) * up * 0.3, ground + up, -y + (Math.random() - 0.5) * up * 0.3), new Vector3(x, ground, -y));
  // … 2 lines
  };

// src/render/renderer.ts:1584-1609 slice of src/render/renderer.ts#createSceneRenderer.startNatureTrees (shared): the feature's statements; "…" marks code left out
  const startNatureTrees = (): void => {
    // … 1 lines
    void loadNatureTrees(anisotropy).then(async (kit) => {
      // Its programs built before the first forest is drawn - its shadow's too, wind-swayed (`uploads.ts` shadowStandIn)
      const sample = buildNatureForest(FOREST_SPECIES.flatMap((species) => [0.1, 0.35, 0.6, 0.85].map((seed) => ({ x: 0, y: 0, z: 0, size: m(12), yaw: 0, seed, species }))), kit);
      const lit = new Group(), shadows = new Group();
      for (const mesh of sample.meshes) {
        lit.add(mesh);
        const standIn = shadowStandIn(mesh);
        if (standIn) shadows.add(standIn);
      }
      await Promise.all([compileAhead(lit), compileAhead(shadows, true)]);
      lit.clear();
      shadows.clear();
      sample.dispose();
      natureTreeKit = kit;
      // The kept trees are grown with it at the next look.
      vegetationRedraw = true;
      onAssetsReady();
    }, (error: unknown) => {
      natureTreesPending = false;
      vegetationRedraw = true;
      console.warn('[nature] trees not loaded; the procedural ones stay', error);
      onAssetsReady();
    });
  };

// src/render/renderer.ts:1733-1759 slice of src/render/renderer.ts#createSceneRenderer.watchRoads (shared): the feature's statements; "…" marks code left out
  const watchRoads = (net: Network): void => {
    // … 7 lines
    for (let k = 0; k < 4; k++) {
      // … 3 lines
      const paved = pavedHeightAt(p.x, p.y), land = terrain.renderedHeightAt(p.x, p.y);
      // … 2 lines
      if (Number.isFinite(paved) && land > paved + m(0.3)) { buried = { x: p.x, y: p.y, land, paved }; break; }
    }
    // … 1 lines
    if ((missing || buried) && !roadsReported) {
      roadsReported = true;
      // Once, latched by `roadsReported`; as text, so a headless console log keeps it.
      console.error('roads missing from the picture', JSON.stringify({
        missing, buried, vertices, inWorld: !!roads?.group.parent, worldJob: worldJob !== null,
        terrainRevision: net.doc.terrainRevision, revision: net.revision,
      }));
    } else if (!missing && !buried) roadsReported = false;
  };

// src/render/renderer.ts:1798-1802 slice of src/render/renderer.ts#createSceneRenderer.readOf (shared): the feature's statements; "…" marks code left out
  const readOf = (solve: RoadElevation): Map<number, number> => {
    let known = groundRead.get(solve);
    if (!known) groundRead.set(solve, known = new Map());
    return known;
  };

// src/render/renderer.ts:1804-1812 slice of src/render/renderer.ts#createSceneRenderer.blockRead (shared): the feature's statements; "…" marks code left out
  const blockRead = (solve: RoadElevation, x0: number, y0: number): number => {
    const known = readOf(solve);
    const key = blockKey(x0, y0);
    const kept = known.get(key);
    if (kept !== undefined) return kept;
    const value = blockDigest(solve, terrain.naturalRenderedHeightAt, x0, y0, SHAPE_BLOCK, TERRAIN_CELL, TERRAIN_HALF);
    known.set(key, value);
    return value;
  };

// src/render/renderer.ts:1814-1820 slice of src/render/renderer.ts#createSceneRenderer.platesUnder (shared): the feature's statements; "…" marks code left out
  const platesUnder = (where: readonly { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number }[] | null):
    { readonly cx: number; readonly cy: number; readonly half: number }[] => {
    if (!where) return terrain.parts.map((p) => ({ cx: p.cx, cy: p.cy, half: TERRAIN_HALF }));
  // … 3 lines
  };

// src/render/renderer.ts:1825-1898 slice of src/render/renderer.ts#createSceneRenderer.compareSolves (shared): the feature's statements; "…" marks code left out
  const compareSolves = (before: RoadElevation, after: RoadElevation, landBlocks: readonly [number, number, number, number][] | null): BlockCompare => {
    const at = performance.now();
    const out: BlockCompare = { blocks: [], ready: false, ms: 0 };
    const candidates: [number, number][] = [];
    // Only blocks a road that differs reaches can differ (`differences`)
    const where = after.differences?.(before) ?? null;
    if (!where || where.length) {
      // The blocks of the plates the differences reach; not knowing where, of the plates drawn in full (only their ground is shaped, `shapeToRoads`), never of a world's plates.
      for (const plate of platesUnder(where)) {
        for (let x = plate.cx - plate.half; x < plate.cx + plate.half; x += SHAPE_BLOCK) {
          for (let y = plate.cy - plate.half; y < plate.cy + plate.half; y += SHAPE_BLOCK) {
            const x1 = x + SHAPE_BLOCK, y1 = y + SHAPE_BLOCK;
            if (where && !where.some((r) => r.minX <= x1 && r.maxX >= x && r.minY <= y1 && r.maxY >= y)) continue;
            candidates.push([x, y]);
          }
        }
      }
    }
    const finish = (): void => {
      const changed: [number, number, number, number][] = [];
      for (const [x, y] of candidates) if (blockRead(before, x, y) !== blockRead(after, x, y)) changed.push([x, y, x + SHAPE_BLOCK, y + SHAPE_BLOCK]);
      if (landBlocks) {
        const seen = new Set(changed.map((b) => `${b[0]},${b[1]}`));
        for (const b of landBlocks) if (!seen.has(`${b[0]},${b[1]}`)) changed.push(b);
      }
      out.blocks = changed;
      out.ready = true;
    };
    const waits: Promise<void>[] = [];
    const ask: { solve: RoadElevation; blocks: [number, number][] }[] = [];
    for (const solve of [before, after]) {
      const known = readOf(solve);
      const reading = groundReading.get(solve);
      const mine: [number, number][] = [];
      for (const [x, y] of candidates) {
        const key = blockKey(x, y);
        if (known.has(key)) continue;
        const job = reading?.get(key);
        if (job) { if (!waits.includes(job)) waits.push(job); continue; }
        mine.push([x, y]);
      }
      if (mine.length) ask.push({ solve, blocks: mine });
    }
    if (ask.length && groundWorkers.available && ask.every((a) => a.solve.shapeCut)) {
      // The roads near the blocks asked and the ground under them, as plain numbers; a block the worker cannot read
      const rect = (x: number, y: number): [number, number, number, number] => [x - 1, y - 1, x + SHAPE_BLOCK + 1, y + SHAPE_BLOCK + 1];
      const job: BlockDigestAsk = {
        solves: ask.map(({ solve, blocks }) => ({
          roads: solve.shapeCut!(blocks.map(([x, y]) => rect(x, y))),
          blocks: Float64Array.from(blocks.flat()),
        })),
        plates: terrain.naturalCut?.(ask.flatMap(({ blocks }) => blocks.map(([x, y]) => rect(x, y)))) ?? [],
        block: SHAPE_BLOCK, cell: TERRAIN_CELL, half: TERRAIN_HALF,
      };
      const filed = groundWorkers.digest(job).then((answers) => {
        ask.forEach(({ solve, blocks }, k) => {
          const known = readOf(solve);
          blocks.forEach(([x, y], j) => { const v = answers[k]![j]!; if (!Number.isNaN(v)) known.set(blockKey(x, y), v); });
        });
      }, (error: unknown) => { console.warn('[ground] blocks digested on the page', error); }).finally(() => {
        for (const { solve, blocks } of ask) for (const [x, y] of blocks) groundReading.get(solve)?.delete(blockKey(x, y));
      });
      for (const { solve, blocks } of ask) {
        let reading = groundReading.get(solve);
        if (!reading) groundReading.set(solve, reading = new Map());
        for (const [x, y] of blocks) reading.set(blockKey(x, y), filed);
      }
      waits.push(filed);
    }
    if (!waits.length) { finish(); out.ms = performance.now() - at; return out; }
    out.ms = performance.now() - at;
    void Promise.all(waits).then(() => { const t = performance.now(); finish(); out.ms += performance.now() - t; });
    return out;
  };

// src/render/renderer.ts:1900-1900 slice of src/render/renderer.ts#createSceneRenderer.regionRect (shared): the feature's statements; "…" marks code left out
  const regionRect = (region: TerrainRegion): Rect => terrain.rectOf(region);

// src/render/renderer.ts:1902-1902 slice of src/render/renderer.ts#createSceneRenderer.terrainRegion (shared): the feature's statements; "…" marks code left out
  const terrainRegion = (box: readonly [number, number, number, number]): TerrainRegion => terrain.regionOf(box);

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

// src/render/renderer.ts:1978-1988 slice of src/render/renderer.ts#Shaping (shared): the feature's statements; "…" marks code left out
  interface Shaping {
    readonly ask: GroundShapeAsk;
    readonly take: (answers: readonly PlateShapeAnswer[]) => boolean;
    // … 2 lines
    answers: PlateShapeAnswer[] | null;
  // … 4 lines
  }

// src/render/renderer.ts:1993-2004 slice of src/render/renderer.ts#createSceneRenderer.landShapes (shared): the feature's statements; "…" marks code left out
  const landShapes = (): void => {
    while (shapings.length) {
      // … 4 lines
      next.take(next.answers ?? shapeGroundAsk(next.ask));
    // … 3 lines
    }
  };

// src/render/renderer.ts:2006-2052 slice of src/render/renderer.ts#createSceneRenderer.shapeGround (shared): the feature's statements; "…" marks code left out
  const shapeGround = (net: Network, regions: TerrainRegion | readonly TerrainRegion[] | null = null, sites = false, padsReady = false): { readonly landed: boolean; readonly ms: number } => {
    const at = performance.now();
    const list: readonly TerrainRegion[] | null = regions === null ? null
      : typeof regions[0] === 'number' ? [regions as TerrainRegion] : regions as readonly TerrainRegion[];
    if (list && !list.length) return LANDED;
    const region = list;
    // The ground is cut and filled here, and only here: what stands on it reads where from the diary
    const parent = net.doc.changes.version;
    const told = (): void => {
      if (padsReady) return;
      derived(net, 'ground', list ? list.map(regionRect) : null, sites ? 'lot ground levelled' : 'ground cut and filled', parent);
    };
    const roads = net.doc.segments.size > 0 ? elevation : null;
    followFloors(net, list ? list.map(regionRect) : null);
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
      // The corners asked, and the roads and platforms they read as they stand now, as plain numbers.
      const asking = terrain.askShape(region, region ? null : [...(roads?.shapeBounds() ?? []), ...(pads?.shapeBounds() ?? [])]);
      const boxes = cornerBoxes(asking.plates, TERRAIN_CELL, TERRAIN_HALF);
      const ask: GroundShapeAsk = { roads: roads ? roads.shapeCut!(boxes) : null, pads: pads ? pads.cut(boxes) : null, plates: asking.plates };
      const shaping: Shaping = { ask, take: asking.take, then: told, answers: null, failed: false, landed: false, ms: 0 };
      shapings.push(shaping);
      if (groundWorkers.available) {
        groundWorkers.shape(ask).then((answers) => { shaping.answers = answers; }, (error: unknown) => {
          console.warn('[ground] a pass is shaped on the page', error);
          shaping.failed = true;
        });
      } else shaping.failed = true;
      shaping.ms = performance.now() - at;
      // No worker: put in place now, after the passes before it.
      if (shaping.failed) landShapes();
      return shaping;
    }
    told();
    const shape = pads ? { shapeBounds: () => [...(roads?.shapeBounds() ?? []), ...pads.shapeBounds()], shapeAt: shapeBoth(roads, pads) } : roads;
    terrain.shapeToRoads(shape, region);
    return { landed: true, ms: performance.now() - at };
  };

// src/render/renderer.ts:2076-2082 slice of src/render/renderer.ts#createSceneRenderer.rebuildUtilities (shared): the feature's statements; "…" marks code left out
  const rebuildUtilities = (net: Network): void => {
    // … 3 lines
    syncUtilities(net, poleGroundAt(elevation, terrain.renderedHeightAt, net));
  // … 1 lines
  };

// src/render/renderer.ts:2151-2159 slice of src/render/renderer.ts#createSceneRenderer.landMovedSince (shared): the feature's statements; "…" marks code left out
  const landMovedSince = (entries: readonly Change[] | null): Rect | null => {
    if (entries === null || entries.length === 0) return null;
    let box: Rect | null = null;
    for (const e of entries) {
      if (e.rects === null) return null;
      for (const r of e.rects) box = unionRect(box, [r[0] - 2 * TERRAIN_CELL, r[1] - 2 * TERRAIN_CELL, r[2] + 2 * TERRAIN_CELL, r[3] + 2 * TERRAIN_CELL]);
    }
    return box;
  };

// src/render/renderer.ts:2161-2175 slice of src/render/renderer.ts#createSceneRenderer.blocksOver (shared): the feature's statements; "…" marks code left out
  const blocksOver = (box: Rect): [number, number, number, number][] => {
    const out: [number, number, number, number][] = [];
    for (const key of platesOver(box)) {
      const plate = plateRegion(key);
      const left = plate.cx - plate.half, right = plate.cx + plate.half;
      const bottom = plate.cy - plate.half, top = plate.cy + plate.half;
      if (box[2] <= left || box[0] >= right || box[3] <= bottom || box[1] >= top) continue;
      const x0 = Math.max(left, Math.floor((box[0] - left) / SHAPE_BLOCK) * SHAPE_BLOCK + left);
      const y0 = Math.max(bottom, Math.floor((box[1] - bottom) / SHAPE_BLOCK) * SHAPE_BLOCK + bottom);
      for (let x = x0; x < Math.min(right, box[2]); x += SHAPE_BLOCK) {
        for (let y = y0; y < Math.min(top, box[3]); y += SHAPE_BLOCK) out.push([x, y, x + SHAPE_BLOCK, y + SHAPE_BLOCK]);
      }
    }
    return out;
  };

// src/render/renderer.ts:2177-2187 slice of src/render/renderer.ts#createSceneRenderer.networkExtent (shared): the feature's statements; "…" marks code left out
  const networkExtent = (net: Network): Rect => {
    let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
    for (const n of net.doc.nodes.values()) {
      if (n.x < minX) minX = n.x;
      if (n.y < minY) minY = n.y;
      if (n.x > maxX) maxX = n.x;
      if (n.y > maxY) maxY = n.y;
    }
    if (!Number.isFinite(minX)) return [0, 0, 0, 0];
    return [minX - SHAPE_BLOCK, minY - SHAPE_BLOCK, maxX + SHAPE_BLOCK, maxY + SHAPE_BLOCK];
  };

// src/render/renderer.ts:2214-2215 slice of src/render/renderer.ts#createSceneRenderer.stationsAsked (shared): the feature's statements; "…" marks code left out
  const stationsAsked = (net: Network, roads: ReturnType<typeof roadsToRead>): boolean =>
    roadStations.waiting(roads, (terrain.renderedHeightAt as SharedGroundAt).share?.current ?? null, net.doc.terrainRevision);

// src/render/renderer.ts:2225-2248 slice of src/render/renderer.ts#createSceneRenderer.completeSolve (shared): the feature's statements; "…" marks code left out
  const completeSolve = (net: Network): boolean => {
    // … 1 lines
    if (!partial?.pending) { solveLeft = null; return true; }
    // … 3 lines
    left.order ??= pendingOrder(net, partial.pendingRoads!(), left.x, left.y);
    const scope: SolveScope = { roads: left.order.slice(left.at, left.at + SOLVE_SHARE) };
    const groundAt = isLevelLand(net) ? FLAT_GROUND : builtGroundAt;
    // … 1 lines
    if (stationsAsked(net, roadsToRead(net, groundAt, partial, null, scope))) return false;
    // … 1 lines
    elevation = buildRoadElevation(net, groundAt, partial, roadStations.current, null, scope);
    // … 2 lines
    const moved = elevation.differences?.(partial) ?? null;
    derived(net, 'elevation', moved ? moved.map((r): ChangeRect => [r.minX, r.minY, r.maxX, r.maxY]) : null, 'road heights solved past the opening', worldCause,
      { ms: performance.now() - started, detail: `${elevation.pending ?? 0} roads left` });
    // … 2 lines
    if (elevation.pending) return false;
  // … 2 lines
  };

// src/render/renderer.ts:2252-2256 slice of src/render/renderer.ts#createSceneRenderer.farCellRect (shared): the feature's statements; "…" marks code left out
  const farCellRect = (key: string): Rect => {
    const [i, j] = key.split(',').map(Number) as [number, number];
    const x = TILE_ORIGIN + i * FAR_ROAD_CELL, y = TILE_ORIGIN + j * FAR_ROAD_CELL;
    return [x, y, x + FAR_ROAD_CELL, y + FAR_ROAD_CELL];
  };

// src/render/renderer.ts:2290-2315 slice of src/render/renderer.ts#createSceneRenderer.takeEditedCells (shared): the feature's statements; "…" marks code left out
  const takeEditedCells = (net: Network): void => {
    // … 5 lines
    const pad = SURFACE_READ_REACH;
    const rects: Rect[] = [];
    for (const entry of edits) {
      // … 1 lines
      for (const r of entry.rects ?? []) {
        const [i0, j0] = farCellOf(r[0] - pad, r[1] - pad).split(',').map(Number) as [number, number];
        const [i1, j1] = farCellOf(r[2] + pad, r[3] + pad).split(',').map(Number) as [number, number];
        for (let i = i0; i <= i1; i++) {
          for (let j = j0; j <= j1; j++) {
            const key = `${i},${j}`;
            if (!left.delete(key)) continue;
            fillTaken.add(key);
            fillAwaiting.add(key);
            rects.push(farCellRect(key));
          }
        }
      }
    }
    if (rects.length) surfaceReuse.withinMoved?.push(...rects);
  };

// src/render/renderer.ts:2317-2394 slice of src/render/renderer.ts#createSceneRenderer.buildWorld (shared): the feature's statements; "…" marks code left out
  const buildWorld = (net: Network, full = false): void => {
    // … 12 lines
    const solveScope: SolveScope | null = opening ? { region: [centre.x - reach, centre.y - reach, centre.x + reach, centre.y + reach] } : null;
    // … 16 lines
    const groundAt = levelLand ? FLAT_GROUND : builtGroundAt;
    // … 2 lines
    const fillSolved = full && fillRects !== null && elevation !== null && !elevation.pending && net.state === solvedState;
    const solvedAhead = fillSolved || (offeredElevation !== null && offeredElevation.revision === net.revision && offeredElevation.terrain === net.doc.terrainRevision);
    // … 1 lines
    const stationAsk = roadsRead.of(groundMoved ? null : [net.state, from, ...(solveScope?.region ?? [])], () => roadsToRead(net, groundAt, from, groundMoved, solveScope));
    // … 1 lines
    stationsWait = !solvedAhead && stationsAsked(net, stationAsk);
    // … 12 lines
    elevation = fillSolved && elevation ? elevation : offered && offered.revision === net.revision && offered.terrain === terrainRevision
      ? offered.elevation
      // On land that has not moved, from the last solve
      : buildRoadElevation(net, groundAt, from, roadStations.current, groundMoved, solveScope);
    // … 2 lines
    if (opening) {
      // … 3 lines
      networkFar.request(net, elevation, terrain.renderedHeightAt, FAR_ROAD_CELL, TILE_ORIGIN,
        [lo(centre.x - openingHalf), lo(centre.y - openingHalf), hi(centre.x + openingHalf), hi(centre.y + openingHalf)]);
    }
  // … 13 lines
  };

// src/render/renderer.ts:2397-2501 slice of src/render/renderer.ts#createSceneRenderer.worldSteps (shared): the feature's statements; "…" marks code left out
  function* worldSteps(net: Network, compares: readonly (BlockCompare | null)[], solved: { readonly cause: number; readonly elevationMs: number },
    started: number, sliced: boolean, region: TerrainRegion | null, levelLand: boolean): Generator<'wait' | void, void, void> {
    // … 17 lines
    if (levelLand) {
    // … 2 lines
    } else if (blocks === null) {
      const shaping = shapeGround(net, region);
      while (!shaping.landed) { landShapes(); if (shaping.landed) break; yield 'wait'; }
    } else if (blocks.length || gradedFor !== net.doc.buildings.revision) yield* shapeBlocksSteps(net, blocks);
    // … 14 lines
    const freshRoads = yield* roadSurfaceSteps(net, solve, materials, terrain.renderedHeightAt, surfaceReuse, terrain.vergeMaterial,
      sliced ? roadTiles : null, surfaceBlocks);
    // … 2 lines
    derived(net, 'surfaces', freshRoads.rebuilt, 'road surfaces rebuilt', worldCause,
      { ms: freshRoads.workMs, detail: `${freshRoads.built} tiles built, ${freshRoads.reused} reused` });
    // … 2 lines
    const freshDetails = (details ??= createStructureCells(materials, terrain.vergeMaterial)).prepare(net, solve, terrain.renderedHeightAt, blocks);
    // … 7 lines
    if (sliced) {
      // … 1 lines
      for (const piece of [freshRoads.group, ...freshDetails.fresh]) {
        if (piece.parent === null) stage.add(piece);
      }
    // … 5 lines
    }
    // … 3 lines
    if (roads && roads !== freshRoads) { triangles -= roads.triangles; world.remove(roads.group); roads.dispose(); }
    triangles -= details?.triangles ?? 0;
    // … 1 lines
    roads = freshRoads;
    // … 16 lines
    freshDetails.apply();
    // … 2 lines
    builtTriangles = triangles + roads.triangles + (details?.triangles ?? 0);
    // … 2 lines
    syncUtilities(net, poleGroundAt(solve, terrain.renderedHeightAt, net));
  // … 7 lines
  }

// src/render/renderer.ts:2506-2544 slice of src/render/renderer.ts#createSceneRenderer.warmRoadShaders (shared): the feature's statements; "…" marks code left out
  const warmRoadShaders = (): void => {
    // … 2 lines
    try {
      // … 12 lines
      const warmElevation = buildRoadElevation(warmNet, flat);
      // … 1 lines
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

// src/render/renderer.ts:2587-2624 slice of src/render/renderer.ts#createSceneRenderer.warmPreviewShaders (shared): the feature's statements; "…" marks code left out
  const warmPreviewShaders = (): void => {
    // … 2 lines
    try {
      // … 19 lines
      const plants = plantSamples(sceneryKit);
      group.add(plants.group);
      void compileAhead(plants.depth, true);
      done.push(() => plants.dispose());
    // … 7 lines
    } catch (error) {
    // … 1 lines
    }
  };

// src/render/renderer.ts:2636-2640 slice of src/render/renderer.ts#createSceneRenderer.waterMeshes (shared): the feature's statements; "…" marks code left out
  const waterMeshes = (): Object3D[] => {
    // … 1 lines
    for (const root of terrain.meshes) root.traverse((object) => { if (object.name.includes('water')) found.push(object); });
  // … 1 lines
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
    // … 2 lines
    terrain.setVegetation?.(quality.vegetation);
    // … 3 lines
    environment.setQuality({ shadows: quality.shadows && effectSwitches.shadows, shadowMapSize: quality.shadowMapSize });
  // … 8 lines
  };

// src/render/renderer.ts:2746-2752 slice of src/render/renderer.ts#NaturePlate (shared): the feature's statements; "…" marks code left out
  interface NaturePlate {
    readonly key: string; readonly part: TerrainPart; readonly odds: Float32Array; readonly band: Uint8Array; readonly di: number; readonly dj: number; readonly sum: number;
  // … 4 lines
  }

// src/render/renderer.ts:2758-2766 slice of src/render/renderer.ts#createSceneRenderer.bringPlateLand (shared): the feature's statements; "…" marks code left out
  const bringPlateLand = (doc: RoadDoc): void => {
    // … 2 lines
    for (const part of terrain.parts) {
      const box: ChangeRect = [part.cx - TERRAIN_HALF - TERRAIN_CELL, part.cy - TERRAIN_HALF - TERRAIN_CELL, part.cx + TERRAIN_HALF + TERRAIN_CELL, part.cy + TERRAIN_HALF + TERRAIN_CELL];
      const reached = entries === null || entries.some((e) => e.rects === null || e.rects.some((r) => r[0] <= box[2] && r[2] >= box[0] && r[1] <= box[3] && r[3] >= box[1]));
      if (reached) plateLand.set(part.surface, ++landSerial);
    }
  };

// src/render/renderer.ts:2767-2855 slice of src/render/renderer.ts#createSceneRenderer.natureSweep (shared): the feature's statements; "…" marks code left out
  const natureSweep = (key: string): NatureSweep => {
    if (quality.vegetation <= 0 || !terrain.parts.some((p) => p.surface.ecology())) return { key };
    // … 3 lines
    const cells = Math.floor((TERRAIN_HALF * 2) / NATURE_SPACING);
    // … 4 lines
    for (const part of terrain.parts) {
      const field = part.surface.ecology();
      if (!field) continue;
      // Kept from the last sweep while its own ecology and the land are as they were
      const plateKey = `${part.surface.ecologyRevision}:${part.surface.woodsRevision ?? 0}:${plateLand.get(part.surface) ?? 0}:${vegetation}`;
      const known = naturePlates.get(part.surface);
      if (known && known.key === plateKey) {
        plates.push(known);
        treeSum += known.sum;
        continue;
      }
      // Its density but the ecosystem's share, kept by the plate on its land (`woodsBase.ts`): only that share added here.
      const base = part.surface.woodsBase?.() ?? null;
      if (!base) { pending++; continue; }
      const odds = known?.odds ?? new Float32Array(cells * cells);
      const band = known?.band ?? new Uint8Array(cells * cells);
      let plateSum = 0;
      const di = Math.round(part.cx / NATURE_SPACING), dj = Math.round(part.cy / NATURE_SPACING);
      for (let j = 0; j < cells; j++) {
        for (let i = 0; i < cells; i++) {
          const x = -TERRAIN_HALF + (i + 0.5) * NATURE_SPACING, y = -TERRAIN_HALF + (j + 0.5) * NATURE_SPACING;
          const ix = Math.round((x + TERRAIN_HALF) / TERRAIN_CELL), iy = Math.round((TERRAIN_HALF - y) / TERRAIN_CELL);
          const k = iy * field.side + ix;
          const ecology = Math.min(1, (field.canopy[k] ?? 0) * 0.9 + (field.trees[k] ?? 0) * 0.5 + (field.emergent[k] ?? 0) * 0.3);
          const o = j * cells + i;
          // Nothing grows past the map's edge (`wholeMap.ts`).
          if (!onMap(x + part.cx, y + part.cy)) { odds[o] = 0; band[o] = 0; continue; }
          // `natureDensity` is linear in the ecology: the base (the land's slope and the patches, `woodsBase.ts`) and its share.
          const density = base[o]! + ecology * 0.3;
          // The bands: inner forest, its edge, scattered trees, a rare lone one.
          if (density > 0.62) { odds[o] = 0.92; band[o] = 3; }
          else if (density > 0.5) { odds[o] = 0.25 + (density - 0.5) / 0.12 * 0.6; band[o] = 2; }
          else if (density > 0.38) { odds[o] = 0.025; band[o] = 1; }
          else { odds[o] = 0.003; band[o] = 1; }
          plateSum += odds[o]!;
        }
      }
      const plate: NaturePlate = { key: plateKey, part, odds, band, di, dj, sum: plateSum, picked: null, kept: null };
      naturePlates.set(part.surface, plate);
      plates.push(plate);
      treeSum += plateSum;
    }
    // … 1 lines
    const plateArea = (2 * TERRAIN_HALF) ** 2;
    // … 1 lines
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
          const o = j * cells + i;
          const gi = i + di, gj = j + dj;
          if (cellHash(gi, gj, 41) >= odds[o]! * treeScale) continue;
          const x = part.cx - TERRAIN_HALF + (i + 0.5 + (cellHash(gi, gj, 43) - 0.5) * 0.9) * NATURE_SPACING;
          const y = part.cy - TERRAIN_HALF + (j + 0.5 + (cellHash(gi, gj, 44) - 0.5) * 0.9) * NATURE_SPACING;
          // Tall in the heart of a wood, crowns meeting into one canopy; lower at its edge; short and crooked out in the open.
          const inner = band[o] === 3, edge = band[o] === 2;
          const h = inner ? m(11) + m(8) * cellHash(gi, gj, 45) : edge ? m(8) + m(6) * cellHash(gi, gj, 45) : m(5.5) + m(4) * cellHash(gi, gj, 45);
          const roll = cellHash(gi, gj, 46);
          const species = inner ? (roll < 0.6 ? 'broadleafTall' : 'broadleaf') : roll < 0.25 ? 'broadleafTall' : 'broadleaf';
          // Which of the low-poly trees (`natureTrees.ts`): oaks and cypresses most, a palm now and then out in the open.
          const pick = cellHash(gi, gj, 49);
          const kind = pick < 0.55 ? 'oak' : pick < (inner ? 0.97 : 0.85) ? 'cypress' : 'palm';
          mine.push({ x, y, size: h, yaw: cellHash(gi, gj, 47) * Math.PI * 2, seed: seedOfKind(kind, cellHash(gi, gj, 48)), species });
        }
      }
      plate.picked = { scale: treeScale, trees: mine };
    // … 1 lines
    }
  // … 1 lines
  };

// src/render/renderer.ts:2871-2881 slice of src/render/renderer.ts#createSceneRenderer.natureOpen (shared): the feature's statements; "…" marks code left out
  const natureOpen = (net: Network, x: number, y: number, z: number): boolean => {
    // … 3 lines
    const level = terrain.shoreLevelAt(x, y);
    if (level !== null && level > z - m(0.3)) return false;
    // … 1 lines
    const grade = Math.hypot(terrain.renderedHeightAt(x + g, y) - terrain.renderedHeightAt(x - g, y), terrain.renderedHeightAt(x, y + g) - terrain.renderedHeightAt(x, y - g)) / (2 * g);
    if (grade > 0.75) return false;
  // … 1 lines
  };

// src/render/renderer.ts:2885-2889 slice of src/render/renderer.ts#createSceneRenderer.natureStands (shared): the feature's statements; "…" marks code left out
  const natureStands = (c: Candidate): number | null => {
    // … 1 lines
    const at = terrain.renderedHeightAt(c.x, c.y);
    return natureOpen(net, c.x, c.y, at) && !clearingsOf(net.doc)(c.x, c.y) ? at : null;
  };

// src/render/renderer.ts:2894-2919 slice of src/render/renderer.ts#createSceneRenderer.fileNature (shared): the feature's statements; "…" marks code left out
  const fileNature = (sweep: NatureSweep): void => {
    // … 1 lines
    for (const { plate } of sweep.slices ?? []) {
      const surface = plate.part.surface;
      swept.add(surface);
      // … 1 lines
      const was = natureFiled.get(surface);
      if (was?.trees === trees) continue;
      // … 6 lines
      for (const key of was?.cells ?? []) if (!byCell.has(key)) natureKeeps.fill(key, []);
      // … 2 lines
      natureFiled.set(surface, { trees, cells: [...byCell.keys()] });
    }
  // … 6 lines
  };

// src/render/renderer.ts:2922-2922 slice of src/render/renderer.ts#createSceneRenderer.coverPlain (shared): the feature's statements; "…" marks code left out
  const coverPlain = (c: Candidate): number | null => (coverFree(c.x, c.y) ? terrain.renderedHeightAt(c.x, c.y) : null);

// src/render/renderer.ts:2944-2944 slice of src/render/renderer.ts#createSceneRenderer.plantedIn (shared): the feature's statements; "…" marks code left out
  const plantedIn = (doc: RoadDoc, key: number): PlantedTree[] => doc.treesIn(keepCellRect(key)).filter((t) => keepCellAt(t.x, t.y) === key);

// src/render/renderer.ts:2946-2955 slice of src/render/renderer.ts#createSceneRenderer.plantedForestOf (shared): the feature's statements; "…" marks code left out
  const plantedForestOf = (): PlantCells | null => {
    if (!natureTreeKit) return null;
    const kit = natureTreeKit;
    return plantedForest ??= (() => {
      const made = createTreeCells(kit, 'planted-forest', (cell) => plantedKeeps.standingIn(cell)
        .map(({ c, z }) => ({ x: c.x, y: c.y, z, size: c.height, yaw: c.yaw, seed: c.seed, species: 'broadleaf' as const })));
      world.add(made.group);
      return made;
    })();
  };

// src/render/renderer.ts:2957-2966 slice of src/render/renderer.ts#createSceneRenderer.plantAll (shared): the feature's statements; "…" marks code left out
  const plantAll = (doc: RoadDoc): void => {
    // … 1 lines
    const byCell = new Map<number, PlantedTree[]>();
    for (const t of doc.trees) {
      const key = keepCellAt(t.x, t.y);
      const list = byCell.get(key);
      if (list) list.push(t); else byCell.set(key, [t]);
    }
    for (const [key, list] of byCell) plantedKeeps.fill(key, list);
  };

// src/render/renderer.ts:2982-2989 slice of src/render/renderer.ts#createSceneRenderer.sweepAllCovers (shared): the feature's statements; "…" marks code left out
  const sweepAllCovers = (doc: RoadDoc): void => {
    // … 2 lines
    for (const dab of doc.terrainPaint) {
      if (isCoverKind(dab.kind)) keepCellsOver([dab.x - dab.radius, dab.y - dab.radius, dab.x + dab.radius, dab.y + dab.radius], COVER_SWEEP_PAD, cells);
    }
  // … 1 lines
  };

// src/render/renderer.ts:2993-3002 slice of src/render/renderer.ts#createSceneRenderer.coverCellsOf (shared): the feature's statements; "…" marks code left out
  const coverCellsOf = (): PlantCells => coverCells ??= (() => {
    const made = createCoverCells(coverKit, 'ground-cover', {
      rocksIn: (cell) => [...coverKeeps.rocks.standingIn(cell), ...coverKeeps.shore.standingIn(cell)]
        .map(({ c, z }) => ({ x: c.x, y: c.y, z, size: c.size, yaw: c.yaw, seed: c.seed, rock: terrain.geologyAt(c.x, c.y) })),
      scrubIn: (cell) => coverKeeps.scrub.standingIn(cell).map(({ c, z }) => ({ x: c.x, y: c.y, z, size: c.size, yaw: c.yaw, seed: c.seed })),
      treesIn: (cell) => (!natureTreeKit && !natureTreesPending ? coverKeeps.forest.standingIn(cell).map(({ c, z }) => ({ ...c, z })) : []),
    });
    world.add(made.group);
    return made;
  })();

// src/render/renderer.ts:3004-3012 slice of src/render/renderer.ts#createSceneRenderer.paintedForestOf (shared): the feature's statements; "…" marks code left out
  const paintedForestOf = (): PlantCells | null => {
    if (!natureTreeKit) return null;
    const kit = natureTreeKit;
    return paintedForest ??= (() => {
      const made = createTreeCells(kit, 'painted-forest', (cell) => coverKeeps.forest.standingIn(cell).map(({ c, z }) => ({ ...c, z })));
      world.add(made.group);
      return made;
    })();
  };
