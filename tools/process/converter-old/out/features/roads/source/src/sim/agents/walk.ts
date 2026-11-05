// The code of src/sim/agents/walk.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/walk.ts:917-1192 slice of src/sim/agents/walk.ts#createAgentWalkEngine (shared): the feature's statements; "…" marks code left out
export function createAgentWalkEngine(): PedestrianEngine {
  const bridge: PeopleBridge = {
    // … 1 lines
    hailable(w, lanelet, s0, s1, exclude) {
      // … 3 lines
      const c = lane.centre;
      const a = Math.max(0, s0), b = Math.min(c.length, s1);
      if (a > b) return null;
      const pa = c.sampleAt(a).p, pb = c.sampleAt(b).p;
      let x0 = Math.min(pa.x, pb.x), y0 = Math.min(pa.y, pb.y), x1 = Math.max(pa.x, pb.x), y1 = Math.max(pa.y, pb.y);
      const cum = c.cum, xy = c.xy;
      for (let i = 0; i < c.n; i++) {
        if (cum[i]! <= a || cum[i]! >= b) continue;
        const x = xy[i * 2]!, y = xy[i * 2 + 1]!;
        if (x < x0) x0 = x; if (x > x1) x1 = x;
        if (y < y0) y0 = y; if (y > y1) y1 = y;
      }
      // … 5 lines
      nearNow(s).someIn(Math.floor((x0 - pad) / CELL), Math.floor((y0 - pad) / CELL),
        Math.floor((x1 + pad) / CELL), Math.floor((y1 + pad) / CELL), (p, rank) => {
          walkWork.hailLooked++;
          if (p.done || p.inside || p.player || p.act || p.rush || (p.fright ?? 0) > p.age) return false;
          if (p.view.ageClass === 'child' || p.view.ground !== 'footway' || exclude?.has(p.id)) return false;
          const hit = c.closestPoint({ x: p.x, y: p.y });
          if (hit.s < s0 || hit.s > s1 || hit.distance > HAIL_REACH) return false;
          // The kerb side: right of the way the lane runs (`kerbStops.ts` kerbSide).
          const f = c.sampleAt(hit.s);
          if ((p.x - f.p.x) * f.t.y - (p.y - f.p.y) * f.t.x <= 0) return false;
          if (!best || hit.s < best.s || (hit.s === best.s && rank < bestRank)) { best = { id: p.id, s: hit.s }; bestRank = rank; }
          return false;
        });
      return best;
    },
  // … 43 lines
  };
// … 192 lines
}
