// The code of src/editor/poleRoutes.ts that is the feature's (the rest of the file is shared).

// src/editor/poleRoutes.ts:50-122 slice of src/editor/poleRoutes.ts#buildRouteGraph (shared): the feature's statements; "…" marks code left out
export function buildRouteGraph(net: Network, lines: PoleLines, region: ChangeRect): RouteGraph {
  // … 37 lines
  for (const id of net.segmentCells.idsMeeting(region)) {
    const ribbon = net.ribbons.get(id);
    if (!ribbon || ribbon.road.sidewalk <= 0) continue;
    const centre = ribbon.centre[Level.Asphalt];
    if (!centre || centre.length <= 0) continue;
    // The kerb line stands a kerb stone and the inset off the carriageway's edge.
    const reach = ribbon.road.width / 2 + CURB_BAND + POLE_KERB_INSET;
    for (let at = STATION_STEP / 2; at < centre.length; at += STATION_STEP) {
      const f = centre.sampleAt(at);
      const nx = -f.t.y, ny = f.t.x;
      const left = nearestOnContour(lines, { x: f.p.x + nx * reach, y: f.p.y + ny * reach }, SPAN_LANDING);
      const right = nearestOnContour(lines, { x: f.p.x - nx * reach, y: f.p.y - ny * reach }, SPAN_LANDING);
      if (!left || !right || !onFootway(net, left.at) || !onFootway(net, right.at)) continue;
      const a = stationAt(left), b = stationAt(right);
      if (a === b) continue;
      const key = a < b ? a * n + b : b * n + a;
      if (seen.has(key)) continue;
      seen.add(key);
      const length = Math.hypot(x[a]! - x[b]!, y[a]! - y[b]!);
      if (length > MAX_POLE_SPACING) continue;
      spanFrom.push(a, b); spanToList.push(b, a); spanLen.push(length, length);
    }
  }
// … 11 lines
}
