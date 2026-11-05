// The code of src/sim/agents/kerbBays.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/kerbBays.ts:21-27 slice of src/sim/agents/kerbBays.ts#KerbWorld (shared): the feature's statements; "…" marks code left out
export interface KerbWorld {
  // … 1 lines
  readonly net: Network;
  readonly topologyLog: ChangeLog<GraphChange>;
// … 2 lines
}

// src/sim/agents/kerbBays.ts:72-88 slice of src/sim/agents/kerbBays.ts#KerbBook (shared): the feature's statements; "…" marks code left out
export interface KerbBook {
  // … 1 lines
  net: Network | null;
// … 13 lines
}

// src/sim/agents/kerbBays.ts:190-268 slice of src/sim/agents/kerbBays.ts#followKerbs (shared): the feature's statements; "…" marks code left out
export function followKerbs(w: KerbWorld, book: KerbBook, delta: BuildingDelta | null, reads: KerbReads): Set<SegmentId> {
  // … 2 lines
  const net = book.net === w.net && book.netSeen !== null ? w.net.changesSince(book.netSeen) : null;
  // … 2 lines
  book.netSeen = w.net.state;
  // … 2 lines
  if (net && lanes) {
    for (const id of net.segments) streets.add(id);
    for (const change of lanes) for (const id of change.segments) streets.add(id);
  }
  // … 11 lines
  const whole = net === null || lanes === null;
  const before = whole ? new Map(book.bays) : book.bays;
  const keysBefore = whole ? new Map(book.keys) : book.keys;
  // … 1 lines
  if (whole) readStreets(w, book);
  else {
    for (const id of streets) {
      unfileStreet(book, id);
      book.keys.delete(id);
      book.held.delete(id);
      if (book.bays.delete(id)) changed.add(id);
      fileStreet(w, book, id);
      if (book.reach.has(id)) dirty.add(id);
    }
  }
  // … 3 lines
  if (whole || delta === null) {
    for (const id of book.reach.keys()) dirty.add(id);
  } else {
    for (const box of touched) {
      for (let i = Math.floor(box.x0 / KERB_CELL); i <= Math.floor(box.x1 / KERB_CELL); i++) {
        for (let j = Math.floor(box.y0 / KERB_CELL); j <= Math.floor(box.y1 / KERB_CELL); j++) {
          for (const id of book.streets.get(kerbCell(i, j)) ?? []) {
            const r = book.reach.get(id)!;
            if (box.x1 >= r.x0 && box.x0 <= r.x1 && box.y1 >= r.y0 && box.y0 <= r.y1) dirty.add(id);
          }
        }
      }
    }
  }
  for (const id of [...dirty].sort((a, b) => a - b)) {
    const was = before.get(id);
    if (reads.waits?.(id)) {
      // … 4 lines
      if (was?.length) changed.add(id);
    // … 1 lines
    }
    // … 4 lines
    const now = was && keysBefore.get(id) === key ? was : reads.baked?.(id, key) ?? kerbBaysOf(w, id, entrances, reads.laneAt);
    book.bays.set(id, now);
    // … 1 lines
    if (!was || !sameBays(was, now)) changed.add(id);
  }
  // … 1 lines
  if (whole) for (const id of before.keys()) if (!book.bays.has(id)) changed.add(id);
// … 1 lines
}

// src/sim/agents/kerbBays.ts:295-302 slice of src/sim/agents/kerbBays.ts#readStreets (shared): the feature's statements; "…" marks code left out
function readStreets(w: KerbWorld, book: KerbBook): void {
  // … 5 lines
  for (const id of w.net.ribbons.keys()) fileStreet(w, book, id);
}

// src/sim/agents/kerbBays.ts:305-321 slice of src/sim/agents/kerbBays.ts#fileStreet (shared): the feature's statements; "…" marks code left out
function fileStreet(w: KerbWorld, book: KerbBook, id: SegmentId): void {
  const ribbon = w.net.ribbons.get(id);
  if (!ribbon) return;
  const rt = ribbon.road;
  if (rt.parkingLeft <= 0 && rt.parkingRight <= 0) return;
  const pad = rt.width / 2 + FRONTAGE_REACH;
  const bb = ribbon.full.bbox;
  const reach = { x0: bb.minX - pad, y0: bb.minY - pad, x1: bb.maxX + pad, y1: bb.maxY + pad };
// … 8 lines
}

// src/sim/agents/kerbBays.ts:359-363 slice of src/sim/agents/kerbBays.ts#kerbKey (shared): the feature's statements; "…" marks code left out
function kerbKey(w: KerbWorld, id: SegmentId, entrances: readonly Entrance[], reads: KerbReads): number {
  const digest = parkingDigest(w.net, w.net.ribbons.get(id)!, new Digest().add(id));
  for (const [p, q] of entrances) digest.add(p.x).add(p.y).add(q.x).add(q.y);
  return digest.add(entrances.length).add(reads.linksKey(id)).value();
}

// src/sim/agents/kerbBays.ts:366-387 slice of src/sim/agents/kerbBays.ts#kerbBaysOf (shared): the feature's statements; "…" marks code left out
function kerbBaysOf(w: KerbWorld, id: SegmentId, entrances: readonly Entrance[], laneAt: KerbReads['laneAt']): Bay[] {
  // … 1 lines
  const ribbon = w.net.ribbons.get(id)!;
  // … 1 lines
  const one = Object.create(w.net, { ribbons: { value: new Map([[id, ribbon]]) } }) as Network;
  // … 1 lines
  for (const bay of parkingLayout(one, entrances).bays) {
    const f = bay.facing;
    const ahead = { x: bay.centre.x + f.x * KERB_AHEAD, y: bay.centre.y + f.y * KERB_AHEAD };
    const lane = laneAt(ahead.x, ahead.y, bay.segment);
    // The lane has to run the way the car faces: the traffic beside the bay; and its joining point
    const aligned = lane !== null && lane.tx * f.x + lane.ty * f.y > 0.7
      && (lane.x - bay.centre.x) * f.x + (lane.y - bay.centre.y) * f.y >= KERB_MIN_AHEAD;
    out.push({
      id: out.length, building: NO_LOT, x: bay.centre.x, y: bay.centre.y,
      // The car stands nose along the kerb: `o` points out of its tail.
      ox: -f.x, oy: -f.y, depth: PARKING_PITCH.parallel,
      lane: aligned ? lane : null, via: [], room: PARKING_PITCH.parallel / 2, car: null, kerb: true,
    });
  }
// … 1 lines
}
