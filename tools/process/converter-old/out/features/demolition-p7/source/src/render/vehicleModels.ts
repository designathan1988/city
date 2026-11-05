// The code of src/render/vehicleModels.ts that is the feature's (the rest of the file is shared).

// src/render/vehicleModels.ts:415-448 slice of src/render/vehicleModels.ts#seatShell (shared): the feature's statements; "…" marks code left out
function seatShell(hipX: number, hipY: number, floor: number, width: number, z: number, recline: number,
  backHeight: number, fabric: number): BufferGeometry[] {
  const reach = M(UPRIGHT_CUSHION_REACH);
  // … 6 lines
  const s: P2[] = [
    [hipX + reach - M(0.04), top - M(0.03)],
    [hipX + reach, top - M(0.08)],
    [hipX + reach - M(0.04), base],
    [backX - thick * cos, base],
    [backX - thick * cos - backHeight * sin, top + backHeight * cos - M(0.02)],
    [backX - backHeight * sin + M(0.01), top + backHeight * cos],
    [backX, top + M(0.05)],
    [backX + M(0.06), top],
  ];
  // … 1 lines
  const shell = tint(extrude(s, width, M(0.035), z, 4, 2), fabric);
  const out: BufferGeometry[] = [shell];
  // … 1 lines
  for (const side of [-1, 1] as const) {
    // … 4 lines
    out.push(tint(g, SEAT_SIDE));
  }
  // … 1 lines
  out.push(tint(box(M(0.3), Math.max(M(0.02), base - floor), width * 0.6, hipX + M(0.08), (base + floor) / 2, z), DASH));
  return out;
}
