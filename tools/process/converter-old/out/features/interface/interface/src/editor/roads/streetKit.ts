// What the feature uses of src/editor/roads/streetKit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/streetKit.ts:73-90 function carryStreetKit
/** Runs a road edit of `segments` (`edit`, true when it changed the map), then lays their power line and furniture again on the roads it left (`after`: their ids then, the same by default) */
export function carryStreetKit(doc: RoadDoc, net: Network, segments: readonly SegmentId[], edit: () => boolean,
  after: () => readonly SegmentId[] = () => segments.filter((id) => doc.segment(id) !== undefined)): boolean;
