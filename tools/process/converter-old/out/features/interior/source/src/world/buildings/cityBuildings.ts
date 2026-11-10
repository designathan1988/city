// The code of src/world/buildings/cityBuildings.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/cityBuildings.ts:339-341 function cityBuilding: the feature's own
export function cityBuilding(fn: string): CityBuilding | undefined {
  return CITY_BUILDINGS.find((b) => b.fn === fn);
}
