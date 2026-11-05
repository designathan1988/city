// What the feature uses of src/sim/peds/signalPostIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/peds/signalPostIndex.ts:9-14 interface SignalPostWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface SignalPostWorld {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly graph: LaneletGraph;
  signalChangesSince(serial: number): { readonly serial: number; readonly nodes: readonly NodeId[] | null };
}

// src/sim/peds/signalPostIndex.ts:53-62 method SignalPostIndex.placeJunction
/** DO NOT ask `signalPosts` for them: its whole-map pass is in the same function as the pass round a point. */
  private placeJunction(w: SignalPostWorld, node: NodeId): void;
