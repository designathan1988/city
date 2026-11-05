// The code of src/world/structures.ts that is the feature's (the rest of the file is shared).

// src/world/structures.ts:42-42 slice of src/world/structures.ts#TUNNEL_GRADE (shared): the feature's statements; "…" marks code left out
export const TUNNEL_GRADE = ROAD_TUNING.grade.tunnel;

// src/world/structures.ts:44-50 slice of src/world/structures.ts#ROAD_STRUCTURES (shared): the feature's statements; "…" marks code left out
export const ROAD_STRUCTURES: readonly RoadStructureSpec[] = [
  { id: 'ground', key: 'structure.ground', clearance: 0, deck: m(0.22), supports: false },
  // Fourteen units (5.6 m) to the deck SURFACE, of which the structure itself takes 3.0
  { id: 'elevated', key: 'structure.elevated', clearance: ROAD_TUNING.clearance.elevated, deck: m(1.04), supports: true },
  { id: 'bridge', key: 'structure.bridge', clearance: ROAD_TUNING.clearance.bridge, deck: m(0.96), supports: true },
  { id: 'tunnel', key: 'structure.tunnel', clearance: -TUNNEL_DEPTH, deck: m(0.24), supports: false },
] as const;

// src/world/structures.ts:55-56 slice of src/world/structures.ts#roadStructure (shared): the feature's statements; "…" marks code left out
export const roadStructure = (id: RoadStructure): RoadStructureSpec =>
  ROAD_STRUCTURES.find((value) => value.id === id) ?? (ROAD_STRUCTURES[0] as RoadStructureSpec);

// src/world/structures.ts:72-72 function isSunken: the feature's own
export const isSunken = (id: RoadStructure): boolean => id === 'tunnel';
