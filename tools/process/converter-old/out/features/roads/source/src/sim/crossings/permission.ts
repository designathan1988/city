// The code of src/sim/crossings/permission.ts that is the feature's (the rest of the file is shared).

// src/sim/crossings/permission.ts:17-42 slice of src/sim/crossings/permission.ts#mayEnterCrossing (shared): the feature's statements; "…" marks code left out
export function mayEnterCrossing(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean {
  // … 8 lines
  const junction = w.graph.junctions.get(node);
  // … 5 lines
  if (controller && junction?.signalised) {
    const state = pedestrianSignalState(controller, id, crossing.length);
    if (state !== 'walk') return false;
    // A car turning over this zebra at its own green, that has waited its share while people kept stepping off in front of it
    if (turnOwedToVehicle(w, node, id, controller)) return false;
    return true;
  }
// … 3 lines
}

// src/sim/crossings/permission.ts:54-75 slice of src/sim/crossings/permission.ts#indexReservations (shared): the feature's statements; "…" marks code left out
export function indexReservations(w: SimWorld): void {
  // … 4 lines
  for (const v of w.junctionHolders()) {
    // … 2 lines
    if (v.clearingConnectors.length === 0 && !v.admittedConnector && lane?.kind !== 'connector') continue;
    // … 3 lines
    if (lane?.kind === 'connector' && !alreadyReserved(lane.id)) reservedIds.push(lane.id);
    for (const connectorId of reservedIds) {
      // … 2 lines
      const list = index.get(connector.node);
      if (list) list.push({ v, connector: connectorId });
      else index.set(connector.node, [{ v, connector: connectorId }]);
    }
  }
}

// src/sim/crossings/permission.ts:77-87 slice of src/sim/crossings/permission.ts#crossingReservedByVehicle (shared): the feature's statements; "…" marks code left out
function crossingReservedByVehicle(w: SimWorld, node: NodeId, segment: number, crossing: CrossingId): boolean {
  // … 2 lines
  for (const { v, connector: connectorId } of filed) {
    // … 1 lines
    if (!connector || connector.node !== node) continue;
    const span = w.crossingSpans.span(connector.id, crossing);
    if (reservationCoversCrossing(w, v, connector, segment, span)) return true;
  }
// … 1 lines
}

// src/sim/crossings/permission.ts:90-103 slice of src/sim/crossings/permission.ts#turnOwedToVehicle (shared): the feature's statements; "…" marks code left out
function turnOwedToVehicle(w: SimWorld, node: NonNullable<SidewalkEdge["node"]>, crossing: string, controller: SignalController): boolean {
  for (const laneId of w.graph.junctions.get(node)?.inbound ?? []) {
    const head = w.laneHead(laneId);
    if (!head || head.admittedConnector || head.waited <= PED_COURTESY || !heldOnlyByPedestrians(w, head)) continue;
    const lane = w.lanelet(laneId);
    if (!lane || lane.length - head.s > m(8)) continue;
    const conn = head.route[1] ? w.connector(head.route[1]) : undefined;
    if (!conn || conn.node !== node) continue;
    if (w.crossingSpans.span(conn.id, crossing as never) === null) continue;
    if (signalStateFor(controller, conn.group) !== 'green') continue;
    return true;
  }
// … 1 lines
}

// src/sim/crossings/permission.ts:114-134 slice of src/sim/crossings/permission.ts#pedGapAccepted (shared): the feature's statements; "…" marks code left out
function pedGapAccepted(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean {
  // … 3 lines
  for (const laneId of crossing.lanes ?? []) {
    // … 1 lines
    if (!lane || lane.to !== crossing.node) continue;
    // … 8 lines
    const distance = lane.length - head.s;
    const arrival = distance / Math.max(head.v, 0.5);
    if (arrival < critical || !canStopComfortably(head.driver, head.v, distance)) return false;
  }
// … 1 lines
}

// src/sim/crossings/permission.ts:137-146 slice of src/sim/crossings/permission.ts#vehicleBodyOnCrossing (shared): the feature's statements; "…" marks code left out
function vehicleBodyOnCrossing(w: SimWorld, crossing: SidewalkEdge): boolean {
  for (const laneId of crossing.lanes ?? []) {
    // … 1 lines
    if (!lane || lane.to !== crossing.node) continue;
  // … 3 lines
  }
// … 1 lines
}
