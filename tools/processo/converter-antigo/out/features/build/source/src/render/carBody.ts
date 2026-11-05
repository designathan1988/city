// The code of src/render/carBody.ts that is the feature's (the rest of the file is shared).

// src/render/carBody.ts:382-958 slice of src/render/carBody.ts#buildCarModel (shared): the feature's statements; "…" marks code left out
export function buildCarModel(a: Archetype, styleName: CarStyle = (a.style ?? 'sedan') as CarStyle): VehicleModel {
  // … 422 lines
  const parts = new CabinParts();
  // … 18 lines
  parts.add('floor', -1, tint(box(bulk - (lastX - M(0.6)), M(0.03), inner, (bulk + lastX - M(0.6)) / 2, floor - M(0.015), 0), CARPET));
  // … 1 lines
  {
    // … 14 lines
    parts.add('dash', -1, tint(extrude(s, inner, M(0.03)), DASH),
      tint(roundedSlab(M(0.18), M(0.34), M(0.06), M(0.07), dashRear + M(0.1), dashTop - M(0.005), driver.z), DASH),
      tint(box(M(0.02), M(0.14), M(0.22), dashRear - M(0.005), dashTop - M(0.14), 0), 0x0b1a26));
    // … 1 lines
    parts.add('console', -1, tint(roundedSlab(dashRear - consoleBack, M(0.2), M(0.05), M(0.2), (dashRear + consoleBack) / 2, floor, 0), DASH));
    parts.add('lever', driverSeat, tint(box(M(0.04), M(0.1), M(0.04), driver.x + M(0.3), floor + M(0.25), 0), 0x0f1012));
    // … 4 lines
    parts.add('column', driverSeat, tint(bar(hub.clone().add(new Vector3(M(0.03), -M(0.01), 0)), col, M(0.06)), DASH));
    // … 1 lines
    for (const [dz, wide] of [[M(0.1), M(0.07)], [-M(0.08), M(0.1)], [-M(0.22), M(0.07)]] as const) {
      // … 1 lines
      parts.add('pedal', driverSeat, tint(box(M(0.03), M(0.08), wide, px, floor + M(0.14), driver.z + dz), 0x121315),
        tint(bar(new Vector3(px + M(0.02), floor + M(0.18), driver.z + dz), new Vector3(px + M(0.1), floor + M(0.38), driver.z + dz), M(0.018)), 0x121315));
    }
  }
  // … 1 lines
  for (const side of [-1, 1] as const) {
    // … 3 lines
    parts.add('seat', own, ...seatShell(s.x, s.hipY, floor, M(0.5), s.z, recline, M(0.6), SEAT_FABRIC,
      st.cab ? UPRIGHT_CUSHION_REACH : CAR_PACKAGE.cushionReach));
    // … 1 lines
    parts.add('headrest', own, ...headrest(hr.x, hr.y, s.z));
  }
  if (perSide > 1) {
    // … 4 lines
    parts.add('seat', behind, ...seatShell(s.x, s.hipY, floor, bench, 0, recline + 0.05, M(0.58), SEAT_FABRIC));
    // … 1 lines
    for (const side of [-1, 1] as const) {
      parts.add('headrest', seatIndex(side, perSide - 1, perSide), ...headrest(hr.x - M(0.01), hr.y - M(0.03), seatAt(side, perSide - 1).z, M(0.24)));
    }
    // … 2 lines
    if (styleName === 'sedan' && rear) {
      parts.add('shelf', -1, tint(box(Math.max(M(0.1), benchBack - rear.base), M(0.03), inner, (benchBack + rear.base) / 2, belt(rear.base) - M(0.02), 0), CARPET));
    } else if (styleName === 'pickup') {
      parts.add('wall', -1, tint(box(M(0.04), top(bedFrom + M(0.1)) - floor - M(0.05), inner, bedFrom + M(0.06), (top(bedFrom + M(0.1)) + floor) / 2, 0), CARD));
    } else {
      // … 2 lines
      parts.add('floor', -1, tint(box(s.x - M(0.2) - bootEnd, M(0.03), inner, (s.x - M(0.2) + bootEnd) / 2, bootFloor, 0), CARPET));
      for (const sd of [-1, 1] as const) {
        parts.add('card', -1, tint(box(s.x - M(0.2) - bootEnd, belt(bootEnd) - bootFloor, M(0.03), (s.x - M(0.2) + bootEnd) / 2,
          (belt(bootEnd) + bootFloor) / 2, sd * (halfWidth(bootEnd) - M(0.09))), CARD));
      }
      parts.add('shelf', -1, tint(box(Math.max(M(0.1), benchBack - bootEnd), M(0.015), inner - M(0.04), (benchBack + bootEnd) / 2, belt(bootEnd) - M(0.02), 0), 0x2c2e33));
    }
  } else {
    // … 2 lines
    parts.add('wall', -1, tint(box(M(0.04), top(bx) - floor - M(0.08), inner, bx, (top(bx) + floor) / 2 - M(0.02), 0), 0x8b9198));
  }
  // … 1 lines
  for (const side of [-1, 1] as const) {
    // … 2 lines
    if (xb - xa > M(0.1)) {
      parts.add('card', -1, tint(box(xb - xa, belt(xb) - floor, M(0.03), (xa + xb) / 2, (belt(xb) + floor) / 2, side * (halfWidth((xa + xb) / 2) - M(0.1))), CARD));
    }
  }
  // … 25 lines
  const solids = parts.solids(doors.map((d, i) => ({ kind: 'card' as const, seats: d.seat < 0 ? [] : [d.seat], geometry: cards[i]! })));
  const interior = merge([...parts.geometries.map((g) => g.clone()), ...cards]);
  const openInterior = merge(parts.geometries);
  // … 2 lines
  return {
    // … 5 lines
    interior,
    solids,
    openInterior,
  // … 10 lines
  };
}
