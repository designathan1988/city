// What the feature uses of src/world/buildings/siteGrading.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/siteGrading.ts:13-13 type SiteBox
/** THE BUILDINGS' SITES AS THE GROUND WAS LAST GRADED TO THEM: each building's platform */

/** A world box: [minX, minY, maxX, maxY]. */
export type SiteBox = readonly [number, number, number, number];

// src/world/buildings/siteGrading.ts:15-30 interface SiteGrading
export interface SiteGrading {
  /** The town's platforms as last put together (`rebuild`), reused while a stroke is held. */
  readonly pads: TownPads | null;
  /** What changed among the buildings since the ground was graded: the box of each site added, removed or changed */
  changedSites(buildings: BuildingStore): SiteBox[] | null;
  /** A building's platform worked out again (the roads round it moved), on its floor as the floors keep it (follow them first). */
  refresh(b: Building): void;
  /** A building's platform as worked out elsewhere (the ground's worker, `groundShape.ts` `padsOfAsk`), on its floor as the floors keep it: as `refresh` would make it. */
  take(b: Building, pad: Pad | null): void;
  /** The platforms whose bank (`bankBox`, `reach` past the bounds) a change of what they read (the floor's ground, the paving) since the last `follow` reaches, forgotten (`ground`: the floors' changes); every one when the diary cannot say where. */
  follow(buildings: BuildingStore, ground: GroundSince, reach: number): void;
  /** The town's platforms put together from each building's (`pads`); null for a town with none. */
  rebuild(buildings: BuildingStore): void;
  /** The roads or the land moved: every platform is worked out afresh, and every site read again as graded. */
  forget(buildings: BuildingStore): void;
}

// src/world/buildings/siteGrading.ts:33-181 function siteGrading
/** `groundAt`: the ground a building stands on before its own grading; `pavedAt`: the footway's height; `apron` */
export function siteGrading(
  groundAt: GroundAt,
  pavedAt: PavedAt | undefined,
  apron: number,
  bankBox: (b: Building) => SiteBox,
  /** Each building's floor as kept for every reader (`floors.ts`), on `groundAt` and `pavedAt` */
  floors?: BuildingFloors,
): SiteGrading;
