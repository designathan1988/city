// The code of src/world/network.ts that is the feature's (the rest of the file is shared).

// src/world/network.ts:54-54 slice of src/world/network.ts#Network.junctions (shared): the feature's statements; "…" marks code left out
  readonly junctions = new Map<NodeId, JunctionLevels>();

// src/world/network.ts:58-58 slice of src/world/network.ts#Network.junctionCells (shared): the feature's statements; "…" marks code left out
  readonly junctionCells = new CellFiling<NodeId, JunctionLevels>();

// src/world/network.ts:109-111 slice of src/world/network.ts#Network.junctionAt (shared): the feature's statements; "…" marks code left out
  junctionAt(node: NodeId, level: SurfaceLevel): Junction | undefined {
    return this.junctions.get(node)?.get(level);
  }

// src/world/network.ts:252-261 slice of src/world/network.ts#Network.continues (shared): the feature's statements; "…" marks code left out
  continues(node: NodeId): boolean {
    // … 3 lines
    if (this.junctions.get(node)?.get(Level.Asphalt)?.transition) return true;
    // … 1 lines
    const legs = this.junctions.get(node)?.get(Level.Asphalt)?.legs;
    if (!legs || legs.length !== 2) return true;
    return dot((legs[0] as { dir: Vec2 }).dir, (legs[1] as { dir: Vec2 }).dir) < -Math.cos(TRANSITION_BEND);
  }
