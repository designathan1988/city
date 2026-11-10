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

// src/world/buildings/interior.ts:21-32 interface Furniture
export interface Furniture {
  readonly kind: FurnitureKind;
  /** Centre, local frame. */
  readonly x: number;
  readonly y: number;
  /** Size across (along the facing's normal it is `d`). */
  readonly w: number;
  readonly d: number;
  readonly h: number;
  /** The way it faces, radians in the local frame (0 faces -y, the front). */
  readonly angle: number;
}

// src/world/buildings/interior.ts:35-40 interface Partition
/** A thin wall between rooms, from (x0,y0) to (x1,y1). */
interface Partition {
  readonly x0: number;
  readonly y0: number;
  readonly x1: number;
  readonly y1: number;
}

// src/world/buildings/interior.ts:42-46 interface FloorInterior
export interface FloorInterior {
  readonly level: number;
  readonly furniture: readonly Furniture[];
  readonly partitions: readonly Partition[];
}

// src/world/buildings/interior.ts:194-207 function interiorAt
/** One floor of a building, inside: furnished once for its blocks, the rest it reads and its type (the records sharing them share it; a building emitted, its windows' rooms and the cut-open view alike). */
export function interiorAt(b: Building, level: number): FloorInterior;
