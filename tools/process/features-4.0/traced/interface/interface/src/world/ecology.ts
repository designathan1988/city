// What the feature uses of src/world/ecology.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ecology.ts:7-7 type RegionId
/** The biome a map belongs to (`RoadDoc.nature`); a biome painted overrides it locally. */
export type RegionId = BiomeKind;

// src/world/ecology.ts:14-17 interface NatureSettings
/** A map's ecosystem: its biome and the seed its patches are laid with. Absent on maps made before it. */
export interface NatureSettings {
  readonly region: RegionId;
  readonly seed: number;
}

// src/world/ecology.ts:18-21 function isNatureSettings
export const isNatureSettings = (value: unknown): value is NatureSettings => …;

// src/world/ecology.ts:42-67 interface EcologyField
/** What grows at each corner, every value 0..1. */
export interface EcologyField {
  readonly side: number;
  /** Closed canopy: forest. */
  readonly canopy: Float32Array;
  /** Scattered trees of open country: the cerrado's twisted ones, the caatinga's. */
  readonly trees: Float32Array;
  /** Giants over the canopy (Amazon, Atlantic forest). */
  readonly emergent: Float32Array;
  readonly shrub: Float32Array;
  /** Palms: buriti in the veredas, açaí in the várzea, carnaúba in the caatinga, carandá in the pantanal. */
  readonly palm: Float32Array;
  readonly cactus: Float32Array;
  /** Herbs over the ground: 1 grass everywhere, 0 bare soil. */
  readonly grass: Float32Array;
  /** How dry and golden the herbs are in the dry season. */
  readonly dry: Float32Array;
  /** Waterlogged ground: vereda, brejo, várzea, flooded field. */
  readonly wet: Float32Array;
  /** Plants of the rocky fields: bromeliads, canela-de-ema, cacti on stone. */
  readonly rocky: Float32Array;
  /** The biome that holds most of each corner, as an index in `REGIONS` (which species grow). */
  readonly region: Uint8Array;
  /** What a later update reuses: the distance to water per corner and the lowest ground (`computeEcology`). */
  readonly waterDistance: Float32Array;
  readonly lowest: number;
}
