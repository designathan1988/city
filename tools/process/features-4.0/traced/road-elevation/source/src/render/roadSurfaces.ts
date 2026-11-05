// The code of src/render/roadSurfaces.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaces.ts:267-280 slice of src/render/roadSurfaces.ts#buildRoadSurfaces (shared): the feature's statements; "…" marks code left out
export function buildRoadSurfaces(
  // … 1 lines
  elevation: RoadElevation,
// … 4 lines
): RoadSurfaces {
  const steps = roadSurfaceSteps(net, elevation, materials, terrainAt, reuse, groundMaterial);
  let step = steps.next();
  while (!step.done) step = steps.next();
  step.value.adopt();
  return step.value;
}

// src/render/roadSurfaces.ts:283-794 slice of src/render/roadSurfaces.ts#roadSurfaceSteps (shared): the feature's statements; "…" marks code left out
export function* roadSurfaceSteps(
  // … 1 lines
  elevation: RoadElevation,
// … 8 lines
): Generator<'wait' | void, RoadSurfaces, void> {
  // … 40 lines
  for (const pass of passes) {
    // … 9 lines
    const raised = passRaised(net, pass, elevation, terrainAt);
    // … 2 lines
    const pageReads = passReads(net, elevation, terrainAt, inputs, pass.id);
    const look = (id: SegmentId): RoadLook | undefined => pageReads.look(id);
    const edgesAt = (key: number): ArrayLike<number> | undefined => pageReads.footwayEdges(key);
    // … 3 lines
    const desc: PassDesc = passDesc(net, pass, raised, passLook(materials, groundMaterial), paintColours);
    const specs = passSurfaces(desc, pageReads).map((read) => dress(read, raised, materials, groundMaterial, paintFor));
    const groups = drawGroups(specs);
    // … 7 lines
    for (const spec of specs) {
      if (spec.source.kind === 'paint') paintSalt.set(spec.source.color, specSalt(spec));
      else salt.add(specSalt(spec));
    }
    // … 3 lines
    for (const spec of specs) passSalt.addText(spec.options.name).addText(spec.options.material.uuid).add(spec.renderOrder ?? -1);
    // … 13 lines
    const reach = SURFACE_READ_REACH + 1;
    const reached = (rect: TileRect): boolean => changed === null
      || changed.some((b) => b[0] <= rect[2] + reach && b[2] >= rect[0] - reach && b[1] <= rect[3] + reach && b[3] >= rect[1] - reach);
    // … 2 lines
    if (incremental) {
      // … 9 lines
      for (const b of changed!) over(b, reach);
    // … 2 lines
    } else visit = inputs.tileKeys(pass.id);
    // … 8 lines
    for (const key of visit) {
      // … 16 lines
      if (reuse) {
        // … 1 lines
        const depend = known !== undefined && !reached(rect) ? known : reuse.dependsOn(rect[0] - 1, rect[1] - 1, rect[2] + 1, rect[3] + 1);
        depends.set(key, depend);
        digest.add(depend);
      }
    // … 11 lines
    }
    // … 15 lines
    const dataFor = (rects: readonly TileRect[]): { data: SurfaceData; transfer: ArrayBuffer[] } => {
      // … 2 lines
      const packed = elevation.fieldFor!(around, pass.chain ?? []);
      const field = packed.data;
      const transfer = [...packed.transfer];
      // … 2 lines
      for (const id of [...field.ids as unknown as SegmentId[], ...(pass.chain ?? [])]) {
        if (seen.has(id)) continue;
        seen.add(id);
        const found = look(id);
        if (found) lookList.push([id, found]);
      }
      // … 2 lines
      for (const rect of around) {
        // … 1 lines
        if (raised) continue;
        for (let cx = Math.floor(rect[0] / FOOTWAY_EDGE_CELL); cx <= Math.floor(rect[2] / FOOTWAY_EDGE_CELL); cx++) {
          for (let cy = Math.floor(rect[1] / FOOTWAY_EDGE_CELL); cy <= Math.floor(rect[3] / FOOTWAY_EDGE_CELL); cy++) {
            // … 1 lines
            if (edgesAt(key)) edgeKeys.add(key);
          }
        }
      }
      // … 1 lines
      transfer.push(...flush.transfer);
      const edgeLists = [...edgeKeys].map((key) => Float64Array.from(edgesAt(key)!));
      const joined = concat(edgeLists);
      const edgeData = { keys: Float64Array.from(edgeKeys), ends: joined.lineEnds, xy: joined.lines };
      for (const array of [edgeData.keys, edgeData.ends, edgeData.xy]) transfer.push(array.buffer as ArrayBuffer);
      return { data: { field, looks: lookList, flush: flush.data, ramps: [...ramps].map(rampData), edges: edgeData }, transfer };
    };
    // … 3 lines
    const offPage = tiler && elevation.fieldFor ? tiler : null;
    // … 2 lines
    const buildAll = function* (list: readonly PassTile[], done: (tile: PassTile) => void): Generator<'wait' | void, void, void> {
      // … 28 lines
      if (offPage && list.length) for (let k = 0; k < IN_FLIGHT; k++) askNext();
      // … 1 lines
      for (let from = 0; from < list.length; from += SHAPE_BATCH) {
        // … 2 lines
        if (job && !failed) {
          while (!job.answer.tiles && !job.answer.failed) { offPage!.check(); yield 'wait'; }
        // … 4 lines
        }
        for (let k = from; k < Math.min(list.length, from + SHAPE_BATCH); k++) {
          // … 3 lines
          const bundle = made ? made[k - from]! : reading(() => tileBundle(ringsOf(tile.key)!, tile.rect, specs));
          tile.bundle = bundle;
          tile.surfaces = [...bundle.keys()];
        // … 4 lines
        }
      // … 1 lines
      }
    };
    // … 32 lines
    for (const group of groups) parts.set(group.name, new Map());
    for (const chunk of affected) {
      // … 2 lines
      for (const group of groups) {
        const lists = group.specs.map((spec) => inChunk.filter((tile) => tile.surfaces!.includes(spec.options.name)));
        if (lists.some((list) => list.length)) { parts.get(group.name)!.set(chunk, lists); continue; }
        // No tile of the block has a surface of the group any more: its mesh goes.
        state.blocks.get(group.name)?.delete(chunk);
        const gone = chunks?.get(keyOf(group, chunk));
        if (gone) { retire(gone.mesh); chunks!.delete(keyOf(group, chunk)); }
      }
    }
    // … 1 lines
    for (const group of groups) {
      for (const [chunk, lists] of parts.get(group.name) ?? []) {
        const known = chunks?.get(keyOf(group, chunk));
        if (known && sameTiles(known.parts, lists)) continue;
        for (const tile of lists.flat()) if (!tile.bundle) again.add(tile);
      }
    }
    // … 23 lines
    for (const group of groups) {
      for (const [chunk, lists] of parts.get(group.name) ?? []) {
        const key = keyOf(group, chunk);
        const known = chunks?.get(key);
        // The group's surfaces that have tiles in the block, each with its tiles: drawn one after the other in one mesh.
        const members = group.specs.flatMap((spec, i) => (lists[i]!.length ? [{ spec, list: lists[i]! }] : []));
        let mesh: Mesh | null;
        // DO NOT let the mesh hold its tiles for it: a second copy of every road on the GPU, kept on the CPU
        const sources = members.map(({ spec, list: tiles }) => surfaceSource(spec, tiles.map((tile) => ({ rect: tile.rect, key: tile.key })), reading, ringsOf));
        const source = sources.length === 1 ? sources[0]! : (): Tile[] => sources.flatMap((s) => s());
        if (known && sameTiles(known.parts, lists)) {
          mesh = known.mesh;
          // Bound to this build's rings: the build that made it lets go of its own.
          tilesOnGpu(mesh, source);
          chunksKept++;
        } else {
          if (known) retire(known.mesh);
          // The block's box and centre, to lock its edge in the far level and to tell near from far (`roadLod.ts`).
          // Each surface's own: its block was its tiles' box, a surface of a group keeps it (its far level locked to it, near by it).
          const coarsen = reuse?.near && tiler?.coarsen ? tiler.coarsen.bind(tiler) : null;
          const locks = coarsen ? members.map(({ list: tiles }) => blockBox(tiles.map((tile) => tile.rect))) : null;
          const lock = locks?.[0] ?? null;
          const near = locks ? locks.some((b) => reuse!.near!((b[0] + b[2]) / 2, (b[1] + b[3]) / 2)) : true;
          const memberTiles = members.map(({ spec, list: tiles }) => tiles.map((tile) => tile.bundle!.get(spec.options.name)!));
          // DO NOT name `memberTiles` in a closure here: every closure of this block shares one context (V8), and `levels.bring`, kept as long as the block, held through it every tile's arrays (174 MB in a large city, a heap snapshot of 2026-10-20). The far level cuts by their sizes alone.
          const shares = memberSizes(memberTiles);
          mesh = mergeTiles(memberTiles.flat(), group.options, source,
            lock ? {
              lock, near, also: locks!.slice(1), cell: chunk,
              start: (block, arrays) => {
                coarsening++;
                levelsOf(block)!.pending = true;
                // Each surface simplified on its own, as when it was a block of its own; one too small to gain is drawn whole in the far level.
                const pieces = shares.length === 1 ? [cloneBlock(arrays)] : splitBlock(arrays, shares);
                const whole = shares.length === 1 ? [] : splitBlock(arrays, shares);
                Promise.all(pieces.map((piece, i) => coarsen!(piece, locks![i]!))).then((made) => {
                  // The block may have been built again meanwhile: its far level is of the one it was made for.
                  if (chunks?.get(key)?.mesh !== block) return;
                  if (made.some((m) => m !== null)) {
                    const coarse = made.length === 1 ? made[0]! : joinBlocks(made.map((m, i) => m ?? whole[i]!));
                    const level = attachFarLevel(block, coarse, group.options, sources.map((s, i) => ({ source: s, lock: locks![i]! })), near);
                    // Drawn by its look's batch (`farBlockBatches.ts`): copied there, never uploaded on its own.
                    if (reuse?.farBatches) reuse.farBatches.adopt(level);
                    else if (!near) void warmLooseAhead([level]);
                  } else if (!near) void warmLooseAhead([block]);
                }).catch((error: unknown) => {
                  // No far level for this block: it stays at full detail, sent to the GPU.
                  console.warn('[roads] the far level of a block could not be made', error);
                  if (chunks?.get(key)?.mesh === block) void warmLooseAhead([block]);
                }).finally(() => { coarsening--; const levels = levelsOf(block); if (levels) levels.pending = false; });
              },
            } : undefined);
          // Its full level, when it comes back near, copied from the kept tiles (`reuse.stored`): DO NOT cut and triangulate the block's tiles on the page again (`roadResidency.ts` makeNear).
          const levels = mesh && lock ? levelsOf(mesh) : undefined;
          if (mesh && levels && reuse?.stored) {
            const stored = reuse.stored, material = group.options.material;
            const wanted = members.map(({ spec, list: tiles }) => ({ name: spec.options.name, keys: tiles.map((tile) => tile.stored) }));
            levels.bring = async (): Promise<BlockArrays | null> => {
              const got = await stored.fetch(wanted.flatMap((w) => w.keys));
              const read: Tile[] = [];
              for (const { name, keys } of wanted) {
                for (const k of keys) {
                  const tile = got.get(k)?.get(name);
                  if (!tile) return null;
                  read.push(tile);
                }
              }
              return blockArraysOf(read, material);
            };
          }
          // Its far level made again when its cell comes back from the network's far level (`roadResidency.ts`): the full level's arrays (kept tiles, else cut here) simplified by the workers, a surface at a time as `start` does.
          if (mesh && levels && coarsen) {
            const material = group.options.material;
            levels.remake = async (): Promise<BlockArrays | null> => {
              const full = ((await levels.bring?.()) as BlockArrays | null | undefined) ?? blockArraysOf(sources.flatMap((s) => s()), material);
              if (!full) return null;
              const whole = shares.length === 1 ? [] : splitBlock(full, shares);
              const pieces = shares.length === 1 ? [full] : splitBlock(full, shares);
              const made = await Promise.all(pieces.map((piece, i) => coarsen(piece, locks![i]!)));
              if (made.every((m) => m === null)) return null;
              return made.length === 1 ? made[0]! : joinBlocks(made.map((m, i) => m ?? whole[i]!));
            };
          }
          if (mesh && group.renderOrder !== undefined) mesh.renderOrder = group.renderOrder;
          // DO NOT wait for the camera to frame it: a block out of view was never uploaded and kept its copy on the CPU
          if (mesh && !(lock && !near)) void warmLooseAhead([mesh]);
          if (mesh) chunks?.set(key, { parts: tileSignature(lists), mesh });
          else chunks?.delete(key);
          chunksMerged++;
          yield;
        }
        let order = state.blocks.get(group.name);
        if (!order) state.blocks.set(group.name, order = new Map());
        if (!mesh) { order.delete(chunk); continue; }
        // Where a fresh build draws it: by its first tile holding a surface of the group.
        order.set(chunk, Math.min(...members.map((member) => member.list[0]!.key)));
        if (!chunks) own.set(key, mesh);
      }
    }
    // … 1 lines
    for (const drawn of groups) {
      const order = state.blocks.get(drawn.name);
      if (!order) continue;
      for (const [chunk] of [...order].sort((p, q) => p[1] - q[1])) {
        const key = keyOf(drawn, chunk);
        const mesh = chunks?.get(key)?.mesh ?? own.get(key);
        if (!mesh) continue;
        usedChunks.add(key);
        // A kept block stays where it is drawn until the swap (`adopt`).
        if (mesh.parent === null) addBlock(group, mesh);
        meshes.push(mesh);
        triangles += (mesh.geometry.index?.count ?? 0) / 3;
      }
    }
  // … 12 lines
  }
  // … 14 lines
  return {
    // … 2 lines
    triangles,
  // … 14 lines
  };
}
