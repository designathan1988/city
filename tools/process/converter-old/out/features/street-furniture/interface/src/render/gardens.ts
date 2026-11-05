// What the feature uses of src/render/gardens.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gardens.ts:19-35 interface GardenPlant
/** DO NOT go back to rebuilding the whole town's gardens at every building. */

/** A plant of a building's garden, in the world: a tree, a shrub, a run of hedge or a flower bed. */
export interface GardenPlant {
  readonly kind: 'tree' | 'shrub' | 'hedge' | 'flowers';
  readonly x: number;
  readonly y: number;
  /** Ground height under it. */
  readonly z: number;
  /** Size, world units: along (w), across (d) and height (h); `yaw` the way `w` runs. */
  readonly w: number;
  readonly d: number;
  readonly h: number;
  readonly yaw: number;
  readonly seed: number;
  /** A tree's species, when it is not a garden's mix (a painted forest's). */
  readonly species?: TreeSpecies;
  /** A shrub that never flowers (a wood's understorey). */
  readonly plain?: boolean;
}

// src/render/gardens.ts:57-57 interface Placement
interface Placement { x: number; y: number; z: number; yaw: number; sx: number; sy: number; sz: number; tint?: Color }

// src/render/gardens.ts:60-65 function gardenSpecies
/** A garden tree's species: mostly broadleaf, some conifers, now and then a flowering ipê. */
function gardenSpecies(roll: number): TreeSpecies;

// src/render/gardens.ts:71-71 variable KIND_KEYS
/** The kinds of plant, in the order of the gardens' kinds: the trees by species, then the bushes. */
const KIND_KEYS = [...TREE_SPECIES.map((s) => `tree:${s}`), ...BUSH_KINDS.map((k) => `bush:${k}`)];

// src/render/gardens.ts:72-72 variable KIND_OF
const KIND_OF = new Map(KIND_KEYS.map((key, i) => [key, i]));

// src/render/gardens.ts:75-154 function gardenPack
/** One garden's plants placed, by plant kind (`KIND_KEYS`): each one's pose (DO NOT keep 16-float matrices of every plant of the town), tint and largest scale. */
export function gardenPack(list: readonly GardenPlant[]): ReadonlyMap<number, Pack>;

// src/render/gardens.ts:168-186 function gardenKinds
/** The gardens' kinds (`KIND_KEYS`): each plant's body and leaf cards near, its picture far, chosen by its size on the screen. */
function gardenKinds(kit: SceneryKit, pictures: SceneryPictures): KindSpec[];

// src/render/gardens.ts:171-181 function gardenKinds.kind
/** A plant kind: its body and its leaf cards near, its picture far (`plantImpostors.ts`), chosen by its size on the screen. */
  const kind = (name: string, body: BufferGeometry, material: Material, depth: MeshDepthMaterial, cards: BufferGeometry | undefined,
    cardMaterial: Material, cardDepth: MeshDepthMaterial, picture: number): KindSpec => …;

// src/render/gardens.ts:189-189 function gardenPictures
/** Each garden kind's picture among the scenery's (`KIND_KEYS` order). */
const gardenPictures = (pictures: SceneryPictures): number[] => …;

// src/render/gardens.ts:220-227 function gardenPacksAbout
/** The plants of the gardens standing about a rectangle (at least every one in it): the buildings whose bounds (their elements' too) meet it, each one's garden placed. */
export function gardenPacksAbout(buildings: BuildingStore, groundAt: (x: number, y: number) => number, rect: readonly [number, number, number, number]): ReadonlyMap<number, Pack>;

// src/render/gardens.ts:230-230 variable GARDEN_SLACK
/** How far past a building's bounds its garden's placings may stand: a hedge's clipped piece, a flower bed's clump (both within its element, rounded). */
const GARDEN_SLACK = m(2);

// src/render/gardens.ts:233-244 interface GardenCells
/** THE GARDENS HELD ROUND THE VIEW (`propCells.ts`): placed cell by cell from the buildings of the document, near whole, far as their pictures. */
export interface GardenCells {
  readonly layer: PropLayer;
  readonly meshes: readonly InstancedMesh[];
  readonly triangles: number;
  /** The buildings and the ground the gardens are placed from: the cells placed from another store all dropped. */
  source(buildings: BuildingStore, groundAt: (x: number, y: number) => number): void;
  /** The diary's changes of the buildings and the ground since the last look (when the ground is settled: a stroke over). */
  follow(journal: ChangeJournal): void;
  setMap(map: boolean): void;
  cull(frustum: Frustum, view: Matrix4): void;
  dispose(): void;
}

// src/render/gardens.ts:246-274 function createGardenCells
export function createGardenCells(kit: SceneryKit): GardenCells;
