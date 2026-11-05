// The code of src/world/junctionTurns.ts that is the feature's (the rest of the file is shared).

// src/world/junctionTurns.ts:10-10 slice of src/world/junctionTurns.ts#TurnPlan.asks (shared): the feature's statements; "…" marks code left out
  readonly asks = new Map<string, TurnAsk>();

// src/world/junctionTurns.ts:12-12 slice of src/world/junctionTurns.ts#TurnPlan.surfaces (shared): the feature's statements; "…" marks code left out
  readonly surfaces = new Map<string, SurfaceRings>();

// src/world/junctionTurns.ts:13-13 slice of src/world/junctionTurns.ts#TurnPlan.chosen (shared): the feature's statements; "…" marks code left out
  readonly chosen = new Map<string, ChosenTurn>();

// src/world/junctionTurns.ts:15-15 field TurnPlan.provisional: the feature's own
  readonly provisional = new Map<NodeId, JunctionTurns>();

// src/world/junctionTurns.ts:20-20 slice of src/world/junctionTurns.ts#JunctionTurns.made (shared): the feature's statements; "…" marks code left out
  private readonly made = new Map<string, ChosenTurn>();

// src/world/junctionTurns.ts:22-22 field JunctionTurns.provisional: the feature's own
  provisional = false;

// src/world/junctionTurns.ts:27-27 constructor JunctionTurns.constructor: the feature's own
  constructor(private readonly before: JunctionTurns | null = null, readonly plan: TurnPlan | null = null, readonly node: NodeId | null = null) {}

// src/world/junctionTurns.ts:30-41 method JunctionTurns.find: the feature's own
  find(key: string): ChosenTurn | undefined {
    let known = this.made.get(key);
    if (known === undefined && this.before) {
      known = this.before.find(key);
      if (known !== undefined) this.made.set(key, known);
    }
    if (known === undefined && this.plan) {
      known = this.plan.chosen.get(key);
      if (known !== undefined) this.made.set(key, known);
    }
    return known;
  }

// src/world/junctionTurns.ts:44-46 getter JunctionTurns.deferring: the feature's own
  get deferring(): boolean {
    return this.plan?.recording === true;
  }

// src/world/junctionTurns.ts:49-59 method JunctionTurns.defer: the feature's own
  defer(key: string, ask: () => TurnAsk, surface: () => SurfaceRings): void {
    const plan = this.plan!;
    if (!plan.asks.has(key)) {
      const made = ask();
      plan.asks.set(key, made);
      if (!plan.surfaces.has(made.surfaceKey)) plan.surfaces.set(made.surfaceKey, surface());
    }
    this.provisional = true;
    this.deferred++;
    if (this.node !== null) plan.provisional.set(this.node, this);
  }

// src/world/junctionTurns.ts:61-63 method JunctionTurns.keep: the feature's own
  keep(key: string, turn: ChosenTurn): void {
    this.made.set(key, turn);
  }

// src/world/junctionTurns.ts:73-75 slice of src/world/junctionTurns.ts#JunctionTurns.entries (shared): the feature's statements; "…" marks code left out
  entries(): [string, ChosenTurn][] {
  // … 1 lines
  }

// src/world/junctionTurns.ts:78-82 slice of src/world/junctionTurns.ts#JunctionTurns.of (shared): the feature's statements; "…" marks code left out
  static of(entries: readonly (readonly [string, ChosenTurn])[]): JunctionTurns {
    // … 1 lines
    for (const [key, turn] of entries) turns.made.set(key, turn);
  // … 1 lines
  }
