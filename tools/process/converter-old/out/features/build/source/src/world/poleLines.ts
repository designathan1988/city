// The code of src/world/poleLines.ts that is the feature's (the rest of the file is shared).

// src/world/poleLines.ts:384-472 function markPlates: the feature's own
export function* markPlates(net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void> {
  const { minX, minY, W, H, step } = grid;
  const S = CLIPPER_SCALE;
  const include = polesInclude(net);
  const rect = gridRect(grid);
  const cx = (i: number): number => minX + (i + 0.5) * step;
  const cy = (j: number): number => minY + (j + 0.5) * step;
  // Every ring that can count at some centre, with the run of columns it may count in.
  interface Placed { readonly ring: WindingRing; readonly kerbed: boolean; readonly filed: ChangeRect; readonly lo: number; readonly hi: number }
  const placed: Placed[] = [];
  const place = (rings: FootRings, filed: ChangeRect | undefined): void => {
    if (!filed) return;
    for (const kerbed of [false, true]) {
      for (const ring of kerbed ? rings.kerbed : rings.paving) {
        // insideLevels: x >= r.x0 && x <= r.x1, and idsMeeting: filed[0] <= p.x <= filed[2] - both monotone in the column.
        const lo = firstTrue(0, W, (i) => cx(i) >= filed[0] && cx(i) * S >= ring.x0);
        const hi = firstTrue(lo, W, (i) => cx(i) > filed[2] || cx(i) * S > ring.x1);
        if (lo < hi) placed.push({ ring, kerbed, filed, lo, hi });
      }
    }
  };
  for (const id of net.segmentCells.idsMeeting(rect)) {
    const ribbon = net.ribbons.get(id);
    if (ribbon) place(ribbonRings(net, ribbon, include), net.segmentCells.rectOf(id));
  }
  for (const id of net.junctionCells.idsMeeting(rect)) {
    const byLevel = net.junctions.get(id);
    if (byLevel) place(junctionRings(net, id, byLevel, include), net.junctionCells.rectOf(id));
  }
  yield;
  // Each edge filed under the rows it crosses (a counting sort by row): ring, then edge's end index.
  const rowOf: number[] = [], ringOf: number[] = [], edgeOf: number[] = [];
  for (let p = 0; p < placed.length; p++) {
    const { ring, filed } = placed[p]!;
    const xy = ring.xy;
    for (let i = 0, j = xy.length - 2; i < xy.length; j = i, i += 2) {
      const ay = xy[j + 1]!, by = xy[i + 1]!;
      if (ay === by) continue;
      const lowY = Math.min(ay, by) / S, highY = Math.max(ay, by) / S;
      const r0 = Math.max(0, Math.floor((lowY - minY) / step - 0.5) - 1);
      const r1 = Math.min(H - 1, Math.ceil((highY - minY) / step - 0.5) + 1);
      for (let row = r0; row <= r1; row++) {
        const py = cy(row), y = py * S;
        if (py < filed[1] || py > filed[3] || y < ring.y0 || y > ring.y1) continue;
        if ((ay <= y) === (by <= y)) continue;
        rowOf.push(row); ringOf.push(p); edgeOf.push(i);
      }
    }
  }
  const start = new Int32Array(H + 1);
  for (const row of rowOf) start[row + 1]!++;
  for (let row = 0; row < H; row++) start[row + 1]! += start[row]!;
  const order = new Int32Array(rowOf.length);
  const fill = start.slice(0, H);
  for (let k = 0; k < rowOf.length; k++) order[fill[rowOf[k]!]!++] = k;
  yield;
  const paving = new Int32Array(W + 1), kerbed = new Int32Array(W + 1);
  for (let row = 0; row < H; row++) {
    if (start[row] === start[row + 1]) continue;
    const y = cy(row) * S;
    let from = W, to = 0;
    for (let n = start[row]!; n < start[row + 1]!; n++) {
      const k = order[n]!;
      const { ring, kerbed: isKerb, lo, hi } = placed[ringOf[k]!]!;
      const xy = ring.xy, i = edgeOf[k]!, j = i === 0 ? xy.length - 2 : i - 2;
      const ax = xy[j]!, ay = xy[j + 1]!, bx = xy[i]!, by = xy[i + 1]!;
      // windingOf: upward edges count +1 where the cross is > 0, downward ones -1 where it is < 0; both a run from the left.
      const up = ay <= y;
      const end = firstTrue(lo, hi, (c) => {
        const x = cx(c) * S;
        const cross = (bx - ax) * (y - ay) - (x - ax) * (by - ay);
        return up ? !(cross > 0) : !(cross < 0);
      });
      if (end === lo) continue;
      const acc = isKerb ? kerbed : paving;
      acc[lo]! += up ? 1 : -1;
      acc[end]! -= up ? 1 : -1;
      from = Math.min(from, lo); to = Math.max(to, end);
    }
    let wp = 0, wk = 0;
    const base = row * W;
    for (let c = from; c < to; c++) {
      wp += paving[c]!; wk += kerbed[c]!;
      if ((wp !== 0 || wk !== 0) && label[base + c] === NEAR) label[base + c] = PAVED;
    }
    paving.fill(0, from, to + 1); kerbed.fill(0, from, to + 1);
    if (row % 64 === 63) yield;
  }
}

// src/world/poleLines.ts:392-392 interface Placed: the feature's own
  interface Placed { readonly ring: WindingRing; readonly kerbed: boolean; readonly filed: ChangeRect; readonly lo: number; readonly hi: number }

// src/world/poleLines.ts:481-484 function onPaving: the feature's own
export function onPaving(net: Network, p: Vec2): boolean {
  const inside = insideLevels(net, p);
  return inside.paving || inside.kerbed;
}
