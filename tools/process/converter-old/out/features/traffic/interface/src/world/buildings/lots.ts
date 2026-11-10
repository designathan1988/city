// What the feature uses of src/world/buildings/lots.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/lots.ts:18-25 type LotSlope
/** A lot's surface as plain numbers, copied to a worker as they are */
export type LotSlope =
  | { readonly kind: 'level'; readonly level: number }
  | { readonly kind: 'ramp'; readonly frame: BuildingFrame; readonly from: number; readonly level: number; readonly y0: number; readonly d: number }
  | {
    readonly kind: 'street'; readonly frame: BuildingFrame; readonly side: number;
    readonly x0: number; readonly y0: number; readonly w: number; readonly d: number;
    readonly ha: number; readonly hb: number; readonly floor: number;
  };
