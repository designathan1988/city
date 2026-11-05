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

// src/world/network.ts:75-75 field Network.trims
readonly trims = new Map<SegmentId, SegmentTrims>();

// src/world/network.ts:79-79 field Network.transitions
/** Nodes where one road carries on at another width, built as a taper rather than as a junction (`junction/transition.ts`). */
  readonly transitions = new Set<NodeId>();

// src/world/network.ts:84-84 field Network.forkedFrom
/** The network this one is a working copy of (`fork`), its maps laid over that one's; null for a network of its own. */
  forkedFrom: Network | null = null;

// src/world/network.ts:100-102 constructor Network.constructor
constructor(readonly doc: RoadDoc);

// src/world/network.ts:143-145 method Network.changesSince
/** What changed since the network was in `state` (a token it handed out, `state`), or null: everything */
  changesSince(state: object | null): NetworkDelta | null;

// src/world/network.ts:148-150 getter Network.state
/** The current state's token, for `changesSince`. */
  get state(): object;

// src/world/network.ts:170-172 method Network.isBuilt
/** DO NOT copy the network for an edit (`adopt`, twice per road drawn): `work`'s network, layered over this one, holding only what it writes. */
  /** Whether the network is rebuilt up to its document: its derived maps answer for what the document holds. */
  isBuilt(): boolean;
