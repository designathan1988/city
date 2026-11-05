// The code of src/render/densityCars.ts that is the feature's (the rest of the file is shared).

// src/render/densityCars.ts:112-117 slice of src/render/densityCars.ts#DensityCars (shared): the feature's statements; "…" marks code left out
export interface DensityCars {
  // … 2 lines
  update(net: Network, sim: SimClient, focusX: number, focusY: number): void;
// … 1 lines
}

// src/render/densityCars.ts:284-323 slice of src/render/densityCars.ts#createDensityCars.writeLine (shared): the feature's statements; "…" marks code left out
  function writeLine(net: Network, slot: number): void {
    // … 4 lines
    const line = Number.isNaN(id) ? undefined : net.ribbons.get(segment)?.full;
    if (line && line.length > 0) {
      for (let k = 0; k < LINK_POINTS; k++) {
        const f = line.sampleAt((line.length * k) / (LINK_POINTS - 1)).p;
        lines[at + k * 4] = f.x;
        lines[at + k * 4 + 1] = heightAt(f.x, f.y, segment);
        lines[at + k * 4 + 2] = -f.y;
        lines[at + k * 4 + 3] = line.length;
      }
    } else {
      const r = slot * MESO_RECORD, recs = memory.links;
      const ax = recs[r + ML.ax]!, ay = recs[r + ML.ay]!, mx = recs[r + ML.mx]!, my = recs[r + ML.my]!, bx = recs[r + ML.bx]!, by = recs[r + ML.by]!;
      const first = Math.hypot(mx - ax, my - ay), second = Math.hypot(bx - mx, by - my), L = first + second;
      if (Number.isNaN(id) || !(L > 0)) lines.fill(0, at, at + LINK_POINTS * 4);
      else {
        for (let k = 0; k < LINK_POINTS; k++) {
          const d = (L * k) / (LINK_POINTS - 1);
          const onFirst = d <= first;
          const t = onFirst ? (first > 0 ? d / first : 0) : (second > 0 ? (d - first) / second : 1);
          const x = onFirst ? ax + (mx - ax) * t : mx + (bx - mx) * t;
          const y = onFirst ? ay + (my - ay) * t : my + (by - my) * t;
          lines[at + k * 4] = x;
          lines[at + k * 4 + 1] = heightAt(x, y, undefined);
          lines[at + k * 4 + 2] = -y;
          lines[at + k * 4 + 3] = L;
        }
      }
    }
  // … 6 lines
  }

// src/render/densityCars.ts:326-336 slice of src/render/densityCars.ts#createDensityCars.syncLines (shared): the feature's statements; "…" marks code left out
  function syncLines(net: Network): void {
    if (netState === net.state) return;
    const delta = netState === null ? null : net.changesSince(netState);
    netState = net.state;
    if (delta === null) {
      // @rebuild A map opened: every known slot's line.
      for (const slots of bySegment.values()) for (const slot of slots) writeLine(net, slot);
      return;
    }
    for (const id of delta.segments) for (const slot of bySegment.get(id) ?? []) writeLine(net, slot);
  }

// src/render/densityCars.ts:339-366 slice of src/render/densityCars.ts#createDensityCars.readLines (shared): the feature's statements; "…" marks code left out
  function readLines(net: Network, slots: readonly number[]): void {
    // … 9 lines
    for (const slot of changed) {
      // … 14 lines
      writeLine(net, slot);
    }
  }

// src/render/densityCars.ts:400-460 slice of src/render/densityCars.ts#createDensityCars.writeTile (shared): the feature's statements; "…" marks code left out
  function writeTile(net: Network, tile: CarTile): void {
    // … 20 lines
    for (const slot of tile.slots) {
      // … 8 lines
      const ribbon = net.ribbons.get(linkSegment(id) as SegmentId);
      // … 7 lines
      for (let lane = 0; lane < lanes; lane++) {
        lateralOf[lane] = ribbon ? -laneOffset(ribbon.road, lane, ribbon.direction, dir === 0) : farLateral(lane, lanes, oneWay);
      }
    // … 8 lines
    }
  // … 10 lines
  }
