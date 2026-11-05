// The code of src/render/transit.ts that is the feature's (the rest of the file is shared).

// src/render/transit.ts:323-467 slice of src/render/transit.ts#createTransit (shared): the feature's statements; "…" marks code left out
export function createTransit(): TransitMeshes & {
// … 4 lines
} {
  // … 46 lines
  const LAND_HALF = (TERRAIN_MAX_HEIGHT - TERRAIN_MIN_HEIGHT) / 2;
// … 91 lines
}

// src/render/transit.ts:420-458 slice of src/render/transit.ts#createTransit.update (shared): the feature's statements; "…" marks code left out
    update(trains, ground) {
      // … 1 lines
      for (const train of trains) {
        // … 4 lines
        for (const car of train.cars) {
          // … 1 lines
          if (viewKnown) {
            carSphere.center.set(car.x, (TERRAIN_MAX_HEIGHT + TERRAIN_MIN_HEIGHT) / 2, -car.y);
          // … 2 lines
          }
        // … 10 lines
        }
      }
    // … 12 lines
    },
