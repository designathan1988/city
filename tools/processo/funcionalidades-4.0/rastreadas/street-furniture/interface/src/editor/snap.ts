// What the feature uses of src/editor/snap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/snap.ts:74-81 function ribbonsNear
/** DO NOT walk `net.ribbons` / `doc.segments` instead, and DO NOT go through `Network.ribbonsMeeting`: its order table */
export function ribbonsNear(net: Network, rect: ChangeRect): SegmentRibbon[];
