// What the feature uses of src/world/network.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/network.ts:46-380 class Network
export class Network { … }

// src/world/network.ts:54-54 field Network.junctions
/** The junctions and ribbons by id, in the order the rebuilds filed them, never id order: a reader that needs id order sorts what it takes (`CellFiling.idsMeeting`). */
  readonly junctions = new Map<NodeId, JunctionLevels>();

// src/world/network.ts:55-55 field Network.ribbons
readonly ribbons = new Map<SegmentId, SegmentRibbon>();

// src/world/network.ts:57-57 field Network.segmentCells
/** The ribbons and the junctions filed by the cells their surfaces reach (`world/cells.ts`) */
  readonly segmentCells = new CellFiling<SegmentId, SegmentRibbon>();

// src/world/network.ts:58-58 field Network.junctionCells
readonly junctionCells = new CellFiling<NodeId, JunctionLevels>();

// src/world/network.ts:62-69 method Network.ribbonsMeeting
/** DO NOT go back to a rank of every ribbon made again at each revision (`ribbonOrder`) */
  ribbonsMeeting(rect: ChangeRect): SegmentRibbon[];

// src/world/network.ts:72-72 field Network.impossible
/** Nodes the editor would refuse to create, with the gap that condemns them, in radians. */
  readonly impossible = new Map<NodeId, number>();

// src/world/network.ts:80-80 field Network.polylines
readonly polylines = new PolylineCache();

// src/world/network.ts:87-87 field Network.revision
/** Matches `RoadDoc.revision` at the time of the last rebuild. */
  revision = -1;

// src/world/network.ts:89-89 field Network.trafficRevision
/** Road plan revision; vertical-only edits leave it unchanged. */
  trafficRevision = -1;

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:119-130 method Network.rebuild
rebuild(options: { readonly full?: boolean } = {}): void;

// src/world/network.ts:174-195 method Network.fork
fork(work: RoadDoc): Network;

// src/world/network.ts:198-216 method Network.takeBack
/** A working network (`fork`) taken back once its document was (`RoadDoc.takeBack`): only what it wrote, then its state. */
  takeBack(work: Network): void;

// src/world/network.ts:288-295 method Network.mouthDistance
/** Distance from a node to this segment's junction mouth, at asphalt level. */
  mouthDistance(seg: SegmentId, node: NodeId): number;

// src/world/network.ts:298-316 method Network.stopLineDistance
/** Distance from a node at which vehicles must stop on the given segment. */
  stopLineDistance(seg: SegmentId, node: NodeId): number;

// src/world/network.ts:324-379 method Network.crosswalkDistanceAt
crosswalkDistanceAt(seg: SegmentId, node: NodeId): number;
