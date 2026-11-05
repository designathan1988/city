// What the feature uses of src/world/buildings/interior.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/interior.ts:13-18 variable FURNITURE_KINDS
/** What is inside a building, floor by floor */

export const FURNITURE_KINDS = [
  'bed', 'singleBed', 'sofa', 'armchair', 'table', 'chair', 'desk', 'officeChair', 'shelf', 'bookshelf', 'counter',
  'fridge', 'stove', 'sink', 'toilet', 'bath', 'wardrobe', 'tv', 'plant', 'pew', 'seat', 'wardBed', 'bars', 'screen',
  'altar', 'machine', 'rack', 'locker', 'atm', 'stage', 'barCounter', 'blackboard', 'checkout', 'treadmill', 'pallet',
  'ceilingLamp', 'floorLamp', 'tableLamp', 'stairs', 'stairwell',
] as const;

// src/world/buildings/interior.ts:19-19 type FurnitureKind
export type FurnitureKind = (typeof FURNITURE_KINDS)[number];
