// The code of src/sim/intersections/crossingSpans.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/crossingSpans.ts:34-34 slice of src/sim/intersections/crossingSpans.ts#CrossingSpans.spans (shared): the feature's statements; "…" marks code left out
  private readonly spans = new Map<ConnectorId, Map<CrossingId, CrossingSpan | null>>();

// src/sim/intersections/crossingSpans.ts:41-41 slice of src/sim/intersections/crossingSpans.ts#CrossingSpans.ofJunction (shared): the feature's statements; "…" marks code left out
  private readonly ofJunction = new Map<NodeId, { readonly connectors: ConnectorId[]; readonly lanes: string[]; readonly keys: string[] }>();

// src/sim/intersections/crossingSpans.ts:44-51 slice of src/sim/intersections/crossingSpans.ts#CrossingSpans.build (shared): the feature's statements; "…" marks code left out
  build(w: SimWorld): void {
    // … 5 lines
    for (const node of w.graph.junctions.keys()) this.measureJunction(w, node, previous);
  }

// src/sim/intersections/crossingSpans.ts:73-131 slice of src/sim/intersections/crossingSpans.ts#CrossingSpans.measureJunction (shared): the feature's statements; "…" marks code left out
  private measureJunction(w: SimWorld, node: NodeId, previous: Map<string, Measured>): void {
    const junction = w.graph.junctions.get(node);
    if (!junction) return;
    const made = { connectors: [] as ConnectorId[], lanes: [] as string[], keys: [] as string[] };
    // … 9 lines
    for (const connectorId of junction.connectors) {
      rebuildWork.spans++;
      const connector = w.graph.connectors.get(connectorId);
      const path = connector && w.lanelet(connector.lanelet)?.centre;
      if (!connector || !path) continue;
      made.connectors.push(connector.id);
      for (const segment of incident) {
        const crossing = `${node}:${segment}`;
        const edge = w.sidewalks.edges.get(w.sidewalks.crossings.get(crossing) ?? '');
        if (!edge) continue;
        const id = key(connector.id, crossing);
        const known = previous.get(id);
        const span = known && known.length === edge.length && sameFloats(known.path, path.xy)
          && sameFloats(known.edge, edge.path.xy)
          ? known.span
          : measure(path, edge.path, samplesOf(edge.path, edge.length));
        this.measured.set(id, { path: path.xy, edge: edge.path.xy, length: edge.length, span });
        made.keys.push(id);
        let byCrossing = this.spans.get(connector.id);
        if (!byCrossing) this.spans.set(connector.id, byCrossing = new Map());
        byCrossing.set(crossing, span);
      }
    }
    // … 2 lines
    for (const laneId of junction.inbound) {
      const lane = w.lanelet(laneId);
      if (!lane || lane.kind !== 'link') continue;
      let stop = lane.length;
      for (const segment of incident) {
        if (segment === lane.segment) continue;
        const crossing = `${node}:${segment}`;
        const edge = w.sidewalks.edges.get(w.sidewalks.crossings.get(crossing) ?? '');
        if (!edge) continue;
        const id = `approach:${key(laneId, crossing)}`;
        const known = previous.get(id);
        const span = known && known.length === edge.length && sameFloats(known.path, lane.centre.xy)
          && sameFloats(known.edge, edge.path.xy)
          ? known.span : measure(lane.centre, edge.path, samplesOf(edge.path, edge.length));
        this.measured.set(id, { path: lane.centre.xy, edge: edge.path.xy, length: edge.length, span });
        made.keys.push(id);
        if (span) stop = Math.min(stop, span.along);
      }
      if (stop < lane.length) { this.approachStops.set(laneId, stop); made.lanes.push(laneId); }
    }
  }

// src/sim/intersections/crossingSpans.ts:138-140 slice of src/sim/intersections/crossingSpans.ts#CrossingSpans.span (shared): the feature's statements; "…" marks code left out
  span(connector: ConnectorId, crossing: CrossingId): CrossingSpan | null | undefined {
    return this.spans.get(connector)?.get(crossing);
  }

// src/sim/intersections/crossingSpans.ts:143-143 slice of src/sim/intersections/crossingSpans.ts#key (shared): the feature's statements; "…" marks code left out
const key = (connector: ConnectorId, crossing: CrossingId): string => `${connector}|${crossing}`;

// src/sim/intersections/crossingSpans.ts:163-185 slice of src/sim/intersections/crossingSpans.ts#measure (shared): the feature's statements; "…" marks code left out
function measure(path: Polyline, crossing: Polyline, samples: Float64Array): CrossingSpan | null {
  // … 19 lines
  const along = Math.max(0, centre - CROSSWALK_DEPTH / 2 - PERSON_CLEAR);
  return s0 <= s1 ? { s0, s1, along } : null;
}

// src/sim/intersections/crossingSpans.ts:199-199 slice of src/sim/intersections/crossingSpans.ts#PED_CROSSING_STOP_BUFFER (shared): the feature's statements; "…" marks code left out
export const PED_CROSSING_STOP_BUFFER = CROSSWALK_DEPTH / 2 + m(0.2);

// src/sim/intersections/crossingSpans.ts:203-219 slice of src/sim/intersections/crossingSpans.ts#reservationCoversCrossing (shared): the feature's statements; "…" marks code left out
export function reservationCoversCrossing(w: SimWorld, v: Vehicle, connector: Connector,
  segment: number, span: CrossingSpan | null | undefined): boolean {
  // … 3 lines
  if (lane?.kind === 'link' && connector.id === v.admittedConnector &&
      connector.inSegment === segment && connector.outSegment !== segment) {
    const distance = Math.max(0, lane.length - v.s) + span.along;
    if (distance > m(2) && canStopComfortably(v.driver, v.v, distance)) return false;
  }
  if (connector.id === lane?.id && v.s - v.archetype.length > span.along + CLEAR_PAST) return false;
  if (connector.id !== lane?.id && connector.id !== v.admittedConnector) {
    const token = v.clearingConnectors.find((t) => t.connector === connector.id);
    if (token && connector.length + token.distanceBeyondExit - v.archetype.length > span.along + CLEAR_PAST) return false;
  }
// … 1 lines
}

// src/sim/intersections/crossingSpans.ts:230-240 slice of src/sim/intersections/crossingSpans.ts#pedestrianInSpan (shared): the feature's statements; "…" marks code left out
export function pedestrianInSpan(w: SimWorld, connector: ConnectorId, crossing: CrossingId): CrossingSpan | null {
  // … 2 lines
  const span = w.crossingSpans.span(connector, crossing);
  if (span === null) return null;
  if (span === undefined) return { s0: 0, s1: state.length, along: 0 };
  for (const p of state.occupants) {
    if (pedestrianAffectsSpan(p, span)) return span;
  }
// … 1 lines
}
