// What the feature uses of src/world/growth/zoning.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/growth/zoning.ts:32-34 interface GrowthSite
/** A site a building is grown on: what its checks read, and the id the building will take (its draft's). */
export interface GrowthSite extends SiteReads {
  readonly nextId: BuildingId;
}

// src/world/growth/zoning.ts:256-278 interface GrownChoice
/** WHAT A GROWN BUILDING IS (the city-game's rule: a building is a few numbers and a versioned generator, `LOT_PLAN_VERSION`): the choices `designOnLot` made for it on its lot, from which `replayOnLot` makes the very same record again without its neighbours, its roads or its paving. */
export interface GrownChoice {
  /** The stream at the start of the try that stood (`Rng.state`). */
  readonly state: RngState;
  /** Of the lot's densities (its own, then each lower) and of `TRIES`. */
  readonly density: number;
  readonly attempt: number;
  /** A street along a side (`cornerOf`): 0 none, 1 left, 2 right. */
  readonly corner: 0 | 1 | 2;
  /** Candidates made and the one kept (`unlikeness` against the buildings round it). */
  readonly made: number;
  readonly kept: number;
  /** The storeys beside the lot's front its envelope read (`neighbourStoreys`). */
  readonly neighbours: readonly number[];
  /** Its ground floor over the street (`streetFloor`), world units; NaN when its try read none. */
  readonly floor: number;
  /** 1 when its front was carried out to the street (`reachStreet`), 0 when the plan as laid stood. */
  readonly reach: 0 | 1;
  /** The boundary pieces left out against a neighbour's (`placed`), by their place in its elements. */
  readonly dropped: readonly number[];
  /** The stream at the start of the candidate kept and after the last candidate made: with them a replay makes the kept one alone, not every candidate to bring the stream there (`replayOnLot`); a choice without them (an older tile) is replayed candidate by candidate. */
  readonly keptState?: RngState;
  readonly afterState?: RngState;
}

// src/world/growth/zoning.ts:483-490 function growOnLot
/** Grows one building on a zoned lot without one (`world/lots.ts`), here on the page: the lot picked (`pickLot`), the building designed for it on the document's own site (`designOnLot`) and written (`commitGrown`); the lot set aside when nothing fits it. The city's growth and the zones' run the same steps with the design in a worker (`editor/growthFeed.ts`); this is the same growth in one call, for a probe and the specs. @perItem Runs once a building grown: nothing it reaches may walk the map. */
export function growOnLot(ctx: SiteContext, refused: Set<number>, seed: number, choice?: { value?: GrownChoice }): number | null;
