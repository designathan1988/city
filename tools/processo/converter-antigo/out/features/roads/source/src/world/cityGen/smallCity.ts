// The code of src/world/cityGen/smallCity.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/smallCity.ts:5-5 slice of src/world/cityGen/smallCity.ts#plans (shared): the feature's statements; "…" marks code left out
const plans = new Map<number, CityPlan>();

// src/world/cityGen/smallCity.ts:8-12 slice of src/world/cityGen/smallCity.ts#smallCityPlan (shared): the feature's statements; "…" marks code left out
export function smallCityPlan(seed: number): CityPlan {
  // … 1 lines
  if (!plan) plans.set(seed, plan = planCity({ ...DEFAULT_CITY, seed }));
  return plan;
}
