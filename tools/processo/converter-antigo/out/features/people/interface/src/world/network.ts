// What the feature uses of src/world/network.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/network.ts:46-380 class Network
export class Network { … }

// src/world/network.ts:48-48 field Network.crossingWalkable
/** The footway the crossings are fitted to, by cell (`walkable.ts`): kept across rebuilds. */
  private crossingWalkable: WalkableSurface | null = null;

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

// src/world/network.ts:60-60 field Network.flushCells
/** The ribbons with a footway laid flush on a side, filed like `segmentCells` */
  readonly flushCells = new CellFiling<SegmentId, SegmentRibbon>();

// src/world/network.ts:62-69 method Network.ribbonsMeeting
/** DO NOT go back to a rank of every ribbon made again at each revision (`ribbonOrder`) */
  ribbonsMeeting(rect: ChangeRect): SegmentRibbon[];

// src/world/network.ts:80-80 field Network.polylines
readonly polylines = new PolylineCache();

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:143-145 method Network.changesSince
/** What changed since the network was in `state` (a token it handed out, `state`), or null: everything */
  changesSince(state: object | null): NetworkDelta | null;

// src/world/network.ts:148-150 getter Network.state
/** The current state's token, for `changesSince`. */
  get state(): object;

// src/world/network.ts:288-295 method Network.mouthDistance
/** Distance from a node to this segment's junction mouth, at asphalt level. */
  mouthDistance(seg: SegmentId, node: NodeId): number;

// src/world/network.ts:320-322 method Network.crossingSurface
/** Distance from a node to the centre of this segment's crossing. */
  /** The footway the crossings are fitted to, built once per network */
  crossingSurface(): WalkableSurface;

// src/world/network.ts:324-379 method Network.crosswalkDistanceAt
crosswalkDistanceAt(seg: SegmentId, node: NodeId): number;
