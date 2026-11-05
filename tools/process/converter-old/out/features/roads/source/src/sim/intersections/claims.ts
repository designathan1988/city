// The code of src/sim/intersections/claims.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/claims.ts:5-9 slice of src/sim/intersections/claims.ts#Claim (shared): the feature's statements; "…" marks code left out
export interface Claim {
  // … 1 lines
  readonly connector: ConnectorId;
// … 1 lines
}

// src/sim/intersections/claims.ts:22-32 slice of src/sim/intersections/claims.ts#zoneShareable (shared): the feature's statements; "…" marks code left out
export function zoneShareable(
  // … 1 lines
  a: { readonly connector: ConnectorId; readonly state: HolderState },
  b: { readonly connector: ConnectorId; readonly state: HolderState },
): boolean {
  if (a.connector === b.connector) return true;
  const onA = point.zone(a.connector, a.state.cls, b.state.cls);
  const onB = point.zone(b.connector, b.state.cls, a.state.cls);
  if (!onA || !onB) return true;
  return a.state.centre > onA.exit || b.state.centre > onB.exit;
}

// src/sim/intersections/claims.ts:59-84 slice of src/sim/intersections/claims.ts#ClaimTable.available (shared): the feature's statements; "…" marks code left out
  available(
    // … 2 lines
    connector: ConnectorId,
  // … 4 lines
  ): boolean {
    for (const p of points) {
      // … 1 lines
      for (const claim of this.byPoint.get(p) ?? []) {
        if (claim.vehicle === vehicle || claim.connector === connector) continue;
        // … 6 lines
        if (!zoneShareable(point, { connector: claim.connector, state }, { connector, state: applicant })) {
        // … 1 lines
        }
      }
    }
  // … 1 lines
  }

// src/sim/intersections/claims.ts:87-103 slice of src/sim/intersections/claims.ts#ClaimTable.grantAll (shared): the feature's statements; "…" marks code left out
  grantAll(
    // … 1 lines
    connector: ConnectorId,
  // … 2 lines
  ): void {
    // … 1 lines
    for (const p of points) {
      // … 1 lines
      if (!claims.some((claim) => claim.vehicle === vehicle && claim.connector === connector)) {
        claims.push({ vehicle, connector, grantedTick: tick });
        this.byPoint.set(p, claims);
      }
    // … 1 lines
    }
  // … 1 lines
  }

// src/sim/intersections/claims.ts:106-138 slice of src/sim/intersections/claims.ts#ClaimTable.releasePassed (shared): the feature's statements; "…" marks code left out
  releasePassed(
    // … 1 lines
    connector: ConnectorId,
  // … 3 lines
  ): void {
    // … 4 lines
    for (const p of held) {
      // … 1 lines
      const owner = owners.find((claim) => claim.vehicle === vehicle && claim.connector === connector);
      // … 1 lines
      if (owner?.vehicle !== vehicle || owner.connector !== connector) {
        keep.push(p);
        continue;
      }
      // … 5 lines
      if (centre > clearedAt(point, connector, cls)) {
        this.dropVehicleAtPoint(vehicle, p);
      } else {
        keep.push(p);
      }
    }
  // … 2 lines
  }

// src/sim/intersections/claims.ts:141-147 slice of src/sim/intersections/claims.ts#ClaimTable.releaseConnector (shared): the feature's statements; "…" marks code left out
  releaseConnector(
    // … 1 lines
    connector: ConnectorId,
  // … 1 lines
  ): void {
    this.releasePassed(vehicle, connector, Infinity, HEAVY, conflicts);
  }

// src/sim/intersections/claims.ts:200-208 slice of src/sim/intersections/claims.ts#clearedAt (shared): the feature's statements; "…" marks code left out
function clearedAt(point: ConflictPoint, connector: ConnectorId, cls: BodyClass): number {
  if (point.a !== connector && point.b !== connector) return Infinity;
  // … 1 lines
  for (const theirs of BODY_CLASSES) {
    const zone = point.zone(connector, cls, theirs);
    if (zone) exit = Math.max(exit, zone.exit);
  }
  return exit;
}
