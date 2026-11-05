// The code of src/world/networkBuild.ts that is the feature's (the rest of the file is shared).

// src/world/networkBuild.ts:24-37 slice of src/world/networkBuild.ts#Network (shared): the feature's statements; "…" marks code left out
interface Network {
  // … 2 lines
  readonly junctions: Map<NodeId, JunctionLevels>;
  // … 7 lines
  readonly junctionCells: CellFiling<NodeId, JunctionLevels>;
// … 1 lines
}

// src/world/networkBuild.ts:48-54 slice of src/world/networkBuild.ts#MemoEntry (shared): the feature's statements; "…" marks code left out
interface MemoEntry {
  // … 1 lines
  readonly byLevel: JunctionLevels | null;
// … 3 lines
}

// src/world/networkBuild.ts:56-58 slice of src/world/networkBuild.ts#memoEntry (shared): the feature's statements; "…" marks code left out
function memoEntry(key: number, byLevel: JunctionLevels, plate: MemoEntry['plate']): MemoEntry {
  return { key, byLevel, size: byLevel.size, plate, summary: trimSummary(byLevel) };
}

// src/world/networkBuild.ts:81-81 slice of src/world/networkBuild.ts#BakedMemo (shared): the feature's statements; "…" marks code left out
interface BakedMemo { readonly key: number; readonly levels: JunctionLevelsData | null; readonly size: number; readonly plate: MemoEntry['plate']; readonly summary: readonly number[] }

// src/world/networkBuild.ts:87-90 slice of src/world/networkBuild.ts#memoData (shared): the feature's statements; "…" marks code left out
function memoData(entry: MemoEntry | undefined): BakedMemo | null {
  // … 1 lines
  return { key: entry.key, levels: entry.byLevel ? entry.byLevel.data() : null, size: entry.size, plate: entry.plate, summary: entry.summary };
}

// src/world/networkBuild.ts:92-94 slice of src/world/networkBuild.ts#memoOf (shared): the feature's statements; "…" marks code left out
function memoOf(baked: BakedMemo): MemoEntry {
  return { key: baked.key, byLevel: baked.levels ? JunctionLevels.fromData(baked.levels, JUNCTION_KIT) : null, size: baked.size, plate: baked.plate, summary: baked.summary };
}

// src/world/networkBuild.ts:287-487 slice of src/world/networkBuild.ts#NetworkBuild.rebuild (shared): the feature's statements; "…" marks code left out
  rebuild(): NetworkDelta | null {
    // … 8 lines
    const junctionBefore = new Map<NodeId, JunctionLevels | undefined>();
    // … 78 lines
    for (const node of n3) {
      // … 2 lines
      junctionBefore.set(node, before);
    // … 8 lines
    }
    // … 59 lines
    for (const node of n3) {
      // … 1 lines
      if (byLevel?.get(Level.Asphalt)?.transition) net.transitions.add(node); else net.transitions.delete(node);
      const before = junctionBefore.get(node);
      if (before !== byLevel && before) { const b = junctionBounds(before); if (b) oldRects.push(b); }
    // … 2 lines
    }
  // … 32 lines
  }

// src/world/networkBuild.ts:595-607 slice of src/world/networkBuild.ts#NetworkBuild.levels3 (shared): the feature's statements; "…" marks code left out
  private levels3(node: NodeId): JunctionLevels | null {
  // … 11 lines
  }

// src/world/networkBuild.ts:617-645 slice of src/world/networkBuild.ts#NetworkBuild.solveNode (shared): the feature's statements; "…" marks code left out
  private solveNode(node: NodeId, scales: ReadonlyMap<SegmentId, number>, capped: boolean, known: MemoEntry | undefined, letGo = false): MemoEntry {
    // … 26 lines
    return memoEntry(key, JunctionLevels.of(byLevel, derived, JUNCTION_KIT), plate);
  }

// src/world/networkBuild.ts:727-740 slice of src/world/networkBuild.ts#NetworkBuild.finalTrims (shared): the feature's statements; "…" marks code left out
  private finalTrims(id: SegmentId, seg: RoadSegment): SegmentTrims {
    // … 2 lines
    for (const node of seg.a === seg.b ? [seg.a] : [seg.a, seg.b]) {
      // … 2 lines
      byLevel.forEachTrim((level, legSeg, trim) => {
        if (legSeg !== id) return;
        const side = seg.a === node ? t.a : t.b;
        side[level] = Math.min(side[level] ?? Infinity, trim);
      });
    }
  // … 1 lines
  }

// src/world/networkBuild.ts:758-777 slice of src/world/networkBuild.ts#NetworkBuild.squeeze (shared): the feature's statements; "…" marks code left out
  private squeeze(node: NodeId): number {
    // … 1 lines
    const junction = net.junctions.get(node)?.get(Level.Asphalt);
    if (!junction || junction.transition) return 0;
    // … 1 lines
    for (const corner of junction.corners) {
      const x = corner.x;
      if (!x || corner.psi >= Math.PI / 2) continue;
      for (const index of [corner.i, corner.j]) {
        const leg = junction.legs[index];
        const seg = leg ? net.doc.segment(leg.seg) : undefined;
        const trims = leg ? net.trims.get(leg.seg) : undefined;
        if (!leg || !seg || !trims) continue;
        const along = (x.x - leg.origin.x) * leg.dir.x + (x.y - leg.origin.y) * leg.dir.y;
        const trim = (seg.a === node ? trims.a : trims.b)[Level.Asphalt] ?? 0;
        worst = Math.max(worst, along - trim);
      }
    }
    return worst;
  }
