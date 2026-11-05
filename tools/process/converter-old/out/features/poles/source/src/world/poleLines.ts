// The code of src/world/poleLines.ts that is the feature's (the rest of the file is shared).

// src/world/poleLines.ts:65-70 interface OnContour: the feature's own
export interface OnContour {
  readonly contour: number;
  readonly s: number;
  readonly at: Vec2;
  readonly distance: number;
}

// src/world/poleLines.ts:83-87 function scratchOf: the feature's own
function scratchOf(lines: PoleLines): ContourScratch {
  const scratch = lines.edges.scratch;
  if (scratch.round >= 0xfffffff0) { scratch.stamp.fill(0); scratch.round = 0; }
  return scratch;
}

// src/world/poleLines.ts:90-146 function nearestOnContour: the feature's own
export function nearestOnContour(lines: PoleLines, at: Vec2, reach: number): OnContour | null {
  const { buckets } = lines.edges;
  const px = at.x, py = at.y;
  // Per contour: the least squared distance found, the first edge giving it, and its parameter - in the scratch of the lines (`ContourScratch`), the contours met listed.
  const scratch = scratchOf(lines);
  const { distSq: bestSq, edge: bestEdge, t: bestT, stamp } = scratch;
  const round = ++scratch.round;
  const met = scratch.met;
  met.length = 0;
  for (let cx = Math.floor((px - reach) / EDGE_BUCKET); cx <= Math.floor((px + reach) / EDGE_BUCKET); cx++) {
    for (let cy = Math.floor((py - reach) / EDGE_BUCKET); cy <= Math.floor((py + reach) / EDGE_BUCKET); cy++) {
      const list = buckets.get(edgeBucket(cx, cy));
      if (!list) continue;
      for (let k = 0; k < list.length; k += 2) {
        const contour = list[k]!, i = list[k + 1]!;
        const xy = lines.contours[contour]!.xy;
        // The arithmetic of `Polyline.nearestSegment`, in its order, so the answer is the same bit for bit.
        const ax = xy[i * 2]!, ay = xy[i * 2 + 1]!;
        const abx = xy[i * 2 + 2]! - ax, aby = xy[i * 2 + 3]! - ay;
        const l2 = abx * abx + aby * aby;
        let t = 0, qx = ax, qy = ay;
        if (l2 >= EPS) {
          t = ((px - ax) * abx + (py - ay) * aby) / l2;
          t = t < 0 ? 0 : t > 1 ? 1 : t;
          qx = ax + abx * t;
          qy = ay + aby * t;
        }
        const ex = px - qx, ey = py - qy;
        const distSq = ex * ex + ey * ey;
        if (stamp[contour] !== round) {
          stamp[contour] = round;
          met.push(contour);
          bestSq[contour] = distSq; bestEdge[contour] = i; bestT[contour] = t;
        } else if (distSq < bestSq[contour]! || (distSq === bestSq[contour]! && i < bestEdge[contour]!)) {
          bestSq[contour] = distSq; bestEdge[contour] = i; bestT[contour] = t;
        }
      }
    }
  }
  let best: OnContour | null = null;
  if (met.length > 1) met.sort((a, b) => a - b);
  for (const contour of met) {
    const distSq = bestSq[contour]!, edge = bestEdge[contour]!, t = bestT[contour]!;
    const distance = Math.sqrt(distSq);
    if (distance > reach || (best && !(distance < best.distance))) continue;
    const line = lines.contours[contour]!;
    const xy = line.xy, cum = line.cum;
    const ax = xy[edge * 2]!, ay = xy[edge * 2 + 1]!;
    best = {
      contour,
      s: cum[edge]! + t * (cum[edge + 1]! - cum[edge]!),
      at: { x: ax + (xy[edge * 2 + 2]! - ax) * t, y: ay + (xy[edge * 2 + 3]! - ay) * t },
      distance,
    };
  }
  return best;
}

// src/world/poleLines.ts:475-478 function onFootway: the feature's own
export function onFootway(net: Network, p: Vec2): boolean {
  const inside = insideLevels(net, p);
  return inside.paving && !inside.kerbed;
}
