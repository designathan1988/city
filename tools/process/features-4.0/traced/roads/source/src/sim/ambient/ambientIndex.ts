// The code of src/sim/ambient/ambientIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambientIndex.ts:350-350 slice of src/sim/ambient/ambientIndex.ts#StreetLength.whole (shared): the feature's statements; "…" marks code left out
  private readonly whole = new NetworkLength();

// src/sim/ambient/ambientIndex.ts:352-352 slice of src/sim/ambient/ambientIndex.ts#StreetLength.pieces (shared): the feature's statements; "…" marks code left out
  private readonly pieces = new WeakMap<SegmentRibbon, Float64Array>();

// src/sim/ambient/ambientIndex.ts:354-365 slice of src/sim/ambient/ambientIndex.ts#StreetLength.around (shared): the feature's statements; "…" marks code left out
  around(net: Network, x: number, y: number, r: number): number {
    if (r === Infinity) return Number.isNaN(x) || Number.isNaN(y) ? 0 : this.wholeMap(net);
    // … 3 lines
    for (const ribbon of net.ribbonsMeeting([x - r, y - r, x + r, y + r])) {
      const bb = ribbon.full.bbox;
      if (bb.maxX < x - r || bb.minX > x + r || bb.maxY < y - r || bb.minY > y + r) continue;
      total = this.add(total, ribbon, x, y, r);
    }
    return total;
  }

// src/sim/ambient/ambientIndex.ts:368-370 slice of src/sim/ambient/ambientIndex.ts#StreetLength.wholeMap (shared): the feature's statements; "…" marks code left out
  private wholeMap(net: Network): number {
    return this.whole.of(net);
  }

// src/sim/ambient/ambientIndex.ts:373-388 slice of src/sim/ambient/ambientIndex.ts#StreetLength.add (shared): the feature's statements; "…" marks code left out
  private add(total: number, ribbon: SegmentRibbon, x: number, y: number, r: number): number {
    let p = this.pieces.get(ribbon);
    if (!p) {
      const pts = ribbon.full.toPoints();
      p = new Float64Array(Math.max(0, pts.length - 1) * 3);
      for (let i = 1; i < pts.length; i++) {
        const a = pts[i - 1]!, b = pts[i]!;
        p[(i - 1) * 3] = (a.x + b.x) / 2;
        p[(i - 1) * 3 + 1] = (a.y + b.y) / 2;
        p[(i - 1) * 3 + 2] = Math.hypot(b.x - a.x, b.y - a.y);
      }
      this.pieces.set(ribbon, p);
    }
    for (let i = 0; i < p.length; i += 3) if (Math.hypot(p[i]! - x, p[i + 1]! - y) <= r) total += p[i + 2]!;
    return total;
  }
