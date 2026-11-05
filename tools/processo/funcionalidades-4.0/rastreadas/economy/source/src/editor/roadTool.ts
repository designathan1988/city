// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

// src/editor/roadTool.ts:440-514 slice of src/editor/roadTool.ts#RoadTool.layPart (shared): the feature's statements; "…" marks code left out
  private layPart(d: RoadDraft, start: Anchor, end: Anchor, endHeightOffset: number, pieces: RoadPathPiece[], last: boolean, group: number | null): boolean {
    // … 5 lines
    host.mutate(() => {
      // … 8 lines
      if (result.committed && (settings.profile || settings.width !== null)) {
        // … 8 lines
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
