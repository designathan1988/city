// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:137-166 slice of src/render/agents.ts#AgentRenderOptions (shared): the feature's statements; "…" marks code left out
interface AgentRenderOptions {
  // … 15 lines
  readonly ragdolls?: (citizens: RagdollCitizens) => void;
// … 12 lines
}

// src/render/agents.ts:168-198 slice of src/render/agents.ts#AgentMeshes (shared): the feature's statements; "…" marks code left out
export interface AgentMeshes {
  // … 9 lines
  census(): ReturnType<ReturnType<typeof createRiggedCitizens>['census']>;
// … 19 lines
}

// src/render/agents.ts:552-2596 slice of src/render/agents.ts#createAgentMeshes (shared): the feature's statements; "…" marks code left out
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  // … 2 lines
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes {
  // … 203 lines
  for (const archetype of ARCHETYPES) {
    if (archetype.shape === 'car') {
      for (const style of carStylesFor(archetype)) bodies.push({ key: `${archetype.id}:${style}`, model: buildCarModel(archetype, style) });
    } else if (archetype.shape === 'bus') bodies.push({ key: `${archetype.id}:bus`, model: buildBusModel(archetype) });
    else if (archetype.shape === 'truck') bodies.push({ key: `${archetype.id}:truck`, model: buildTruckModel(archetype) });
  }
  // … 78 lines
  const pedestrians = createRiggedCitizens(CROWD_IDS, onAssetsReady);
  // … 208 lines
  const riggedRagdoll: RagdollCitizens = Object.assign(Object.create(pedestrians) as typeof pedestrians, {
    clipPose: (index: number, key: 'crouchUp' | 'idle' | 'getUpFront' | 'getUpBack' | 'crawl', phase: number) =>
      key === 'getUpFront' || key === 'getUpBack' || key === 'crawl' ? null : pedestrians.clipPose(index, key, phase),
  });
  // … 41 lines
  const procRagdoll: RagdollCitizens = {
    capturedPose(id) {
      const entry = procPeople.get(id);
      const person = entry?.person;
      const palette = person && entry.at && procedural ? procedural.ragdoll.pose(person) : null;
      if (!person || !palette || !entry.at) return pedestrians.capturedPose(id);
      return { index: -1 - id, palette, transform: entry.at.clone().multiply(procScale.makeScale(person.scale, person.scale, person.scale)) };
    },
    skeletonOf(index) {
      const person = procOf(index);
      return person && procedural ? procedural.ragdoll.skeleton(person) : pedestrians.skeletonOf(index);
    },
    drawPalette(index, palette, instance, charred, alive) {
      const person = procOf(index);
      if (!person || !procedural) { pedestrians.drawPalette(index, palette, instance, charred); return; }
      // On the ground: in pain if alive, slack if not.
      person.activity = alive ? 'hurt' : 'dead';
      if (index <= TWIN_BASE) {
        person.matrix.copy(instance).multiply(procScale.makeScale(1 / person.scale, 1 / person.scale, 1 / person.scale));
        procedural.ragdoll.hold(person, palette);
        return;
      }
      // The first piece of the body is the person; pieces torn off are their twins (`twin`).
      const id = -1 - index;
      if (procHeldNow.has(id)) return;
      procHeldNow.add(id);
      person.matrix.copy(instance).multiply(procScale.makeScale(1 / person.scale, 1 / person.scale, 1 / person.scale));
      procedural.ragdoll.hold(person, palette);
    },
    clipPose(index, key, phase) {
      const person = procOf(index);
      if (!person || !procedural) return key === 'crawl' ? null : pedestrians.clipPose(index, key === 'getUpFront' || key === 'getUpBack' ? 'crouchUp' : key, phase);
      const clip = key === 'crouchUp' ? 'getUp' : key === 'getUpFront' ? 'riseFront' : key === 'getUpBack' ? 'riseBack' : key === 'crawl' ? 'crawl' : 'idle';
      const palette = procedural.ragdoll.standing(person, phase, clip);
      return palette ? { palette, duration: clip === 'idle' ? 1.2 : procedural.ragdoll.duration(person, clip) } : null;
    },
    loadedIndices: () => pedestrians.loadedIndices(),
    twin(index) {
      const person = procOf(index);
      // A cooked body draws as many palettes as it has pieces.
      if (!person || !procedural) return index >= 0 ? index : null;
      const twin = procedural.ragdoll.twin(person);
      if (!twin) return null;
      const key = TWIN_BASE - procTwinNext++;
      procTwins.set(key, twin);
      return key;
    },
    untwin(index) {
      const twin = procTwins.get(index);
      if (!twin || !procedural) return;
      procedural.ragdoll.untwin(twin);
      procTwins.delete(index);
    },
    char(index) {
      const person = procOf(index);
      if (person && procedural) procedural.ragdoll.char(person, true);
    },
    wound(index, part) {
      const person = procOf(index);
      if (!person || !procedural) return;
      procedural.ragdoll.wound(person, part);
      // The jerk of the hit is the captured one now (the `hit` clip, `procDraw` flinch), not a turn of the spine laid over the clip.
    },
    drench(index) {
      const person = procOf(index);
      if (person && procedural) procedural.ragdoll.drench(person);
    },
  };
  // … 2 lines
  const meshes = [...allParts.map((part) => part.mesh), blobs.mesh, pedestrians.group, ...(procedural ? [procedural.group] : [])];
  // … 470 lines
  const seatWho: { -readonly [K in keyof ClipIdentity]-?: ClipIdentity[K] } = { seed: 0, gender: 'f', ageClass: 'adult', company: 'car', companyId: 0, hasChild: false };
  // … 379 lines
  return {
    meshes,
  // … 575 lines
  };
}

// src/render/agents.ts:651-656 slice of src/render/agents.ts#createAgentMeshes.bodyKey (shared): the feature's statements; "…" marks code left out
  const bodyKey = (vehicle: SimVehicle): string => {
    // … 3 lines
    return `${a.id}:${carStyleOf(a, agentHash(vehicle.id ^ 0x57e1))}`;
  };

// src/render/agents.ts:927-1050 slice of src/render/agents.ts#createAgentMeshes.procPose (shared): the feature's statements; "…" marks code left out
  const procPose = (id: number, x: number, y: number, heading: number, deck: number, speed: number, walking: boolean, dt: number, activity?: string,
    // … 1 lines
    wound?: { readonly part: BodyPart; readonly grave: boolean }, age?: PersonAgeClass): void => {
    // … 35 lines
    if (activity === 'flinch' && act) {
      // … 4 lines
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
    // … 3 lines
    }
  // … 64 lines
  };

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
      // … 2 lines
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
  // … 10 lines
  };

// src/render/agents.ts:1749-1756 slice of src/render/agents.ts#createAgentMeshes.placeKerbPerson.walkTo (shared): the feature's statements; "…" marks code left out
    const walkTo = (fromX: number, fromY: number, toX: number, toY: number, t: number, height: number): void => {
      // … 4 lines
      pedestrians.drawClip(identity, x, y, height, heading,
        [{ key: 'walk', phase: 0, distance: distance * t, weight: 1 }], 0, Infinity, true);
    };

// src/render/agents.ts:1809-1839 slice of src/render/agents.ts#createAgentMeshes.placeRiderFigure (shared): the feature's statements; "…" marks code left out
  const placeRiderFigure = (vehicle: SimVehicle, model: TwoWheelerModel, cyclist: boolean, s: VehicleDraw): void => {
    // … 23 lines
    const drawn = pedestrians.drawClip(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), x, y, height, frame[FRAME_YAW]!, riderPlays, frame[FRAME_ROLL]!, Infinity, false, 1,
      cyclist ? null : helmetMatrix);
    if (!cyclist && drawn > 0) {
      // A motorcyclist's helmet, fitted to this body's head (`riderPoses.helmetShape`) and put where the pose just drawn has it.
      write(helmets, helmetMatrix, s.helmet);
    }
  };

// src/render/agents.ts:2023-2023 slice of src/render/agents.ts#createAgentMeshes.census (shared): the feature's statements; "…" marks code left out
    census: () => pedestrians.census(),

// src/render/agents.ts:2086-2088 slice of src/render/agents.ts#createAgentMeshes.driverBody (shared): the feature's statements; "…" marks code left out
    driverBody(vehicle, x, y) {
      return pedestrians.indexFor(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), vehicle.archetype.shape === 'motorcycle', x, y);
    },

// src/render/agents.ts:2089-2146 method createAgentMeshes.carcass: the feature's own
    carcass(vehicle, intact = false) {
      const a = vehicle.archetype;
      const plan = planOf(a);
      const pieces: BufferGeometry[] = [];
      const put = (g: BufferGeometry, x: number, y: number, z: number, sx = 1, sy = 1, sz = 1): void => {
        pieces.push(g.clone().scale(sx, sy, sz).translate(x, y, z));
      };
      const d = plan.wheelRadius * 2;
      if (a.shape === 'motorcycle' || a.shape === 'bicycle') {
        const t = twoWheelerParts.get(a.id);
        if (!t) return null;
        const md = t.model;
        put(md.body, 0, 0, 0); put(md.trim, 0, 0, 0); put(md.steering, md.headX, md.headY, 0);
        if (md.cranks) put(md.cranks, md.bracketX, md.bracketY, 0);
        // The tyres burnt away: the rims left, sagging to the ground (whole, on a machine merely dropped).
        for (const along of plan.axleAlong) {
          if (intact) {
            put(a.shape === 'bicycle' ? thinTyreGeometry : wheelGeometry, along, plan.wheelRadius, 0, d, d, plan.tread);
            put(a.shape === 'bicycle' ? spokedGeometry : hubGeometry, along, plan.wheelRadius, 0, d * (a.shape === 'bicycle' ? 1 : 0.45), d * (a.shape === 'bicycle' ? 1 : 0.45), plan.tread);
          } else if (a.shape === 'bicycle') put(spokedGeometry, along, plan.wheelRadius * 0.93, 0, d * 0.95, d * 0.95, plan.tread);
          else put(hubGeometry, along, plan.wheelRadius * 0.75, 0, d * 0.72, d * 0.72, plan.tread);
        }
      } else {
        const car = carParts.get(bodyKey(vehicle as SimVehicle));
        if (!car) return null;
        const md = car.model;
        put(md.shell, 0, 0, 0); put(md.trim, 0, 0, 0); put(md.interior, 0, 0, 0);
        if (md.accent) put(md.accent, 0, 0, 0);
        if (md.roof) put(md.roof, 0, 0, 0);
        for (const along of plan.axleAlong) {
          for (const side of plan.axleSide === 0 ? [0] : SIDES) {
            put(hubGeometry, along, plan.wheelRadius * 0.72, -side * plan.axleSide, d * 0.62, d * 0.62, plan.tread * 0.6);
          }
        }
      }
      const welded = merge(pieces);
      // A wreck is drawn faceted (a loose triangle list's `computeVertexNormals`), as before `merge` was indexed: one geometry a wreck
      const merged = welded.toNonIndexed();
      welded.dispose();
      // Each wreck its own: the panels buckled and the roof sagged by the heat and the blast, by a pattern of its own (its id).
      merged.computeBoundingBox();
      const box = merged.boundingBox!;
      const centre = box.getCenter(new Vector3());
      merged.translate(-centre.x, -centre.y, -centre.z);
      if (intact) { merged.computeVertexNormals(); return merged; }
      const pos = merged.getAttribute('position');
      const seed = (vehicle.id * 2654435761) >>> 0;
      const wave = (x: number, y: number, z: number, k: number): number => Math.sin(x * k + (seed % 97)) * Math.cos(z * k * 1.3 + (seed % 61)) + Math.sin(y * k * 0.7 + (seed % 13));
      const half = (box.max.y - box.min.y) / 2;
      for (let i = 0; i < pos.count; i++) {
        const x = pos.getX(i), y = pos.getY(i), z = pos.getZ(i);
        const top = Math.max(0, y / Math.max(1e-6, half));
        const dent = wave(x, y, z, 1 / m(0.45)) * m(0.04) + wave(x, y, z, 1 / m(0.18)) * m(0.015);
        pos.setXYZ(i, x + dent * 0.5, y - top * top * m(0.12) + dent, z + dent * 0.6);
      }
      merged.computeVertexNormals();
      return merged;
    },

// src/render/agents.ts:2147-2147 method createAgentMeshes.setBleed: the feature's own
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
          // … 21 lines
          if (procedural && ped.id !== PLAYER_ID) {
          // … 10 lines
          else {
            // … 3 lines
            pedestrians.draw(ped, pose.p.x, pose.p.y, pose.angle, deck, alpha, ground);
          }
        // … 1 lines
        }
      }
      // … 20 lines
      pedestrians.finish();
      // … 3 lines
      casting = pedestrians.casting || (procedural ? procedural.casting : false);
    // … 24 lines
    },

// src/render/agents.ts:2574-2594 slice of src/render/agents.ts#createAgentMeshes.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      pedestrians.dispose();
    // … 18 lines
    },
