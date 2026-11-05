// What the feature uses of src/world/weather.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/weather.ts:3-20 interface Weather
/** THE MAP'S WEATHER (Paisagem > Terreno > Clima) */

export interface Weather {
  /** 0 dry .. 1 a downpour. */
  readonly rain: number;
  /** Wind speed, metres a second (0 still .. 30 a gale). */
  readonly wind: number;
  /** The way the wind blows TOWARDS, degrees: 0 east (+x), 90 north (+y). */
  readonly windDirection: number;
  /** Lightning strikes a minute (0 none). */
  readonly lightning: number;
  /** Thunder's loudness, 0..1. */
  readonly thunder: number;
  /** The rivers' and lakes' waves, 0 glassy .. 1 a rough sea. */
  readonly waves: number;
  /** How much foam on the water, 0..1. */
  readonly foam: number;
  /** How fast the rivers run, metres a second (0 still .. 4 a torrent). */
  readonly current: number;
}

// src/world/weather.ts:22-22 variable DEFAULT_WEATHER
export const DEFAULT_WEATHER: Weather = { rain: 0, wind: 0, windDirection: 30, lightning: 0, thunder: 0.6, waves: 0.3, foam: 0.4, current: 1.2 };

// src/world/weather.ts:51-54 function windVector
/** The wind on the map, world units a second along x and y. */
export function windVector(weather: Weather, unitsPerMetre: number): { x: number; y: number };
