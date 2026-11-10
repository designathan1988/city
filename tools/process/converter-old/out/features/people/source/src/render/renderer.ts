// The code of src/render/renderer.ts that is the feature's (the rest of the file is shared).

// src/render/renderer.ts:227-396 slice of src/render/renderer.ts#SceneHandle (shared): the feature's statements; "…" marks code left out
export interface SceneHandle {
  // … 106 lines
  wound(x: number, y: number, z: number, dirX: number, dirY: number, severed: Severable | null): void;
  // … 1 lines
  shootBody(a: readonly [number, number, number], b: readonly [number, number, number]): { alive: number; part: BodyPart } | 'hit' | null;
// … 59 lines
}

// src/render/renderer.ts:432-4205 slice of src/render/renderer.ts#createSceneRenderer (shared): the feature's statements; "…" marks code left out
export function createSceneRenderer(
// … 11 lines
): SceneHandle {
  // … 722 lines
  const programs = createShaderCatalogue(personProgramStandIns, lightVariants);
  // … 117 lines
  const pavedHeightAt: PavingSource = Object.assign((x: number, y: number): number => {
  // … 2 lines
  }, {
    pack: (rect: readonly [number, number, number, number]): { data: PavingData; transfer: ArrayBuffer[] } | null => {
      // … 3 lines
      const flush = worldNet ? packFlush(worldNet, [rect]) : null;
      return { data: { rect, field: field?.data ?? null, flush: flush?.data ?? null },
        transfer: [...field?.transfer ?? [], ...flush?.transfer ?? []] };
    },
  });
  // … 1737 lines
  const handle: SceneHandle = {
    // … 96 lines
    get programsReady() {
      return programs.ready;
    },
    // … 24 lines
    openingState() {
      return { roadsIn: roads !== null && mapRoadsIn, coarsePending: buildings.coarsePending, viewPending: buildings.viewPending, treesStarted: natureTreesStarted,
        treesPending: natureTreesPending, programs: openingPrograms, worked: reveal.worked, worldBusy: worldJob !== null, fillPending: openingFillPending, fillCellsLeft: fillLeft?.size ?? 0 };
    },
    beginLoad() {
      // … 1 lines
      programs.begin();
    // … 14 lines
    },
    // … 118 lines
    draw(net, sim, alpha, delta, options) {
      // … 396 lines
      if (optimizer.wear) {
        for (const ped of sim.pedViews) if (ped.v > 0.05) wear.feet(ped.x, ped.y, wallDt);
      // … 2 lines
      }
      // … 103 lines
      if (renderer.shadowMap.enabled && optimizer.shadowDue()) {
        // … 26 lines
        if (environment.shadowPass({ changed: shadowMoved, effects: simRunning && (fx?.castersMoving() ?? false), still, keep: viewStill, patch, opened: reveal.opened, view: crowdFrustum })) {
        // … 1 lines
        }
      // … 1 lines
      }
      // … 122 lines
      if (!reveal.opened) {
        // … 16 lines
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
        if (!revealedPrograms) { revealedPrograms = true; programs.afterReveal(); }
      // … 64 lines
      } else lookAgain();
    // … 16 lines
    },
  // … 114 lines
  };
// … 1 lines
}

// src/render/renderer.ts:1267-1267 slice of src/render/renderer.ts#createSceneRenderer.footwayRiseHere (shared): the feature's statements; "…" marks code left out
  const footwayRiseHere = (x: number, y: number): number => (worldNet ? footwayRiseAt(worldNet, x, y) : FOOTWAY_RISE);

// src/render/renderer.ts:1269-1276 slice of src/render/renderer.ts#createSceneRenderer.levelGroundDigest (shared): the feature's statements; "…" marks code left out
  const levelGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    // … 1 lines
    const flush = worldNet ? packFlush(worldNet, [[minX, minY, maxX, maxY]]).data : null;
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
  // … 3 lines
  };

// src/render/renderer.ts:1278-1283 slice of src/render/renderer.ts#createSceneRenderer.landGroundDigest (shared): the feature's statements; "…" marks code left out
  const landGroundDigest = (minX: number, minY: number, maxX: number, maxY: number): number => {
    // … 1 lines
    const flush = worldNet ? packFlush(worldNet, [[minX, minY, maxX, maxY]]).data : null;
    if (flush) out.addAll(flush.rects).addAll(flush.lineEnds).addAll(flush.lines).addPlain(flush.roads);
  // … 1 lines
  };
