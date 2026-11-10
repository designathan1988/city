// The code of src/sim/vehicles/state.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/state.ts:11-21 slice of src/sim/vehicles/state.ts#Kinematics (shared): the feature's statements; "…" marks code left out
export interface Kinematics {
  readonly lanelet: LaneletId;
  // … 7 lines
  readonly rearPath: readonly LaneletId[];
}

// src/sim/vehicles/state.ts:23-28 slice of src/sim/vehicles/state.ts#ConnectorClearance (shared): the feature's statements; "…" marks code left out
interface ConnectorClearance {
  // … 1 lines
  readonly connector: ConnectorId;
// … 2 lines
}

// src/sim/vehicles/state.ts:30-144 slice of src/sim/vehicles/state.ts#Vehicle (shared): the feature's statements; "…" marks code left out
export interface Vehicle {
  // … 8 lines
  lanelet: LaneletId;
  // … 19 lines
  route: LaneletId[];
  // … 1 lines
  destination: LaneletId | null;
  // … 1 lines
  rearPath: LaneletId[];
  // … 4 lines
  admittedConnector: ConnectorId | null;
  // … 1 lines
  reservedConnectors: ConnectorId[];
  // … 26 lines
  desiredLane: LaneletId | null;
  // … 1 lines
  movementIntent: ConnectorId | null;
  // … 8 lines
  laneChange: LaneletId | null;
  // … 2 lines
  laneIntent: LaneletId | null;
  // … 16 lines
  commute: { readonly trip: number; readonly lanelet: LaneletId; readonly at: number } | null;
  // … 6 lines
  shadow: { readonly lanelet: LaneletId; readonly offset: number; readonly clearAt: number } | null;
// … 8 lines
}

// src/sim/vehicles/state.ts:181-243 slice of src/sim/vehicles/state.ts#createVehicle (shared): the feature's statements; "…" marks code left out
export function createVehicle(
  // … 4 lines
  lanelet: LaneletId,
// … 2 lines
): Vehicle {
  const base: Kinematics = { lanelet, s: 0, v: 0, lateral: 0, lateralSlope: 0, rearPath: [] };
  return {
    // … 4 lines
    lanelet,
    // … 9 lines
    route: [lanelet],
  // … 35 lines
  };
}
