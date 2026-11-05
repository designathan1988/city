// What the feature uses of src/render/gardens.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gardens.ts:171-181 function gardenKinds.kind
/** A plant kind: its body and its leaf cards near, its picture far (`plantImpostors.ts`), chosen by its size on the screen. */
  const kind = (name: string, body: BufferGeometry, material: Material, depth: MeshDepthMaterial, cards: BufferGeometry | undefined,
    cardMaterial: Material, cardDepth: MeshDepthMaterial, picture: number): KindSpec => …;

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
