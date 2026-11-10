// What the feature uses of src/editor/roadParking.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roadParking.ts:4-4 variable ROAD_PARKING_PRESETS
/** The parking a new road is drawn with, chosen in the road tool's options as Cities: Skylines II offers road variants */
export const ROAD_PARKING_PRESETS = ['none', 'parallel', 'parallelRight', 'parallelLeft', 'cycle', 'cycleParking'] as const;

// src/editor/roadParking.ts:5-5 type RoadParkingPreset
export type RoadParkingPreset = (typeof ROAD_PARKING_PRESETS)[number];

// src/editor/roadParking.ts:20-27 function setRoadParkingPreset
export function setRoadParkingPreset(next: RoadParkingPreset): void;

// src/editor/roadParking.ts:30-40 function roadParking
/** The parking of a road drawn with the current preset, or undefined for none. */
export function roadParking(): SegmentParking | undefined;
