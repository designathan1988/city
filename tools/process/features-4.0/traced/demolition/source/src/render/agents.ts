// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:137-166 slice of src/render/agents.ts#AgentRenderOptions (shared): the feature's statements; "…" marks code left out
interface AgentRenderOptions {
  // … 15 lines
  readonly ragdolls?: (citizens: RagdollCitizens) => void;
// … 12 lines
}

// src/render/agents.ts:552-2596 slice of src/render/agents.ts#createAgentMeshes (shared): the feature's statements; "…" marks code left out
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  // … 2 lines
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes {
  // … 496 lines
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
// … 1431 lines
}

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
