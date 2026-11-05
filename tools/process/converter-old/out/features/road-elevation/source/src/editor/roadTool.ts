// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

// src/editor/roadTool.ts:65-100 slice of src/editor/roadTool.ts#RoadToolHost (shared): the feature's statements; "…" marks code left out
export interface RoadToolHost {
  // … 23 lines
  offerElevation(solution: RoadElevation, revision: number): void;
  // … 1 lines
  groundSolve?(): RoadElevation | null;
// … 8 lines
}

// src/editor/roadTool.ts:356-369 slice of src/editor/roadTool.ts#RoadTool.verdict (shared): the feature's statements; "…" marks code left out
  verdict(): RefusalReason | null {
    // … 2 lines
    if (key !== this.judged.key) {
      this.judged = { key, reason: null, cost: null, stations: null };
      this.judgeAt = key === null ? null : performance.now() + VERDICT_DELAY;
    }
    // … 5 lines
    return this.judged.reason;
  }

// src/editor/roadTool.ts:371-372 slice of src/editor/roadTool.ts#RoadTool.judged (shared): the feature's statements; "…" marks code left out
  private judged: { key: DraftKey | null; reason: RefusalReason | null; cost: number | null; stations: readonly RoadStation[] | null } =
    { key: null, reason: null, cost: null, stations: null };

// src/editor/roadTool.ts:375-378 slice of src/editor/roadTool.ts#RoadTool.stations (shared): the feature's statements; "…" marks code left out
  stations(): readonly RoadStation[] | null {
    // … 1 lines
    return this.judged.stations;
  }

// src/editor/roadTool.ts:381-384 slice of src/editor/roadTool.ts#RoadTool.cost (shared): the feature's statements; "…" marks code left out
  cost(): number | null {
    // … 1 lines
    return this.judged.cost;
  }

// src/editor/roadTool.ts:419-431 slice of src/editor/roadTool.ts#RoadTool.judge (shared): the feature's statements; "…" marks code left out
  private judge(key: DraftKey): void {
    // … 9 lines
    this.judged = { key, reason, cost: result.cost ?? null, stations: result.stations ?? null };
  // … 1 lines
  }
