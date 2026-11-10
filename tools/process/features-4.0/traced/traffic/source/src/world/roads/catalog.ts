// The code of src/world/roads/catalog.ts that is the feature's (the rest of the file is shared).

// src/world/roads/catalog.ts:45-48 slice of src/world/roads/catalog.ts#shapeProfile.kerb (shared): the feature's statements; "…" marks code left out
  const kerb = (k: 'parking' | 'cycle' | undefined): void => {
    if (k === 'parking') e.push({ kind: 'parking', width: PARKING_DEPTH.parallel });
    if (k === 'cycle') e.push({ kind: 'cycle', width: PARKING_DEPTH.cycle });
  };
