// The code of src/render/carBody.ts that is the feature's (the rest of the file is shared).

// src/render/carBody.ts:382-958 slice of src/render/carBody.ts#buildCarModel (shared): the feature's statements; "…" marks code left out
export function buildCarModel(a: Archetype, styleName: CarStyle = (a.style ?? 'sedan') as CarStyle): VehicleModel {
  // … 443 lines
  {
    // … 2 lines
    const dashRear = driver.x + M((st.cab ? CAB_WHEEL : DRIVER_WHEEL).forward) + M(0.12);
    const s: P2[] = [
      [screenBase + M(0.02), dashTop - M(0.03)],
      [bulk + M(0.05), floor + M(0.14)],
      [bulk - M(0.06), floor + M(0.14)],
      [dashRear + M(0.12), dashTop - M(0.3)],
      [dashRear, dashTop - M(0.18)],
      [dashRear - M(0.02), dashTop - M(0.05)],
      [dashRear + M(0.06), dashTop + M(0.01)],
    ];
    // … 2 lines
    parts.add('dash', -1, tint(extrude(s, inner, M(0.03)), DASH),
      tint(roundedSlab(M(0.18), M(0.34), M(0.06), M(0.07), dashRear + M(0.1), dashTop - M(0.005), driver.z), DASH),
      tint(box(M(0.02), M(0.14), M(0.22), dashRear - M(0.005), dashTop - M(0.14), 0), 0x0b1a26));
    // … 1 lines
    parts.add('console', -1, tint(roundedSlab(dashRear - consoleBack, M(0.2), M(0.05), M(0.2), (dashRear + consoleBack) / 2, floor, 0), DASH));
    // … 2 lines
    const w = st.cab ? CAB_WHEEL : DRIVER_WHEEL;
    const hub = new Vector3(driver.x + M(w.forward), driver.hipY + M(w.up), driver.z);
    const col = new Vector3(dashRear + M(0.1), hub.y - M(0.1), driver.z);
    parts.add('column', driverSeat, tint(bar(hub.clone().add(new Vector3(M(0.03), -M(0.01), 0)), col, M(0.06)), DASH));
  // … 6 lines
  }
  // … 52 lines
  const wheelSpec = st.cab ? CAB_WHEEL : DRIVER_WHEEL;
  const steering = driverIndex >= 0 ? { geometry: steeringWheel(M(wheelSpec.radius)), seat: driverIndex, wheel: wheelSpec } : null;
// … 45 lines
}
