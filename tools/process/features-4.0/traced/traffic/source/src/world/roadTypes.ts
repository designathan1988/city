// The code of src/world/roadTypes.ts that is the feature's (the rest of the file is shared).

// src/world/roadTypes.ts:29-80 slice of src/world/roadTypes.ts#RoadType (shared): the feature's statements; "…" marks code left out
export interface RoadType {
  // … 30 lines
  readonly parkingLeftKind: ParkingKind;
  readonly parkingRightKind: ParkingKind;
// … 18 lines
}

// src/world/roadTypes.ts:248-260 slice of src/world/roadTypes.ts#roadProfile (shared): the feature's statements; "…" marks code left out
export function roadProfile(
  // … 4 lines
  parking?: SegmentParking,
): RoadType {
  const byKey = profilesOf(section ?? NO_PART)(parking ?? NO_PART);
  // … 1 lines
  let profile = byKey.get(key);
  if (!profile) byKey.set(key, profile = resolveProfile(typeIndex, configuredLanes, direction, section, parking));
  return profile;
}

// src/world/roadTypes.ts:262-285 slice of src/world/roadTypes.ts#resolveProfile (shared): the feature's statements; "…" marks code left out
function resolveProfile(
  // … 4 lines
  parking: SegmentParking | undefined,
): RoadType {
  // … 1 lines
  if (!parking) return travel;
  // … 2 lines
  const left = fit(parking.left);
  const right = fit(parking.right);
  if (left === 'none' && right === 'none') return travel;
  const parkingLeft = PARKING_DEPTH[left], parkingRight = PARKING_DEPTH[right];
  return {
    // … 1 lines
    width: travel.width + parkingLeft + parkingRight,
    parkingLeft,
    parkingRight,
    parkingLeftKind: left,
    parkingRightKind: right,
  };
}

// src/world/roadTypes.ts:272-272 slice of src/world/roadTypes.ts#resolveProfile.fit (shared): the feature's statements; "…" marks code left out
  const fit = (kind: ParkingKind): ParkingKind => parkingAllowed(kind, travel) ? kind : 'none';
