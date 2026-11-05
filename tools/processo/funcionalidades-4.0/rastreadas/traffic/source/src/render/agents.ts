// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:168-198 slice of src/render/agents.ts#AgentMeshes (shared): the feature's statements; "…" marks code left out
export interface AgentMeshes {
  // … 11 lines
  carcass(vehicle: { readonly id: number; readonly archetype: Archetype }, intact?: boolean): BufferGeometry | null;
// … 17 lines
}

// src/render/agents.ts:379-401 slice of src/render/agents.ts#BodyPlan (shared): the feature's statements; "…" marks code left out
interface BodyPlan {
  readonly shape: VehicleShape;
// … 20 lines
}

// src/render/agents.ts:403-432 slice of src/render/agents.ts#bodyPlan (shared): the feature's statements; "…" marks code left out
function bodyPlan(a: Archetype): BodyPlan {
  const twoWheeler = a.shape === 'motorcycle' || a.shape === 'bicycle';
  const bus = a.shape === 'bus';
  const truck = a.shape === 'truck';
  const tread = twoWheeler ? a.width * (a.shape === 'bicycle' ? 0.14 : 0.3) : a.width * 0.14;
  // … 2 lines
  const axleAlong: readonly number[] = axleStations(a);
  // … 2 lines
  const seatY = a.height * (bus ? 0.55 : truck ? 0.5 : 0.46);
  return {
    shape: a.shape,
    length: a.length,
    width: a.width,
    height: a.height,
    wheelRadius: a.wheelRadius,
    tread,
    axleAlong,
    axleSide: twoWheeler ? 0 : a.width * 0.5 - tread * 0.5,
    cabinLength: a.length * a.cabinFraction,
    cabinAlong: a.length * a.cabinShift,
    seats: a.seats,
    seatY,
    headY: seatY + SEAT_HEAD_RISE,
    seatAlong: bus ? a.length * 0.36 : a.length * a.cabinShift,
    seatPitch: bus ? -a.length * 0.24 : truck ? 0 : -a.length * 0.2,
    seatSide: a.width * 0.24,
  };
}

// src/render/agents.ts:434-436 slice of src/render/agents.ts#PLANS (shared): the feature's statements; "…" marks code left out
const PLANS: ReadonlyMap<string, BodyPlan> = new Map(
  ARCHETYPES.map((a) => [a.id, bodyPlan(a)] as const),
);

// src/render/agents.ts:437-437 slice of src/render/agents.ts#FALLBACK_PLAN (shared): the feature's statements; "…" marks code left out
const FALLBACK_PLAN = bodyPlan(archetypeById('sedan'));

// src/render/agents.ts:438-438 slice of src/render/agents.ts#planOf (shared): the feature's statements; "…" marks code left out
const planOf = (a: Archetype): BodyPlan => PLANS.get(a.id) ?? FALLBACK_PLAN;

// src/render/agents.ts:526-527 slice of src/render/agents.ts#levelsKey (shared): the feature's statements; "…" marks code left out
const levelsKey = (plan: BodyPlan, pixels: number): number =>
  levelFor(pixels) + LEVELS * (levelFor(pixels * tyreSize(plan)) + LEVELS * levelFor(pixels * rimSize(plan)));

// src/render/agents.ts:530-543 slice of src/render/agents.ts#standingRange (shared): the feature's statements; "…" marks code left out
function standingRange(plan: BodyPlan, own: number, pixels: number, ratio: number, occupantZoom: number, out: { lo: number; hi: number }): void {
  // … 5 lines
  for (let k = 0; k < 3; k++) {
    // … 1 lines
    const unit = ratio * (k === 0 ? 1 : k === 1 ? tyreSize(plan) : rimSize(plan)), level = levelFor(pixels * unit);
    if (level < LEVEL_ERRORS.length) lo = Math.max(lo, ERROR_PIXELS / (LEVEL_ERRORS[level]! * unit));
    if (level > 0) hi = Math.min(hi, ERROR_PIXELS / (LEVEL_ERRORS[level - 1]! * unit));
  }
  out.lo = lo; out.hi = hi;
}

// src/render/agents.ts:552-2596 slice of src/render/agents.ts#createAgentMeshes (shared): the feature's statements; "…" marks code left out
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  // … 2 lines
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes {
  // … 8 lines
  const joinedPaint = panelPaint(paint);
  // … 16 lines
  const windowMaterial = windowGlassMaterial(glassMaterial);
  // … 18 lines
  const wheelGeometry = tyreGeometry();
  const hubGeometry = rimGeometry();
  // … 1 lines
  const thinTyreGeometry = tyreGeometry(0.035);
  const spokedGeometry = spokedRimGeometry();
  // … 1 lines
  const wheels = instanced('vehicle-wheels', wheelGeometry, rubber);
  const hubs = instanced('vehicle-hubs', hubGeometry, trim, false);
  const thinWheels = instanced('bicycle-wheels', thinTyreGeometry, rubber);
  const spokes = instanced('bicycle-spokes', spokedGeometry, trim, false);
  // … 6 lines
  const helmetGeometry = merge([new SphereGeometry(0.5, HELMET_SEGMENTS.width, HELMET_SEGMENTS.height)]);
  const helmets = instanced('rider-helmets', helmetGeometry, paint);
  // … 1 lines
  const parts: readonly Part[] = [wheels, hubs, thinWheels, spokes, lamps, helmets];
  // … 1 lines
  interface CarParts {
    readonly model: VehicleModel;
    /** The body, glazing and cabin with every door shut: one instance each, at the level its size on the screen allows. */
    readonly shell: Ladder;
    readonly glass: Ladder;
    readonly interior: Ladder;
    /** The same while a door moves: the pieces, and each leaf on its own. */
    readonly openShell: Part;
    readonly openGlass: Part;
    readonly openInterior: Part;
    readonly doors: readonly Part[];
    readonly doorGlass: readonly Part[];
    readonly doorCards: readonly (Part | null)[];
    readonly trim: Ladder;
    /** The roof panel in paint, or in glass on a car with a panoramic roof. */
    readonly roof: Ladder | null;
    readonly roofGlass: Part | null;
    readonly accent: Ladder | null;
    /** The body, roof and accent of the middle level: the same, casting no shadow (a soft disc instead). */
    readonly shellLite: Ladder;
    readonly roofLite: Ladder | null;
    readonly accentLite: Ladder | null;
    readonly far: Part;
    readonly steering: Part | null;
    /** The body and its panel (the roof, or else the accent) in one draw, doors shut (`vehiclePanels.ts`); null for a body with no panel. */
    readonly panel: Ladder | null;
    readonly panelLite: Ladder | null;
  }
  // … 51 lines
  const joinPanels = createPanelJoiner();
  // … 54 lines
  const wheelLevels = ladder(wheels, 'boxes', 4);
  const hubLevels = ladder(hubs, 'boxes', 4);
  // … 1 lines
  const bodies: { key: string; model: VehicleModel }[] = [];
  for (const archetype of ARCHETYPES) {
    if (archetype.shape === 'car') {
      for (const style of carStylesFor(archetype)) bodies.push({ key: `${archetype.id}:${style}`, model: buildCarModel(archetype, style) });
    } else if (archetype.shape === 'bus') bodies.push({ key: `${archetype.id}:bus`, model: buildBusModel(archetype) });
    else if (archetype.shape === 'truck') bodies.push({ key: `${archetype.id}:truck`, model: buildTruckModel(archetype) });
  }
  for (const { key, model } of bodies) {
    const id = key.replace(':', '-');
    const glazed = windowGlass(model);
    const own = new Set<BufferGeometry>([model.shell, model.glass, glazed, model.interior, model.openShell, model.openGlass,
      model.openInterior, model.trim, model.far, ...model.doors.flatMap((d) => [d.panel, d.glass, ...(d.card ? [d.card] : [])])]);
    if (model.roof) own.add(model.roof);
    if (model.accent) own.add(model.accent);
    if (model.steering) own.add(model.steering.geometry);
    modelGeometries.push(...own);
    carParts.set(key, {
      model,
      shell: ladder(instanced(`car-${id}-shell`, model.shell, paint), 'smooth'),
      glass: glazing(instanced(`car-${id}-glass`, glazed, windowMaterial, false)),
      interior: ladder(instanced(`car-${id}-interior`, model.interior, cabinMaterial, false), 'boxes'),
      openShell: instanced(`car-${id}-openshell`, model.openShell, paint),
      openGlass: instanced(`car-${id}-openglass`, model.openGlass, glassMaterial, false),
      openInterior: instanced(`car-${id}-openinterior`, model.openInterior, cabinMaterial, false),
      doors: model.doors.map((d, i) => instanced(`car-${id}-door${i}`, d.panel, paint)),
      doorGlass: model.doors.map((d, i) => instanced(`car-${id}-doorglass${i}`, d.glass, glassMaterial, false)),
      doorCards: model.doors.map((d, i) => d.card ? instanced(`car-${id}-doorcard${i}`, d.card, cabinMaterial, false) : null),
      trim: ladder(instanced(`car-${id}-trim`, model.trim, trim, false), 'boxes'),
      roof: model.roof ? ladder(instanced(`car-${id}-roof`, model.roof, paint), 'smooth') : null,
      roofGlass: model.roof ? instanced(`car-${id}-roofglass`, model.roof, glassMaterial, false) : null,
      accent: model.accent ? ladder(instanced(`car-${id}-accent`, model.accent, paint), 'smooth') : null,
      shellLite: ladder(instanced(`car-${id}-shell-lite`, model.shell, paint, false), 'smooth'),
      roofLite: model.roof ? ladder(instanced(`car-${id}-roof-lite`, model.roof, paint, false), 'smooth') : null,
      accentLite: model.accent ? ladder(instanced(`car-${id}-accent-lite`, model.accent, paint, false), 'smooth') : null,
      // A car under twelve pixels: its shadow is the soft disc under it.
      far: instanced(`car-${id}-far`, model.far, paint, false),
      steering: model.steering ? instanced(`car-${id}-wheel`, model.steering.geometry, cabinMaterial, false) : null,
      panel: panelLadder(`car-${id}-body`, model.shell, model.roof ?? model.accent, true),
      panelLite: panelLadder(`car-${id}-body-lite`, model.shell, model.roof ?? model.accent, false),
    });
  }
  // … 18 lines
  interface TwoWheelerParts {
    readonly model: TwoWheelerModel;
    readonly body: Ladder;
    readonly trim: Ladder;
    readonly steering: Ladder;
    readonly cranks: Ladder | null;
  }
  // … 1 lines
  for (const archetype of ARCHETYPES) {
    if (archetype.shape !== 'motorcycle' && archetype.shape !== 'bicycle') continue;
    const model = buildTwoWheelerModel(archetype);
    const id = archetype.id;
    modelGeometries.push(model.body, model.trim, model.steering, ...(model.cranks ? [model.cranks] : []));
    twoWheelerParts.set(id, {
      model,
      body: ladder(instanced(`bike-${id}-body`, model.body, paint), 'smooth'),
      trim: ladder(instanced(`bike-${id}-trim`, model.trim, trim), 'boxes'),
      steering: ladder(instanced(`bike-${id}-steering`, model.steering, trim, false), 'boxes'),
      cranks: model.cranks ? ladder(instanced(`bike-${id}-cranks`, model.cranks, trim, false), 'boxes') : null,
    });
  }
  // … 2 lines
  const allParts: readonly Part[] = [...parts, ...carPartList, ...twoWheelerPartList, ...ladderParts, ...panelParts];
  // … 326 lines
  const meshes = [...allParts.map((part) => part.mesh), blobs.mesh, pedestrians.group, ...(procedural ? [procedural.group] : [])];
  // … 450 lines
  const seated: SeatPerson = { seed: 0, gender: 'm', ageClass: 'adult' };
  // … 70 lines
  const seatPlays: Play[] = [];
  // … 30 lines
  const kerb = createKerbFigure();
  // … 157 lines
  interface VehicleDraw extends KeptHolder, LeanState {
    /** The view it was last drawn for: another view of the same id asks its lean and its ground again. */
    view: SimVehicle | null;
    archetype: Archetype | null;
    plan: BodyPlan;
    car: CarParts | null;
    bike: TwoWheelerParts | null;
    /** `vehicleLook`, a pure function of the id and the seat count, and its two tints. */
    look: VehicleLook;
    lookSeats: number;
    accent: Tint;
    helmet: Tint;
    color: string | null;
    paint: number;
    paintTint: Tint;
    /** Suspension: the frame eases towards the road, never snaps; and the level it was drawn at (-1: none yet). */
    rideSet: boolean;
    deck: number;
    pitch: number;
    roll: number;
    lod: number;
    /** The frame it was last drawn in. */
    seen: number;
    /** The ground under it off the road (`offRoadDeck`): where it was asked and the diary's serial then. */
    deckKnown: boolean;
    deckX: number;
    deckY: number;
    deckValue: number;
    deckSeen: number;
    deckSegment: SegmentId | undefined;
    /** Standing still in its bay: its place in `keptList` (-1: none), the frame it was last drawn kept, its kept instances and what they were written for. */
    keptAt: number;
    keptSeen: number;
    readonly keptParts: Part[];
    keptX: number;
    keptY: number;
    keptAngle: number;
    keptDeck: number;
    keptBand: number;
    keptLevels: number;
    keptVersion: number;
    keptPaint: number;
    keptShadows: boolean;
    keptCastOnly: boolean;
    /** Kept as a parked car (lamps off) or as one stopped in the traffic. */
    keptParked: boolean;
    /** The deck last asked for it and where (`sync`'s cull): a later frame tests the view with it, grown by the most it can have climbed since. */
    cullKnown: boolean;
    cullX: number;
    cullY: number;
    cullDeck: number;
  }
  // … 88 lines
  return {
    meshes,
  // … 575 lines
  };
}

// src/render/agents.ts:621-648 slice of src/render/agents.ts#CarParts (shared): the feature's statements; "…" marks code left out
  interface CarParts {
    readonly model: VehicleModel;
  // … 25 lines
  }

// src/render/agents.ts:651-656 slice of src/render/agents.ts#createAgentMeshes.bodyKey (shared): the feature's statements; "…" marks code left out
  const bodyKey = (vehicle: SimVehicle): string => {
    // … 1 lines
    if (a.shape === 'bus') return `${a.id}:bus`;
    if (a.shape === 'truck') return `${a.id}:truck`;
    return `${a.id}:${carStyleOf(a, agentHash(vehicle.id ^ 0x57e1))}`;
  };

// src/render/agents.ts:664-672 slice of src/render/agents.ts#createAgentMeshes.levelsFor (shared): the feature's statements; "…" marks code left out
  const levelsFor = (source: BufferGeometry, weld: Weld, copies = 1): Promise<(BufferGeometry | null)[]> => {
    // … 1 lines
    if (!levels) {
      levels = partLevels(source, weld, copies);
      levelsOf.set(source, levels);
      void levels.then((all) => { for (const g of new Set(all)) if (g && g !== source) modelGeometries.push(g); });
    }
    return levels;
  };

// src/render/agents.ts:674-699 slice of src/render/agents.ts#createAgentMeshes.ladder (shared): the feature's statements; "…" marks code left out
  const ladder = (base: Part, weld: Weld, copies = 1): Ladder => {
    // … 1 lines
    for (let k = 1; k < LEVELS; k++) {
      const part = instanced(`${base.mesh.name}@${k}`, base.mesh.geometry, base.mesh.material as Material, base.mesh.castShadow);
      parts.push(part);
      ladderParts.push(part);
    }
    const made: Ladder = { parts, route: new Array<number>(LEVELS).fill(0) };
    void levelsFor(base.mesh.geometry, weld, copies).then((all) => {
      for (let k = 1; k < all.length; k++) {
        // … 1 lines
        if (!geometry) made.route[k] = -1;
        else if (geometry === all[k - 1]) made.route[k] = made.route[k - 1]!;
        else {
          // … 3 lines
          if (drops) geometry.setAttribute(WINDOW_DROP, drops);
        // … 1 lines
        }
      }
    // … 2 lines
    }, (error: unknown) => console.error(`[vehicles] ${base.mesh.name}: levels not made, drawn whole`, error));
  // … 1 lines
  };

// src/render/agents.ts:704-714 slice of src/render/agents.ts#createAgentMeshes.panelPart (shared): the feature's statements; "…" marks code left out
  const panelPart = (name: string, joined: JoinedPanels, casts: boolean): Part => {
    const colours = new InstancedBufferAttribute(new Float32Array(PART_CAPACITY * 3).fill(1), 3);
    colours.setUsage(DynamicDrawUsage);
    const geometry = panelGeometry(joined, colours);
    modelGeometries.push(geometry);
    const part = instanced(name, geometry, joinedPaint, casts);
    part.panel = colours;
    part.tinted = true;
    panelParts.push(part);
    return part;
  };

// src/render/agents.ts:716-743 slice of src/render/agents.ts#createAgentMeshes.panelLadder (shared): the feature's statements; "…" marks code left out
  const panelLadder = (name: string, body: BufferGeometry, panel: BufferGeometry | null, casts: boolean): Ladder | null => {
    // … 4 lines
    for (let k = 0; k < LEVELS; k++) parts.push(panelPart(k ? `${name}@${k}` : name, base, casts));
    const made: Ladder = { parts, route: new Array<number>(LEVELS).fill(0) };
    void Promise.all([levelsFor(body, 'smooth'), levelsFor(panel, 'smooth')]).then(([bodies, panels]) => {
      // … 2 lines
      for (let k = 1; k < LEVELS; k++) {
        const b = at(bodies, k), p = at(panels, k);
        if (b === at(bodies, k - 1) && p === at(panels, k - 1)) { made.route[k] = made.route[k - 1]!; continue; }
        const joined = joinPanels(b, p);
        if (!joined) {
          if (b || p) { panelsApart.add(made); console.error(`[vehicles] ${name}@${k}: body and panel not joined, drawn apart`); }
          made.route[k] = -1;
          continue;
        }
        parts[k]!.mesh.geometry = panelGeometry(joined, parts[k]!.panel!);
        modelGeometries.push(parts[k]!.mesh.geometry);
        made.route[k] = k;
      }
    // … 2 lines
    }, (error: unknown) => console.error(`[vehicles] ${name}: levels not made, drawn whole`, error));
  // … 1 lines
  };

// src/render/agents.ts:745-753 slice of src/render/agents.ts#createAgentMeshes.glazing (shared): the feature's statements; "…" marks code left out
  const glazing = (base: Part): Ladder => {
    // … 5 lines
    base.mesh.geometry.setAttribute(WINDOW_DROP, base.windows!);
  // … 1 lines
  };

// src/render/agents.ts:817-823 slice of src/render/agents.ts#TwoWheelerParts (shared): the feature's statements; "…" marks code left out
  interface TwoWheelerParts {
    readonly model: TwoWheelerModel;
  // … 4 lines
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

// src/render/agents.ts:1416-1453 slice of src/render/agents.ts#createAgentMeshes.growPart (shared): the feature's statements; "…" marks code left out
  const growPart = (part: Part, needed: number): void => {
    // … 16 lines
    if (part.windows) {
      // … 4 lines
      mesh.geometry.setAttribute(WINDOW_DROP, windows);
    }
  // … 13 lines
  };

// src/render/agents.ts:1462-1502 slice of src/render/agents.ts#createAgentMeshes.placeWheels (shared): the feature's statements; "…" marks code left out
  const placeWheels = (plan: BodyPlan, band: number): void => {
    // … 2 lines
    const wheels = rung(wheelLevels, levelFor(frame[FRAME_PIXELS]! * tyreSize(plan)));
    const hubs = rung(hubLevels, levelFor(frame[FRAME_PIXELS]! * rimSize(plan)));
    // … 1 lines
    for (let axle = 0; axle < axles.length; axle++) {
      // … 8 lines
      if (plan.axleSide === 0) {
        if (wheels) place(wheels, along, 0, plan.wheelRadius, d, d, plan.tread, T_RUBBER, 0, steer);
        if (band >= 1 && hubs) {
          place(hubs, along, 0, plan.wheelRadius, d * 0.45, d * 0.45, plan.tread * 1.1, T_HUB, lamp.spin, steer);
        }
      // … 1 lines
      }
      for (const side of SIDES) {
        if (wheels) place(wheels, along, side * plan.axleSide, plan.wheelRadius, d, d, plan.tread, T_RUBBER, 0, steer);
        if (band >= 1 && hubs) {
          place(
            hubs,
            along,
            side * (plan.axleSide + plan.tread * 0.3),
            plan.wheelRadius,
            d * 0.52,
            d * 0.52,
            plan.tread * 0.5,
            T_HUB,
            lamp.spin,
            steer,
          );
        }
      }
    }
  };

// src/render/agents.ts:1532-1611 slice of src/render/agents.ts#createAgentMeshes.drawCar (shared): the feature's statements; "…" marks code left out
  const drawCar = (vehicle: SimVehicle, s: VehicleDraw, band: number): void => {
    // … 4 lines
    if (band < 1) {
      // … 2 lines
      for (const l of model.taillamps) place(lamps, l.x, -l.z, l.y, l.sx, l.sy, l.sz, lamp.tail);
    // … 1 lines
    }
    // … 3 lines
    const detail = levelFor(frame[FRAME_PIXELS]!);
    // … 6 lines
    const doors = model.doors;
    if (joined) wholePanels(joined, detail, paint, panelTint);
    if (!doorsMoving) {
      if (!joined) wholeAt(shell, detail, paint);
      // … 1 lines
      wholeGlass(car.glass, detail, (look.windowsDown & 1) !== 0 ? look.windowDrop : 0, (look.windowsDown & 2) !== 0 ? look.windowDrop : 0);
    } else {
      // … 2 lines
      for (let i = 0; i < doors.length; i++) {
        const door = doors[i]!;
        const open = doorOpening(vehicle, door.index);
        if (door.kind === 'slide') {
          // A plug door: out a little, then along the body.
          const out = Math.min(1, open * 4) * m(0.07);
          const along = door.hingeX + door.slide * smooth01((open - 0.15) / 0.85);
          const across = -(door.hingeZ + door.side * out);
          place(car.doors[i]!, along, across, 0, 1, 1, 1, paint);
          place(car.doorGlass[i]!, along, across, 0, 1, 1, 1, null);
          continue;
        }
        // Swung about the hinge, outwards on its own side.
        const swing = open * DOOR_SWING;
        place(car.doors[i]!, door.hingeX, -door.hingeZ, 0, 1, 1, 1, paint, 0, door.side * swing);
        place(car.doorGlass[i]!, door.hingeX, -door.hingeZ, 0, 1, 1, 1, null, 0, door.side * swing);
      }
    }
    // … 2 lines
    if (roof && car.roofGlass) {
      if (look.glassRoof) placeWhole(car.roofGlass, null);
      else if (!joined) wholeAt(roof, detail, look.blackRoof ? T_BLACK_ROOF : paint);
    }
    if (!joined || roof) wholeAt(accent, detail, s.accent);
    // … 2 lines
    wholeAt(car.trim, detail, null);
    for (const l of model.headlamps) place(lamps, l.x, -l.z, l.y, l.sx, l.sy, l.sz, lamp.head);
    for (const l of model.taillamps) place(lamps, l.x, -l.z, l.y, l.sx, l.sy, l.sz, lamp.tail);
    for (const l of model.indicators) {
      // `lamp.indicate` is +1 for left, and the model's left is -Z.
      if (lamp.indicate !== 0 && lamp.indicate === -l.side) {
        place(lamps, l.x, -l.z, l.y, l.sx * 1.2, l.sy, l.sz, T_INDICATOR);
      }
    }
    // … 1 lines
    if (!doorsMoving) wholeAt(car.interior, detail, null);
    else {
      placeWhole(car.openInterior, null);
      for (let i = 0; i < doors.length; i++) {
        const door = doors[i]!, card = car.doorCards[i];
        if (!card || door.kind !== 'hinge') continue;
        place(card, door.hingeX, -door.hingeZ, 0, 1, 1, 1, null, 0, door.side * doorOpening(vehicle, door.index) * DOOR_SWING);
      }
    }
    const plates = model.plates;
    for (let i = 0; i < plates.length; i++) { const l = plates[i]!; place(lamps, l.x, -l.z, l.y, l.sx, l.sy, l.sz, i < 2 ? T_PLATE : T_DESTINATION); }
  // … 1 lines
  };

// src/render/agents.ts:1614-1616 slice of src/render/agents.ts#createAgentMeshes.vehicleCompany (shared): the feature's statements; "…" marks code left out
  const vehicleCompany = (vehicle: SimVehicle): Company =>
    vehicle.archetype.shape === 'bus' ? 'bus' : vehicle.archetype.shape === 'truck' ? 'truck'
      : vehicle.archetype.shape === 'motorcycle' || vehicle.archetype.shape === 'bicycle' ? 'rider' : 'car';

// src/render/agents.ts:1620-1624 slice of src/render/agents.ts#createAgentMeshes.carriesChild (shared): the feature's statements; "…" marks code left out
  const carriesChild = (vehicle: SimVehicle): boolean => {
    const seats = Math.min(32, vehicle.archetype.seats);
    for (let i = 0; i < seats; i++) if ((vehicle.seats & (1 << i)) !== 0 && seatAgeClass(vehicle, i) === 'child') return true;
  // … 1 lines
  };

// src/render/agents.ts:1651-1686 slice of src/render/agents.ts#createAgentMeshes.placeCarOccupants (shared): the feature's statements; "…" marks code left out
  const placeCarOccupants = (vehicle: SimVehicle, car: CarParts): void => {
    // … 3 lines
    const moving = vehicle.kerbStop?.phase === 'transfer' ? vehicle.kerbStop.seat : -1;
    // … 2 lines
    const seatCount = Math.min(model.seats.length, vehicle.archetype.seats);
    // … 2 lines
    for (let round = 0; round < 2; round++) for (let index = 0; index < seatCount; index++) {
      if ((vehicle.seats & (1 << index)) === 0 || index === moving) continue;
      if ((vehicle.people[index] !== undefined) !== (round === 0)) continue;
      const seat = model.seats[index]!;
      if (seat.row > rowsDrawn) continue;
      seatWorldInto(seat, seatPoint, seat.hipY);
      const who = seatIdentity(vehicle, seatPersonInto(vehicle, index, seated), hasChild);
      // The seat's pose (`riderPoses.ts`) with a glance now and then (`occupants.ts`).
      const slain = seat.driver && vehicle.slain === true;
      const plays = occupantPlays(seat, who.seed, frame[FRAME_CLOCK]!, seatPlays, slain);
      const fixed = seat.pose === 'car' ? 0 : 1;
      const drawn = pedestrians.drawClip(who, seatPoint.x, seatPoint.y, seatPoint.h ?? frame[FRAME_DECK]! + seat.hipY, frame[FRAME_YAW]!, plays as readonly { key: CitizenClipKey; phase: number; weight: number }[],
        0, seatFitScale(seat), false, fixed, null, slain ? 'dead' : undefined);
      if (seat.driver) driverScale = drawn;
    }
    // … 1 lines
    if (car.steering && model.steering) {
      const seat = model.seats[model.steering.seat]!;
      const wheel = model.steering.wheel;
      const s = driverScale > 0 ? driverScale / m(1) : 1;
      placeSteeringWheel(car.steering, seat.x + m(wheel.forward) * s, -seat.z, seat.hipY + m(wheel.up) * s,
        wheel.tilt, Math.max(-1.6, Math.min(1.6, lamp.steer * 5)), s);
    }
  // … 2 lines
  };

// src/render/agents.ts:1729-1774 slice of src/render/agents.ts#createAgentMeshes.placeKerbPerson (shared): the feature's statements; "…" marks code left out
  const placeKerbPerson = (vehicle: SimVehicle, model: VehicleModel, stop: KerbStop): void => {
    const progress = kerbTransfer(stop);
    const person = stop.person;
    const door = model.doors.find((d) => d.index === stop.door);
    if (!progress || !person || !door) return;
    const side = door.side;
    const along = door.hingeX - door.length / 2;
    const flank = Math.abs(door.hingeZ);
    const seat = stop.seatStage ? model.seats[stop.seat] : undefined;
    const identity = seatIdentity(vehicle, person, carriesChild(vehicle));
    if (seat && door.kind === 'hinge' && !(progress.walked >= 1 && (stop.phase === 'hold' || stop.phase === 'close'))) {
      const f = kerbFigure({ seat, door, foot: toVehicle(person.footX, person.footY) }, stop.kind, progress.seated, progress.walked, kerb);
      seatWorldInto(f, seatPoint, f.y);
      pedestrians.drawClip(identity, seatPoint.x, seatPoint.y, seatPoint.h ?? frame[FRAME_DECK]! + f.y, frame[FRAME_YAW]! + f.heading, f.plays,
        0, f.seated ? seatFitScale(seat) : Infinity, f.anchor === 'pelvis' ? false : f.anchor === 'feet' ? true : 'pelvisOver');
      return;
    }
    // Just outside the door opening on the road, and just inside it.
    const outside = seatWorld({ x: along, z: side * (flank + m(0.45)) });
    const inside = seatWorld({ x: along, z: side * (flank - m(0.4)) });
    const walkTo = (fromX: number, fromY: number, toX: number, toY: number, t: number, height: number): void => {
      const x = fromX + (toX - fromX) * t;
      const y = fromY + (toY - fromY) * t;
      const distance = hypot2(toX - fromX, toY - fromY);
      const heading = Math.atan2(toY - fromY, toX - fromX);
      pedestrians.drawClip(identity, x, y, height, heading,
        [{ key: 'walk', phase: 0, distance: distance * t, weight: 1 }], 0, Infinity, true);
    };
    if (progress.walked >= 1 && (stop.phase === 'hold' || stop.phase === 'close')) {
      // Standing on the footway: the truck's mate with the load, facing the truck.
      pedestrians.drawClip(identity, person.footX, person.footY, frame[FRAME_DECK]!,
        Math.atan2(outside.y - person.footY, outside.x - person.footX),
        [{ key: 'idle', phase: (stop.t * 0.1) % 1, weight: 1 }], 0, Infinity, true);
      return;
    }
    if (progress.walked > 0 || progress.seated <= 0) {
      // Between the door and the footway.
      if (stop.kind === 'drop') walkTo(outside.x, outside.y, person.footX, person.footY, progress.walked, frame[FRAME_DECK]!);
      else walkTo(person.footX, person.footY, outside.x, outside.y, 1 - progress.walked, frame[FRAME_DECK]!);
      return;
    }
    // A bus: stepping up through the door, then gone inside.
    const t = stop.kind === 'drop' ? 1 - progress.seated : progress.seated;
    if (stop.kind === 'pick' && t > 0.9) return;
    walkTo(outside.x, outside.y, inside.x, inside.y, stop.kind === 'pick' ? t : 1 - t, frame[FRAME_DECK]! + m(0.25) * t);
  };

// src/render/agents.ts:1777-1795 slice of src/render/agents.ts#createAgentMeshes.drawTwoWheeler (shared): the feature's statements; "…" marks code left out
  const drawTwoWheeler = (vehicle: SimVehicle, s: VehicleDraw, band: number): void => {
    // … 3 lines
    const detail = levelFor(frame[FRAME_PIXELS]!);
    wholeAt(bike.body, detail, s.paintTint);
    const steering = rung(bike.steering, detail);
    if (steering) place(steering, model.headX, 0, model.headY, 1, 1, 1, T_CHROME, 0, lamp.steer);
    // … 2 lines
    wholeAt(bike.trim, detail, T_TRIM);
    const cranks = bike.cranks ? rung(bike.cranks, detail) : null;
    if (cranks) place(cranks, model.bracketX, 0, model.bracketY, 1, 1, 1, T_CHROME, -lamp.crank);
    const h = model.headlamp;
    place(lamps, h.x, 0, h.y, h.sx, h.sy, h.sz, T_HEADLAMP, 0, lamp.steer);
    const t = model.taillamp;
    place(lamps, t.x, 0, t.y, t.sx, t.sy, t.sz, lamp.tail);
  // … 1 lines
  };

// src/render/agents.ts:1809-1839 slice of src/render/agents.ts#createAgentMeshes.placeRiderFigure (shared): the feature's statements; "…" marks code left out
  const placeRiderFigure = (vehicle: SimVehicle, model: TwoWheelerModel, cyclist: boolean, s: VehicleDraw): void => {
    if (occupantBand < 1) return;
    const pelvisUp = model.seatY;
    // The seat point rolled about the road-level forward axis (`place`).
    const sideOffset = pelvisUp * frame[FRAME_SIN]!;
    const x = frame[FRAME_X]! + frame[FRAME_DX]! * model.seatX - frame[FRAME_DY]! * sideOffset;
    const y = frame[FRAME_Y]! + frame[FRAME_DY]! * model.seatX + frame[FRAME_DX]! * sideOffset;
    const height = frame[FRAME_DECK]! + pelvisUp * frame[FRAME_COS]!;
    const moving = riderMoving(vehicle);
    const steer = Math.max(-1, Math.min(1, lamp.steer / STEER_FULL));
    riderPlays.length = 0;
    if (cyclist) {
      const phase = (-lamp.crank - Math.PI) / (2 * Math.PI);
      riderPlay('bikePedal', phase, moving * (1 - Math.abs(steer) * 0.4));
      if (steer > 0) riderPlay('bikeLeft', 0, moving * steer * 0.4);
      if (steer < 0) riderPlay('bikeRight', 0, moving * -steer * 0.4);
      riderPlay('bikeStop', 0, 1 - moving);
    } else {
      riderPlay('motoRide', 0, moving * (1 - Math.abs(steer)));
      if (steer > 0) riderPlay('motoLeft', 0, moving * steer);
      if (steer < 0) riderPlay('motoRight', 0, moving * -steer);
      riderPlay('motoStop', 0, 1 - moving);
    }
    // At the size the pose was solved at: the hands are on the grips and the feet on the pegs or pedals only there.
    const drawn = pedestrians.drawClip(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), x, y, height, frame[FRAME_YAW]!, riderPlays, frame[FRAME_ROLL]!, Infinity, false, 1,
      cyclist ? null : helmetMatrix);
    if (!cyclist && drawn > 0) {
      // A motorcyclist's helmet, fitted to this body's head (`riderPoses.helmetShape`) and put where the pose just drawn has it.
      write(helmets, helmetMatrix, s.helmet);
    }
  };

// src/render/agents.ts:1878-1929 slice of src/render/agents.ts#VehicleDraw (shared): the feature's statements; "…" marks code left out
  interface VehicleDraw extends KeptHolder, LeanState {
    // … 2 lines
    archetype: Archetype | null;
  // … 47 lines
  }

// src/render/agents.ts:1935-1967 slice of src/render/agents.ts#createAgentMeshes.drawOf (shared): the feature's statements; "…" marks code left out
  const drawOf = (vehicle: SimVehicle): VehicleDraw => {
    // … 13 lines
    if (s.archetype !== a) {
      // … 2 lines
      s.car = a.shape === 'car' || a.shape === 'bus' || a.shape === 'truck' ? carParts.get(bodyKey(vehicle)) ?? null : null;
      s.bike = a.shape === 'motorcycle' || a.shape === 'bicycle' ? twoWheelerParts.get(a.id) ?? null : null;
    // … 6 lines
    }
  // … 6 lines
  };

// src/render/agents.ts:2086-2088 slice of src/render/agents.ts#createAgentMeshes.driverBody (shared): the feature's statements; "…" marks code left out
    driverBody(vehicle, x, y) {
      return pedestrians.indexFor(seatIdentity(vehicle, seatPersonInto(vehicle, 0, seated), false), vehicle.archetype.shape === 'motorcycle', x, y);
    },

// src/render/agents.ts:2089-2146 slice of src/render/agents.ts#createAgentMeshes.carcass (shared): the feature's statements; "…" marks code left out
    carcass(vehicle, intact = false) {
      // … 7 lines
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
      // … 1 lines
      const merged = welded.toNonIndexed();
      welded.dispose();
      // … 1 lines
      merged.computeBoundingBox();
      const box = merged.boundingBox!;
      const centre = box.getCenter(new Vector3());
      merged.translate(-centre.x, -centre.y, -centre.z);
      if (intact) { merged.computeVertexNormals(); return merged; }
      const pos = merged.getAttribute('position');
      // … 2 lines
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

// src/render/agents.ts:2153-2573 slice of src/render/agents.ts#createAgentMeshes.sync (shared): the feature's statements; "…" marks code left out
    sync(world, net, alpha, detailed, zoom = Number.POSITIVE_INFINITY, options = {}) {
      // … 29 lines
      const levelRatio = pixelRatio * (ERROR_PIXELS / (options.tier?.vehicleErrorPixels ?? ERROR_PIXELS));
      // … 216 lines
      if (box) {
        // … 2 lines
        for (const cell of world.parkedCellsIn(box, cellsIn)) {
          // … 8 lines
          if (was && options.vehicleInside && screenScale && was.still && was.onScreen && was.kept.length === was.count && was.count === cell.items.length
            && was.version === cell.version && was.serial === offRoadSerial && was.band === vehicleBand && was.ratio === levelRatio && was.nearCap === nearCap && was.shadows === softShadows
            && was.occupant === occupantZoom && was.levels === levelsVersion && was.bounded && was.rangeLo < was.rangeHi) {
            const across = Math.hypot((was.x1 - was.x0) / 2, (was.y1 - was.y0) / 2);
            const upright = (was.hi + 0.3 * was.radius - was.lo) / 2;
            const radius = Math.hypot(across, upright) + was.radius;
            if (options.vehicleInside((was.x0 + was.x1) / 2, (was.y0 + was.y1) / 2, was.lo + upright - 0.3 * radius, radius)) {
              const field = depthField(screenScale, box);
              // One over the scale at the box's nearest and farthest corners (each axis at the end its slope picks).
              let near = field[0]!, far = field[0]!;
              for (const [axis, a, b] of [[1, was.x0, was.x1], [2, was.y0, was.y1], [3, was.lo, was.hi]] as const) {
                const g = field[axis]!;
                near += Math.min(g * a, g * b);
                far += Math.max(g * a, g * b);
              }
              if (field.valid && near > 0 && 1 / far * (1 - FIELD_SLACK) > was.rangeLo && 1 / near * (1 + FIELD_SLACK) < was.rangeHi) {
                for (const s of was.kept) { seeKept(s); s.seen = suspensionFrame; }
                if (was.wheeled) carsWheeled = true;
                continue;
              }
            }
          }
          // … 10 lines
          Object.assign(cellRecord, { count: cell.items.length, onScreen: true, rangeLo: -Infinity, rangeHi: Infinity, x0: Infinity, y0: Infinity, x1: -Infinity, y1: -Infinity, radius: 0,
            band: vehicleBand, ratio: levelRatio, nearCap, shadows: softShadows, occupant: occupantZoom, levels: levelsVersion });
        // … 11 lines
        }
      // … 2 lines
      } else for (const list of [world.traffic, world.parked]) for (const vehicle of list) drawVehicle(vehicle);
      // … 10 lines
      for (const part of wheelLevels.parts) part.mesh.castShadow = bikesCast || carsWheeled;
    // … 101 lines
    },

// src/render/agents.ts:2219-2379 slice of src/render/agents.ts#createAgentMeshes.sync.drawVehicle (shared): the feature's statements; "…" marks code left out
      const drawVehicle = (vehicle: SimVehicle): void => {
        // … 85 lines
        const fit = twoWheeled ? s.bike?.model.fit : undefined;
        // … 8 lines
        if (free || (level === 0 && !twoWheeled)) { want.deck = deck; want.pitch = 0; want.roll = 0; }
        else if (slope) roadFrameInto(elevationOnCurrent, pose.p.x, pose.p.y, pose.angle, front, rear, twoWheeled ? 0 : plan.axleSide, deck, want);
        else { want.deck = deck; want.pitch = s.pitch; want.roll = s.roll; }
        // … 13 lines
        frameAt(pose.p.x, pose.p.y, pose.angle, s.deck,
          twoWheeled ? leanOf(vehicle, s) + (fit ? fit.stopTilt * (1 - riderMoving(vehicle)) : 0) : 0,
          s.pitch, s.roll);
        // … 9 lines
        if (level >= 1 || twoWheeled) {
          // … 1 lines
          lamp.indicate = side !== 0 && blinkOn(vehicle.age, vehicle.id) ? side : 0;
          // … 1 lines
          lamp.steer = free ? 0 : steerFrom(vehicle.curvature, (axles[0] ?? 0) - (axles[axles.length - 1] ?? 0));
        // … 4 lines
        } else { lamp.indicate = 0; lamp.steer = 0; }
          // … 27 lines
          };
