// What the feature uses of src/world/footwayPath.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/footwayPath.ts:11-15 interface FootwayRibbon
/** DO NOT import `network.ts` or `doc.ts` here */
export interface FootwayRibbon {
  readonly id: SegmentId;
  readonly road: RoadType;
  readonly centre: Record<number, Polyline>;
}

// src/world/footwayPath.ts:16-19 interface FootwayNet
export interface FootwayNet {
  readonly doc: { segment(id: SegmentId): { readonly direction: SegmentDirection } | undefined };
  ribbonsMeeting(rect: ChangeRect): Iterable<FootwayRibbon>;
}

// src/world/footwayPath.ts:22-29 function footwayPath
/** One side's footway of a road, as the walking network lays it: down the middle of that side's through zone, along a to b */
export function footwayPath(ribbon: FootwayRibbon, direction: SegmentDirection, side: 1 | -1): Polyline | null;
