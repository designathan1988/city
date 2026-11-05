// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:99-99 slice of src/render/agents.ts#VEHICLE_NEAR_CAP (shared): the feature's statements; "…" marks code left out
const VEHICLE_NEAR_CAP = VEHICLE_WHOLE_CAP;

// src/render/agents.ts:137-166 slice of src/render/agents.ts#AgentRenderOptions (shared): the feature's statements; "…" marks code left out
interface AgentRenderOptions {
  // … 11 lines
  readonly tier?: Pick<QualitySettings, 'vehicleErrorPixels' | 'vehicleWholeCap' | 'hairStrandPeople'>;
// … 16 lines
}

// src/render/agents.ts:530-543 slice of src/render/agents.ts#standingRange (shared): the feature's statements; "…" marks code left out
function standingRange(plan: BodyPlan, own: number, pixels: number, ratio: number, occupantZoom: number, out: { lo: number; hi: number }): void {
  // … 5 lines
  for (let k = 0; k < 3; k++) {
    // … 2 lines
    if (level < LEVEL_ERRORS.length) lo = Math.max(lo, ERROR_PIXELS / (LEVEL_ERRORS[level]! * unit));
    if (level > 0) hi = Math.min(hi, ERROR_PIXELS / (LEVEL_ERRORS[level - 1]! * unit));
  }
  out.lo = lo; out.hi = hi;
}

// src/render/agents.ts:552-2596 slice of src/render/agents.ts#createAgentMeshes (shared): the feature's statements; "…" marks code left out
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  // … 2 lines
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes {
  // … 694 lines
  let nearCandidates = new Float64Array(VEHICLE_NEAR_CAP * 2);
// … 1345 lines
}

// src/render/agents.ts:2153-2573 slice of src/render/agents.ts#createAgentMeshes.sync (shared): the feature's statements; "…" marks code left out
    sync(world, net, alpha, detailed, zoom = Number.POSITIVE_INFINITY, options = {}) {
      // … 29 lines
      const levelRatio = pixelRatio * (ERROR_PIXELS / (options.tier?.vehicleErrorPixels ?? ERROR_PIXELS));
      const nearCap = options.tier?.vehicleWholeCap ?? VEHICLE_NEAR_CAP;
      // … 215 lines
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
      // … 2 lines
      if (nearCount > nearCap) nearFloor = nearCandidates.subarray(0, nearCount).sort()[nearCount - nearCap]!;
      else nearFloor = 0;
      // … 61 lines
      if (procedural) {
        // … 4 lines
        procedural.update(options.eye, gaitClock < 0 ? undefined : gaitClock, options.shadows !== false, options.tier?.hairStrandPeople);
      }
    // … 29 lines
    },
