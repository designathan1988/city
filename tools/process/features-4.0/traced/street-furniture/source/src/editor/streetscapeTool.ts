// The code of src/editor/streetscapeTool.ts that is the feature's (the rest of the file is shared).

// src/editor/streetscapeTool.ts:42-84 slice of src/editor/streetscapeTool.ts#StreetscapeTool.down (shared): the feature's statements; "…" marks code left out
  down(world: Vec2, shift: boolean): void {
    // … 14 lines
    const approach = placed.ok && placed.hit && (signType === 'stop' || signType === 'yield')
      ? signApproach(host.net, placed.at, placed.hit.segment) : null;
    if (approach && (signType === 'stop' || signType === 'yield')) {
      host.mutate(() => {
        const node = doc.node(approach.node);
        if (!node) return false;
        if (node.control !== 'priority') doc.setNodeControl(node.id, 'priority');
        const rules = (doc.node(node.id)?.approachRules ?? []).filter((e) => e.segment !== approach.segment);
        doc.setNodeApproachRules(node.id, [...rules, { segment: approach.segment, rule: signType }]);
        return true;
      });
      host.hint(`hint.streetscape.rule.${signType}`);
      this.hover = null;
      host.redraw();
      return;
    }
  // … 11 lines
  }

// src/editor/streetscapeTool.ts:115-115 slice of src/editor/streetscapeTool.ts#ROW_SPACING (shared): the feature's statements; "…" marks code left out
const ROW_SPACING: Partial<Record<LandscapeKind, number>> = Object.fromEntries(FURNITURE_CATALOG.map((e) => [e.kind, e.rowEvery]));
