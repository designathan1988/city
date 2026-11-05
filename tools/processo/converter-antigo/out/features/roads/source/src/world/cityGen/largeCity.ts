// The code of src/world/cityGen/largeCity.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/largeCity.ts:10-12 slice of src/world/cityGen/largeCity.ts#largeCityPlan (shared): the feature's statements; "…" marks code left out
export function largeCityPlan(seed: number, radius: number = LARGE_CITY_RADIUS): CityPlan {
  return planCity({ ...DEFAULT_CITY, seed, size: 'large', radius });
}
