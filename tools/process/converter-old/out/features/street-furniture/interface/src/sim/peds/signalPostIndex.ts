// What the feature uses of src/sim/peds/signalPostIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/peds/signalPostIndex.ts:9-14 interface SignalPostWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface SignalPostWorld {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly graph: LaneletGraph;
  signalChangesSince(serial: number): { readonly serial: number; readonly nodes: readonly NodeId[] | null };
}

// src/sim/peds/signalPostIndex.ts:17-17 variable POST_CELL
/** Cell of the posts' grid, world units: a walker's look ahead (`walk.ts` LOOK). */
const POST_CELL = m(8);

// src/sim/peds/signalPostIndex.ts:18-18 function cellOf
const cellOf = (gx: number, gy: number): number => …;

// src/sim/peds/signalPostIndex.ts:23-23 field SignalPostIndex.byNode
private readonly byNode = new Map<NodeId, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:24-24 field SignalPostIndex.cells
private readonly cells = new Map<number, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:26-26 field SignalPostIndex.found
/** What `near` hands out, written again by each call. */
  private readonly found: SignalPost[] = [];

// src/sim/peds/signalPostIndex.ts:53-62 method SignalPostIndex.placeJunction
/** DO NOT ask `signalPosts` for them: its whole-map pass is in the same function as the pass round a point. */
  private placeJunction(w: SignalPostWorld, node: NodeId): void;

// src/sim/peds/signalPostIndex.ts:65-67 method SignalPostIndex.placeAll
/** @rebuild Every post of the map placed: the junctions were rebuilt (an edit of the roads, a new map). */
  private placeAll(w: SignalPostWorld): void;

// src/sim/peds/signalPostIndex.ts:69-75 method SignalPostIndex.file
private file(post: SignalPost): void;

// src/sim/peds/signalPostIndex.ts:78-90 method SignalPostIndex.lift
/** A junction's posts taken away. */
  private lift(node: NodeId): void;

// src/sim/peds/signalPostIndex.ts:93-104 method SignalPostIndex.near
/** The posts filed in the cells within `reach` of a point (a superset: the caller keeps its own test). */
  near(x: number, y: number, reach: number): readonly SignalPost[];
