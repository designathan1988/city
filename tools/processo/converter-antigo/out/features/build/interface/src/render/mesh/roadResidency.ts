// What the feature uses of src/render/mesh/roadResidency.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/roadResidency.ts:76-100 interface RoadResidency
/**
 * THE ROADS' LEVELS UNDER BUDGETS (`quality.ts` GEOMETRY_BUDGETS roadsNearMB, roadsFarMB), REFINED BY REPLACEMENT (3D Tiles `REPLACE`):
 * the network's far level (`networkFarRoads.ts`, made from the network for the whole map, a few MB) stands for every road cell for good;
 * a cell's blocks' far levels (`attachFarLevel`, 0.12 m off the full one) replace it where the far budget holds them, and a block's full
 * level replaces its far one where it is wanted and the near budget holds it.
 *
 * Near: wanted within the reach and meeting the view's frustum (Cesium's traversal visits only the tiles in the frustum); past the
 * budget, the near blocks out of the view go first, the least recently seen first (3DTilesRendererJS `lruCache`), then the farthest in
 * it - never a hole, its far level stands. A block is brought near only where its bytes fit (as last measured, or its far level's times
 * `NEAR_PER_FAR` before it ever was near).
 *
 * Far: the cells ranked as the near blocks are (in the view first, the nearest first; then the most recently seen), the ones the budget
 * holds keep their blocks' far levels, the rest are let go a cell at a time (`dropCell`: every block of the cell, so the network's level
 * never lies under a block's) and the network's level shown there; a cell let go that the budget holds again is made again off the page
 * (`BlockLevels.remake`) and the network's level hidden only once every block of it is back (`restoreCell`). A cell with a block near,
 * coming near or whose far level is being made, or that the network's level does not cover, keeps its levels (Cesium's
 * `maximumCacheOverflowBytes`: what the view needs is never let go for the budget).
 */
export interface RoadResidency {
  /** The view's frustum (three's axes); null: every block is in view. */
  view?: Frustum | null;
  /** The most the full levels may hold, bytes. */
  budget?: number;
  /** The most the blocks' far levels may hold, bytes; none: every far level kept. */
  farBudget?: number;
  /** What draws a cell whose blocks' far levels were let go (the network's far level); none: none is let go. */
  network?: CellCover | null;
  /** The looks so far (`updateRoadLevels`): when each block last met the view (`BlockLevels.seenAt`). */
  looks?: number;
  /** The cells let go, the network's far level drawing them. */
  dropped?: Set<string>;
  /** The cells being made again (`restoreCell`). */
  restoring?: Set<string>;
  /** Each cell's far levels' bytes as last measured: what bringing a dropped one back costs. */
  cellBytes?: Map<string, number>;
  /** Set by a look over the far budget that found cells the network's level could not stand for (none, or made before the roads changed): the owner makes it again. */
  wantsNetwork?: boolean;
  /** From this distance (scene units) the network's far level draws a cell in place of its blocks, whatever the budget: the map's view
   *  (`renderer.ts`), where a block's paint and verges are under a pixel; none: the budget alone decides. */
  networkFrom?: number | undefined;
  /** The cells drawn by the network's level for the view (`networkFrom`): their blocks kept, only hidden, so coming back costs nothing. */
  parked?: Set<string>;
}

// src/render/mesh/roadResidency.ts:134-139 interface CellCover
/** The network's far level as the residency sees it (`networkFarRoads.ts`). */
export interface CellCover {
  /** Whether it draws the cell's roads as they are now (made, of the roads standing, not cut by an opening's view). */
  covers(key: string): boolean;
  show(key: string): void;
  hide(key: string): void;
}

// src/render/mesh/roadResidency.ts:157-170 interface Cell
/** A road cell as one look sees it. */
interface Cell {
  readonly key: string;
  readonly blocks: Mesh[];
  /** The nearest of its blocks, scene units. */
  d: number;
  seen: boolean;
  at: number;
  /** Its blocks' far levels' bytes now. */
  bytes: number;
  /** A block near, coming near or whose far level is being made: its levels stay. */
  held: boolean;
  /** Blocks whose far level stands (not dropped). */
  live: number;
}

// src/render/mesh/roadResidency.ts:192-217 function restoreCell
/** A dropped cell's blocks' far levels made again off the page, shown together in place of the network's level once all are back. */
function restoreCell(cell: Cell, residency: RoadResidency): void;
