// What the feature uses of src/render/streetFurnitureLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/streetFurnitureLayer.ts:25-25 variable STREET_TREE_MIN
/** DO NOT go back to that: one bench placed paid for every bench of the city. */

/** Street trees are pollarded smaller than a tree in open ground. */
const STREET_TREE_MIN = m(6.5);

// src/render/streetFurnitureLayer.ts:26-26 variable STREET_TREE_RANGE
const STREET_TREE_RANGE = m(3);

// src/render/streetFurnitureLayer.ts:34-34 variable FURNITURE_MODELS
/** The furniture's stores, in the order the scenery's meshes were made. */
const FURNITURE_MODELS = ['lamp', 'lens', 'pool', 'beam', 'bin', 'bench', 'hydrant', 'postbox', 'phone', 'drain', 'treePit'] as const;

// src/render/streetFurnitureLayer.ts:35-35 type FurnitureModel
type FurnitureModel = (typeof FURNITURE_MODELS)[number];

// src/render/streetFurnitureLayer.ts:36-36 variable MODEL
const MODEL = Object.fromEntries(FURNITURE_MODELS.map((model, i) => [model, i])) as Record<FurnitureModel, number>;

// src/render/streetFurnitureLayer.ts:39-51 interface Placed
/** One instance of a placed item: its store, where, how turned and sized, its tint. */
interface Placed {
  readonly store: number;
  readonly x: number;
  readonly y: number;
  readonly z: number;
  readonly yaw: number;
  readonly sx: number;
  readonly sy: number;
  readonly sz: number;
  readonly tint: Color | null;
  /** A plant: its full height, times its growth; `sx`, `sy`, `sz` are then times the height it has. 0: it does not grow. */
  readonly h: number;
}

// src/render/streetFurnitureLayer.ts:54-59 interface FurniturePlace
/** A placed item as it stands: its instances, where it is (its filing), when it was planted. */
interface FurniturePlace {
  readonly x: number;
  readonly y: number;
  readonly planted: number | undefined;
  readonly placed: readonly Placed[];
}

// src/render/streetFurnitureLayer.ts:164-164 interface LampLens
/** Where a lamp's lens stands on its post (`SceneryKit.lens`). */
export interface LampLens { readonly reach: number; readonly height: number }

// src/render/streetFurnitureLayer.ts:167-231 function furniturePlace
/** Where a placed item's instances stand, or undefined when it draws nothing. */
function furniturePlace(net: Network, elevation: RoadElevation, placed: LandscapeItem, lens: LampLens): FurniturePlace | undefined;
