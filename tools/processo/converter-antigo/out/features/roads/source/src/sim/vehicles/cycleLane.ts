// The code of src/sim/vehicles/cycleLane.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/cycleLane.ts:13-19 slice of src/sim/vehicles/cycleLane.ts#inside (shared): the feature's statements; "…" marks code left out
function inside(lane: Lanelet, centre: number): number {
  const d = Math.min(centre, lane.length - centre);
  if (d <= 0) return 0;
  if (d >= CYCLE_EASE) return 1;
  const t = d / CYCLE_EASE;
  return t * t * (3 - 2 * t);
}

// src/sim/vehicles/cycleLane.ts:22-25 slice of src/sim/vehicles/cycleLane.ts#cycleShift (shared): the feature's statements; "…" marks code left out
export function cycleShift(v: Vehicle, lane: Lanelet | undefined, s: number): number {
  if (!lane?.cycleShift || !isBicycle(v)) return 0;
  return lane.cycleShift * inside(lane, s - v.archetype.length / 2);
}

// src/sim/vehicles/cycleLane.ts:28-32 slice of src/sim/vehicles/cycleLane.ts#outOfTheWay (shared): the feature's statements; "…" marks code left out
export function outOfTheWay(follower: Vehicle, lead: Vehicle, lane: Lanelet | undefined): boolean {
  if (isBicycle(follower) || !isBicycle(lead) || !lane?.cycleShift || lane.id !== lead.lanelet) return false;
  // … 1 lines
  return inside(lane, lead.s - lead.archetype.length) > 0.8 && inside(lane, lead.s) > 0.8;
}
