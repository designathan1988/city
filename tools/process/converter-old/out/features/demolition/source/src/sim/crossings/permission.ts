// The code of src/sim/crossings/permission.ts that is the feature's (the rest of the file is shared).

// src/sim/crossings/permission.ts:17-42 function mayEnterCrossing: the feature's own
export function mayEnterCrossing(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean {
  const node = crossing.node;
  const segment = crossing.segment;
  if (node === undefined || segment === undefined) return true;

  // A signal or accepted gap cannot make an occupied piece of road empty.
  if (vehicleBodyOnCrossing(w, crossing)) return false;

  const controller = w.controller(node);
  const junction = w.graph.junctions.get(node);
  const id = makeCrossingId(node, segment);

  // Connector admission is also a crossing reservation.
  if (crossingReservedByVehicle(w, node, segment, id)) return false;

  if (controller && junction?.signalised) {
    const state = pedestrianSignalState(controller, id, crossing.length);
    if (state !== 'walk') return false;
    // A car turning over this zebra at its own green, that has waited its share while people kept stepping off in front of it
    if (turnOwedToVehicle(w, node, id, controller)) return false;
    return true;
  }

  // Uncontrolled: accept a gap in the lanes actually being crossed.
  return pedGapAccepted(w, crossing, waited);
}

// src/sim/crossings/permission.ts:49-52 function alreadyReserved: the feature's own
const alreadyReserved = (id: string): boolean => {
  for (const s of reservedIds) if (s === id) return true;
  return false;
};

// src/sim/crossings/permission.ts:54-75 function indexReservations: the feature's own
export function indexReservations(w: SimWorld): void {
  const index = RESERVATIONS.get(w) ?? new Map<NodeId, Reservation[]>();
  index.clear();
  RESERVATIONS.set(w, index);
  // Only the vehicles at a junction (`SimWorld.junctionHolders`), not the fleet.
  for (const v of w.junctionHolders()) {
    const lane = w.lanelet(v.lanelet);
    // A vehicle mid-lane, clearing nothing and admitted nowhere, holds no reservation
    if (v.clearingConnectors.length === 0 && !v.admittedConnector && lane?.kind !== 'connector') continue;
    reservedIds.length = 0;
    for (const token of v.clearingConnectors) if (!alreadyReserved(token.connector)) reservedIds.push(token.connector);
    if (v.admittedConnector && !alreadyReserved(v.admittedConnector)) reservedIds.push(v.admittedConnector);
    if (lane?.kind === 'connector' && !alreadyReserved(lane.id)) reservedIds.push(lane.id);
    for (const connectorId of reservedIds) {
      const connector = w.connector(connectorId);
      if (!connector) continue;
      const list = index.get(connector.node);
      if (list) list.push({ v, connector: connectorId });
      else index.set(connector.node, [{ v, connector: connectorId }]);
    }
  }
}

// src/sim/crossings/permission.ts:77-87 function crossingReservedByVehicle: the feature's own
function crossingReservedByVehicle(w: SimWorld, node: NodeId, segment: number, crossing: CrossingId): boolean {
  const filed = RESERVATIONS.get(w)?.get(node);
  if (!filed || !((w.doc.node(node)?.incident as readonly number[] | undefined)?.includes(segment) ?? false)) return false;
  for (const { v, connector: connectorId } of filed) {
    const connector = w.connector(connectorId);
    if (!connector || connector.node !== node) continue;
    const span = w.crossingSpans.span(connector.id, crossing);
    if (reservationCoversCrossing(w, v, connector, segment, span)) return true;
  }
  return false;
}

// src/sim/crossings/permission.ts:90-103 function turnOwedToVehicle: the feature's own
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
  return false;
}

// src/sim/crossings/permission.ts:109-111 function heldOnlyByPedestrians: the feature's own
function heldOnlyByPedestrians(w: SimWorld, v: Vehicle): boolean {
  return v.heldByPedestriansTick !== undefined && w.clock.tick - v.heldByPedestriansTick <= 1;
}

// src/sim/crossings/permission.ts:114-134 function pedGapAccepted: the feature's own
function pedGapAccepted(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean {
  const impatience = Math.min(1.5, 0.05 * waited);
  const critical = Math.max(2.5, PED.criticalGap - impatience);

  for (const laneId of crossing.lanes ?? []) {
    const lane = w.lanelet(laneId);
    if (!lane || lane.to !== crossing.node) continue;
    const head = w.laneHead(laneId);
    if (!head) continue;
    // A vehicle standing at its line is not arriving.
    if (head.v < 0.5 && !head.admittedConnector) {
      // Its turn: a car that has stood at the zebra this long while people kept stepping off in front of it goes next.
      if (head.waited > PED_COURTESY && heldOnlyByPedestrians(w, head)) return false;
      continue;
    }
    const distance = lane.length - head.s;
    const arrival = distance / Math.max(head.v, 0.5);
    if (arrival < critical || !canStopComfortably(head.driver, head.v, distance)) return false;
  }
  return true;
}

// src/sim/crossings/permission.ts:137-146 function vehicleBodyOnCrossing: the feature's own
function vehicleBodyOnCrossing(w: SimWorld, crossing: SidewalkEdge): boolean {
  for (const laneId of crossing.lanes ?? []) {
    const lane = w.lanelet(laneId);
    if (!lane || lane.to !== crossing.node) continue;
    for (const body of w.bodiesIn(laneId)) {
      if (vehicleBodyIntersectsCrossing(w, body.vehicle, crossing)) return true;
    }
  }
  return false;
}
