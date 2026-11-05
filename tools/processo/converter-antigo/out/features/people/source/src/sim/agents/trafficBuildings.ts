// The code of src/sim/agents/trafficBuildings.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/trafficBuildings.ts:84-84 slice of src/sim/agents/trafficBuildings.ts#TRAFFIC_KINDS (shared): the feature's statements; "…" marks code left out
export const TRAFFIC_KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

// src/sim/agents/trafficBuildings.ts:91-102 slice of src/sim/agents/trafficBuildings.ts#TrafficFacts (shared): the feature's statements; "…" marks code left out
export interface TrafficFacts {
  // … 3 lines
  readonly kind: keyof Mix;
// … 6 lines
}

// src/sim/agents/trafficBuildings.ts:136-138 slice of src/sim/agents/trafficBuildings.ts#factsOf (shared): the feature's statements; "…" marks code left out
export function factsOf(b: TrafficBuilding): TrafficFacts {
  return { id: b.id, x: b.x, y: b.y, kind: kindOf(b), traits: traitsOf(b), frontage: frontageBound(b), bounds: boxOf(b.bounds), reach: boxOf(b.reach) };
}

// src/sim/agents/trafficBuildings.ts:215-215 method docBuildings.eachFootprints: the feature's own
    eachFootprints: (visit) => { for (const b of doc.buildings.all()) visit(b.id, trafficBuilding(b).footprints); },

// src/sim/agents/trafficBuildings.ts:343-345 method TrafficBuildingTable.eachFootprints: the feature's own
  eachFootprints(visit: (id: BuildingId, footprints: readonly (readonly Vec2[])[]) => void): void {
    for (const id of this.ids()) visit(id, this.footprints(id));
  }

// src/sim/agents/trafficBuildings.ts:459-487 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place (shared): the feature's statements; "…" marks code left out
  private place(b: TrafficBuilding, ref: RecordRef): number {
    // … 19 lines
    this.kinds[slot] = TRAFFIC_KINDS.indexOf(kindOf(b));
  // … 7 lines
  }
