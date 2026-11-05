// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

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

// src/editor/roadTool.ts:440-514 slice of src/editor/roadTool.ts#RoadTool.layPart (shared): the feature's statements; "…" marks code left out
  private layPart(d: RoadDraft, start: Anchor, end: Anchor, endHeightOffset: number, pieces: RoadPathPiece[], last: boolean, group: number | null): boolean {
    // … 5 lines
    host.mutate(() => {
      // … 8 lines
      if (result.committed && (settings.profile || settings.width !== null)) {
        // … 1 lines
        const money = roadsBefore(doc);
        // … 6 lines
        if (!settleRoadEdit(money, doc).affordable) {
          for (const s of kept) {
            doc.setSegmentType(s.id, s.type);
            doc.setSegmentDirection(s.id, s.direction);
            doc.setSegmentLanes(s.id, s.lanes);
            doc.setSegmentSection(s.id, s.section);
            doc.setSegmentParking(s.id, s.parking);
          }
          host.hint('hint.rule.funds');
        }
      }
    // … 14 lines
    });
  // … 24 lines
  }
