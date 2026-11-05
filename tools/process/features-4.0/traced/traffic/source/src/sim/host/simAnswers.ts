// The code of src/sim/host/simAnswers.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simAnswers.ts:36-54 slice of src/sim/host/simAnswers.ts#count (shared): the feature's statements; "…" marks code left out
function count(sim: SimWorld, lanes: Iterable<string>, volume: number): TrafficCount {
  // … 1 lines
  for (const id of lanes) {
    const lane = sim.graph.lanelet(id as never);
    if (!lane) continue;
    capacity += Math.max(1, lane.length / 12);
    const order = sim.runtime.get(lane.id)?.order;
    if (!order) continue;
    vehicles += order.length;
    for (const vehicleId of order) {
      const vehicle = sim.veh(vehicleId);
      if (!vehicle) continue;
      speedSum += vehicle.v;
      speedCount++;
      if (vehicle.v < QUEUED) queued++;
    }
  }
  return { vehicles, queued, speedSum, speedCount, capacity, volume };
}

// src/sim/host/simAnswers.ts:57-67 slice of src/sim/host/simAnswers.ts#roadLanes (shared): the feature's statements; "…" marks code left out
function* roadLanes(sim: SimWorld, doc: RoadDoc, id: SegmentId): Generator<string> {
  // … 2 lines
  for (const [from, to] of [[seg.a, seg.b], [seg.b, seg.a]] as const) {
    for (let k = 0; ; k++) {
      const lid = laneletId(id, from, to, k);
      if (!sim.graph.lanelet(lid)) break;
      yield lid;
    }
  }
}

// src/sim/host/simAnswers.ts:69-77 slice of src/sim/host/simAnswers.ts#answer (shared): the feature's statements; "…" marks code left out
export function answer(sim: SimWorld, doc: RoadDoc, _net: Network, ask: SimAsk): unknown {
  switch (ask.kind) {
    // … 4 lines
    case 'nodeLanes': return laneEnds(sim.graph, ask.node);
  }
}

// src/sim/host/simAnswers.ts:79-102 slice of src/sim/host/simAnswers.ts#junctionAnswer (shared): the feature's statements; "…" marks code left out
function junctionAnswer(sim: SimWorld, doc: RoadDoc, id: NodeId): JunctionAnswer | null {
  // … 2 lines
  const junction = sim.graph.junctions.get(id);
  // … 2 lines
  const traffic = count(sim, (junction?.inbound ?? []) as Iterable<string>, volume);
  // … 1 lines
  for (const cid of junction?.connectors ?? []) {
    const c = sim.connector(cid);
    if (c) connectors.push({ inSegment: c.inSegment, outSegment: c.outSegment, turn: c.turn ?? null });
  }
  return {
    ...traffic,
    known: junction !== undefined,
    signalised: junction?.signalised === true,
    // … 3 lines
    plan: controller && junction?.signalised
      ? { cycle: controller.plan.cycle, stageIndex: controller.stageIndex, sub: controller.sub, greens: controller.plan.stages.map((s) => s.targetGreen) }
      : null,
  };
}
