// What the feature uses of src/sim/intersections/claims.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/claims.ts:5-9 interface Claim
export interface Claim {
  readonly vehicle: VehicleId;
  readonly connector: ConnectorId;
  readonly grantedTick: number;
}

// src/sim/intersections/claims.ts:12-16 interface HolderState
/** Where a claim holder's body is, along the movement it holds. */
export interface HolderState {
  readonly cls: BodyClass;
  /** Body-centre arc position along the claimed connector, from its stop line. */
  readonly centre: number;
}

// src/sim/intersections/claims.ts:19-19 type LocateHolder
/** Resolves a claim to its holder's live position; null when it cannot be placed. */
export type LocateHolder = (claim: Claim) => HolderState | null;

// src/sim/intersections/claims.ts:22-32 function zoneShareable
/** Whether two movements may hold one conflict zone together right now. */
export function zoneShareable(
  point: ConflictPoint,
  a: { readonly connector: ConnectorId; readonly state: HolderState },
  b: { readonly connector: ConnectorId; readonly state: HolderState },
): boolean;

// src/sim/intersections/claims.ts:59-84 method ClaimTable.available
/** True when every listed zone is free, already held by this vehicle, held by a vehicle on this exact connector */
  available(
    points: readonly number[],
    vehicle: VehicleId,
    connector: ConnectorId,
    cls: BodyClass,
    conflicts: ConflictIndex,
    locate: LocateHolder,
    share?: (point: ConflictPoint, claim: Claim) => boolean,
  ): boolean;

// src/sim/intersections/claims.ts:87-103 method ClaimTable.grantAll
/** Grants the whole set atomically. Callers must check `available` first. */
  grantAll(
    vehicle: VehicleId,
    connector: ConnectorId,
    points: readonly number[],
    tick: number,
  ): void;

// src/sim/intersections/claims.ts:106-138 method ClaimTable.releasePassed
/** Releases zones the vehicle's body has already driven out of. */
  releasePassed(
    vehicle: VehicleId,
    connector: ConnectorId,
    centre: number,
    cls: BodyClass,
    conflicts: ConflictIndex,
  ): void;

// src/sim/intersections/claims.ts:141-147 method ClaimTable.releaseConnector
/** Releases every point held for one connector, preserving other movements. */
  releaseConnector(
    vehicle: VehicleId,
    connector: ConnectorId,
    conflicts: ConflictIndex,
  ): void;

// src/sim/intersections/claims.ts:200-208 function clearedAt
/** Body-centre position past which `cls` on `connector` is clear of the zone. */
function clearedAt(point: ConflictPoint, connector: ConnectorId, cls: BodyClass): number;
