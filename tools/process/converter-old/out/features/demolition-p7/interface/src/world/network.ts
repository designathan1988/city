// What the feature uses of src/world/network.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/network.ts:46-380 class Network
export class Network { … }

// src/world/network.ts:62-69 method Network.ribbonsMeeting
/** DO NOT go back to a rank of every ribbon made again at each revision (`ribbonOrder`) */
  ribbonsMeeting(rect: ChangeRect): SegmentRibbon[];

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:119-130 method Network.rebuild
rebuild(options: { readonly full?: boolean } = {}): void;
