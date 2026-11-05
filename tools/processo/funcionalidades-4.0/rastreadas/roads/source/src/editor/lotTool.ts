// The code of src/editor/lotTool.ts that is the feature's (the rest of the file is shared).

// src/editor/lotTool.ts:32-46 slice of src/editor/lotTool.ts#LotToolHost (shared): the feature's statements; "…" marks code left out
export interface LotToolHost {
  // … 1 lines
  readonly net: Network;
// … 11 lines
}

// src/editor/lotTool.ts:171-171 slice of src/editor/lotTool.ts#LotTool.net (shared): the feature's statements; "…" marks code left out
  private get net(): Network { return this.host.net; }

// src/editor/lotTool.ts:174-184 slice of src/editor/lotTool.ts#LotTool.refusedNow (shared): the feature's statements; "…" marks code left out
  refusedNow(): Set<number> {
    const net = this.net;
    if (this.refusedNet === net.state) return this.refused;
    const delta = this.refusedNet === null ? null : net.changesSince(this.refusedNet);
    this.refusedNet = net.state;
    if (delta === null) { this.refused.clear(); return this.refused; }
    for (const r of delta.rects) {
      for (const lot of this.doc.lotsIn([r[0] - REFUSED_REACH, r[1] - REFUSED_REACH, r[2] + REFUSED_REACH, r[3] + REFUSED_REACH])) this.refused.delete(lot.id);
    }
  // … 1 lines
  }

// src/editor/lotTool.ts:226-233 slice of src/editor/lotTool.ts#LotTool.proposalKey (shared): the feature's statements; "…" marks code left out
  private proposalKey(): number {
    // … 1 lines
    if (this.net.state !== this.proposalNet || changed === null || changed.length > 0) {
      this.proposalNet = this.net.state;
      this.proposalStamp++;
    }
  // … 1 lines
  }

// src/editor/lotTool.ts:334-334 slice of src/editor/lotTool.ts#LotTool.snap (shared): the feature's statements; "…" marks code left out
  private snap(p: Vec2): Vec2 { return lotSnapper(this.doc, this.net)(p, this.snapReach()).p; }

// src/editor/lotTool.ts:370-373 slice of src/editor/lotTool.ts#LotTool.snapExcept (shared): the feature's statements; "…" marks code left out
  private snapExcept(p: Vec2, from: Vec2): Vec2 {
    // … 1 lines
    return lotSnapper(this.doc, this.net, undefined, from)(p, this.snapReach()).p;
  }

// src/editor/lotTool.ts:376-385 slice of src/editor/lotTool.ts#LotTool.frontSideOf (shared): the feature's statements; "…" marks code left out
  private frontSideOf(points: readonly Vec2[]): number {
    const sides = points.map((p, i) => {
      // … 2 lines
      const d = nearestRibbon(this.net, mid)?.distance ?? Infinity;
      return { i, d, length: Math.hypot(q.x - p.x, q.y - p.y) };
    });
  // … 2 lines
  }

// src/editor/lotTool.ts:388-391 slice of src/editor/lotTool.ts#LotTool.landLot (shared): the feature's statements; "…" marks code left out
  private landLot(points: readonly Vec2[]): { corners: Vec2[]; front: number } | null {
    const corners = onLand(this.net, points);
    return corners ? { corners, front: this.frontSideOf(corners) } : null;
  }

// src/editor/lotTool.ts:394-400 slice of src/editor/lotTool.ts#LotTool.streetAngleNear (shared): the feature's statements; "…" marks code left out
  private streetAngleNear(p: Vec2): number {
    const nearest = nearestRibbon(this.net, p);
    if (!nearest) return 0;
    const line = nearest.ribbon.full;
    const f = line.sampleAt(line.closestPoint(p).s);
    return Math.atan2(f.t.y, f.t.x);
  }

// src/editor/lotTool.ts:791-855 slice of src/editor/lotTool.ts#LotTool.refreshGesture (shared): the feature's statements; "…" marks code left out
  private refreshGesture(d: LotDecor, hoverLot: Lot | undefined, lotsSeen: number): void {
    // … 47 lines
    if (this.drawn) {
      // … 2 lines
      const cut = rect ? onLand(this.net, rect) : null;
      if (cut) polygons.push({ corners: cut, fill: 0xffffff, fillAlpha: 0.2, line: 0xffffff, lineAlpha: 1, width: 0.5 });
    }
  // … 10 lines
  }
