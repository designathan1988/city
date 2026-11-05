// What the feature uses of src/editor/roads/streetKit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/streetKit.ts:62-70 function removeWithStreetKit
/** Runs an edit that takes `segments` away (`edit`, true when it changed the map), and their power line and furniture with them: nothing of a street left standing in the grass. */
export function removeWithStreetKit(doc: RoadDoc, net: Network, segments: readonly SegmentId[], edit: () => boolean): boolean;
