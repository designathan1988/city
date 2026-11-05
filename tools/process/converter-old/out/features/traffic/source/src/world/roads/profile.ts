// The code of src/world/roads/profile.ts that is the feature's (the rest of the file is shared).

// src/world/roads/profile.ts:34-40 slice of src/world/roads/profile.ts#ProfileSource (shared): the feature's statements; "…" marks code left out
export interface ProfileSource {
  // … 4 lines
  readonly parking?: SegmentParking;
}

// src/world/roads/profile.ts:54-57 slice of src/world/roads/profile.ts#profileOf.band (shared): the feature's statements; "…" marks code left out
  const band = (kind: ParkingKind): void => {
    if (kind === 'parallel') elements.push({ kind: 'parking', width: PARKING_DEPTH.parallel });
    else if (kind === 'cycle') elements.push({ kind: 'cycle', width: PARKING_DEPTH.cycle });
  };

// src/world/roads/profile.ts:185-192 slice of src/world/roads/profile.ts#AppliedProfile (shared): the feature's statements; "…" marks code left out
export interface AppliedProfile {
  // … 5 lines
  readonly parking: SegmentParking | undefined;
}

// src/world/roads/profile.ts:215-215 slice of src/world/roads/profile.ts#authoredFields.kindAt (shared): the feature's statements; "…" marks code left out
  const kindAt = (x: ProfileElement | undefined): ParkingKind => x?.kind === 'parking' ? 'parallel' : x?.kind === 'cycle' ? 'cycle' : 'none';
