// The code of src/editor/poleRoutes.ts that is the feature's (the rest of the file is shared).

// src/editor/poleRoutes.ts:50-122 slice of src/editor/poleRoutes.ts#buildRouteGraph (shared): the feature's statements; "…" marks code left out
export function buildRouteGraph(net: Network, lines: PoleLines, region: ChangeRect): RouteGraph {
  // … 37 lines
  for (const id of net.segmentCells.idsMeeting(region)) {
    // … 6 lines
    for (let at = STATION_STEP / 2; at < centre.length; at += STATION_STEP) {
      // … 11 lines
      if (length > MAX_POLE_SPACING) continue;
    // … 1 lines
    }
  }
// … 11 lines
}
