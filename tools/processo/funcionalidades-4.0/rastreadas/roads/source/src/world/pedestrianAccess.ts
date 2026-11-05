// The code of src/world/pedestrianAccess.ts that is the feature's (the rest of the file is shared).

// src/world/pedestrianAccess.ts:4-6 slice of src/world/pedestrianAccess.ts#carriesPedestrians (shared): the feature's statements; "…" marks code left out
export function carriesPedestrians(road: Pick<RoadType, 'id'>): boolean {
  return road.id !== 'highway' && road.id !== 'ramp';
}
