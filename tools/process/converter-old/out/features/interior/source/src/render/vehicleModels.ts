// The code of src/render/vehicleModels.ts that is the feature's (the rest of the file is shared).

// src/render/vehicleModels.ts:119-151 slice of src/render/vehicleModels.ts#VehicleModel (shared): the feature's statements; "…" marks code left out
export interface VehicleModel {
  // … 13 lines
  readonly solids: readonly InteriorSolid[];
// … 17 lines
}

// src/render/vehicleModels.ts:735-1018 slice of src/render/vehicleModels.ts#buildBusModel (shared): the feature's statements; "…" marks code left out
export function buildBusModel(a: Archetype): VehicleModel {
  // … 129 lines
  const parts = new CabinParts();
  // … 1 lines
  parts.add('floor', -1, tint(box(L - M(0.3), M(0.04), W - wall * 2, 0, floor - M(0.02), 0), BUS_FLOOR));
  // … 1 lines
  for (const side of [-1, 1] as const) {
    parts.add('sill', -1, tint(box(L - M(0.4), sillY - floor, M(0.03), 0, (sillY + floor) / 2, side * (halfW - wall - M(0.015))), BUS_SHELL));
  }
  // … 16 lines
  for (const ax of axles.slice(1)) {
    for (const side of [-1, 1] as const) {
      // … 7 lines
      parts.add('podium', -1, tint(box(x1 - x0, podiumTop - floor, podiumDepth, (x0 + x1) / 2, (podiumTop + floor) / 2,
        side * (halfW - wall - podiumDepth / 2 - M(0.005))), BUS_SHELL));
    }
  }
  // … 6 lines
  parts.add('dash', -1, tint(extrude([[front - M(0.12), screenBase + M(0.05)], [front - M(0.12), floor], [front - M(0.5), floor], [front - M(0.55), screenBase - M(0.05)], [front - M(0.45), screenBase + M(0.12)]], W - wall * 2 - M(0.02), M(0.03), 0, 4, 2), DASH));
  // … 1 lines
  parts.add('seat', 0, ...seatShell(driverX, driverHip, floor, M(0.52), driverZ, 0.3, M(0.62), SEAT_FABRIC));
  // … 1 lines
  parts.add('headrest', 0, ...headrest(hr.x, hr.y, driverZ));
  parts.add('wall', -1, tint(box(M(0.04), M(1.1), M(0.9), driverX - M(0.48), floor + M(0.55), -halfW + M(0.5)), PANEL_GREY));
  // … 1 lines
  parts.add('column', 0, tint(bar(wheelAt, new Vector3(front - M(0.45), floor + M(0.3), driverZ), M(0.07)), DASH));
  // … 31 lines
  passenger.forEach((s, i) => {
    // … 1 lines
    parts.add('seat', k < 0 ? -1 : 1 + k, ...busSeat(s.x, s.hip, s.floor, seatWidth, s.z));
  });
  // … 19 lines
  for (let x = firstRow + M(0.3); x > lastRow; x -= pitch * 2) {
    for (const side of [-1, 1] as const) {
      // … 1 lines
      parts.add('pole', -1, tint(new CylinderGeometry(M(0.02), M(0.02), cant - floor, 6).translate(x, (cant + floor) / 2, side * aisle), POLE_YELLOW));
    }
  }
  for (const [x0, x1] of doorSpans) {
    parts.add('pole', -1, tint(new CylinderGeometry(M(0.02), M(0.02), cant - floor, 6).translate((x0 + x1) / 2, (cant + floor) / 2, halfW - wall - M(0.35)), POLE_YELLOW));
  }
  for (const side of [-1, 1] as const) {
    parts.add('pole', -1, tint(new CylinderGeometry(M(0.018), M(0.018), L - M(3), 6).rotateZ(Math.PI / 2).translate(-M(0.5), cant - M(0.12), side * aisle), POLE_YELLOW));
  }
  // … 30 lines
  const solids = parts.solids();
  const interiorGeo = merge(parts.geometries);
  return {
    // … 1 lines
    trim: merge(trim), interior: interiorGeo, openInterior: interiorGeo, solids,
  // … 1 lines
  };
}

// src/render/vehicleModels.ts:1054-1229 slice of src/render/vehicleModels.ts#buildTruckModel (shared): the feature's statements; "…" marks code left out
export function buildTruckModel(a: Archetype): VehicleModel {
  // … 17 lines
  const parts = new CabinParts();
  // … 62 lines
  parts.add('floor', -1, tint(box(cabLength - M(0.2), M(0.04), W - skin * 2 - M(0.04), cabBack + cabLength / 2, cabFloor - M(0.02), 0), CARPET));
  // … 1 lines
  parts.add('console', -1, tint(box(M(0.5), M(0.28), M(0.6), seatX + M(0.35), cabFloor + M(0.14), 0), DASH));
  parts.add('dash', -1, tint(extrude([[cabFront - M(0.1), belt + M(0.04)], [cabFront - M(0.1), cabFloor], [cabFront - M(0.45), cabFloor], [cabFront - M(0.6), belt - M(0.15)], [cabFront - M(0.55), belt + M(0.08)]], W - skin * 2 - M(0.06), M(0.03), 0, 4, 2), DASH));
  parts.add('wall', -1, tint(box(M(0.04), cabTop - cabFloor, W - skin * 2 - M(0.04), cabBack + M(0.08), (cabTop + cabFloor) / 2, 0), 0x3c3a36));
  for (const side of [-1, 1] as const) {
    // … 3 lines
    parts.add('seat', own, ...seatShell(seatX, hipY, cabFloor, M(0.52), z, 0.3, M(0.62), SEAT_FABRIC));
    // … 1 lines
    parts.add('headrest', own, ...headrest(h.x, h.y, z));
    // … 2 lines
    if (side === -1) {
      // … 1 lines
      parts.add('column', own, tint(bar(wheel, new Vector3(cabFront - M(0.5), belt - M(0.1), z), M(0.07)), DASH));
    }
  }
  // … 55 lines
  return {
    // … 5 lines
    solids: parts.solids(doors.map((d, i) => ({ kind: 'card' as const, seats: [d.seat], geometry: closedCards[i]! }))),
    interior: merge([...parts.geometries.map((g) => g.clone()), ...closedCards]),
    openInterior: merge(parts.geometries),
  // … 10 lines
  };
}
