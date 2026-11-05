// The code of src/sim/signals/fsm.ts that is the feature's (the rest of the file is shared).

// src/sim/signals/fsm.ts:41-68 slice of src/sim/signals/fsm.ts#SignalDeps (shared): the feature's statements; "…" marks code left out
export interface SignalDeps {
  // … 1 lines
  readonly connectorsOf: (id: string) => Connector | undefined;
  // … 19 lines
  readonly settings?: (node: NodeId) => SignalSettings | undefined;
// … 4 lines
}

// src/sim/signals/fsm.ts:76-100 slice of src/sim/signals/fsm.ts#createController (shared): the feature's statements; "…" marks code left out
export function createController(
  junction: JunctionTopology,
// … 4 lines
): SignalController {
  const { plan, degraded } = safePlan(junction, crossings, deps);
  // … 1 lines
  const phase = seekPhase(plan, offset);
  return timedController({
    node: junction.node,
    plan,
    stageIndex: phase.stageIndex,
    sub: phase.sub,
    // … 1 lines
    base: phase.elapsed,
    // … 1 lines
    lastServed: new Map(plan.groups.map((g) => [g, deps.tick()])),
    stageServed: new Map(plan.stages.map((_, i) => [i, deps.tick()])),
    // … 2 lines
    degraded,
  });
}

// src/sim/signals/fsm.ts:142-162 slice of src/sim/signals/fsm.ts#safePlan (shared): the feature's statements; "…" marks code left out
function safePlan(
  junction: JunctionTopology,
// … 2 lines
): { plan: SignalPlan; degraded: boolean } {
  try {
    const conflicts = deps.conflictsKnown && !deps.conflictsKnown(junction.node) ? undefined : deps.conflictsOf;
    return { plan: buildSignalPlan(junction, crossings, deps.connectorsOf, conflicts, deps.approachSpeed), degraded: false };
  } catch (err) {
    // … 3 lines
    return {
      plan: roundRobinPlan(
        junction.groups.map((g) => g.id),
        crossings,
      ),
    // … 1 lines
    };
  }
}

// src/sim/signals/fsm.ts:171-265 slice of src/sim/signals/fsm.ts#stepController (shared): the feature's statements; "…" marks code left out
export function stepController(c: SignalController, deps: SignalDeps): boolean {
  // … 6 lines
  switch (c.sub) {
    case 'GREEN': {
      // … 1 lines
      if (deps.quiet?.(c) && !deps.pedestriansCrossing(c.node, st.pedWalk)) {
        // … 1 lines
        if (resting?.mode !== 'fixed' && !resting?.busPriority) { signalWork.rested++; return true; }
      }
      // … 6 lines
      if (settings?.mode === 'fixed') {
        const target = settings.greens?.[c.stageIndex] ?? st.targetGreen;
        if (c.elapsed >= target && (!pedestriansInside || c.elapsed >= target + PED_HOLD_LIMIT)) {
          for (const g of st.greenGroups) c.lastServed.set(g, deps.tick());
          c.stageServed.set(c.stageIndex, deps.tick());
          c.sub = 'AMBER';
          c.elapsed = 0;
        }
        break;
      }
      // … 1 lines
      if (settings?.busPriority && deps.busNear?.(c.node, st.greenGroups) && c.elapsed < st.maxGreen + BUS_EXTENSION) break;
      // … 1 lines
      if (settings?.busPriority && mayEnd && c.plan.stages.some((other, i) => i !== c.stageIndex && other.greenGroups.length &&
        deps.busNear?.(c.node, other.greenGroups))) {
        for (const g of st.greenGroups) c.lastServed.set(g, deps.tick());
        c.stageServed.set(c.stageIndex, deps.tick());
        c.sub = 'AMBER';
        c.elapsed = 0;
        break;
      }
    // … 31 lines
    }
    // … 6 lines
    case 'ALL_RED':
      // … 1 lines
      if (c.elapsed >= st.allRed) {
        // … 1 lines
        const bus = settings?.busPriority
          ? c.plan.stages.findIndex((other, i) => i !== c.stageIndex && other.greenGroups.length > 0 && !!deps.busNear?.(c.node, other.greenGroups))
          : -1;
        c.stageIndex = settings?.mode === 'fixed' ? (c.stageIndex + 1) % Math.max(1, c.plan.stages.length)
          : bus >= 0 ? bus : pickNextStage(c, deps);
      // … 2 lines
      }
      break;
  }
// … 1 lines
}

// src/sim/signals/fsm.ts:363-402 slice of src/sim/signals/fsm.ts#rebuildController (shared): the feature's statements; "…" marks code left out
export function rebuildController(
  // … 1 lines
  junction: JunctionTopology,
// … 2 lines
): void {
  // … 1 lines
  const { plan, degraded } = safePlan(junction, crossings, deps);
  // … 3 lines
  plan.stages.forEach((s, i) => {
    const score = s.greenGroups.filter((g) => wasGreen.has(g)).length;
    if (score > bestScore) {
      bestScore = score;
      best = i;
    }
  });
  // … 1 lines
  c.plan = plan;
  c.degraded = degraded;
  c.stageIndex = best;
  // … 1 lines
  if (bestScore > 0 && c.sub === 'GREEN') {
    c.elapsed = Math.min(c.elapsed, plan.stages[best]?.maxGreen ?? SIGNAL.maxGreen);
  } else {
    // What was green no longer exists, or we were mid-transition.
    c.sub = 'ALL_RED';
    c.elapsed = 0;
  }
  // … 2 lines
  for (const g of plan.groups) {
    if (!c.lastServed.has(g)) c.lastServed.set(g, now);
  }
  c.stageServed = new Map(plan.stages.map((_, i) => [i, now]));
  for (const g of [...c.lastServed.keys()]) {
    if (!plan.groups.includes(g)) c.lastServed.delete(g);
  }
}

// src/sim/signals/fsm.ts:410-412 slice of src/sim/signals/fsm.ts#fixedCycle (shared): the feature's statements; "…" marks code left out
export function fixedCycle(plan: SignalPlan, settings: SignalSettings | undefined): number {
  return plan.stages.reduce((sum, s, i) => sum + (settings?.greens?.[i] ?? s.targetGreen) + s.amber + s.allRed, 0);
}

// src/sim/signals/fsm.ts:415-430 slice of src/sim/signals/fsm.ts#seekFixed (shared): the feature's statements; "…" marks code left out
export function seekFixed(c: SignalController, settings: SignalSettings, seconds: number): void {
  const total = fixedCycle(c.plan, settings);
  if (!(total > 0)) return;
  let left = (((seconds - (settings.offset ?? 0)) % total) + total) % total;
  for (let index = 0; index < c.plan.stages.length; index++) {
    // … 1 lines
    for (const [sub, span] of [
      ['GREEN', settings.greens?.[index] ?? stage.targetGreen],
      ['AMBER', stage.amber],
      ['ALL_RED', stage.allRed],
    ] as const) {
      if (left < span) { c.stageIndex = index; c.sub = sub; c.elapsed = left; return; }
      left -= span;
    }
  }
}
