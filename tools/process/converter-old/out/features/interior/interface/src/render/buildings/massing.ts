// What the feature uses of src/render/buildings/massing.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/massing.ts:35-40 interface MassDetail
/** WHAT A LEVEL OF THE MASSES LEAVES OUT: the cell's (all of it), a tile's (L2) and a plate's (L3, `massLevels.ts`) */
export interface MassDetail {
  /** The tallest thing left out or laid flat, m: a relief and its cheeks, a parapet, a box on the roof, a machine room. */
  readonly drop: number;
  /** Each block a box: its pitched roof under `drop` laid flat at its eaves (L3). */
  readonly boxes: boolean;
}

// src/render/buildings/massing.ts:43-43 variable TILE_DETAIL
/** A tile's: under 3 m (a machine room, `MACHINE_ROOM_H`; a parapet; an arcade's set-back, `ARCADE`). */
export const TILE_DETAIL: MassDetail = { drop: m(3), boxes: false };

// src/render/buildings/massing.ts:45-45 variable PLATE_DETAIL
/** A plate's: under 8 m, the pitched roofs of houses and low blocks flattened too. */
export const PLATE_DETAIL: MassDetail = { drop: m(8), boxes: true };
