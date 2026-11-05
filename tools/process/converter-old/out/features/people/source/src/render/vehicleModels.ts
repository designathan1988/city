// The code of src/render/vehicleModels.ts that is the feature's (the rest of the file is shared).

// src/render/vehicleModels.ts:111-117 slice of src/render/vehicleModels.ts#SteeringModel (shared): the feature's statements; "…" marks code left out
interface SteeringModel {
  // … 4 lines
  readonly wheel: WheelSpec;
}

// src/render/vehicleModels.ts:535-555 slice of src/render/vehicleModels.ts#TwoWheelerModel (shared): the feature's statements; "…" marks code left out
export interface TwoWheelerModel {
  // … 16 lines
  readonly fit: TwoWheelerFit;
// … 2 lines
}

// src/render/vehicleModels.ts:571-575 slice of src/render/vehicleModels.ts#gripInHead (shared): the feature's statements; "…" marks code left out
function gripInHead(fit: TwoWheelerFit, pelvisX: number, headX: number, headY: number, side: DoorSide): Vector3 {
  // … 1 lines
  const g = gripPoint(fit, side === -1 ? 1 : -1);
  return new Vector3(pelvisX + M(g[2]) - headX, M(fit.pelvisY + g[1]) - headY, -M(g[0]));
}

// src/render/vehicleModels.ts:578-591 slice of src/render/vehicleModels.ts#handlebar (shared): the feature's statements; "…" marks code left out
function handlebar(fit: TwoWheelerFit, pelvisX: number, headX: number, headY: number, clampY: number, thick: number): BufferGeometry[] {
  // … 2 lines
  for (const side of [-1, 1] as const) {
    const grip = gripInHead(fit, pelvisX, headX, headY, side);
    // … 1 lines
    const inner = grip.clone().add(new Vector3(0, 0, -side * M(0.07)));
    // … 1 lines
    out.push(bar(clamp.clone(), knee, thick), bar(knee, inner, thick));
    // … 1 lines
    out.push(tint(rubber.translate(grip.x, grip.y, grip.z + side * M(0.005)), 0x161618));
  }
// … 1 lines
}

// src/render/vehicleModels.ts:593-662 slice of src/render/vehicleModels.ts#motorcycleModel (shared): the feature's statements; "…" marks code left out
function motorcycleModel(a: Archetype): TwoWheelerModel {
  const fit = MOTO_FIT;
  // … 6 lines
  const py = M(fit.pelvisY);
  const headX = px + M(fit.steerAxis);
  // … 1 lines
  const saddleTop = py - M(fit.saddleDrop);
  // … 4 lines
  body.push(extrude([[headX - M(0.04), M(0.82)], [headX - M(0.02), M(0.96)], [headX - M(0.16), M(1.02)],
    [tankRear + M(0.06), saddleTop + M(0.08)], [tankRear - M(0.02), saddleTop - M(0.02)], [tankRear + M(0.06), M(0.74)]], M(0.34), M(0.07)));
  // … 1 lines
  body.push(extrude([[tankRear, M(0.74)], [tankRear, saddleTop - M(0.04)], [px - M(0.3), saddleTop - M(0.02)], [px - M(0.24), M(0.62)]], M(0.26), M(0.03)));
  body.push(extrude([[px - M(0.28), saddleTop - M(0.03)], [px - M(0.34), saddleTop + M(0.03)], [rear + M(0.02), saddleTop + M(0.08)],
    [rear + M(0.08), saddleTop - M(0.03)]], M(0.2), M(0.04)));
  // … 1 lines
  body.push(strut(headX, headY - M(0.05), -M(0.15), M(0.45), 0, M(0.07), M(0.08)));
  // … 1 lines
  trim.push(tint(extrude([[tankRear + M(0.02), saddleTop - M(0.02)], [px, saddleTop], [px - M(0.3), saddleTop + M(0.02)],
    [px - M(0.33), saddleTop - M(0.06)], [tankRear, saddleTop - M(0.09)]], M(0.28), M(0.04), 0, 4, 2), 0x1a1a1c));
  // … 9 lines
  for (const side of [-1, 1] as const) trim.push(tint(strut(rear + M(0.3), r + M(0.06), px - M(0.2), saddleTop - M(0.08), side * M(0.1), M(0.04), M(0.04)), 0xc0392b));
  // … 2 lines
  const peg = fit.peg!;
  for (const side of [-1, 1] as const) {
    const at = new Vector3(px + M(peg.forward), py + M(peg.up), side * M(peg.side));
    trim.push(tint(new CylinderGeometry(M(0.018), M(0.018), M(0.11), 6).rotateX(Math.PI / 2).translate(at.x, at.y, at.z), 0x2a2c30));
    trim.push(tint(bar(new Vector3(at.x - M(0.04), at.y + M(0.02), side * M(0.12)), new Vector3(at.x, at.y, at.z - side * M(0.04)), M(0.025)), DARK_TRIM));
  }
  // … 2 lines
  const dx = front - headX;
  // … 1 lines
  for (const side of [-1, 1] as const) steering.push(strut(0, M(0.08), dx, dy, side * M(0.09), M(0.05), M(0.05)));
  // … 1 lines
  steering.push(...handlebar(fit, px, headX, headY, M(0.12), M(0.022)));
  // … 1 lines
  steering.push(tint(box(M(0.34), M(0.03), M(0.13), dx - M(0.02), dy + r + M(0.06), 0), DARK_TRIM));
  return {
    // … 3 lines
    headX,
    // … 5 lines
    seatY: py,
    fit,
    headlamp: { x: headX + M(0.18), y: headY - M(0.02), z: 0, sx: M(0.04), sy: M(0.12), sz: M(0.14) },
    taillamp: { x: rear - M(0.02), y: saddleTop + M(0.02), z: 0, sx: M(0.03), sy: M(0.05), sz: M(0.12) },
  };
}

// src/render/vehicleModels.ts:664-730 slice of src/render/vehicleModels.ts#bicycleModel (shared): the feature's statements; "…" marks code left out
function bicycleModel(a: Archetype): TwoWheelerModel {
  const fit = BIKE_FIT;
  // … 5 lines
  const py = M(fit.pelvisY);
  const bracketX = px + M(fit.bracket!.forward);
  const bracketY = py + M(fit.bracket!.up);
  const saddleTop = py - M(fit.saddleDrop);
  const headX = px + M(fit.steerAxis);
  // … 2 lines
  const seatTop = { x: px - M(0.02), y: saddleTop - M(0.1) };
  // … 3 lines
  body.push(strut(bracketX, bracketY, seatTop.x, seatTop.y, 0, tube, tube));
  body.push(strut(seatTop.x, seatTop.y - M(0.04), headX, headY - M(0.04), 0, tube, tube));
  body.push(strut(bracketX, bracketY, headX, headY - M(0.12), 0, tube * 1.2, tube * 1.2));
  // … 1 lines
  body.push(strut(headX + M(0.02), headY - M(0.16), headX, headY + M(0.02), 0, tube * 1.3, tube * 1.3));
  for (const side of [-1, 1] as const) {
    body.push(strut(bracketX, bracketY, rear, r, side * M(0.06), tube * 0.8, tube * 0.8));
  // … 1 lines
  }
  // … 2 lines
  trim.push(strut(seatTop.x, seatTop.y, px - M(0.03), saddleTop - M(0.04), 0, M(0.025), M(0.025)));
  trim.push(tint(extrude([[px + M(0.16), saddleTop - M(0.01)], [px + M(0.14), saddleTop - M(0.04)], [px - M(0.1), saddleTop - M(0.05)],
    [px - M(0.12), saddleTop - M(0.01)], [px - M(0.06), saddleTop]], M(0.15), M(0.02), 0, 2, 1), 0x1a1a1c));
  // … 5 lines
  for (const side of [-1, 1] as const) {
    steering.push(strut(0, 0, front - headX, r - headY, side * M(0.05), tube * 0.8, tube * 0.8));
  }
  // … 1 lines
  steering.push(...handlebar(fit, px, headX, headY, M(0.1), M(0.014)));
  // … 1 lines
  const crank = M(fit.crank!);
  const pedalSide = M(fit.pedalSide!);
  // … 1 lines
  for (const side of [-1, 1] as const) {
    cranks.push(new BoxGeometry(crank, M(0.025), M(0.02)).translate(side * crank / 2, 0, side * M(0.09)));
    cranks.push(tint(box(M(0.1), M(0.02), M(0.09), side * crank, 0, side * pedalSide), 0x1a1a1c));
  }
  // … 1 lines
  return {
    // … 3 lines
    headX,
    // … 2 lines
    bracketX,
    bracketY,
    // … 1 lines
    seatY: py,
    fit,
    headlamp: { x: headX + M(0.06), y: headY - M(0.08), z: 0, sx: M(0.03), sy: M(0.05), sz: M(0.06) },
  // … 1 lines
  };
}

// src/render/vehicleModels.ts:735-1018 slice of src/render/vehicleModels.ts#buildBusModel (shared): the feature's statements; "…" marks code left out
export function buildBusModel(a: Archetype): VehicleModel {
  // … 177 lines
  const wheelAt = new Vector3(driverX + M(CAB_WHEEL.forward), driverHip + M(CAB_WHEEL.up), driverZ);
  parts.add('column', 0, tint(bar(wheelAt, new Vector3(front - M(0.45), floor + M(0.3), driverZ), M(0.07)), DASH));
  // … 84 lines
  const steering: SteeringModel = { geometry: steeringWheel(M(CAB_WHEEL.radius)), seat: 0, wheel: CAB_WHEEL };
// … 18 lines
}

// src/render/vehicleModels.ts:1054-1229 slice of src/render/vehicleModels.ts#buildTruckModel (shared): the feature's statements; "…" marks code left out
export function buildTruckModel(a: Archetype): VehicleModel {
  // … 85 lines
  for (const side of [-1, 1] as const) {
    // … 8 lines
    if (side === -1) {
      const wheel = new Vector3(seatX + M(CAB_WHEEL.forward), hipY + M(CAB_WHEEL.up), z);
      parts.add('column', own, tint(bar(wheel, new Vector3(cabFront - M(0.5), belt - M(0.1), z), M(0.07)), DASH));
    }
  }
  // … 43 lines
  const steering: SteeringModel = { geometry: steeringWheel(M(CAB_WHEEL.radius)), seat: 0, wheel: CAB_WHEEL };
// … 31 lines
}
