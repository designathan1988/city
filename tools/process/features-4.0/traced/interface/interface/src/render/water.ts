// What the feature uses of src/render/water.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/water.ts:46-53 interface WaterLook
/** How the water looks and moves (`world/weather.ts`) */
export interface WaterLook {
  readonly waves: number;
  readonly foam: number;
  readonly current: number;
  readonly windX: number;
  readonly windZ: number;
  readonly windSpeed: number;
}
