// What the feature uses of src/world/network.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/network.ts:46-380 class Network
export class Network { … }

// src/world/network.ts:72-72 field Network.impossible
/** Nodes the editor would refuse to create, with the gap that condemns them, in radians. */
  readonly impossible = new Map<NodeId, number>();

// src/world/network.ts:80-80 field Network.polylines
readonly polylines = new PolylineCache();

// src/world/network.ts:87-87 field Network.revision
/** Matches `RoadDoc.revision` at the time of the last rebuild. */
  revision = -1;

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:119-130 method Network.rebuild
rebuild(options: { readonly full?: boolean } = {}): void;

// src/world/network.ts:288-295 method Network.mouthDistance
/** Distance from a node to this segment's junction mouth, at asphalt level. */
  mouthDistance(seg: SegmentId, node: NodeId): number;
