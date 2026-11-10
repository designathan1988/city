// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:76-76 slice of src/render/agents.ts#<ExpressionStatement>@76 (shared): the feature's statements; "…" marks code left out
providePeopleLod(lodPool);

// src/render/agents.ts:168-198 slice of src/render/agents.ts#AgentMeshes (shared): the feature's statements; "…" marks code left out
export interface AgentMeshes {
  // … 9 lines
  census(): ReturnType<ReturnType<typeof createRiggedCitizens>['census']>;
  // … 7 lines
  forceClip(id: number, clip: ProcClip | null): void;
// … 11 lines
}

// src/render/agents.ts:248-248 slice of src/render/agents.ts#gaitClip (shared): the feature's statements; "…" marks code left out
const gaitClip = (c: ProcClip): boolean => c === 'walk' || c === 'run' || c === 'sprint' || c === 'hurtWalk' || c === 'hurtRun';

// src/render/agents.ts:552-2596 slice of src/render/agents.ts#createAgentMeshes (shared): the feature's statements; "…" marks code left out
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  // … 2 lines
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes {
  // … 60 lines
  const helmetGeometry = merge([new SphereGeometry(0.5, HELMET_SEGMENTS.width, HELMET_SEGMENTS.height)]);
  const helmets = instanced('rider-helmets', helmetGeometry, paint);
  // … 1 lines
  const parts: readonly Part[] = [wheels, hubs, thinWheels, spokes, lamps, helmets];
  // … 220 lines
  const allParts: readonly Part[] = [...parts, ...carPartList, ...twoWheelerPartList, ...ladderParts, ...panelParts];
  // … 2 lines
  const pedestrians = createRiggedCitizens(CROWD_IDS, onAssetsReady);
  // … 1 lines
  const procedural = typeof location !== 'undefined' && new URLSearchParams(location.search).get('bodies') === 'cooked'
    ? null : createProceduralCrowd({ unit: m(1) });
  // … 1 lines
  const procPeople = new Map<number, { person: ProceduralPerson | null; seen: number; at?: Matrix4; age?: PersonAgeClass; cover?: number }>();
  // … 9 lines
  const procSpare: ProceduralPerson[] = [];
  // … 6 lines
  if (procedural) {
    for (const seed of [0x51a7e, 0x2b0d1]) {
      void procedural.add(randomPerson(-seed, seed)).then((person) => {
        person.matrix.makeScale(0, 0, 0);
        procSpare.push(person);
      }, () => {});
    }
    // And the rest of the classes while the map is still opening, before the first walkers come in (`warmClasses`).
    void procedural.warmClasses().catch(() => {});
  }
  // … 177 lines
  const riggedRagdoll: RagdollCitizens = Object.assign(Object.create(pedestrians) as typeof pedestrians, {
    clipPose: (index: number, key: 'crouchUp' | 'idle' | 'getUpFront' | 'getUpBack' | 'crawl', phase: number) =>
      key === 'getUpFront' || key === 'getUpBack' || key === 'crawl' ? null : pedestrians.clipPose(index, key, phase),
  });
  // … 30 lines
  const procForced = new Map<number, ProcClip>();
  // … 4 lines
  const procTwins = new Map<number, ProceduralPerson>();
  // … 5 lines
  const procRagdoll: RagdollCitizens = {
    capturedPose(id) {
      const entry = procPeople.get(id);
      const person = entry?.person;
      const palette = person && entry.at && procedural ? procedural.ragdoll.pose(person) : null;
      if (!person || !palette || !entry.at) return pedestrians.capturedPose(id);
      return { index: -1 - id, palette, transform: entry.at.clone().multiply(procScale.makeScale(person.scale, person.scale, person.scale)) };
    },
    skeletonOf(index) {
      // … 1 lines
      return person && procedural ? procedural.ragdoll.skeleton(person) : pedestrians.skeletonOf(index);
    },
    drawPalette(index, palette, instance, charred, alive) {
      // … 1 lines
      if (!person || !procedural) { pedestrians.drawPalette(index, palette, instance, charred); return; }
      // … 1 lines
      person.activity = alive ? 'hurt' : 'dead';
      if (index <= TWIN_BASE) {
        person.matrix.copy(instance).multiply(procScale.makeScale(1 / person.scale, 1 / person.scale, 1 / person.scale));
        procedural.ragdoll.hold(person, palette);
      // … 1 lines
      }
      // … 4 lines
      person.matrix.copy(instance).multiply(procScale.makeScale(1 / person.scale, 1 / person.scale, 1 / person.scale));
      procedural.ragdoll.hold(person, palette);
    },
    clipPose(index, key, phase) {
      // … 1 lines
      if (!person || !procedural) return key === 'crawl' ? null : pedestrians.clipPose(index, key === 'getUpFront' || key === 'getUpBack' ? 'crouchUp' : key, phase);
      // … 1 lines
      const palette = procedural.ragdoll.standing(person, phase, clip);
      return palette ? { palette, duration: clip === 'idle' ? 1.2 : procedural.ragdoll.duration(person, clip) } : null;
    },
    loadedIndices: () => pedestrians.loadedIndices(),
    twin(index) {
      // … 2 lines
      if (!person || !procedural) return index >= 0 ? index : null;
      const twin = procedural.ragdoll.twin(person);
      if (!twin) return null;
      // … 1 lines
      procTwins.set(key, twin);
    // … 1 lines
    },
    untwin(index) {
      const twin = procTwins.get(index);
      if (!twin || !procedural) return;
      procedural.ragdoll.untwin(twin);
      procTwins.delete(index);
    },
    char(index) {
      // … 1 lines
      if (person && procedural) procedural.ragdoll.char(person, true);
    },
    wound(index, part) {
      // … 1 lines
      if (!person || !procedural) return;
      procedural.ragdoll.wound(person, part);
    // … 1 lines
    },
    drench(index) {
      // … 1 lines
      if (person && procedural) procedural.ragdoll.drench(person);
    },
  };
  // … 1 lines
  const blobs = partOf(blobMesh('vehicle-shadows', MAX_VEHICLES, 0.4));
  const meshes = [...allParts.map((part) => part.mesh), blobs.mesh, pedestrians.group, ...(procedural ? [procedural.group] : [])];
  // … 470 lines
  const seatWho: { -readonly [K in keyof ClipIdentity]-?: ClipIdentity[K] } = { seed: 0, gender: 'f', ageClass: 'adult', company: 'car', companyId: 0, hasChild: false };
  // … 159 lines
  const riderPlays: { key: CitizenClipKey; phase: number; weight: number }[] = [];
  const riderPool = [0, 1, 2, 3].map(() => ({ key: 'bikeStop' as CitizenClipKey, phase: 0, weight: 0 }));
  // … 218 lines
  return {
    meshes,
    // … 2 lines
    get held() { return { vehicles: draws.size, cells: cellsDrawn.size, kept: keptList.length, people: procPeople.size }; },
  // … 572 lines
  };
}

// src/render/agents.ts:850-852 slice of src/render/agents.ts#createAgentMeshes.procSuits (shared): the feature's statements; "…" marks code left out
  const procSuits = (person: ProceduralPerson, age: PersonAgeClass | undefined): boolean =>
    age === undefined || (age === 'child' ? person.band === 'child' : age === 'elder' ? person.band === 'senior'
      : person.band === 'young' || person.band === 'adult');

// src/render/agents.ts:854-857 slice of src/render/agents.ts#createAgentMeshes.procSpareFor (shared): the feature's statements; "…" marks code left out
  const procSpareFor = (age: PersonAgeClass | undefined): ProceduralPerson | null => {
    const i = procSpare.findIndex((p) => procSuits(p, age));
    return i >= 0 ? procSpare.splice(i, 1)[0]! : null;
  };

// src/render/agents.ts:875-898 slice of src/render/agents.ts#createAgentMeshes.procBuildNext (shared): the feature's statements; "…" marks code left out
  const procBuildNext = (): void => {
    // … 12 lines
    while (procSpare.length > 12) procedural!.ragdoll.untwin(procSpare.shift()!);
    // … 4 lines
    void procedural!.add(randomPerson(target, (Math.imul(target, 2654435761) >>> 0) + 1, age ? { body: { age: ageFromYears(years) } } : {})).then((person) => {
      const e = procPeople.get(target);
      if (e && !e.person) e.person = person;
      else { person.matrix.makeScale(0, 0, 0); procSpare.push(person); }
    }, () => { procPeople.delete(target); }).finally(() => { procBuilding = false; });
  };

// src/render/agents.ts:904-926 slice of src/render/agents.ts#createAgentMeshes.procDraw (shared): the feature's statements; "…" marks code left out
  const procDraw = (id: number, x: number, y: number, heading: number, deck: number, speed: number, walking: boolean, dt: number, activity?: string,
    lost?: readonly Severable[], act?: { readonly t: number; readonly hold: number },
    wound?: { readonly part: BodyPart; readonly grave: boolean }, age?: PersonAgeClass): void => {
    const before = procPeople.get(id)?.person ?? null;
    const was = before?.clip;
    const wasPhase = before?.phase ?? 0;
    procPose(id, x, y, heading, deck, speed, walking, dt, activity, lost, act, wound, age);
    const person = procPeople.get(id)?.person ?? null;
    // Somebody new in this body this frame starts as they are: nothing to fade from.
    if (!person || person !== before || was === undefined) return;
    const metres = speed / m(1);
    const out = person.fade;
    if (out) {
      out.phase += gaitClip(out.clip) ? dt * metres / Math.max(0.1, procedural!.stride(person, out.clip))
        : dt / procedural!.clipDuration(person, out.clip);
      out.weight -= dt / CLIP_FADE;
      if (out.weight <= 0) person.fade = undefined;
    }
    // One gait into another goes on through the same stride (`procPose`): no fade.
    if (person.clip !== was && !(gaitClip(was) && gaitClip(person.clip))) {
      person.fade = { clip: was, phase: wasPhase, weight: 1 };
    }
  };

// src/render/agents.ts:927-1050 slice of src/render/agents.ts#createAgentMeshes.procPose (shared): the feature's statements; "…" marks code left out
  const procPose = (id: number, x: number, y: number, heading: number, deck: number, speed: number, walking: boolean, dt: number, activity?: string,
    lost?: readonly Severable[], act?: { readonly t: number; readonly hold: number },
    wound?: { readonly part: BodyPart; readonly grave: boolean }, age?: PersonAgeClass): void => {
    let entry = procPeople.get(id);
    if (!entry) {
      const spare = procSpareFor(age);
      if (spare) {
        // Somebody new in that body: none of the last one's wounds.
        procedural?.ragdoll.heal(spare);
        entry = { person: spare, seen: procFrame, ...(age ? { age } : {}) };
        procPeople.set(id, entry);
      } else {
        if (procPeople.size >= PROC_CAP) return;
        entry = { person: null, seen: procFrame, ...(age ? { age } : {}) };
        procPeople.set(id, entry);
        procWaiting.push(id);
      }
    }
    entry.seen = procFrame;
    const person = entry.person;
    if (!person) return;
    procMatrix.makeTranslation(x, deck, -y)
      .multiply(procTurn.makeRotationY(heading + Math.PI / 2))
      .multiply(procSize.makeScale(m(1), m(1), m(1)));
    person.matrix.copy(procMatrix);
    // How tall they stand on the screen: the level they are drawn at (`crowdLod.ts`).
    person.pixels = procPixels ? procPixels(x, y, deck, m(person.height)) : undefined;
    // Where they were last drawn: a ragdoll takes them from there when they drop out of the street
    (entry.at ??= new Matrix4()).copy(procMatrix);
    person.activity = activity;
    person.lost = lost;
    const forced = procForced.get(id);
    if (forced) {
      if (person.clip !== forced) { person.clip = forced; person.phase = 0; }
      person.phase += dt / procedural!.clipDuration(person);
      return;
    }
    procFrozen.delete(id);
    if (activity === 'flinch' && act) {
      // A first wound (`walk.ts` FLINCH), as GTA's shot peds take it
      person.activity = 'hurt';
      // The hit itself first, as captured (Quaternius Hit_Chest, a third of a second); then knocked back
      const hitFor = procedural!.ragdoll.duration(person, 'hit');
      const back = staggerSpeed(act.t) / m(1);
      if (act.t < hitFor) {
        if (person.clip !== 'hit') person.clip = 'hit';
        person.phase = Math.min(0.999, act.t / hitFor);
      } else if (back > 0.05) {
        // Knocked back: steps back as captured (CMU 76_11), played over the stagger.
        if (person.clip !== 'staggerBack') person.clip = 'staggerBack';
        person.phase = Math.min(0.999, (act.t - STAGGER_FROM) / STAGGER_TIME * 0.85);
      } else {
        if (person.clip !== 'idle') { person.clip = 'idle'; person.phase = 0; }
        person.phase += dt / procedural!.clipDuration(person);
      }
      const k = Math.min(1, Math.max(0, (act.t - 0.12) / 0.3));
      procedural!.ragdoll.posture(person, { hunch: (wound?.grave ? 0.55 : 0.42) * k, reach: Math.min(1, Math.max(0, (act.t - 0.2) / 0.35)), part: wound?.part ?? 'torso' });
      return;
    }
    if ((activity === 'crouch' || activity === 'mourn') && act && !wound) {
      // Crouching in fear (a `crouch` act): down into it (the crouch-down clip), held, and up again before the act ends
      const down = 0.5, up = 0.9;
      const next: ProcClip = act.t < down ? 'duck' : act.t > act.hold - up ? 'getUp' : 'cower';
      if (person.clip !== next) { person.clip = next; person.phase = 0; }
      if (next === 'duck') person.phase = Math.min(0.999, act.t / down);
      else if (next === 'getUp') person.phase = Math.min(0.999, (act.t - (act.hold - up)) / up);
      else person.phase += dt / procedural!.clipDuration(person);
      // Both hands over the head while down (GTA's cower), on as they duck; grieving beside somebody they walked with
      if (activity === 'mourn') {
        person.activity = 'cry';
        entry.cover = 0;
        procedural!.ragdoll.posture(person, null);
        return;
      }
      entry.cover = Math.min(1, act.t / 0.4) * Math.min(1, Math.max(0, (act.hold - act.t) / 0.5));
      procedural!.ragdoll.posture(person, { hunch: 0, reach: 0, part: 'torso', cover: entry.cover });
      return;
    }
    if (activity === 'crawl') {
      // Dragging themself off, gravely hurt: the captured crawl (CMU 111_03), stepped by the ground covered.
      person.activity = 'hurt';
      if (person.clip !== 'crawl') { person.clip = 'crawl'; person.phase = 0; }
      person.phase += dt * (speed / m(1)) / Math.max(0.05, procedural!.stride(person));
      entry.cover = 0;
      procedural!.ragdoll.posture(person, null);
      return;
    }
    if (activity === 'look' && act && !wound) {
      // A bystander's first moment after a shot (`walk.ts` frighten): turned to it, standing scared
      if (person.clip !== 'nervous') { person.clip = 'nervous'; person.phase = 0; }
      person.phase += dt / procedural!.clipDuration(person);
      entry.cover = 0;
      procedural!.ragdoll.posture(person, null);
      return;
    }
    // Hurt: a hand pressed to the wound over the captured hurt gaits - the injured run while they flee, the limp after
    const metres = speed / m(1);
    const hurtRun = wound !== undefined && walking && metres > (person.clip === 'hurtRun' ? 1.3 : 1.7);
    // Fleeing in panic: both hands over the head as they run
    const panicRun = !wound && activity === 'panic' && walking && metres > 1.6;
    entry.cover = (entry.cover ?? 0) + ((panicRun ? 1 : 0) - (entry.cover ?? 0)) * Math.min(1, dt * 5);
    procedural!.ragdoll.posture(person, wound ? { hunch: wound.grave ? 0.5 : walking && metres > 0.15 ? 0.08 : 0.42, reach: 1, part: wound.part }
      : entry.cover > 0.02 ? { hunch: 0, reach: 0, part: 'torso', cover: entry.cover } : null);
    if (wound) person.activity = 'hurt';
    // Running from danger runs, past a brisk walk; struck with fear
    if (activity === 'fall') {
      if (person.clip !== 'idle') { person.clip = 'idle'; person.phase = 0; }
      procedural!.ragdoll.posture(person, null);
      return;
    }
    const was = person.clip;
    const moving = walking && metres > 0.15;
    const sprint = metres > (was === 'sprint' ? 3.6 : 4.2);
    const run = metres > (was === 'run' || was === 'sprint' ? 2.1 : 2.6);
    const clip: ProcClip = activity === 'photo' && !moving ? 'photo'
      : moving ? (wound ? (hurtRun ? 'hurtRun' : 'hurtWalk') : sprint ? 'sprint' : run ? 'run' : 'walk') : 'idle';
    // Walk, run and sprint all start on the same foot: the stride goes on through a change of pace.
    if (person.clip !== clip) {
      if (!(gaitClip(was) && gaitClip(clip))) person.phase = 0;
      person.clip = clip;
    }
    if (gaitClip(clip)) person.phase += dt * metres / Math.max(0.1, procedural!.stride(person));
    else person.phase += dt / procedural!.clipDuration(person);
  };

// src/render/agents.ts:1057-1078 slice of src/render/agents.ts#createAgentMeshes.procFinish (shared): the feature's statements; "…" marks code left out
  const procFinish = (eye: Vector3 | undefined, live: ReadonlySet<number>): void => {
    for (const [id, entry] of procPeople) {
      // … 4 lines
      entry.person.matrix.makeScale(0, 0, 0);
    // … 2 lines
    }
    // … 1 lines
    for (let i = 0; i < procWaiting.length && procSpare.length; i++) {
      // … 3 lines
      if (spare) { procedural?.ragdoll.heal(spare); e.person = spare; procWaiting.splice(i--, 1); }
    }
  // … 4 lines
  };

// src/render/agents.ts:1094-1095 slice of src/render/agents.ts#createAgentMeshes.procOf (shared): the feature's statements; "…" marks code left out
  const procOf = (index: number): ProceduralPerson | null => (index <= TWIN_BASE ? procTwins.get(index) ?? null
    : index < 0 ? procPeople.get(-1 - index)?.person ?? null : null);

// src/render/agents.ts:1410-1414 slice of src/render/agents.ts#createAgentMeshes.placeBlob (shared): the feature's statements; "…" marks code left out
  const placeBlob = (x: number, y: number, deck: number, angle: number, radius: number, length: number): void => {
    // … 2 lines
    blobInto(blobs.mesh.instanceMatrix.array as Float32Array, at * 16, x, deck, -y, angle, radius, length);
  };

// src/render/agents.ts:1614-1616 slice of src/render/agents.ts#createAgentMeshes.vehicleCompany (shared): the feature's statements; "…" marks code left out
  const vehicleCompany = (vehicle: SimVehicle): Company =>
    vehicle.archetype.shape === 'bus' ? 'bus' : vehicle.archetype.shape === 'truck' ? 'truck'
      : vehicle.archetype.shape === 'motorcycle' || vehicle.archetype.shape === 'bicycle' ? 'rider' : 'car';

// src/render/agents.ts:1628-1632 slice of src/render/agents.ts#createAgentMeshes.forgetGone (shared): the feature's statements; "…" marks code left out
  const forgetGone = (vehicle: SimVehicle): void => {
    // … 2 lines
    pedestrians.forgetCompany(vehicleCompany(vehicle), vehicle.id);
  };

// src/render/agents.ts:1639-1645 slice of src/render/agents.ts#createAgentMeshes.seatIdentity (shared): the feature's statements; "…" marks code left out
  const seatIdentity = (vehicle: SimVehicle, person: { seed: number; gender: 'f' | 'm'; ageClass: 'child' | 'adult' | 'elder' },
    hasChild: boolean): ClipIdentity => {
    castVehicles.set(vehicle.id, vehicle);
    seatWho.seed = person.seed; seatWho.gender = person.gender; seatWho.ageClass = person.ageClass;
    seatWho.company = vehicleCompany(vehicle); seatWho.companyId = vehicle.id; seatWho.hasChild = hasChild;
    return seatWho;
  };

// src/render/agents.ts:1651-1686 slice of src/render/agents.ts#createAgentMeshes.placeCarOccupants (shared): the feature's statements; "…" marks code left out
  const placeCarOccupants = (vehicle: SimVehicle, car: CarParts): void => {
    // … 9 lines
    for (let round = 0; round < 2; round++) for (let index = 0; index < seatCount; index++) {
      // … 8 lines
      const plays = occupantPlays(seat, who.seed, frame[FRAME_CLOCK]!, seatPlays, slain);
      // … 1 lines
      const drawn = pedestrians.drawClip(who, seatPoint.x, seatPoint.y, seatPoint.h ?? frame[FRAME_DECK]! + seat.hipY, frame[FRAME_YAW]!, plays as readonly { key: CitizenClipKey; phase: number; weight: number }[],
        0, seatFitScale(seat), false, fixed, null, slain ? 'dead' : undefined);
      if (seat.driver) driverScale = drawn;
    }
    // … 1 lines
    if (car.steering && model.steering) {
      // … 2 lines
      const s = driverScale > 0 ? driverScale / m(1) : 1;
      placeSteeringWheel(car.steering, seat.x + m(wheel.forward) * s, -seat.z, seat.hipY + m(wheel.up) * s,
        wheel.tilt, Math.max(-1.6, Math.min(1.6, lamp.steer * 5)), s);
    }
  // … 2 lines
  };

// src/render/agents.ts:1729-1774 slice of src/render/agents.ts#createAgentMeshes.placeKerbPerson (shared): the feature's statements; "…" marks code left out
  const placeKerbPerson = (vehicle: SimVehicle, model: VehicleModel, stop: KerbStop): void => {
    // … 9 lines
    if (seat && door.kind === 'hinge' && !(progress.walked >= 1 && (stop.phase === 'hold' || stop.phase === 'close'))) {
      const f = kerbFigure({ seat, door, foot: toVehicle(person.footX, person.footY) }, stop.kind, progress.seated, progress.walked, kerb);
      // … 1 lines
      pedestrians.drawClip(identity, seatPoint.x, seatPoint.y, seatPoint.h ?? frame[FRAME_DECK]! + f.y, frame[FRAME_YAW]! + f.heading, f.plays,
        0, f.seated ? seatFitScale(seat) : Infinity, f.anchor === 'pelvis' ? false : f.anchor === 'feet' ? true : 'pelvisOver');
    // … 1 lines
    }
    // … 11 lines
    if (progress.walked >= 1 && (stop.phase === 'hold' || stop.phase === 'close')) {
      // … 1 lines
      pedestrians.drawClip(identity, person.footX, person.footY, frame[FRAME_DECK]!,
        Math.atan2(outside.y - person.footY, outside.x - person.footX),
        [{ key: 'idle', phase: (stop.t * 0.1) % 1, weight: 1 }], 0, Infinity, true);
    // … 1 lines
    }
    if (progress.walked > 0 || progress.seated <= 0) {
      // … 1 lines
      if (stop.kind === 'drop') walkTo(outside.x, outside.y, person.footX, person.footY, progress.walked, frame[FRAME_DECK]!);
      else walkTo(person.footX, person.footY, outside.x, outside.y, 1 - progress.walked, frame[FRAME_DECK]!);
    // … 1 lines
    }
  // … 4 lines
  };

// src/render/agents.ts:1749-1756 slice of src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo (shared): the feature's statements; "…" marks code left out
    const walkTo = (fromX: number, fromY: number, toX: number, toY: number, t: number, height: number): void => {
      // … 4 lines
      pedestrians.drawClip(identity, x, y, height, heading,
        [{ key: 'walk', phase: 0, distance: distance * t, weight: 1 }], 0, Infinity, true);
    };

// src/render/agents.ts:1800-1806 slice of src/render/agents.ts#createAgentMeshes.riderPlay (shared): the feature's statements; "…" marks code left out
  const riderPlay = (key: CitizenClipKey, phase: number, weight: number): void => {
    const play = riderPool[riderPlays.length]!;
    play.key = key;
    play.phase = phase;
    play.weight = weight;
    riderPlays.push(play);
  };

// src/render/agents.ts:1809-1839 slice of src/render/agents.ts#createAgentMeshes.placeRiderFigure (shared): the feature's statements; "…" marks code left out
  const placeRiderFigure = (vehicle: SimVehicle, model: TwoWheelerModel, cyclist: boolean, s: VehicleDraw): void => {
    // … 8 lines
    const steer = Math.max(-1, Math.min(1, lamp.steer / STEER_FULL));
    // … 1 lines
    if (cyclist) {
      // … 1 lines
      riderPlay('bikePedal', phase, moving * (1 - Math.abs(steer) * 0.4));
      if (steer > 0) riderPlay('bikeLeft', 0, moving * steer * 0.4);
      if (steer < 0) riderPlay('bikeRight', 0, moving * -steer * 0.4);
    // … 1 lines
    } else {
      riderPlay('motoRide', 0, moving * (1 - Math.abs(steer)));
      if (steer > 0) riderPlay('motoLeft', 0, moving * steer);
      if (steer < 0) riderPlay('motoRight', 0, moving * -steer);
    // … 1 lines
    }
    // … 1 lines
    const drawn = pedestrians.drawClip(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), x, y, height, frame[FRAME_YAW]!, riderPlays, frame[FRAME_ROLL]!, Infinity, false, 1,
      cyclist ? null : helmetMatrix);
    if (!cyclist && drawn > 0) {
      // A motorcyclist's helmet, fitted to this body's head (`riderPoses.helmetShape`) and put where the pose just drawn has it.
      write(helmets, helmetMatrix, s.helmet);
    }
  };

// src/render/agents.ts:2023-2023 slice of src/render/agents.ts#createAgentMeshes.census (shared): the feature's statements; "…" marks code left out
    census: () => pedestrians.census(),

// src/render/agents.ts:2024-2072 slice of src/render/agents.ts#createAgentMeshes.meshProbe (shared): the feature's statements; "…" marks code left out
    meshProbe() {
      // … 3 lines
      for (const [id, entry] of procPeople) {
        // … 2 lines
        const pal = procedural.ragdoll.pose(person);
        const sk = procedural.ragdoll.skeleton(person);
        if (!pal || !sk) continue;
        // … 1 lines
        const bind = sk.inverses.map((m4) => { const e = inv.copy(m4).invert().elements; return [e[12]!, e[13]!, e[14]!] as const; });
        // … 1 lines
        const rest = procedural.ragdoll.standing(person, 0);
        if (!rest) continue;
        const atIn = (m: Float32Array, b: number): [number, number, number] => {
          const o = b * 16, [x, y, z] = bind[b]!;
          return [m[o]! * x + m[o + 4]! * y + m[o + 8]! * z + m[o + 12]!, m[o + 1]! * x + m[o + 5]! * y + m[o + 9]! * z + m[o + 13]!,
            m[o + 2]! * x + m[o + 6]! * y + m[o + 10]! * z + m[o + 14]!];
        };
        const at = (b: number): [number, number, number] => atIn(pal, b);
        const norm = (b: number): number => Math.max(Math.hypot(pal[b * 16]!, pal[b * 16 + 1]!, pal[b * 16 + 2]!),
          Math.hypot(pal[b * 16 + 4]!, pal[b * 16 + 5]!, pal[b * 16 + 6]!), Math.hypot(pal[b * 16 + 8]!, pal[b * 16 + 9]!, pal[b * 16 + 10]!));
        // … 2 lines
        for (let b = 0; b < sk.parents.length; b++) {
          const p = sk.parents[b]!;
          if (p >= 0) longest = Math.max(longest, Math.hypot(bind[b]![0] - bind[p]![0], bind[b]![1] - bind[p]![1], bind[b]![2] - bind[p]![2]));
        }
        for (let b = 0; b < sk.parents.length; b++) {
          const name = sk.names[b] ?? '';
          if (/Finger|Toe|Nub|_end|twist/i.test(name)) continue;
          const n = norm(b);
          if (n > scale) { scale = n; scaleBone = name; }
          const p = sk.parents[b]!;
          // Not against the root (it stays at the feet: a crouch lowers the pelvis to it, no skin between).
          if (p < 0 || sk.parents[p]! < 0 || n < 0.5 || norm(p) < 0.5) continue;
          const ra = atIn(rest, b), rc = atIn(rest, p);
          const l0 = Math.hypot(ra[0] - rc[0], ra[1] - rc[1], ra[2] - rc[2]);
          // Bones of real length only: a few millimetres between the spine and the pelvis made a 3x from nothing.
          if (l0 < longest * 0.12) continue;
          const a = at(b), c = at(p);
          const r = Math.hypot(a[0] - c[0], a[1] - c[1], a[2] - c[2]) / l0;
          if (Math.abs(r - 1) > Math.abs(stretch - 1)) { stretch = r; stretchBone = `${name}<${sk.names[p]} ${(l0 * 100).toFixed(1)}cm ${person.sex}-${person.band}`; }
        }
        out.push({ id, stretch, stretchBone, scale, scaleBone, held: procHeld.has(id), clip: person.clip });
      }
    // … 1 lines
    },

// src/render/agents.ts:2073-2082 slice of src/render/agents.ts#createAgentMeshes.animProbe (shared): the feature's statements; "…" marks code left out
    animProbe(id) {
      // … 2 lines
      const duration = procedural.ragdoll.duration(person, person.clip);
      // … 1 lines
      return {
        clip: person.clip, time: (person.phase - Math.floor(person.phase)) * duration, duration, weight: held ? 0 : 1,
        paused: procFrozen.has(id), layers: procedural.ragdoll.layers(person), source: held ? 'physics' : 'animation',
      };
    },

// src/render/agents.ts:2086-2088 slice of src/render/agents.ts#createAgentMeshes.driverBody (shared): the feature's statements; "…" marks code left out
    driverBody(vehicle, x, y) {
      return pedestrians.indexFor(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), vehicle.archetype.shape === 'motorcycle', x, y);
    },

// src/render/agents.ts:2147-2147 slice of src/render/agents.ts#createAgentMeshes.setBleed (shared): the feature's statements; "…" marks code left out
    setBleed: (fn) => { pedestrians.onBleed = fn; procBleed = fn; },

// src/render/agents.ts:2151-2151 slice of src/render/agents.ts#createAgentMeshes.renderPalettes (shared): the feature's statements; "…" marks code left out
    renderPalettes(renderer) { procedural?.renderPalettes(renderer); pedestrians.renderPalettes(renderer); },

// src/render/agents.ts:2152-2152 slice of src/render/agents.ts#createAgentMeshes.keepPeople (shared): the feature's statements; "…" marks code left out
    keepPeople: () => pedestrians.keepAll(),

// src/render/agents.ts:2153-2573 slice of src/render/agents.ts#createAgentMeshes.sync (shared): the feature's statements; "…" marks code left out
    sync(world, net, alpha, detailed, zoom = Number.POSITIVE_INFINITY, options = {}) {
      // … 15 lines
      pedestrians.begin(options.pedestrianDetail ?? 2, zoom, options.personPixels ?? null, options.pixelRatio ?? 1);
      // … 308 lines
      if (band >= 1) {
        // … 3 lines
        for (const ped of world.pedViews) {
          // … 2 lines
          if (box) {
            const ax = ped.prev.x, ay = ped.prev.y, bx = ped.x, by = ped.y;
            if ((ax < bx ? bx : ax) < box.x0 || (ax < bx ? ax : bx) > box.x1 || (ay < by ? by : ay) < box.y0 || (ay < by ? ay : by) > box.y1) continue;
          }
          if (options.hiddenPed?.(ped.id)) continue;
          // … 3 lines
          const open = ped.ground === 'open';
          const segment = ped.segment;
          currentSegment = segment;
          const land = open && groundAt ? groundAt : elevationOnCurrent;
          // … 1 lines
          const below = open ? (roughGroundAt ?? land)(pose.p.x, pose.p.y) : elevationAt(pose.p.x, pose.p.y, segment);
          if (options.pedestrianVisible && !options.pedestrianVisible(pose.p.x, pose.p.y, below)) continue;
          const rise = open ? m(0.04) : footwayRiseAt(net, pose.p.x, pose.p.y) * walkingRise(net, pose.p.x, pose.p.y, ped.ground !== 'crossing',
            segment === undefined ? 'ground' : net.doc.segment(segment)?.structure ?? 'ground');
          const deck = (open && roughGroundAt ? land(pose.p.x, pose.p.y) : below) + rise;
          // … 1 lines
          if (procedural && ped.id !== PLAYER_ID) {
            // What they are doing shows before the fright on their face (a photo held up, a crouch, a fall).
            const doing = ped.gesture?.kind;
            // Shot and still going: the pain on their face over the fright.
            const shown = doing === 'photo' || doing === 'crouch' || doing === 'fall' || doing === 'flinch' || doing === 'look' || doing === 'mourn' || doing === 'crawl' ? doing : ped.bleeding ? 'hurt' : ped.panic ? 'panic' : doing;
            if (ped.bleeding || ped.lost?.length) procBleed?.(ped.id, pose.p.x, pose.p.y, deck);
            // The gait at the body's real speed: a step aside is steps, not a glide.
            const gait = gaitSpeed(ped);
            procDraw(ped.id, pose.p.x, pose.p.y, pose.angle, deck, gait, ped.walking || gait > m(0.15), gaitDt, shown,
              ped.lost ?? (ped.maimed ? [ped.maimed] : undefined), ped.gesture ? { t: ped.gesture.t, hold: ped.gesture.hold ?? 0 } : undefined, ped.wound, ped.ageClass);
          }
          else {
            // The rise is the same on both sides of the difference, so the gradient is the road's own under the walker.
            const ground = groundGradient(land,
              pose.p.x, pose.p.y, deck - rise);
            pedestrians.draw(ped, pose.p.x, pose.p.y, pose.angle, deck, alpha, ground);
          }
        // … 1 lines
        }
      }
      // … 1 lines
      if (procedural) {
        // … 1 lines
        if (procLiveTick !== world.tick || procLiveViews !== world.pedViews.length) {
          // … 2 lines
          for (const ped of world.pedViews) procLive.add(ped.id);
        }
      // … 1 lines
      }
      // … 3 lines
      if (procedural) {
        // … 1 lines
        for (const id of procHeld) if (!procHeldNow.has(id)) { const p = procPeople.get(id)?.person; if (p) procedural.ragdoll.hold(p, null); }
        // … 2 lines
        procedural.update(options.eye, gaitClock < 0 ? undefined : gaitClock, options.shadows !== false, options.tier?.hairStrandPeople);
      }
      pedestrians.finish();
      // … 3 lines
      casting = pedestrians.casting || (procedural ? procedural.casting : false);
    // … 24 lines
    },

// src/render/agents.ts:2219-2379 slice of src/render/agents.ts#createAgentMeshes.sync.drawVehicle (shared): the feature's statements; "…" marks code left out
      const drawVehicle = (vehicle: SimVehicle): void => {
        // … 110 lines
        frameAt(pose.p.x, pose.p.y, pose.angle, s.deck,
          twoWheeled ? leanOf(vehicle, s) + (fit ? fit.stopTilt * (1 - riderMoving(vehicle)) : 0) : 0,
          s.pitch, s.roll);
          // … 46 lines
          };

// src/render/agents.ts:2574-2594 slice of src/render/agents.ts#createAgentMeshes.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      pedestrians.dispose();
    // … 18 lines
    },
