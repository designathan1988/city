// The code of src/editor/streetscapeTool.ts that is the feature's (the rest of the file is shared).

// src/editor/streetscapeTool.ts:42-84 slice of src/editor/streetscapeTool.ts#StreetscapeTool.down (shared): the feature's statements; "…" marks code left out
  down(world: Vec2, shift: boolean): void {
    // … 16 lines
    if (approach && (signType === 'stop' || signType === 'yield')) {
      host.mutate(() => {
        // … 3 lines
        const rules = (doc.node(node.id)?.approachRules ?? []).filter((e) => e.segment !== approach.segment);
        doc.setNodeApproachRules(node.id, [...rules, { segment: approach.segment, rule: signType }]);
      // … 1 lines
      });
    // … 4 lines
    }
  // … 11 lines
  }
