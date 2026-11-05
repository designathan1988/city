// What the feature uses of src/render/plantImpostors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/plantImpostors.ts:38-42 interface ImpostorModel
/**
 * THE NEAR PLANTS' FAR PICTURES: each tree and bush model taken once from 8 x 8 directions over the
 * upper hemi-octahedron (Ryan Brucks, "Octahedral Impostors", shaderbits.com, 2018; the far woods'
 * own pictures, `farForest.ts`), drawn as one quad an instance where the model would be a few pixels.
 * Under `THIN_PX` the pictures are thinned and the ones kept grown to keep the cover (Cook, Halstead,
 * Planck, Ryu, "Stochastic Simplification of Aggregate Detail", SIGGRAPH 2007): no plant is drawn
 * smaller than a couple of pixels, none thrown into the shadow map under a texel.
 */

/** A model to take pictures of: its body, its foliage cards, and whether the cards throw its shadow (a palm's fronds). */
export interface ImpostorModel {
  readonly body: BufferGeometry;
  readonly cards: BufferGeometry | null;
  readonly cardsCast: boolean;
}

// src/render/plantImpostors.ts:255-274 interface PlantImpostors
export interface PlantImpostors {
  /** Each model's quad: four corners, its sphere and its block on the atlas in their attributes. */
  readonly quads: readonly BufferGeometry[];
  /** The atlas's colour and normal pictures, its blocks across and down, and each model's sphere (one unit tall): what the far woods draw from too (`farForest.ts`). */
  readonly albedo: Texture;
  readonly normals: Texture;
  readonly blocks: { readonly x: number; readonly y: number };
  readonly spheres: readonly { readonly centre: Vector3; readonly radius: number }[];
  /** Each model's sphere radius, one unit tall: the size a plant's level is chosen by (`cellInstances.ts`). */
  readonly reach: readonly number[];
  readonly material: MeshStandardMaterial;
  readonly depth: MeshDepthMaterial;
  /** The view the instances were culled for and half the screen's height, device pixels: what the pictures thin by. */
  setView(view: Matrix4, halfHeight: number): void;
  /** Whether the pictures are taken (`takeWaitingPictures`): until then the plants are drawn whole. */
  ready(): boolean;
  /** A mesh drawn with the pictures: the shadow map's size read before its shadow. */
  attach(mesh: InstancedMesh): void;
  dispose(): void;
}

// src/render/plantImpostors.ts:285-461 function createPlantImpostors
/** The pictures of these models, taken at the next `takeWaitingPictures`; `key` names their programs; `ownModels`: the models' geometries let go once taken. */
export function createPlantImpostors(models: readonly ImpostorModel[], wind: WindResponse, key: string, ownModels = false): PlantImpostors;

// src/render/plantImpostors.ts:439-442 method createPlantImpostors.setView
setView(view, halfHeight);

// src/render/plantImpostors.ts:443-443 method createPlantImpostors.ready
ready: () => …;

// src/render/plantImpostors.ts:444-451 method createPlantImpostors.attach
attach(mesh);

// src/render/plantImpostors.ts:464-464 variable PICTURE_WIND
/** The trees' wind on their pictures (`scenery.ts` TREE_WIND, `natureTrees.ts` FOREST_WIND: the same breeze). */
export const PICTURE_WIND: WindResponse = { sway: 0.045, flutter: 0.009 };

// src/render/plantImpostors.ts:467-467 variable PICTURE_PX
/** Device pixels across under which a plant is drawn as its picture (the frames are `FRAME_PX` wide: never magnified past it). */
export const PICTURE_PX = 48;

// src/render/plantImpostors.ts:470-476 interface SceneryPictures
/** The gardens' and the street's plants' pictures: the scenery's models, its trees then its bushes. */
export interface SceneryPictures {
  readonly set: PlantImpostors;
  treeAt(species: TreeSpecies): number;
  bushAt(kind: BushKind): number;
  /** Let go by a layer: the pictures go with the last one. */
  release(): void;
}

// src/render/plantImpostors.ts:479-483 interface PlantModels
/** The plants of the scenery's kit the pictures are made of (`scenery.ts` SceneryKit): only these, so this file does not import the kit, which imports the poses that import the pictures. */
interface PlantModels {
  readonly trees: Readonly<Record<TreeSpecies, BufferGeometry>>;
  readonly bushes: Readonly<Record<BushKind, BufferGeometry>>;
  readonly cards: Readonly<Partial<Record<TreeSpecies | BushKind, BufferGeometry>>>;
}

// src/render/plantImpostors.ts:485-485 variable sceneryShared
const sceneryShared = new WeakMap<PlantModels, { set: PlantImpostors; users: number }>();

// src/render/plantImpostors.ts:487-511 function sceneryPictures
/** One set of pictures for the gardens and the street (`gardens.ts`, `streetFurnitureLayer.ts`): one atlas, one program. */
export function sceneryPictures(kit: PlantModels): SceneryPictures;

// src/render/plantImpostors.ts:501-501 method sceneryPictures.treeAt
treeAt: (species) => …;

// src/render/plantImpostors.ts:502-502 method sceneryPictures.bushAt
bushAt: (kind) => …;

// src/render/plantImpostors.ts:503-509 method sceneryPictures.release
release();
