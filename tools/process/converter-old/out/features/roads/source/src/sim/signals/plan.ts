// The code of src/sim/signals/plan.ts that is the feature's (the rest of the file is shared).

// src/sim/signals/plan.ts:36-46 slice of src/sim/signals/plan.ts#buildSignalPlan (shared): the feature's statements; "…" marks code left out
export function buildSignalPlan(
  junction: JunctionTopology,
  // … 1 lines
  connectorsOf: (id: string) => Connector | undefined,
// … 2 lines
): SignalPlan {
  const raw = buildStages(junction, crossings, connectorsOf, conflictsOf);
  const plan = annotate(raw, junction, connectorsOf, conflictsOf);
  return validate(approachSpeed && junction.signalised ? withAmbers(plan, junction, connectorsOf, approachSpeed) : plan);
}

// src/sim/signals/plan.ts:58-75 slice of src/sim/signals/plan.ts#withAmbers (shared): the feature's statements; "…" marks code left out
function withAmbers(plan: SignalPlan, junction: JunctionTopology, connectorsOf: (id: string) => Connector | undefined,
  approachSpeed: (lane: string) => number): SignalPlan {
  // … 1 lines
  for (const id of junction.connectors) {
    const c = connectorsOf(id);
    if (!c) continue;
    fastest.set(c.group, Math.max(fastest.get(c.group) ?? 0, approachSpeed(c.fromLane)));
  }
// … 9 lines
}

// src/sim/signals/plan.ts:78-126 slice of src/sim/signals/plan.ts#annotate (shared): the feature's statements; "…" marks code left out
function annotate(
  // … 1 lines
  junction: JunctionTopology,
  connectorsOf: (id: string) => Connector | undefined,
// … 1 lines
): SignalPlan {
  const connectors = junction.connectors
    .map(connectorsOf)
    .filter((c): c is Connector => !!c);
  // … 14 lines
  if (conflictsOf && junction.signalised && junction.groups.length > 1) {
    for (const group of junction.groups) {
      const mine = connectors.filter((c) => c.group === group.id);
      const serving = stages.filter((s) => s.greenGroups.includes(group.id));
      const neverProtected = mine.filter((c) =>
        c.turn !== 'through' && serving.every((s) => !s.protectedMovements.includes(c.id)));
      if (!neverProtected.length) continue;
      const green = [group.id];
      stages.push({
        ...stage({
          green,
          // No WALK: a turn stage is too short for anybody to finish crossing, so its lamp would only ever flash
          pedWalk: [],
          target: SIGNAL.exclusiveGreen,
        }),
        // Short: it only serves the turns that could not find a gap
        maxGreen: SIGNAL.exclusiveGreen * EXCLUSIVE_MAX_FACTOR,
        protectedMovements: protectedIn(green),
        demandMovements: neverProtected.map((c) => c.id),
      });
    }
  }
// … 3 lines
}

// src/sim/signals/plan.ts:87-97 slice of src/sim/signals/plan.ts#annotate.protectedIn (shared): the feature's statements; "…" marks code left out
  const protectedIn = (green: readonly GroupId[]): string[] => connectors
    .filter((c) => green.includes(c.group))
    .filter((c) => {
      if (!conflictsOf) return c.turn === 'through';
      return conflictsOf(c.id).every((otherId) => {
        const other = connectorsOf(otherId);
        return !other || other.inSegment === c.inSegment || !green.includes(other.group) ||
          TURN_RANK[other.turn] < TURN_RANK[c.turn];
      });
    })
    .map((c) => c.id);

// src/sim/signals/plan.ts:131-131 slice of src/sim/signals/plan.ts#TURN_RANK (shared): the feature's statements; "…" marks code left out
const TURN_RANK: Record<Connector['turn'], number> = { through: 3, right: 2, left: 1, uturn: 0 };

// src/sim/signals/plan.ts:133-222 slice of src/sim/signals/plan.ts#buildStages (shared): the feature's statements; "…" marks code left out
function buildStages(
  junction: JunctionTopology,
  // … 1 lines
  connectorsOf: (id: string) => Connector | undefined,
// … 1 lines
): SignalPlan {
  const groups = junction.groups.map((g) => g.id);
  // … 1 lines
  if (!junction.signalised) {
    // Unsignalised junctions hold a permanent green; right of way is resolved by gap acceptance instead.
    return validate({
      stages: [
        stage({
          green: groups,
          pedWalk: crossings,
          target: SIGNAL.uncontrolledGreen,
        }),
      ],
      cycle: SIGNAL.uncontrolledGreen,
      groups,
      crossings,
    });
  }
  // … 1 lines
  if (groups.length <= 1) {
    // A signalised junction still needs a real cycle when all its approaches collapse into one geometry group.
    const compatible = crossingsCompatibleWith(
      junction,
      groups,
      crossings,
      connectorsOf,
    );
    const compatibleSet = new Set(compatible);
    const uncovered = crossings.filter((crossing) => !compatibleSet.has(crossing));
    const exclusiveCrossings = groups.length === 0 ? crossings : uncovered;
    const stages: Stage[] = [];

    if (groups.length) {
      stages.push(
        stage({
          green: groups,
          pedWalk: compatible,
          target: SIGNAL.baseGreen,
        }),
      );
    }
    if (exclusiveCrossings.length || groups.length === 0) {
      stages.push(
        stage({
          green: [],
          pedWalk: exclusiveCrossings,
          target: SIGNAL.baseGreen,
          exclusivePed: true,
        }),
      );
    }

    const cycle = stages.reduce((sum, current) =>
      sum + current.targetGreen + current.amber + current.allRed, 0);
    return validate({ stages, cycle, groups, crossings });
  }
  // … 3 lines
  const together_ = conflictsOf
    ? conflictFreeStages(junction, connectorsOf, conflictsOf)
    : pairOpposingGroups(junction);
  for (const together of together_) {
    const green = clamp(SIGNAL.baseGreen, SIGNAL.minGreen, SIGNAL.maxGreen);
    const walk = crossingsCompatibleWith(junction, together, crossings, connectorsOf);
    for (const crossing of walk) covered.add(crossing);
    stages.push(stage({ green: together, pedWalk: walk, target: green }));
  }
  // … 15 lines
  return validate({ stages, cycle, groups, crossings });
}

// src/sim/signals/plan.ts:225-278 slice of src/sim/signals/plan.ts#conflictFreeStages (shared): the feature's statements; "…" marks code left out
function conflictFreeStages(
  junction: JunctionTopology,
  connectorsOf: (id: string) => Connector | undefined,
// … 1 lines
): GroupId[][] {
  const movements = new Map<GroupId, Connector[]>();
  for (const id of junction.connectors) {
    const c = connectorsOf(id);
    if (!c) continue;
    const list = movements.get(c.group);
    if (list) list.push(c);
    else movements.set(c.group, [c]);
  }
  // … 14 lines
  const pending = junction.groups.slice().sort((a, b) => a.meanAngle - b.meanAngle || a.id - b.id);
  const byId = new Map(pending.map((g) => [g.id, g] as const));
  // … 1 lines
  while (pending.length) {
    const head = pending.shift();
    if (!head) break;
    const stage = [head.id];
    for (;;) {
      let best = -1;
      let bestError = Infinity;
      for (let i = 0; i < pending.length; i++) {
        const candidate = pending[i]!;
        if (!stage.every((g) => compatible(g, candidate.id) && compatible(candidate.id, g))) continue;
        const error = Math.min(...stage.map((g) => Math.abs(
          Math.abs(normaliseAngle(candidate.meanAngle - (byId.get(g)?.meanAngle ?? 0))) - Math.PI)));
        if (error < bestError) {
          bestError = error;
          best = i;
        }
      }
      if (best < 0) break;
      stage.push(pending.splice(best, 1)[0]!.id);
    }
    stages.push(stage);
  }
// … 1 lines
}

// src/sim/signals/plan.ts:238-250 slice of src/sim/signals/plan.ts#conflictFreeStages.compatible (shared): the feature's statements; "…" marks code left out
  const compatible = (a: GroupId, b: GroupId): boolean => {
    const theirs = new Set((movements.get(b) ?? []).map((c) => c.id));
    for (const c of movements.get(a) ?? []) {
      for (const other of conflictsOf(c.id)) {
        if (!theirs.has(other)) continue;
        const o = connectorsOf(other);
        // Two movements of one approach leave side by side; that is lane discipline and car-following, not a signal conflict.
        if (o && o.inSegment === c.inSegment) continue;
        return false;
      }
    }
  // … 1 lines
  };

// src/sim/signals/plan.ts:283-315 slice of src/sim/signals/plan.ts#pairOpposingGroups (shared): the feature's statements; "…" marks code left out
function pairOpposingGroups(junction: JunctionTopology): GroupId[][] {
  const pending = junction.groups.slice();
  // … 2 lines
  while (pending.length) {
    const head = pending.shift();
    if (!head) break;

    let bestIndex = -1;
    let bestError = OPPOSED_TOLERANCE;
    for (let i = 0; i < pending.length; i++) {
      const candidate = pending[i];
      if (!candidate) continue;
      // How far from anti-parallel the two mean directions are.
      const delta = Math.abs(
        Math.abs(normaliseAngle(candidate.meanAngle - head.meanAngle)) - Math.PI,
      );
      if (delta < bestError) {
        bestError = delta;
        bestIndex = i;
      }
    }

    if (bestIndex < 0) {
      stages.push([head.id]);
      continue;
    }
    const partner = pending.splice(bestIndex, 1)[0];
    stages.push(partner ? [head.id, partner.id] : [head.id]);
  }
// … 2 lines
}

// src/sim/signals/plan.ts:347-364 slice of src/sim/signals/plan.ts#crossingsCompatibleWith (shared): the feature's statements; "…" marks code left out
export function crossingsCompatibleWith(
  junction: JunctionTopology,
  // … 2 lines
  connectorsOf: (id: string) => Connector | undefined,
): CrossingId[] {
  const green = junction.connectors
    .map(connectorsOf)
    .filter((c): c is Connector => !!c && greenGroups.includes(c.group));
  // … 1 lines
  return crossings.filter((x) => {
    // … 2 lines
    if (green.some((c) => c.inSegment === segment)) return false;
    // … 1 lines
    return !green.some((c) => c.outSegment === segment && !mustYieldToPeds(c));
  });
}

// src/sim/signals/plan.ts:367-367 slice of src/sim/signals/plan.ts#mustYieldToPeds (shared): the feature's statements; "…" marks code left out
const mustYieldToPeds = (c: Connector): boolean => c.turn !== 'through';
