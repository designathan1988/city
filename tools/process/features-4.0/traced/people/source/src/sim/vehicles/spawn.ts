// The code of src/sim/vehicles/spawn.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/spawn.ts:167-226 slice of src/sim/vehicles/spawn.ts#spawnAt (shared): the feature's statements; "…" marks code left out
function spawnAt(w: SimWorld, id: string): boolean {
  // … 24 lines
  for (const s of [0, (arch.length + entryClearance) / 2, arch.length + entryClearance]) {
    // … 1 lines
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return false;
  }
// … 30 lines
}

// src/sim/vehicles/spawn.ts:232-277 slice of src/sim/vehicles/spawn.ts#spawnVehicleAt (shared): the feature's statements; "…" marks code left out
export function spawnVehicleAt(w: SimWorld, id: string, s: number, arch: Archetype, pick: DestinationPicker = chooseVehicleDestination): number | null {
  // … 14 lines
  for (const at of [rear, (rear + s) / 2, s]) {
    // … 1 lines
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return null;
  }
// … 26 lines
}
