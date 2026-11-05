// The code of src/render/riggedCitizens.ts that is the feature's (the rest of the file is shared).

// src/render/riggedCitizens.ts:108-110 function companyOf: the feature's own
function companyOf(party: Pick<PartyView, 'size' | 'archetype'>): Company {
  return party.size > 1 ? party.archetype : 'solo';
}

// src/render/riggedCitizens.ts:118-123 function citizenLevelAt: the feature's own
export function citizenLevelAt(zoom: number): number {
  for (let level = CITIZEN_LEVEL_ERROR.length - 1; level > 0; level--) {
    if (CITIZEN_LEVEL_ERROR[level]! * BODY_EXTENT * zoom <= 1) return level;
  }
  return 0;
}

// src/render/riggedCitizens.ts:126-128 function citizenBandAt: the feature's own
export function citizenBandAt(zoom: number): number {
  return zoom >= perM(20) ? 0 : zoom >= perM(5) ? 1 : zoom >= perM(3) ? 2 : 3;
}

// src/render/riggedCitizens.ts:131-133 function citizenLevelFor: the feature's own
export function citizenLevelFor(zoom: number): number {
  return citizenBandAt(zoom) === 0 ? 0 : citizenLevelAt(zoom);
}

// src/render/riggedCitizens.ts:136-138 function citizenLevelOfFigure: the feature's own
export function citizenLevelOfFigure(pixels: number, ratio: number): number {
  return citizenLevelFor(pixels / (BODY_EXTENT * ratio));
}

// src/render/riggedCitizens.ts:141-148 interface ClipIdentity: the feature's own
export interface ClipIdentity {
  readonly seed: number;
  readonly gender: 'f' | 'm';
  readonly ageClass: 'child' | 'adult' | 'elder';
  readonly company: Company;
  readonly companyId: number;
  readonly hasChild?: boolean;
}

// src/render/riggedCitizens.ts:169-174 function seatedChat: the feature's own
function seatedChat(seed: number, time: number): boolean {
  const hash = personHash(seed ^ 0x2f6b1d93);
  if ((hash & 3) === 0) return false; // a quiet traveller
  const cycle = 14 + (hash >>> 4 & 7) * 2;
  return ((time + (hash >>> 8 & 255) / 10) % cycle) < cycle * 0.3;
}

// src/render/riggedCitizens.ts:184-194 function setFacialExpression: the feature's own
function setFacialExpression(mesh: InstancedMesh, source: SkinnedMesh, slot: number, face: FaceWeights): void {
  const influences = source.morphTargetInfluences;
  const targets = source.morphTargetDictionary;
  if (!influences || !targets || !mesh.geometry.morphAttributes.position) return;
  applyFace(influences, targets, face);
  // three allocates an InstancedMesh's morph texture from `count`.
  const visible = mesh.count;
  if (mesh.morphTexture === null) mesh.count = CAPACITY;
  mesh.setMorphAt(slot, source);
  mesh.count = visible;
}

// src/render/riggedCitizens.ts:251-266 function growLevel: the feature's own
function growLevel(level: LevelMesh, needed: number): void {
  const mesh = level.mesh;
  let capacity = Math.max(1, mesh.instanceMatrix.count);
  while (capacity < needed) capacity *= 2;
  capacity = Math.min(CAPACITY, capacity);
  const matrices = new InstancedBufferAttribute(new Float32Array(capacity * 16), 16);
  matrices.array.set(mesh.instanceMatrix.array);
  matrices.setUsage(DynamicDrawUsage);
  const colours = new InstancedBufferAttribute(new Float32Array(capacity * 3).fill(1), 3);
  if (mesh.instanceColor) colours.array.set(mesh.instanceColor.array);
  colours.setUsage(DynamicDrawUsage);
  // DO NOT `dispose` the mesh to let its old buffers go: three's releaseStatesOfObject walks every geometry the renderer knows (WebGLBindingStates), ms a call; the old buffers stay on the GPU till the context goes (less than the new one's size).
  mesh.instanceMatrix = matrices;
  mesh.instanceColor = colours;
  level.colourChanged = true;
}

// src/render/riggedCitizens.ts:370-402 function createRiggedCitizens.maim: the feature's own
  const maim = (batch: CitizenBatch, px: Float32Array, offset: number, limb: Severable): void => {
    let info = maimBones.get(batch);
    if (!info) {
      const source = batch.sources[0];
      info = {};
      if (source) {
        const bones = source.skeleton.bones;
        const parents = bones.map((b) => bones.indexOf(b.parent as never));
        for (const [key, name] of [['armL', 'Bip01_L_Forearm'], ['armR', 'Bip01_R_Forearm'], ['legL', 'Bip01_L_Calf'], ['legR', 'Bip01_R_Calf'], ['head', 'Bip01_Head']] as const) {
          const root = bones.findIndex((b) => b.name === name);
          if (root < 0) { info[key] = null; continue; }
          const set = [root];
          for (let i = 0; i < bones.length; i++) {
            let j = parents[i]!;
            while (j >= 0 && j !== root) j = parents[j]!;
            if (j === root) set.push(i);
          }
          const joint = new Vector3().setFromMatrixPosition(maimTmp.copy(source.skeleton.boneInverses[root]!).invert());
          info[key] = { bones: set, parent: parents[root]!, joint };
        }
      }
      maimBones.set(batch, info);
    }
    const which = info[limb];
    if (!which || which.parent < 0) return;
    maimTmp.fromArray(px, offset + which.parent * SKIN_BONE_FLOATS);
    maimP.copy(which.joint).applyMatrix4(maimTmp);
    for (const i of which.bones) {
      const o = offset + i * SKIN_BONE_FLOATS;
      px.fill(0, o, o + 16);
      px[o + 12] = maimP.x; px[o + 13] = maimP.y; px[o + 14] = maimP.z; px[o + 15] = 1;
    }
  };

// src/render/riggedCitizens.ts:456-457 function createRiggedCitizens.levelOf: the feature's own
  const levelOf = (x: number, y: number, z: number): number =>
    figurePixels ? citizenLevelOfFigure(figurePixels(x, y, z, BODY_EXTENT), figureRatio) : lod;

// src/render/riggedCitizens.ts:749-768 function createRiggedCitizens.pump: the feature's own
  function pump(): void {
    while (activeLoads < 6 && queuedLoads.size > 0) {
      let priority: [number, { priority: boolean; wantedAt: number; resolve: () => void; reject: (error: unknown) => void }] | undefined;
      for (const pair of queuedLoads) if (pair[1].priority) { priority = pair; break; }
      if (!priority && activeLoads >= 5) break; // Keep one slot for a newly visible rider or occupant.
      const [index, entry] = priority ?? queuedLoads.entries().next().value!;
      queuedLoads.delete(index);
      if (!resident.has(index) && performance.now() - entry.wantedAt > 1000) {
        loading.delete(index);
        wantedFrame.delete(index);
        entry.resolve();
        continue;
      }
      activeLoads++;
      void load(index).then(entry.resolve, entry.reject).finally(() => {
        activeLoads--;
        pump();
      });
    }
  }

// src/render/riggedCitizens.ts:770-795 function createRiggedCitizens.request: the feature's own
  function request(index: number, priority = false): Promise<void> {
    // A body outside the reviewed whitelist must never be asked for; the browser checks (`verify:visual`) fail on this error.
    if (!CROWD_IDS.includes(models[index] ?? '')) console.error(`Citizen outside the whitelist requested: ${models[index]}`);
    wantedFrame.set(index, visualFrame);
    const existing = loading.get(index);
    if (existing) {
      const queued = queuedLoads.get(index);
      if (queued) { queued.priority ||= priority; queued.wantedAt = performance.now(); }
      return existing;
    }
    const failedAt = failed.get(index);
    // Failed a moment ago (said then): not drawn, not asked again yet.
    if (failedAt !== undefined && performance.now() - failedAt < RETRY_MS) return Promise.resolve();
    const work = new Promise<void>((resolve, reject) => {
      queuedLoads.set(index, { priority, wantedAt: performance.now(), resolve, reject });
    });
    loading.set(index, work);
    // A load that failed is let go, to be asked again later: DO NOT keep the rejected promise
    work.catch((error: unknown) => {
      console.error(`Citizen ${models[index]} failed to load; asked again in ${RETRY_MS} ms`, error);
      if (loading.get(index) === work) loading.delete(index);
      failed.set(index, performance.now());
    });
    pump();
    return work;
  }

// src/render/riggedCitizens.ts:822-822 function createRiggedCitizens.helmetFits: the feature's own
  const helmetFits = (id: string): boolean => !NO_HELMET.has(id);

// src/render/riggedCitizens.ts:823-825 function createRiggedCitizens.bodyFor: the feature's own
  function bodyFor(ctx: CastingContext): { index: number; size: number } | null {
    return registry.pickCitizenModel(ctx, helmetFits);
  }

// src/render/riggedCitizens.ts:831-836 function createRiggedCitizens.contextOf: the feature's own
  function contextOf(identity: ClipIdentity, helmet: boolean, x: number, y: number): CastingContext {
    scratch.seed = identity.seed; scratch.gender = identity.gender; scratch.ageClass = identity.ageClass;
    scratch.company = identity.company; scratch.companyId = identity.companyId; scratch.hasChild = identity.hasChild;
    scratch.helmet = helmet; scratch.x = x; scratch.y = y;
    return scratch as CastingContext;
  }

// src/render/riggedCitizens.ts:844-863 function createRiggedCitizens.want: the feature's own
  function want(batch: CitizenBatch, at: number): void {
    const job = batch.deferred.get(at);
    if (!job) return;
    batch.deferred.delete(at);
    let waiting = baking.get(batch);
    if (!waiting) baking.set(batch, waiting = new Set());
    waiting.add(at);
    batch.baking = batch.baking.then(async () => {
      // In a worker on the body's rig, timed by its shuffle; here (in slices) only where no worker can.
      batch.rigNodes ??= bakeRig(batch.scene);
      const clip = await bakeDeferredInWorker(batch.rigNodes, batch.sex, at, batch.clips[WALK_SHUFFLE]!) ?? await job.make();
      waiting.delete(at);
      if (disposed || batch.disposed) return;
      batch.clips[at] = clip;
      batch.gait = gaitClips(batch.clips);
      addToAtlas(batch, clip);
      batch.animationBytes += clip.data.byteLength;
      group.userData.animationBytes = (group.userData.animationBytes ?? 0) + clip.data.byteLength;
    }).catch(() => {});
  }

// src/render/riggedCitizens.ts:871-883 function createRiggedCitizens.pose: the feature's own
  function pose(batch: CitizenBatch, row: number, clips: readonly ClipFrames[], phases: readonly number[],
    weights: readonly number[], lost: readonly Severable[] | null): void {
    const single = band >= 2;
    const n = lost ? -1 : gpuPlays(passPlays, batch.atlas, clips, phases, weights, single);
    if (n >= 0) {
      batch.pass.setPlays(row, passLists[n]!, 0);
      return;
    }
    cpuPose(batch.scratch, 0, clips, phases, weights, batch.width, single);
    for (const limb of lost ?? []) maim(batch, batch.scratch, 0, limb);
    batch.pass.setOverride(row, batch.scratch);
    batch.pass.setPlays(row, null, 0);
  }

// src/render/riggedCitizens.ts:886-902 function createRiggedCitizens.place: the feature's own
  function place(batch: CitizenBatch, level: number, instance: Matrix4, colour: Color, face: FaceWeights | undefined): void {
    for (let i = 0; i < batch.parts.length; i++) {
      const part = batch.parts[i]!;
      const at = part.levels[part.at[Math.min(level, part.at.length - 1)]!]!;
      if (at.count >= at.mesh.instanceMatrix.count) growLevel(at, at.count + 1);
      const slot = at.count++;
      matrix.multiplyMatrices(instance, batch.local[i]!);
      at.mesh.setMatrixAt(slot, matrix);
      const c = at.mesh.instanceColor!.array as Float32Array, o = slot * 3;
      if (c[o] !== colour.r || c[o + 1] !== colour.g || c[o + 2] !== colour.b) {
        c[o] = colour.r; c[o + 1] = colour.g; c[o + 2] = colour.b;
        at.colourChanged = true;
      }
      (at.rows.array as Float32Array)[slot] = batch.count;
      if (face) setFacialExpression(at.mesh, batch.sources[i]!, slot, face);
    }
  }

// src/render/riggedCitizens.ts:905-919 function createRiggedCitizens.emit: the feature's own
  function emit(batch: CitizenBatch, clips: readonly ClipFrames[], phases: readonly number[],
    weights: readonly number[], x: number, height: number, y: number, heading: number, scale: number,
    lean = 0, ground: Gradient | null = null, expression?: FaceWeights, lost: readonly Severable[] | null = null, level = lod): void {
    pose(batch, batch.count, clips, phases, weights, lost);
    transform.position.set(x, height, -y);
    // Yaw, then a roll about the body's own forward axis
    transform.rotation.set(0, heading + Math.PI / 2, -lean, 'YXZ');
    transform.scale.set(scale, scale, scale);
    transform.updateMatrix();
    if (ground) shearMatrix(transform.matrix, ground, x, -y);
    // Faces are read only close up: from the nearest level of detail on, no expression is computed or uploaded.
    place(batch, level, transform.matrix, UNBURNT, level === 0 ? expression : undefined);
    batch.count++;
    batch.lastUsed = frameNow;
  }

// src/render/riggedCitizens.ts:922-929 function createRiggedCitizens.grow: the feature's own
  function grow(batch: CitizenBatch): void {
    const before = paletteBytes(batch);
    batch.rows = Math.min(CAPACITY, batch.rows * 2);
    batch.pass.grow(batch.rows);
    batch.uniform.value = batch.pass.texture;
    batch.base.value = batch.pass.base;
    group.userData.paletteBytes += paletteBytes(batch) - before;
  }

// src/render/riggedCitizens.ts:932-939 function createRiggedCitizens.addToAtlas: the feature's own
  function addToAtlas(batch: CitizenBatch, clip: ClipFrames): void {
    const before = paletteBytes(batch);
    const old = batch.atlas.anim.atlas;
    if (!appendClip(batch.atlas, clip)) return;
    batch.pass.setAtlas(batch.atlas.anim.atlas);
    old.dispose();
    group.userData.paletteBytes += paletteBytes(batch) - before;
  }

// src/render/riggedCitizens.ts:944-964 method createRiggedCitizens.begin: the feature's own
    begin(level = 2, zoom = Infinity, pixels: ((x: number, y: number, z: number, height: number) => number) | null = null, ratio = 1) {
      figurePixels = pixels;
      figureRatio = ratio;
      frameNow = performance.now();
      visualFrame++;
      held.begin();
      detail = level;
      registry.beginFrame();
      // The mesh by its error on screen; what the body does by the zoom.
      const nextBand = citizenBandAt(zoom);
      const nextLod = citizenLevelFor(zoom);
      const levelChanged = nextLod !== lod || nextBand !== band;
      lod = nextLod;
      band = nextBand;
      group.userData.lod = lod;
      for (const batch of batches.values()) {
        batch.count = 0;
        for (const part of batch.parts) for (const at of part.levels) at.count = 0;
        if (levelChanged) applyBand(batch);
      }
    },

// src/render/riggedCitizens.ts:967-1058 method createRiggedCitizens.draw: the feature's own
    draw(ped: PedView, x: number, y: number, heading: number, deck: number, alpha: number,
      ground: Gradient | null = null, lean = 0, priority = false) {
      const hash = personHash(ped.id);
      scratch.seed = ped.id; scratch.gender = ped.gender; scratch.ageClass = ped.ageClass; scratch.company = companyOf(ped.party);
      scratch.companyId = ped.party.id; scratch.hasChild = ped.party.hasChild; scratch.helmet = false; scratch.x = x; scratch.y = y;
      const body = bodyFor(scratch as CastingContext);
      if (!body) return;
      const index = body.index;
      const batch = batches.get(index);
      if (!batch) {
        const known = loading.has(index);
        const work = request(index, priority);
        if (!known) void work.catch((error: unknown) => {
          group.userData.error = String(error);
          console.error('Citizen asset could not be loaded', models[index], error);
        });
        return;
      }
      if (batch.count >= CAPACITY) return;
      if (batch.count >= batch.rows) grow(batch);
      const time = Math.max(0, ped.age - (1 - alpha) * DT);
      const scale = body.size * (0.92 + ((hash >>> 8) & 255) / 255 * 0.17);
      let gait = motion.get(ped);
      if (!gait) {
        gait = createGait(ped, time, heading, hash);
        motion.set(ped, gait);
      }
      stepGait(gait, ped, batch.gait, time, heading, m(scale) / m(1), hash);
      const playing = gaitPlays(gait, batch.gait, plays);
      mixClips.length = 0; mixPhases.length = 0; mixWeights.length = 0;
      const carrying = ped.carry !== undefined;
      for (let p = 0; p < playing; p++) {
        const play = plays[p]!;
        const at = (carrying ? CARRY_AT[play.name] : undefined) ?? GAIT_AT[play.name];
        if (batch.deferred.size) want(batch, at);
        mixClips.push(batch.clips[at]!);
        mixPhases.push(play.frame);
        mixWeights.push(play.weight);
      }
      // Every limb lost (shots take more than one), the stump closed at its joint.
      const lost = ped.lost ?? (ped.maimed ? [ped.maimed] : null);
      const level = levelOf(x, y, deck);
      emit(batch, mixClips, mixPhases, mixWeights, x, deck, y, gaitHeading(gait), m(scale), lean, ground,
        level === 0 ? faceAt(ped.id, time, ped.panic ? (ped.gesture?.kind === 'crouch' ? 'cry' : 'panic') : ped.gesture?.kind, CROWD[index]?.person?.mood) : undefined,
        lost, level);
      if (lost) onBleed?.(ped.id, x, y, deck);
      {
        // What they play, kept: the palette is blended from it only if a ragdoll takes them (`capturedPose`).
        let kept = lastPose.get(ped.id);
        if (!kept || kept.palette.length !== batch.width) {
          kept = { index, palette: new Float32Array(batch.width), transform: new Matrix4(), frame: 0, clips: [], phases: [], weights: [], single: false, lost: null };
          lastPose.set(ped.id, kept);
        }
        kept.index = index;
        kept.clips.length = 0; kept.phases.length = 0; kept.weights.length = 0;
        for (let i = 0; i < mixClips.length; i++) { kept.clips.push(mixClips[i]!); kept.phases.push(mixPhases[i]!); kept.weights.push(mixWeights[i]!); }
        kept.single = band >= 2;
        kept.lost = lost;
        kept.transform.copy(transform.matrix);
        kept.frame = visualFrame;
      }
      // In the hand, what the gesture is done with, where the hand is in the clip carrying the most weight this frame.
      if (carrying && band < 2) {
        // The box between the two hands, as wide as they are apart.
        let best = -1;
        for (let i = 0; i < mixWeights.length; i++) if (best < 0 || mixWeights[i]! > mixWeights[best]!) best = i;
        const clip = best >= 0 ? mixClips[best] : undefined;
        if (clip?.hands && mixWeights[best]! > 0) {
          const frame = Math.min(clip.frames, Math.max(0, Math.round(mixPhases[best]!)));
          boxR.fromArray(clip.hands, frame * 32 + 12);
          boxL.fromArray(clip.hands, frame * 32 + 28);
          const width = Math.min(0.5, Math.max(0.24, boxR.distanceTo(boxL) - 0.07));
          boxAt.addVectors(boxR, boxL).multiplyScalar(0.5);
          boxAt.z += 0.03; boxAt.y += 0.02;
          handMatrix.compose(boxAt, boxTurn, boxSize.set(width, 0.3, 0.38));
          handMatrix.premultiply(transform.matrix);
          held.placeMatrix('box', handMatrix);
        }
      }
      const thing = ped.gesture && !carrying ? HELD[ped.gesture.kind] : undefined;
      if (thing && band < 2) {
        let best = -1;
        for (let i = 0; i < mixWeights.length; i++) if (best < 0 || mixWeights[i]! > mixWeights[best]!) best = i;
        const clip = best >= 0 ? mixClips[best] : undefined;
        if (clip?.hands && mixWeights[best]! > 0.5) {
          const frame = Math.min(clip.frames, Math.max(0, Math.round(mixPhases[best]!)));
          handMatrix.fromArray(clip.hands, frame * 32 + (thing.left ? 16 : 0));
          handMatrix.premultiply(transform.matrix);
          held.place(thing.kind, handMatrix, m(scale));
        }
      }
    },

// src/render/riggedCitizens.ts:1060-1135 method createRiggedCitizens.drawClip: the feature's own
    drawClip(identity: ClipIdentity,
      pelvisX: number, pelvisY: number, pelvisHeight: number, heading: number,
      plays: readonly { readonly key: CitizenClipKey; readonly phase: number; readonly weight: number;
        /** For a walk: ground covered, world units; the phase then follows this body's own stride. */
        readonly distance?: number }[],
      lean = 0, maxScale = Infinity, fromGround: boolean | 'pelvisOver' = false, fixedScale = 0,
      helmet: Matrix4 | null = null, face?: 'dead'): number {
      const hash = personHash(identity.seed);
      const body = bodyFor(contextOf(identity, helmet !== null, pelvisX, pelvisY));
      if (!body) return 0;
      const batch = batches.get(body.index);
      if (!batch) {
        const known = loading.has(body.index);
        const work = request(body.index, true);
        if (!known) void work.catch(() => {});
        return 0;
      }
      if (batch.count >= CAPACITY) return 0;
      if (batch.count >= batch.rows) grow(batch);
      mixClips.length = 0;
      mixPhases.length = 0;
      mixWeights.length = 0;
      let pelvis = 0;
      let pelvisLeft = 0;
      let pelvisAhead = 0;
      let total = 0;
      const scale = m(fixedScale > 0 ? fixedScale : Math.min(maxScale, body.size * (0.92 + ((hash >>> 8) & 255) / 255 * 0.17)));
      let headWeight = 0;
      let headClip: ClipFrames | undefined;
      for (const play of plays) {
        const at = play.key === 'walk' ? (identity.ageClass === 'elder' ? WALK_ELDER : WALK)
          : play.key in RIDER_AT ? RIDER_AT[play.key as RiderClipKey] : LIBRARY_AT[play.key as Played];
        if (batch.deferred.size) want(batch, at);
        const clip = batch.clips[at];
        if (!clip || play.weight <= 0) continue;
        // A walk played by distance plants the feet: one cycle per stride of THIS body
        const phase = play.distance !== undefined ? play.distance / Math.max(1e-6, clip.stride * scale) : play.phase;
        const f = clip.loop ? ((phase % 1) + 1) % 1 : Math.min(1, Math.max(0, phase));
        mixClips.push(clip);
        mixPhases.push(f * clip.frames);
        mixWeights.push(play.weight);
        pelvis += clip.pelvisY * play.weight;
        pelvisLeft += clip.pelvisX * play.weight;
        pelvisAhead += clip.pelvisZ * play.weight;
        total += play.weight;
        if (play.weight > headWeight) {
          headWeight = play.weight;
          headClip = clip;
        }
      }
      if (!mixClips.length) return 0;
      pelvis /= total;
      // The model's origin, found back from the pelvis along the leaned up axis
      const drop = fromGround ? 0 : pelvis * scale;
      const leftX = -Math.sin(heading);
      const leftY = Math.cos(heading);
      const aheadX = Math.cos(heading);
      const aheadY = Math.sin(heading);
      const shiftLeft = fromGround === true ? 0 : (pelvisLeft / total) * scale;
      const shiftAhead = fromGround === true ? 0 : (pelvisAhead / total) * scale;
      const originX = pelvisX - leftX * drop * Math.sin(lean) - leftX * shiftLeft - aheadX * shiftAhead;
      const originY = pelvisY - leftY * drop * Math.sin(lean) - leftY * shiftLeft - aheadY * shiftAhead;
      const originHeight = pelvisHeight - drop * Math.cos(lean);
      const level = levelOf(originX, originY, originHeight);
      emit(batch, mixClips, mixPhases, mixWeights, originX, originHeight, originY, heading, scale, lean, null,
        // In a seat a face lives too: blinking, glancing, a passenger chatting now and then (wall time: the render's own clock).
        level === 0 ? faceAt(identity.seed, performance.now() / 1000 + identity.seed * 0.13,
          face ?? (seatedChat(identity.seed, performance.now() / 1000) ? 'talk' : undefined), CROWD[body.index]?.person?.mood) : undefined,
        null, level);
      if (helmet) {
        // This body's helmet on the head of the pose carrying the most weight
        if (!headClip?.head || !batch.helmet) return -scale;
        helmet.multiplyMatrices(transform.matrix, helmetBone.multiplyMatrices(headClip.head, batch.helmet));
      }
      return scale;
    },

// src/render/riggedCitizens.ts:1137-1140 method createRiggedCitizens.census: the feature's own
    census() {
      registry.recordCensus = true;
      return registry.census();
    },

// src/render/riggedCitizens.ts:1143-1183 method createRiggedCitizens.finish: the feature's own
    finish() {
      casting = false;
      if (visualFrame % 300 === 0) for (const [id, kept] of lastPose) if (visualFrame - kept.frame > 600) lastPose.delete(id);
      for (const batch of batches.values()) {
        for (const part of batch.parts) for (const at of part.levels) {
          const mesh = at.mesh, count = at.count;
          // Its last figure gone from the shadow map: the map changes.
          if (mesh.castShadow && mesh.visible && mesh.count > 0 && count === 0) casting = true;
          mesh.count = count;
          // An empty level is still a program bind and its uniforms in every pass
          mesh.visible = count > 0;
          mesh.castShadow = detail > 0 && band < 2;
          if (mesh.castShadow && count > 0) casting = true;
          if (count === 0) continue;
          fitInstances(mesh, count, LIMB_REACH);
          mesh.instanceMatrix.clearUpdateRanges();
          mesh.instanceMatrix.addUpdateRange(0, count * 16);
          mesh.instanceMatrix.needsUpdate = true;
          if (mesh.instanceColor && at.colourChanged) {
            mesh.instanceColor.clearUpdateRanges();
            mesh.instanceColor.addUpdateRange(0, count * 3);
            mesh.instanceColor.needsUpdate = true;
            at.colourChanged = false;
          }
          at.rows.clearUpdateRanges();
          at.rows.addUpdateRange(0, count);
          at.rows.needsUpdate = true;
          if (mesh.morphTexture) {
            // The faces of the bodies drawn, a row each (three sends an update range as one row of the image)
            const morph = mesh.morphTexture;
            morph.clearUpdateRanges();
            const row = morph.image.width;
            // A row a call for a few faces; past them the whole texture in one (three sends each range alone, a row high: AGT-06).
            // DO NOT give the range in texels: three r186 `updateTexture` reads it in RGBA components whatever the format (componentStride 4), and this texture is Red,
            // one float a texel - the rows past the first were never sent, their base influence 0 drew the body as a point (a rider's helmet alone in the air, 2026-10-23).
            if (count <= MOST_ROW_RANGES) for (let i = 0; i < count; i++) morph.addUpdateRange(i * row * 4, row * 4);
            morph.needsUpdate = true;
          }
        }
      }
    },

// src/render/riggedCitizens.ts:1185-1192 method createRiggedCitizens.renderPalettes: the feature's own
    renderPalettes(renderer: WebGLRenderer): void {
      for (const batch of batches.values()) {
        if (batch.count === 0) continue;
        batch.pass.render(renderer, batch.count);
        batch.uniform.value = batch.pass.texture;
        batch.base.value = batch.pass.base;
      }
    },

// src/render/riggedCitizens.ts:1194-1202 method createRiggedCitizens.capturedPose: the feature's own
    capturedPose(id: number): { index: number; palette: Float32Array; transform: Matrix4 } | null {
      const kept = lastPose.get(id);
      if (!kept || visualFrame - kept.frame >= 240) return null;
      const batch = batches.get(kept.index);
      if (!batch || batch.width !== kept.palette.length) return null;
      cpuPose(kept.palette, 0, kept.clips, kept.phases, kept.weights, batch.width, kept.single);
      for (const limb of kept.lost ?? []) maim(batch, kept.palette, 0, limb);
      return kept;
    },

// src/render/riggedCitizens.ts:1204-1216 method createRiggedCitizens.skeletonOf: the feature's own
    skeletonOf(index: number): { names: string[]; parents: number[]; inverses: Matrix4[]; local: Matrix4; bind: Matrix4 } | null {
      const batch = batches.get(index);
      const source = batch?.sources[0];
      if (!batch || !source) return null;
      const bones = source.skeleton.bones;
      return {
        names: bones.map((b) => b.name),
        parents: bones.map((b) => bones.indexOf(b.parent as never)),
        inverses: source.skeleton.boneInverses,
        local: batch.local[0]!,
        bind: source.bindMatrix,
      };
    },

// src/render/riggedCitizens.ts:1221-1223 method createRiggedCitizens.loadedIndices: the feature's own
    loadedIndices(): number[] {
      return [...batches.entries()].filter(([, b]) => b.clips.length > 0).map(([i]) => i);
    },

// src/render/riggedCitizens.ts:1224-1239 method createRiggedCitizens.clipPose: the feature's own
    clipPose(index: number, key: Played, phase: number): { palette: Float32Array; duration: number } | null {
      const batch = batches.get(index);
      if (!batch) return null;
      const at = LIBRARY_AT[key];
      if (batch.deferred.size) want(batch, at);
      const clip = batch.clips[at];
      // A stand-in until baked: not the clip asked for.
      if (!clip || batch.deferred.has(at) || baking.get(batch)?.has(at)) return null;
      const out = new Float32Array(batch.width);
      const bones = batch.width / SKIN_BONE_FLOATS;
      const packedWidth = bones * PACKED_BONE_FLOATS;
      const f = Math.min(1, Math.max(0, phase)) * clip.frames;
      const whole = Math.min(clip.frames, Math.floor(f));
      blendPackedFrames(out, 0, clip.data, whole * packedWidth, packedWidth, bones, 1 - (f - whole), f - whole);
      return { palette: out, duration: clip.duration };
    },

// src/render/riggedCitizens.ts:1242-1244 method createRiggedCitizens.indexFor: the feature's own
    indexFor(identity: ClipIdentity, helmet: boolean, x: number, y: number): number | null {
      return bodyFor(contextOf(identity, helmet, x, y))?.index ?? null;
    },

// src/render/riggedCitizens.ts:1246-1252 method createRiggedCitizens.keepAll: the feature's own
    keepAll(): void {
      for (let index = 0; index < models.length; index++) {
        resident.add(index);
        // A failed load is said and asked again by `request` itself.
        if (!batches.has(index)) void request(index);
      }
    },

// src/render/riggedCitizens.ts:1254-1256 method createRiggedCitizens.forgetCompany: the feature's own
    forgetCompany(company: Company, companyId: number): void {
      registry.forgetCompany(company, companyId);
    },

// src/render/riggedCitizens.ts:1261-1274 method createRiggedCitizens.drawPalette: the feature's own
    drawPalette(index: number, palette: Float32Array, instance: Matrix4, charred = false): void {
      const batch = batches.get(index);
      if (!batch || batch.count >= CAPACITY || palette.length !== batch.width) return;
      if (batch.count >= batch.rows) grow(batch);
      // Worked out by the ragdoll: the pass copies it as it is.
      batch.pass.setOverride(batch.count, palette);
      batch.pass.setPlays(batch.count, null, 0);
      const e = instance.elements;
      // Its face's weights too, close up: the slot otherwise keeps whatever the morph texture held there - zero, never written
      const level = levelOf(e[12]!, -e[14]!, e[13]!);
      place(batch, level, instance, charred ? BURNT : UNBURNT, level === 0 ? NEUTRAL_FACE : undefined);
      batch.count++;
      batch.lastUsed = frameNow;
    },

// src/render/riggedCitizens.ts:1275-1287 method createRiggedCitizens.dispose: the feature's own
    dispose() {
      disposed = true;
      clearInterval(evictionTimer);
      for (const entry of queuedLoads.values()) entry.resolve();
      queuedLoads.clear();
      for (const batch of batches.values()) {
        batch.disposed = true;
        disposeOwned(batch.resources);
      }
      disposeOwned(resources);
      resources.clear(); batches.clear(); loading.clear(); wantedFrame.clear(); group.clear();
      releaseIdleBakeWorkers();
    },
