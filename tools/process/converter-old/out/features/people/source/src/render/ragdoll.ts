// The code of src/render/ragdoll.ts that is the feature's (the rest of the file is shared).

// src/render/ragdoll.ts:14-33 slice of src/render/ragdoll.ts#RagdollCitizens (shared): the feature's statements; "…" marks code left out
export interface RagdollCitizens {
  // … 15 lines
  wound?(index: number, part: BodyPart, fromX?: number, fromY?: number): void;
// … 2 lines
}

// src/render/ragdoll.ts:122-124 slice of src/render/ragdoll.ts#LOST_PARTS (shared): the feature's statements; "…" marks code left out
const LOST_PARTS: Readonly<Record<Severable, readonly PartName[]>> = {
  armL: ['lfa'], armR: ['rfa'], legL: ['lca', 'lfo'], legR: ['rca', 'rfo'], head: ['head'],
};

// src/render/ragdoll.ts:176-176 slice of src/render/ragdoll.ts#J_STUMP (shared): the feature's statements; "…" marks code left out
const J_STUMP: Readonly<Record<Severable, number>> = { armL: J_LUA, armR: J_RUA, legL: J_LTH, legR: J_RTH, head: J_TORSO };

// src/render/ragdoll.ts:222-291 slice of src/render/ragdoll.ts#Body (shared): the feature's statements; "…" marks code left out
interface Body {
  // … 44 lines
  partHits?: Partial<Record<BodyPart, number>>;
// … 23 lines
}

// src/render/ragdoll.ts:368-399 slice of src/render/ragdoll.ts#Ragdolls (shared): the feature's statements; "…" marks code left out
export interface Ragdolls {
  // … 1 lines
  absorb(list: readonly Casualty[], citizens: RagdollCitizens, world: RagdollWorld): void;
  // … 11 lines
  shootBody(a: Vector3, b: Vector3): { alive: number; part: BodyPart } | 'hit' | null;
// … 16 lines
}

// src/render/ragdoll.ts:401-1956 slice of src/render/ragdoll.ts#createRagdolls (shared): the feature's statements; "…" marks code left out
export function createRagdolls(exhaust: Exhaust, getUp: GetUp, gore: Gore | null = null): Ragdolls {
  // … 7 lines
  const taken = new WeakSet<Casualty>();
  // … 177 lines
  const STRUCK: Readonly<Record<BodyPart, readonly number[]>> = {
    head: [HEA, TOP], torso: [CHE, NEC, BEL], armL: [LE, LW], armR: [RE, RW], legL: [LK, LA], legR: [RK, RA],
  };
  // … 179 lines
  const STUMP: Readonly<Record<Severable, { body: readonly [number, number]; piece: readonly [number, number] }>> = {
    armL: { body: [LS, LE], piece: [LW, LE] }, armR: { body: [RS, RE], piece: [RW, RE] },
    legL: { body: [LH, LK], piece: [LA, LK] }, legR: { body: [RH, RK], piece: [RA, RK] },
    head: { body: [CHE, NEC], piece: [TOP, HEA] },
  };
// … 1182 lines
}

// src/render/ragdoll.ts:554-584 slice of src/render/ragdoll.ts#createRagdolls.blast (shared): the feature's statements; "…" marks code left out
  const blast = (body: Body, c: Casualty, strength: number): Vector3 => {
    const p = body.p;
    const dir = new Vector3(c.x - c.blastX, 0, -(c.y - c.blastY));
    if (dir.lengthSq() < 1e-6) dir.set(Math.random() - 0.5, 0, Math.random() - 0.5);
    dir.normalize();
    const speed = strength * (0.85 + Math.random() * 0.3);
    const lift = speed * (0.45 + 0.25 * Math.random());
    let low = Infinity, high = -Infinity;
    for (const v of p) { low = Math.min(low, v.y); high = Math.max(high, v.y); }
    const com = p.reduce((s, v) => s.add(v), new Vector3()).divideScalar(p.length);
    // Pitch: head back towards the blast; a little roll and twist besides.
    const spin = new Vector3().crossVectors(dir, UP).normalize().multiplyScalar((2 + 5 * c.power) * (0.7 + 0.6 * Math.random()))
      .addScaledVector(dir, (Math.random() - 0.5) * 4)
      .addScaledVector(UP, (Math.random() - 0.5) * 5);
    const scatter = LIMB_JOINTS.map(() => new Vector3(Math.random() - 0.5, Math.random() - 0.2, Math.random() - 0.5).multiplyScalar(speed * 0.45));
    launch(body, (k, v) => {
      const h = (p[k]!.y - low) / Math.max(1e-3, high - low);
      v.copy(dir).multiplyScalar(speed * (1.1 - 0.25 * h));
      v.y += lift;
      v.add(tmpD.crossVectors(spin, tmpC.subVectors(p[k]!, com)));
      LIMB_JOINTS.forEach(([, a, mid, b], i) => {
        if (k === b) v.add(scatter[i]!);
        else if (k === mid) v.addScaledVector(scatter[i]!, 0.6);
        else if (k === a) v.addScaledVector(scatter[i]!, 0.2);
      });
      if (k === LT) v.add(scatter[2]!);
      if (k === RT) v.add(scatter[3]!);
      return v;
    });
    return dir;
  };

// src/render/ragdoll.ts:592-609 slice of src/render/ragdoll.ts#createRagdolls.shove (shared): the feature's statements; "…" marks code left out
  const shove = (body: Body, c: Casualty): Vector3 => {
    const dir = new Vector3(c.x - c.blastX, 0, -(c.y - c.blastY));
    if (dir.lengthSq() < 1e-6) dir.set(1, 0, 0);
    dir.normalize();
    const hit = STRUCK[c.struck ?? 'torso'];
    const leg = c.struck === 'legL' || c.struck === 'legR';
    for (let k = 0; k < body.p.length; k++) {
      const push = hit.includes(k) ? m(leg ? 2.2 : 1.6) : k === CHE || k === NEC || k === HEA || k === TOP ? m(0.7) : 0;
      if (push > 0) body.o[k]!.addScaledVector(dir, -push * STEP);
    }
    // And over it goes, backwards, the way the round went: the upper body carried on along it while the knees give.
    const buckle = c.kind === 'knocked' && leg;
    const over = buckle ? new Vector3(Math.cos(c.heading), 0, -Math.sin(c.heading)) : dir;
    for (const k of [CHE, NEC, HEA, TOP, LS, RS, BEL]) body.o[k]!.addScaledVector(over, -(buckle ? m(0.9) : c.kind === 'knocked' ? m(1.5) : m(1.8)) * STEP);
    body.o[PEL]!.addScaledVector(over, -m(buckle ? 0.2 : 0.5) * STEP);
    body.asleep = false;
    return dir;
  };

// src/render/ragdoll.ts:612-700 slice of src/render/ragdoll.ts#createRagdolls.spawn (shared): the feature's statements; "…" marks code left out
  const spawn = (c: Casualty, citizens: RagdollCitizens, world: RagdollWorld,
    given?: { index: number; palette: Float32Array; transform: Matrix4 }): void => {
    const groundHere = world.groundAt(c.x, c.y);
    const known = bodies.find((b) => b.survivor?.id === c.id);
    if (known) {
      // On the ground already, and struck again: thrown again, and killed if it was a killing blow.
      if (c.faded) {
        // Bled out where they lay: the writhing stops, the body goes still in a pool.
        known.survivor = undefined;
        known.asleep = false; known.still = 0;
        const at = known.p[PEL]!;
        bleed(at.x, -at.z, world.groundAt(at.x, -at.z), m(1.6), 20);
        return;
      }
      if (c.kind !== 'knocked') {
        // Killed where the body lies, not where they first fell from.
        known.survivor = undefined; known.torn = false;
        const at = known.p[PEL]!;
        bleed(at.x, -at.z, world.groundAt(at.x, -at.z), m(0.8), 4);
      }
      else if (known.survivor) {
        const alive = known.survivor;
        alive.phase = 'fall'; alive.t = 0; delete alive.from;
        if (c.lieFor !== undefined) alive.lie = Math.max(alive.lie, c.lieFor);
        if (c.crawl && !alive.crawl) alive.crawl = { since: 0 };
      }
      for (const limb of c.lost ?? []) if (!(c.severed ?? []).includes(limb)) for (const part of LOST_PARTS[limb]) known.lostParts.add(part);
      known.asleep = false; known.still = 0; known.flying = 0;
      const away = c.struck ? shove(known, c) : blast(known, c, c.kind === 'knocked' ? m(2.5 + 4 * c.power) : m(4 + 9 * c.power));
      sever(known, c, away, citizens);
      if (c.opened && !known.opened && !known.pin) openBelly(known, away.clone().multiplyScalar(m(1.5)), 1 + Math.floor(Math.random() * 2));
      return;
    }
    const body = build(c.id, c.heading, citizens, world, c.x, c.y, c.kind, given);
    if (!body) { if (c.kind !== 'knocked') bleed(c.x, c.y, groundHere, m(1.8), 15); return; }
    for (const limb of c.lost ?? []) for (const part of LOST_PARTS[limb]) body.lostParts.add(part);
    body.recordId = c.id;
    if (body.survivor && c.lieFor !== undefined) body.survivor.lie = c.lieFor;
    // Bled out on their feet or crawling: the strength goes and they slump over, the trunk down and to a side
    if (c.faded) {
      const side = Math.random() < 0.5 ? 1 : -1;
      const lean = new Vector3(Math.cos(c.heading + side * Math.PI / 2), -1.2, -Math.sin(c.heading + side * Math.PI / 2));
      for (const k of [CHE, NEC, HEA, TOP, LS, RS, BEL]) body.o[k]!.addScaledVector(lean, -m(1.4) * STEP);
    }
    if (body.survivor && c.crawl) body.survivor.crawl = { since: 0 };
    // A bullet does not throw a body: a small push where it went in
    const speed = c.struck ? m(0.6) : c.kind === 'knocked' ? m(2.5 + 4 * c.power) : m(4 + 9 * c.power);
    const dir = c.struck ? shove(body, c) : blast(body, c, speed);
    // Alive going down: the arms out the way they fall, to take it
    if (c.struck && c.kind === 'knocked') {
      const leg = c.struck === 'legL' || c.struck === 'legR';
      const way = leg ? new Vector3(Math.cos(c.heading), 0, -Math.sin(c.heading)) : dir;
      for (const k of [LW, RW, LE, RE]) body.o[k]!.addScaledVector(way, -m(k === LW || k === RW ? 1.4 : 0.8) * STEP).y += m(0.6) * STEP;
    }
    // Joint friction while it falls: dead weight, not a rag.
    body.stiff = 1.6;
    const chest = body.p[CHE]!;
    if (c.kind === 'torn') {
      // Torn: thrown harder, limbs flung, blood everywhere - but whole.
      for (const limb of LIMBS.filter(() => Math.random() < 0.5)) {
        const kick = new Vector3(Math.random() - 0.5, 0.3 + Math.random() * 0.6, Math.random() - 0.5).multiplyScalar(speed * 0.5).addScaledVector(dir, speed * 0.3);
        for (const k of limb) body.o[k]!.addScaledVector(kick, -STEP);
      }
      exhaust.burst(chest.x, -chest.z, chest.y, 90, 4, m(0.6), m(0.06), 1.2);
      for (let k = 0; k < 18; k++) {
        const a = Math.random() * Math.PI * 2, r = m(1 + Math.random() * 6);
        bleed(c.x + Math.cos(a) * r, c.y + Math.sin(a) * r, groundHere, m(0.5 + Math.random() * 1.1), 0);
      }
      bleed(c.x, c.y, groundHere, m(3.4), 1.8);
      // Pieces of them: an arm, a leg, and what was inside, flung over the street.
      api.onGore?.(chest.x, -chest.z, chest.y, dir.x, -dir.z, speed, 'torn');
    } else if (c.kind === 'dead') {
      exhaust.burst(chest.x, -chest.z, chest.y, 24, 4, m(0.25), m(0.045), 0.9);
      // Dead: the pool spreads under the body where it comes to rest
      for (let k = 0; k < 4; k++) {
        const a = Math.random() * Math.PI * 2, r = m(0.8 + Math.random() * 2.5);
        bleed(c.x + Math.cos(a) * r, c.y + Math.sin(a) * r, groundHere, m(0.3 + Math.random() * 0.5), 0);
      }
      if (!c.struck && Math.random() < 0.35) api.onGore?.(chest.x, -chest.z, chest.y, dir.x, -dir.z, speed * 0.6, 'dead');
    }
    add(body);
    sever(body, c, dir, citizens);
    // A blast tears a body open (one torn apart, most often); a bullet never does it at once
    if (!c.struck && (c.kind === 'torn' ? Math.random() < 0.7 : c.kind === 'dead' && Math.random() < 0.12)) {
      openBelly(body, dir.clone().multiplyScalar(m(3)), 2 + Math.floor(Math.random() * 3));
    }
    // Shot open (`walk.ts` shot: a grave trunk wound, two or three rounds in it).
    if (c.opened && !body.opened) openBelly(body, dir.clone().multiplyScalar(m(1.5)), 1 + Math.floor(Math.random() * 2));
  };

// src/render/ragdoll.ts:703-714 slice of src/render/ragdoll.ts#createRagdolls.sever (shared): the feature's statements; "…" marks code left out
  const sever = (body: Body, c: Casualty, dir: Vector3, citizens: RagdollCitizens): void => {
    // Charred first: the pieces are twins of the body as it is.
    if (c.charred) { body.charred = true; citizens.char?.(body.index); }
    for (const limb of c.severed ?? []) {
      // A bullet takes a limb off and drops it near; a blast throws it.
      const speed = (c.struck ? m(0.8 + Math.random() * 0.8) : m(2.5 + 6 * Math.max(0.15, c.power))) * (0.7 + Math.random() * 0.6);
      const spread = c.struck ? m(0.6) : m(3);
      const kick = new Vector3(dir.x * speed + (Math.random() - 0.5) * spread, (c.struck ? m(0.4 + Math.random() * 0.6) : m(1.5 + Math.random() * 3) * (0.6 + c.power)),
        dir.z * speed + (Math.random() - 0.5) * spread);
      detach(body, limb, citizens, kick);
    }
  };

// src/render/ragdoll.ts:800-842 slice of src/render/ragdoll.ts#createRagdolls.detach (shared): the feature's statements; "…" marks code left out
  const detach = (body: Body, limb: Severable, citizens: RagdollCitizens, kick: Vector3): void => {
    const parts = LOST_PARTS[limb];
    const index = citizens.twin ? citizens.twin(body.index) : body.index;
    if (index === null) { for (const part of parts) body.lostParts.add(part); return; }
    const keep = new Set<number>();
    for (const part of parts) { keep.add(PARTS[part].origin); keep.add(PARTS[part].to); }
    const root = PARTS[parts[0]!].origin;
    const piece: Body = {
      ...body,
      index,
      p: body.p.map((v) => v.clone()),
      o: body.o.map((v) => v.clone()),
      ground: body.ground.slice(),
      sticks: body.sticks.map((s) => ({ ...s, broken: s.broken || !(keep.has(s.a) && keep.has(s.b)) })),
      normals: { larm: body.normals.larm.clone(), rarm: body.normals.rarm.clone(), lleg: body.normals.lleg.clone(), rleg: body.normals.rleg.clone() },
      lostParts: new Set(PART_NAMES.filter((part) => !parts.includes(part))),
      survivor: undefined, pin: { root, keep }, drop: index !== body.index ? () => citizens.untwin?.(index) : undefined,
      comp: [], palettes: [], palettePool: [], anchor: new Matrix4(), still: 0, asleep: false, age: 0, flying: 0, pooled: false, splats: 0, spray: 0, torn: true,
    };
    components(piece);
    if (body.jolt) {
      // In Jolt: the joint holding it lets go, and its parts go with the piece
      const torn = [...new Set(parts.map((q) => J_PART[q]))];
      const at = body.p[limb === 'head' ? NEC : root]!.clone().divideScalar(METRE);
      const moved = body.p.map((_, k) => k).filter((k) => torn.includes(body.jolt!.partOf(k)));
      piece.jolt = body.jolt.tear(torn, J_STUMP[limb], at, moved, { particle: root, part: J_PART[parts[0]!] });
      piece.synced = body.synced ? { p: body.synced.p.map((v) => v.clone()), o: body.synced.o.map((v) => v.clone()) } : undefined;
      if (body.colliders) body.colliders.users++;
    }
    const spin = new Vector3((Math.random() - 0.5) * 2, Math.random(), (Math.random() - 0.5) * 2).multiplyScalar(kick.length() * 0.6);
    for (const k of keep) piece.o[k]!.copy(piece.p[k]!).addScaledVector(kick, -STEP).addScaledVector(spin, k === root ? 0 : -STEP);
    for (const part of parts) body.lostParts.add(part);
    body.asleep = false;
    // The stump spurting.
    const at = body.p[root]!;
    exhaust.burst(at.x, -at.z, at.y, 30, 4, m(0.22), m(0.04), 0.9);
    add(piece);
    // Now and then the bone shows: out of the stump, out of the piece, or both.
    const stump = STUMP[limb];
    const roll = Math.random();
    if (roll < 0.55) boneOut(body, stump.body[0], stump.body[1], m(limb === 'head' ? 0.05 : 0.07));
    if (roll > 0.3 && roll < 0.8) boneOut(piece, stump.piece[0], stump.piece[1], m(limb === 'head' ? 0.04 : 0.06));
  };

// src/render/ragdoll.ts:919-936 slice of src/render/ragdoll.ts#createRagdolls.fling (shared): the feature's statements; "…" marks code left out
    fling(list, citizens, world) {
      // … 3 lines
      for (const o of list) {
        // … 5 lines
        const all: Severable[] = ['head', 'armL', 'armR', 'legL', 'legR'];
        const severed = o.kind === 'torn' ? all.filter(() => Math.random() < 0.45)
          : o.kind === 'dead' && Math.random() < 0.35 ? [all[1 + Math.floor(Math.random() * 4)]!] : [];
        const c = { x: o.x, y: o.y, heading: o.heading, kind: o.kind, t: 0, id: o.id, gender: 'm', ageClass: 'adult', party: null,
          blastX: o.blastX, blastY: o.blastY, power: o.power, severed, lost: severed, charred: o.charred === true } as unknown as Casualty;
        spawn(c, citizens, world, { index, palette: clip.palette, transform });
      }
    },

// src/render/ragdoll.ts:937-987 slice of src/render/ragdoll.ts#createRagdolls.shootBody (shared): the feature's statements; "…" marks code left out
    shootBody(a, b) {
      // … 18 lines
      const part: BodyPart = k === HEA || k === TOP ? 'head' : k === LS || k === LE || k === LW ? 'armL' : k === RS || k === RE || k === RW ? 'armR'
        : k === LH || k === LK || k === LA || k === LT ? 'legL' : k === RH || k === RK || k === RA || k === RT ? 'legR' : 'torso';
      if (body.survivor) return { alive: body.survivor.id, part };
      // … 9 lines
      citizens.wound?.(body.index, part);
      // … 2 lines
      tally[part] = (tally[part] ?? 0) + 1;
      if (part === 'torso' && !body.opened && (tally.torso ?? 0) >= 2) openBelly(body, dir.clone().multiplyScalar(m(1.2)), 1);
      // … 1 lines
      if (!body.pin && part !== 'torso') {
        const limb = part as Severable;
        const gone = LOST_PARTS[limb].every((q) => body.lostParts.has(q));
        if (!gone && tally[part]! >= (part === 'head' ? 1 + (Math.random() < 0.5 ? 1 : 0) : 2)) {
          const kick = dir.clone().multiplyScalar(m(1)).add(new Vector3((Math.random() - 0.5) * m(0.6), m(0.5 + Math.random() * 0.8), (Math.random() - 0.5) * m(0.6)));
          detach(body, limb, citizens, kick);
        }
      }
    // … 5 lines
    },
