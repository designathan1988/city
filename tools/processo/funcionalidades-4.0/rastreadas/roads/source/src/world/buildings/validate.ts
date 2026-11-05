// The code of src/world/buildings/validate.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/validate.ts:63-71 slice of src/world/buildings/validate.ts#SiteContext (shared): the feature's statements; "…" marks code left out
export interface SiteContext {
  // … 2 lines
  readonly net: Network | null;
// … 4 lines
}

// src/world/buildings/validate.ts:74-78 slice of src/world/buildings/validate.ts#RoadReachRibbon (shared): the feature's statements; "…" marks code left out
export interface RoadReachRibbon {
  // … 1 lines
  readonly road: Pick<RoadType, 'width' | 'sidewalk'>;
// … 1 lines
}

// src/world/buildings/validate.ts:88-96 slice of src/world/buildings/validate.ts#RoadReach (shared): the feature's statements; "…" marks code left out
export interface RoadReach {
  // … 4 lines
  readonly junctions: { get(node: NodeId): { get(level: SurfaceLevel): { readonly ring: JunctionRing } | undefined } | undefined };
// … 2 lines
}

// src/world/buildings/validate.ts:256-277 slice of src/world/buildings/validate.ts#touchesRoad (shared): the feature's statements; "…" marks code left out
export function touchesRoad(net: RoadReach, rect: readonly Vec2[]): boolean {
  // … 2 lines
  for (const id of net.segmentCells.idsMeeting(area)) {
    // … 2 lines
    const reach = halfWidth(ribbon.road, Level.Sidewalk) + ROAD_CLEARANCE;
    // … 1 lines
    if (bb.minX - reach > box.maxX || bb.maxX + reach < box.minX || bb.minY - reach > box.maxY || bb.maxY + reach < box.minY) continue;
    if (polylineDistance(lineOf(ribbon), rect) < reach - ROAD_CONTACT_EPS) return true;
  }
  for (const node of net.junctionCells.idsMeeting(area)) {
    // … 1 lines
    const junction = net.junctions.get(node)?.get(Level.Curb);
    if (!junction || junction.ring.isEmpty) continue;
    const jb = junction.ring.bbox;
    if (jb.minX > box.maxX || jb.maxX < box.minX || jb.minY > box.maxY || jb.maxY < box.minY) continue;
    const ring = ringOf(junction.ring);
    if (polygonsOverlap(ring, rect) && overlapArea(ring, rect) > JUNCTION_TOUCH) return true;
  }
// … 1 lines
}
