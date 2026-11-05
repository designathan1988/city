// The code of src/sim/invariants.ts that is the feature's (the rest of the file is shared).

// src/sim/invariants.ts:14-26 slice of src/sim/invariants.ts#AuditMemory (shared): the feature's statements; "…" marks code left out
interface AuditMemory {
  // … 5 lines
  readonly onsets: WeakMap<SignalController, { audited: number; readonly since: Map<number, number> }>;
// … 5 lines
}

// src/sim/invariants.ts:35-259 slice of src/sim/invariants.ts#runAudit (shared): the feature's statements; "…" marks code left out
export function runAudit(w: SimWorld, level: 'cheap' | 'full', signals?: readonly SignalController[]): AuditIssue[] {
  // … 8 lines
  const maxDark = SIGNAL.amber + SIGNAL.minAllRed + 0.5;
  for (const c of auditedSignals(w, signals, planned)) {
    if (c.degraded) {
      out.push(issue('degradedPlan', tick, c.node, 'plan failed validation'));
    }
    const currentStage = c.plan.stages[c.stageIndex];
    if (!anyGreen(c) && c.sub === 'GREEN' && !currentStage?.exclusivePed) {
      out.push(issue('planCoverageGap', tick, c.node, 'green stage serves no group'));
    }
    // Against the stage's own yellow and all-red: a stage releasing a fast approach has a longer yellow
    const stageDark = currentStage ? currentStage.amber + currentStage.allRed + 0.5 : maxDark;
    if (c.sub !== 'GREEN' && c.elapsed > Math.max(maxDark, stageDark)) {
      out.push(
        issue('allRedTooLong', tick, c.node, `${c.sub} for ${c.elapsed.toFixed(1)}s`),
      );
    }
    // A busy stage is allowed to extend from its target to maxGreen.
    const limit = SIGNAL.starvationCycles * maxSignalCycle(c) + maxDark;
    // Starved is demand left waiting, not a group long unused
    const onsets = onsetsOf(memory, c, pass);
    for (const g of c.plan.groups) {
      const last = c.lastServed.get(g);
      if (last === undefined) continue;
      const demanded =
        signalDeps.demandOn(c.node, [g]) ||
        signalDeps.reservationDemandOn(c.node, [g]);
      if (!demanded || signalStateFor(c, g) !== 'red') { onsets.delete(g); continue; }
      let since = onsets.get(g);
      if (since === undefined) onsets.set(g, since = tick);
      const red = (tick - Math.max(last, since)) * DT;
      if (red > limit) {
        out.push(issue('groupStarved', tick, `${c.node}/${g}`, `red for ${red.toFixed(1)}s with demand waiting`));
      }
    }
  }
  // … 4 lines
  for (const v of w.vehicles.values()) {
    const lane = w.lanelet(v.lanelet);
    if (!lane) {
      out.push(issue('routeless', tick, v.id, 'vehicle on a missing lanelet'));
      continue;
    }
    // … 1 lines
    if (v.s < v.prev.s - COARSE_EPS && v.lanelet === v.prev.lanelet) {
      out.push(
        issue('nonMonotoneS', tick, v.id, `s went ${v.prev.s.toFixed(3)} -> ${v.s.toFixed(3)}`),
      );
    }
    // … 1 lines
    if (v.route.length === 0) {
      out.push(issue('routeless', tick, v.id, 'empty route'));
    }
    // … 2 lines
    if (v.greenStall > 5) {
      out.push(issue('greenBlocked', tick, v.id, `${v.greenStall.toFixed(1)}s stalled on green`));
    }
    // … 2 lines
    if (lane.kind === 'link' && lane.controlled && lane.to !== undefined) {
      const controller = w.controller(lane.to);
      if (controller && v.greenDenied > maxSignalCycle(controller)) {
        out.push(
          issue(
            'greenBlocked',
            tick,
            v.id,
            `${v.greenDenied.toFixed(1)}s held at green, longer than the whole cycle`,
          ),
        );
      }
    }
    // … 1 lines
    if (lane.kind === 'link' && lane.controlled && v.v < m(0.04)) {
      const stalled = w.clock.since(v.lastMovedTick);

      // Being held out of a full lane is correct behaviour — it is what stops the network gridlocking
      if (
        stalled > MIN_WEDGE && stalled > wedgeLimit() &&
        v.constraints.obstacles.some((o) => o.kind === 'spillback')
      ) {
        if (junctionIsStuck(w, lane.to, wedgeLimit())) {
          out.push(
            issue('spillbackWedge', tick, v.id, `held ${stalled.toFixed(0)}s at a stalled junction`),
          );
        }
      }

      // Cross-node compact reservations can keep one access blocked while the same junction continues serving other directions.
      const requestWait =
        v.firstRequestTick === null ? 0 : w.clock.since(v.firstRequestTick);
      const ownsCompoundAllocation =
        v.reservedConnectors.length > 0 &&
        (v.admittedConnector !== null ||
          v.clearingConnectors.length > 0);
      // `firstRequestTick` deliberately spans the complete compact-box transaction so admission can preserve FIFO priority
      const currentAccessWait = Math.min(
        requestWait,
        w.clock.since(v.lastMovedTick),
      );
      if (
        ownsCompoundAllocation &&
        currentAccessWait > MIN_WEDGE && currentAccessWait > wedgeLimit() &&
        v.constraints.obstacles.some(
          (o) => o.kind === 'yield' || o.kind === 'conflict' || o.kind === 'pedestrian',
        )
      ) {
        out.push(
          issue(
            'greenBlocked',
            tick,
            v.id,
            `access arbitration unresolved ${currentAccessWait.toFixed(0)}s`,
          ),
        );
      }
    }
  }
  // … 6 lines
  for (const rt of w.runtime.values()) {
    for (let i = 0; i + 1 < rt.order.length; i++) {
      // … 5 lines
      const gap = b.s - b.archetype.length - a.s;
      if (gap < -m(0.004)) {
        out.push(issue('overlap', tick, `${a.id}/${b.id}`, `gap ${gap.toFixed(3)}`));
      }
    }
  }
  // … 2 lines
  for (const v of w.vehicles.values()) {
    for (const p of v.claims) {
      if (!w.claims.holds(v.id, p)) {
        out.push(issue('orphanClaim', tick, v.id, `point ${p}`));
      }
    }
    if (v.admittedConnector && w.clock.since(v.lastMovedTick) > STUCK_SECONDS) {
      out.push(issue('staleClaim', tick, v.id, 'holder immobile'));
    }
  }
  // … 2 lines
  for (const lane of w.graph.lanelets.values()) {
    if (lane.kind !== 'link') continue;
    if (lane.length <= 0) {
      out.push(issue('degenerateGeometry', tick, lane.id, 'zero length'));
    } else if (lane.length < L_MIN) {
      out.push(issue('shortLink', tick, lane.id, `length ${lane.length.toFixed(1)} < ${L_MIN.toFixed(1)}`));
    }
  }
  // … 5 lines
  const holding = new Map<number, { v: Vehicle; ids: string[] }[]>();
  for (const v of w.vehicles.values()) {
    const lane = w.lanelet(v.lanelet);
    if (!v.clearingConnectors.length && !v.admittedConnector && lane?.kind !== 'connector') continue;
    const connectorIds = new Set(v.clearingConnectors.map((token) => token.connector));
    if (v.admittedConnector) connectorIds.add(v.admittedConnector);
    if (lane?.kind === 'connector') connectorIds.add(lane.id);
    for (const id of connectorIds) {
      const node = w.connector(id)?.node;
      if (node === undefined) continue;
      // The vehicles are filed one after another: its entry at a node, if it has one, is that node's last.
      const list = holding.get(node);
      const last = list?.[list.length - 1];
      if (last?.v === v) last.ids.push(id);
      else if (list) list.push({ v, ids: [id] });
      else holding.set(node, [{ v, ids: [id] }]);
    }
  }
  for (const [crossing, state] of w.crossingStates) {
    // … 4 lines
    const here = holding.get(node);
    if (!here) continue;
    for (const occupant of state.occupants) {
      for (const { v, ids } of here) {
        for (const connectorId of ids) {
          const connector = w.connector(connectorId);
          if (connector?.node === node) {
            const span = w.crossingSpans.span(connector.id, crossing);
            if (span === null || (span && !pedestrianAffectsSpan(occupant, span))) continue;
            if (!reservationCoversCrossing(w, v, connector, segment, span)) continue;
            out.push(
              issue(
                'pedSignalContradiction',
                tick,
                `${occupant.id}/${v.id}`,
                `crossing ${crossing} overlaps reserved connector ${connector.id}`,
              ),
            );
          }
        }
      }
    }
  }
// … 2 lines
}

// src/sim/invariants.ts:287-297 slice of src/sim/invariants.ts#auditedSignals (shared): the feature's statements; "…" marks code left out
function auditedSignals(w: SimWorld, signals: readonly SignalController[] | undefined, planned: readonly NodeId[] | null): Iterable<SignalController> {
  if (!signals || planned === null) return everySignal(w);
  if (!planned.length) return signals;
  const out = [...signals];
  const seen = new Set(signals);
  for (const node of planned) {
    // … 1 lines
    if (c && !seen.has(c)) { seen.add(c); out.push(c); }
  }
  return out;
}

// src/sim/invariants.ts:300-302 slice of src/sim/invariants.ts#everySignal (shared): the feature's statements; "…" marks code left out
function everySignal(w: SimWorld): Iterable<SignalController> {
// … 1 lines
}

// src/sim/invariants.ts:305-311 slice of src/sim/invariants.ts#onsetsOf (shared): the feature's statements; "…" marks code left out
function onsetsOf(memory: AuditMemory, c: SignalController, pass: number): Map<number, number> {
  let entry = memory.onsets.get(c);
  if (!entry) memory.onsets.set(c, entry = { audited: pass, since: new Map() });
  else if (entry.audited !== pass - 1) entry.since.clear();
  entry.audited = pass;
  return entry.since;
}

// src/sim/invariants.ts:314-327 slice of src/sim/invariants.ts#followSignals (shared): the feature's statements; "…" marks code left out
function followSignals(w: SimWorld, memory: AuditMemory): readonly NodeId[] | null {
  // … 6 lines
  for (const node of log.nodes) {
    // … 2 lines
    if (c) keepCycle(memory, node, c.plan.cycle);
  }
// … 1 lines
}

// src/sim/invariants.ts:330-336 slice of src/sim/invariants.ts#refileCycles (shared): the feature's statements; "…" marks code left out
function refileCycles(w: SimWorld, memory: AuditMemory): void {
  // … 4 lines
  for (const [node, c] of w.controllers) keepCycle(memory, node, c.plan.cycle);
}

// src/sim/invariants.ts:356-361 slice of src/sim/invariants.ts#maxSignalCycle (shared): the feature's statements; "…" marks code left out
function maxSignalCycle(c: SignalController): number {
  return c.plan.stages.reduce(
    (total, stage) => total + stage.maxGreen + stage.amber + stage.allRed,
    0,
  );
}
