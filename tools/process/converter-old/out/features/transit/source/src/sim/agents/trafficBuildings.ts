// The code of src/sim/agents/trafficBuildings.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/trafficBuildings.ts:706-719 slice of src/sim/agents/trafficBuildings.ts#trafficMapOf (shared): the feature's statements; "…" marks code left out
export function trafficMapOf(doc: RoadDoc): TrafficMap {
  return {
    doc: {
      // … 3 lines
      ...(hasTransit(doc.transit) ? { transit: doc.transit } : {}),
    },
  // … 4 lines
  };
}
