// The code of src/world/markings.ts that is the feature's (the rest of the file is shared).

// src/world/markings.ts:176-205 slice of src/world/markings.ts#approachLegendsAt (shared): the feature's statements; "…" marks code left out
export function approachLegendsAt(net: Network, nodeId: NodeId, styleId: MarkingStyleId, out: StrokeSpec[]): void {
  // … 3 lines
  for (const segId of node.incident) {
    const kind = approachSign(net.doc, node, segId);
    if (!kind || !arrivesAt(net.doc, node.id, segId)) continue;
    // … 12 lines
    const height = kind === 'stop' ? style.legendHeight(speedKmh) : m(3);
    const foot = stopAt - LEGEND_GAP - height;
    if (foot < m(2)) continue;
    // … 1 lines
    const word = kind === 'stop' ? wordStrokes(style.stopText) : YIELD_SYMBOL;
    const letter = kind === 'stop' ? Math.min((lw * 0.78) / Math.max(1, word.width), height * 0.5) : Math.min(lw * 0.55, m(1.5));
    for (let lane = 0; lane < travelLanes(rt, ribbon.direction); lane++) {
      paintInLane(out, ribbon, centre, forward, lane, foot, word, letter, height, m(kind === 'stop' ? 0.2 : 0.16));
    }
  }
}
