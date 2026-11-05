// What the feature uses of src/render/buildings/lightSlots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/lightSlots.ts:19-29 function spaceSeed
/** The seed of one space: building, floor, block, and the space's corner (to a decimetre): FNV-1a of "building:level:volume:x:y", hashed digit by digit, never written out (a pane's slot asked thousands of times a job). */
export function spaceSeed(building: number, level: number, volume: number, x: number, y: number): number;
