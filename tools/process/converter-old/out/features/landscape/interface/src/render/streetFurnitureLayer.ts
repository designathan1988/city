// What the feature uses of src/render/streetFurnitureLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/streetFurnitureLayer.ts:25-25 variable STREET_TREE_MIN
/** DO NOT go back to that: one bench placed paid for every bench of the city. */

/** Street trees are pollarded smaller than a tree in open ground. */
const STREET_TREE_MIN = m(6.5);

// src/render/streetFurnitureLayer.ts:26-26 variable STREET_TREE_RANGE
const STREET_TREE_RANGE = m(3);

// src/render/streetFurnitureLayer.ts:29-29 variable ROAD_REACH
/** How far a placed item stands from what moves it: a road within reach of its footway and of the crossings near it. */
const ROAD_REACH = ACCESS_REACH + MAX_ITEM_RADIUS + m(1);

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

// src/render/streetFurnitureLayer.ts:62-67 interface FurnitureSource
/** What the layer is refreshed from: the diary it reads, and the roads and their heights it places on. */
export interface FurnitureSource {
  readonly changes: ChangeJournal;
  readonly net: Network;
  readonly elevation: RoadElevation;
  readonly groundAt: (x: number, y: number) => number;
}

// src/render/streetFurnitureLayer.ts:94-144 function furnitureStores
/** The meshes of the furniture and the street plants: the stores, and where each plant's body and cards are. */
export function furnitureStores(kit: SceneryKit, pictures: SceneryPictures): RunStore[];

// src/render/streetFurnitureLayer.ts:147-147 function treeStore
/** Each street plant's store (`furnitureStores`): the props', then the trees' by species, then the bushes'. */
const treeStore = (species: TreeSpecies): number => …;

// src/render/streetFurnitureLayer.ts:148-148 function bushStore
const bushStore = (kind: BushKind): number => …;

// src/render/streetFurnitureLayer.ts:157-161 function streetSpecies
/** Picks a species for a street tree: mostly planes, some flowering ipês. */
export function streetSpecies(roll: number): TreeSpecies;

// src/render/streetFurnitureLayer.ts:164-164 interface LampLens
/** Where a lamp's lens stands on its post (`SceneryKit.lens`). */
export interface LampLens { readonly reach: number; readonly height: number }

// src/render/streetFurnitureLayer.ts:167-231 function furniturePlace
/** Where a placed item's instances stand, or undefined when it draws nothing. */
function furniturePlace(net: Network, elevation: RoadElevation, placed: LandscapeItem, lens: LampLens): FurniturePlace | undefined;

// src/render/streetFurnitureLayer.ts:246-299 class PlacedProduct
/** Placed items of some kinds as a product of the diary, key by key (the item's id) */
class PlacedProduct<V extends { readonly x: number; readonly y: number }> implements DerivedSpec<FurnitureSource, number, V> { … }

// src/render/streetFurnitureLayer.ts:263-268 method PlacedProduct.allKeys
/** @rebuild The document's items of the product's kinds: read only when everything changed (a load, the diary forgotten). */
  allKeys(source: FurnitureSource): Iterable<number>;

// src/render/streetFurnitureLayer.ts:270-288 method PlacedProduct.dirty
dirty(change: Change, out: Set<number>, source: FurnitureSource): void;

// src/render/streetFurnitureLayer.ts:290-293 method PlacedProduct.compute
compute(key: number, source: FurnitureSource): V | undefined;

// src/render/streetFurnitureLayer.ts:302-306 interface MeadowPlace
/** A placed item's grass: its clump's tufts and flowers as one run. */
interface MeadowPlace {
  readonly x: number;
  readonly y: number;
  readonly run: Run;
}

// src/render/streetFurnitureLayer.ts:316-319 function MEADOW_STORES
const MEADOW_STORES = (kit: SceneryKit): RunStore[] => …;

// src/render/streetFurnitureLayer.ts:322-331 function meadowProduct
/** The long grass's product: each placed clump's run, laid on the ground as drawn. */
export function meadowProduct(kit: SceneryKit): Derived<FurnitureSource, number, MeadowPlace>;

// src/render/streetFurnitureLayer.ts:334-342 function meadowInto
/** A clump's tufts and flowers put in a run of the long grass's stores (`MEADOW_STORES`). */
function meadowInto(builder: RunBuilder, elevation: RoadElevation, groundAt: (x: number, y: number) => number, item: LandscapeItem): void;

// src/render/streetFurnitureLayer.ts:345-355 function placeInto
/** A placed item's instances at the city's time (the hour's start), put in a run of the furniture's stores: a plant sized by its growth then. */
function placeInto(builder: RunBuilder, place: FurniturePlace, now: number): void;

// src/render/streetFurnitureLayer.ts:358-374 interface StreetFurnitureLayer
/** The street furniture held round the view (`propCells.ts`), the long grass kept item by item. */
export interface StreetFurnitureLayer {
  /** The furniture and the street plants: their visibility is set every frame by the zoom. */
  readonly group: Group;
  readonly meshes: readonly InstancedMesh[];
  /** The furniture's cells, brought to the view with the gardens' (`lookProps`). */
  readonly layer: PropLayer;
  /** The long grass, drawn at every zoom (as it was). */
  readonly grass: Group;
  readonly triangles: number;
  /** Brings the furniture and the long grass up to the diary (and the hour): the furniture's cells an entry reached dropped, placed again as the view asks; the work done (the engine's ledger, `core/engine.ts`). */
  sync(net: Network, elevation: RoadElevation, groundAt: (x: number, y: number) => number, minutes: number): number;
  setMap(map: boolean): void;
  /** A mesh of each of its stores, as it draws them */
  warm(): InstancedMesh[];
  cull(frustum: Frustum, view: Matrix4): void;
  dispose(): void;
}

// src/render/streetFurnitureLayer.ts:377-378 function stillGrowing
/** Whether a plant planted then is still growing at `minutes` (unknown while the clock is). */
const stillGrowing = (planted: number | undefined, minutes: number): boolean => …;

// src/render/streetFurnitureLayer.ts:381-381 variable FURNITURE_SPREAD
/** How far from its item a placed thing may stand: a lamp's pool of light out over the widest road. */
const FURNITURE_SPREAD = m(40);

// src/render/streetFurnitureLayer.ts:384-384 function isFurniture
/** The items the furniture draws (not the long grass, the signs, the street names). */
const isFurniture = (item: LandscapeItem): boolean => …;

// src/render/streetFurnitureLayer.ts:387-398 function furnitureAbout
/** The furniture's things standing about a rectangle at the city's time `now` (the hour's start): each item near it placed (`furniturePlace`), a plant sized by its growth then. */
export function furnitureAbout(source: FurnitureSource, stores: readonly RunStore[], lens: LampLens, now: number, rect: ChangeRect): PropPlacement;

// src/render/streetFurnitureLayer.ts:400-470 function createStreetFurniture
export function createStreetFurniture(kit: SceneryKit): StreetFurnitureLayer;
