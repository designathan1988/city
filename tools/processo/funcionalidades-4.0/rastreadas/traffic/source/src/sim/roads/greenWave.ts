// The code of src/sim/roads/greenWave.ts that is the feature's (the rest of the file is shared).

// src/sim/roads/greenWave.ts:8-36 slice of src/sim/roads/greenWave.ts#greenWave (shared): the feature's statements; "…" marks code left out
export function greenWave(w: SimWorld, start: NodeId): Map<NodeId, SignalSettings> {
  // … 1 lines
  const signals = corridor.filter((c) => w.graph.junctions.get(c.node)?.signalised && w.controllers.get(c.node));
  // … 1 lines
  if (signals.length < 2) return out;
  // … 1 lines
  const plans = signals.map((c) => {
    const ctl = w.controllers.get(c.node)!;
    // DO NOT search `w.graph.connectors`: every movement of the map, copied, for each signal.
    const conn = (w.graph.junctions.get(c.node)?.connectors ?? []).map((id) => w.graph.connectors.get(id))
      .find((x) => x !== undefined && x.inSegment === c.from && x.outSegment === c.to);
    const main = Math.max(0, ctl.plan.stages.findIndex((s) => conn !== undefined && s.greenGroups.includes(conn.group)));
    return { ...c, ctl, main, cycle: fixedCycle(ctl.plan, undefined) };
  });
  const cycle = Math.max(...plans.map((p) => p.cycle));
  for (const p of plans) {
    const greens = p.ctl.plan.stages.map((s) => Math.round(s.targetGreen));
    greens[p.main] = Math.round((greens[p.main] ?? 0) + (cycle - p.cycle));
    let mainStart = 0;
    for (let i = 0; i < p.main; i++) {
      const s = p.ctl.plan.stages[i]!;
      mainStart += greens[i]! + s.amber + s.allRed;
    }
    const offset = Math.round(((p.travel - mainStart) % cycle + cycle) % cycle);
    const before = w.doc.node(p.node)?.signal;
    out.set(p.node, { mode: 'fixed', greens, ...(offset > 0 ? { offset } : {}), ...(before?.busPriority ? { busPriority: true as const } : {}) });
  }
// … 1 lines
}
