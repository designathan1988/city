// The code of src/sim/vehicles/kerbStops.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/kerbStops.ts:20-20 slice of src/sim/vehicles/kerbStops.ts#KerbPerson (shared): the feature's statements; "…" marks code left out
type KerbPerson = Boarder;

// src/sim/vehicles/kerbStops.ts:92-92 slice of src/sim/vehicles/kerbStops.ts#Seated (shared): the feature's statements; "…" marks code left out
type Seated = { readonly id: number; readonly people: readonly number[]; readonly peopleAge: readonly (PersonAgeClass | undefined)[]; readonly archetype: { readonly doorsPerSide: number } };

// src/sim/vehicles/kerbStops.ts:95-95 slice of src/sim/vehicles/kerbStops.ts#SeatPerson (shared): the feature's statements; "…" marks code left out
export interface SeatPerson { seed: number; gender: PersonGender; ageClass: PersonAgeClass }

// src/sim/vehicles/kerbStops.ts:102-114 slice of src/sim/vehicles/kerbStops.ts#seatPersonInto (shared): the feature's statements; "…" marks code left out
export function seatPersonInto(v: Seated, seat: number, out: SeatPerson): SeatPerson {
  // … 2 lines
  const hash = personHash(seed);
  // … 1 lines
  out.gender = (hash >>> 3) & 1 ? 'f' : 'm';
  // … 4 lines
  out.ageClass = back && ((hash >>> 7) & 3) === 0 ? 'child' : 'adult';
// … 1 lines
}

// src/sim/vehicles/kerbStops.ts:117-123 slice of src/sim/vehicles/kerbStops.ts#seatAgeClass (shared): the feature's statements; "…" marks code left out
export function seatAgeClass(v: Seated, seat: number): PersonAgeClass {
  // … 4 lines
  return back && ((personHash(PERSON_BASE + v.id * SEAT_STRIDE + seat) >>> 7) & 3) === 0 ? 'child' : 'adult';
}

// src/sim/vehicles/kerbStops.ts:192-275 slice of src/sim/vehicles/kerbStops.ts#stepKerbStops (shared): the feature's statements; "…" marks code left out
export function stepKerbStops(w: SimWorld): void {
  for (const v of w.vehiclesInIdOrder()) {
    // … 21 lines
    switch (stop.phase) {
      // … 17 lines
      case 'hold':
        // … 1 lines
        if (stop.t >= stop.hold && !stop.person) {
        // … 1 lines
        } else if (stop.t >= stop.hold && stop.person) {
          // … 1 lines
          stop.fetchTime = Math.hypot(stop.person.footX - door.x, stop.person.footY - door.y) / KERB_PACE;
        // … 2 lines
        }
        break;
    // … 29 lines
    }
  }
}

// src/sim/vehicles/kerbStops.ts:303-362 slice of src/sim/vehicles/kerbStops.ts#plan (shared): the feature's statements; "…" marks code left out
function plan(w: SimWorld, v: Vehicle): void {
  // … 51 lines
  const person = w.pedEngine.bridge.hailable(w, lane.id, earliest, Math.min(latest, earliest + PICK_REACH));
  if (!person) return;
  const at = Math.min(latest, Math.max(earliest, person.s + doorAlongFromFront(v, seat)));
  if (queuedAhead(w, v, at)) return;
  const stop = newStop('pick', lane.id, at, seat);
  stop.pedId = person.id;
  v.kerbStop = stop;
}

// src/sim/vehicles/kerbStops.ts:365-397 slice of src/sim/vehicles/kerbStops.ts#planBusStop (shared): the feature's statements; "…" marks code left out
function planBusStop(w: SimWorld, v: Vehicle, lanelet: LaneletId, earliest: number, latest: number): void {
  // … 16 lines
  for (let i = 0; i < 2 && i < free.length; i++) {
    // … 2 lines
    const walker = w.pedEngine.bridge.hailable(w, lanelet, doorAt - BUS_FETCH * 0.7, doorAt + BUS_FETCH * 0.7, taken);
    if (!walker) break;
    taken.add(walker.id);
    // … 1 lines
    task.pedId = walker.id;
  // … 2 lines
  }
// … 4 lines
}

// src/sim/vehicles/kerbStops.ts:434-453 slice of src/sim/vehicles/kerbStops.ts#footwayBeside (shared): the feature's statements; "…" marks code left out
function footwayBeside(w: SimWorld, lanelet: LaneletId, s: number): { x: number; y: number; t: { x: number; y: number } } | null {
  // … 7 lines
  for (const edge of w.sidewalks.walksBeside(lane.segment)) {
    const hit = edge.path.closestPoint(side.p);
    const dx = hit.point.x - side.p.x;
    const dy = hit.point.y - side.p.y;
    if (dx * side.right.x + dy * side.right.y <= 0) continue;
    if (hit.distance < bestDistance) {
      bestDistance = hit.distance;
      best = { x: hit.point.x, y: hit.point.y, t: edge.path.sampleAt(hit.s).t };
    }
  }
  return best;
}

// src/sim/vehicles/kerbStops.ts:468-487 slice of src/sim/vehicles/kerbStops.ts#roomToOpen (shared): the feature's statements; "…" marks code left out
function roomToOpen(w: SimWorld, v: Vehicle, stop: KerbStop): boolean {
  // … 2 lines
  const people = w.pedEngine.bridge;
  if (people.anyoneWithin(w, door.x, door.y, DOOR_CLEAR, stop.pedId)) return false;
  if (stop.kind === 'drop' && people.anyoneWithin(w, foot.x, foot.y, m(0.9), stop.pedId)) return false;
// … 13 lines
}

// src/sim/vehicles/kerbStops.ts:515-527 slice of src/sim/vehicles/kerbStops.ts#hail (shared): the feature's statements; "…" marks code left out
function hail(w: SimWorld, v: Vehicle, stop: KerbStop): void {
  // … 2 lines
  const person = stop.pedId !== null ? w.pedEngine.bridge.board(w, stop.pedId, door, reach) : null;
  if (!person) {
    abandon(v);
    return;
  }
  stop.person = person;
  stop.fetchTime = Math.hypot(person.footX - door.x, person.footY - door.y) / KERB_PACE;
// … 2 lines
}

// src/sim/vehicles/kerbStops.ts:529-557 slice of src/sim/vehicles/kerbStops.ts#completeTransfer (shared): the feature's statements; "…" marks code left out
function completeTransfer(w: SimWorld, v: Vehicle, stop: KerbStop): void {
  if (stop.kind === 'drop') {
    // … 1 lines
    const staysAboard = !!stop.person && !stop.keep && !w.pedEngine.bridge.alight(w, stop.person);
    if (!staysAboard) {
      v.seats &= ~(1 << stop.seat);
      if (!stop.keep) {
        setSeat(v.people, stop.seat, undefined);
        setSeat(v.peopleAge, stop.seat, undefined);
      }
    }
  } else {
    // … 1 lines
    if (stop.person && !v.people[stop.seat] && stop.person.seed < PERSON_BASE) {
      setSeat(v.people, stop.seat, stop.person.seed);
      setSeat(v.peopleAge, stop.seat, stop.person.ageClass);
    }
  }
// … 10 lines
}
