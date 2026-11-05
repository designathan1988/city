// What the feature uses of src/world/doc.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/doc.ts:153-2888 class RoadDoc
export class RoadDoc { … }

// src/world/doc.ts:289-289 field RoadDoc.buildings
/** Modular buildings. */
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:321-323 method RoadDoc.lotsIn
/** The lots whose box meets a rectangle (a superset: test each), in increasing id order. */
  lotsIn(rect: ChangeRect): Lot[];

// src/world/doc.ts:450-452 method RoadDoc.polesIn
/** The poles standing in the cells a rectangle reaches (`PoleIndex.near`): a superset, test each. */
  polesIn(rect: ChangeRect): UtilityPole[];

// src/world/doc.ts:521-527 method RoadDoc.removeBarrier
removeBarrier(id: number): boolean;

// src/world/doc.ts:535-537 method RoadDoc.barriersIn
barriersIn(rect: ChangeRect): Barrier[];

// src/world/doc.ts:596-602 method RoadDoc.removeLandscape
removeLandscape(id: number): boolean;

// src/world/doc.ts:605-607 method RoadDoc.landscapeIn
/** The street furniture standing in the cells a rectangle reaches (a superset: test each), in increasing id order. */
  landscapeIn(rect: ChangeRect): LandscapeItem[];

// src/world/doc.ts:654-668 method RoadDoc.removePole
/** Removes a pole and every wire that reached it. */
  removePole(id: PoleId): void;

// src/world/doc.ts:671-684 method RoadDoc.poleNear
/** The pole nearest a point, within `radius`, or null. */
  poleNear(at: { x: number; y: number }, radius: number): UtilityPole | null;

// src/world/doc.ts:767-785 method RoadDoc.removeSegment
removeSegment(id: SegmentId): void;

// src/world/doc.ts:1386-1402 method RoadDoc.pruneOrphanNodes
/** DO NOT walk every node of the map for it: every road taken away ends here. */
  pruneOrphanNodes(): number;
