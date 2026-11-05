// What the feature uses of src/render/scenery.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/scenery.ts:131-172 interface SceneryKit
export interface SceneryKit {
  readonly trees: Record<TreeSpecies, BufferGeometry>;
  readonly bushes: Record<BushKind, BufferGeometry>;
  /** Each plant's foliage cards (`lowPolyTrees.ts`), drawn with the same instances; none on a conifer. */
  readonly cards: Partial<Record<TreeSpecies | BushKind, BufferGeometry>>;
  readonly treeCards: MeshStandardMaterial;
  readonly treeCardsDepth: MeshDepthMaterial;
  readonly shrubCards: MeshStandardMaterial;
  readonly shrubCardsDepth: MeshDepthMaterial;
  readonly furniture: Record<Exclude<FurnitureKind, 'streetTree' | 'shrub'>, BufferGeometry>;
  readonly lampLens: BufferGeometry;
  /** The lighting column drawn a dozen pixels tall or less (`lampFarGeometry`). */
  readonly lampFar: BufferGeometry;
  readonly treePit: BufferGeometry;
  readonly tuft: BufferGeometry;
  readonly flower: BufferGeometry;
  readonly foliage: MeshStandardMaterial;
  readonly foliageDepth: MeshDepthMaterial;
  readonly shrubs: MeshStandardMaterial;
  readonly shrubsDepth: MeshDepthMaterial;
  readonly grass: MeshStandardMaterial;
  readonly flowers: MeshStandardMaterial;
  readonly props: MeshStandardMaterial;
  readonly glow: MeshBasicMaterial;
  /** The warm pool of light under a street lamp, seen only after dark. */
  readonly pool: BufferGeometry;
  readonly poolGlow: MeshBasicMaterial;
  /** The cone of light from a street lamp's lens down to its pool, seen only after dark: a unit cone, apex */
  readonly beam: BufferGeometry;
  readonly beamGlow: ShaderMaterial;
  /** `glow`, `poolGlow` and `beamGlow` for the street furniture, drawn from its poses (`cellInstances.ts`): the poles draw the others with three's matrices. */
  readonly posedGlow: MeshBasicMaterial;
  readonly posedPoolGlow: MeshBasicMaterial;
  readonly posedBeamGlow: ShaderMaterial;
  /** Where a street lamp's lens is, from its column's foot: out over the road and up. */
  readonly lens: { readonly reach: number; readonly height: number };
  /** Lamps lit as night falls: 0 by day, 1 at night. */
  setNight(dark: number): void;
  /** How thick the air is, 0 clear to 1 rain or fog: a lamp's cone of light is seen only in air that scatters it. */
  setHaze(haze: number): void;
  dispose(): void;
}

// src/render/scenery.ts:370-370 variable PLANT_MAP_ZOOM
/** Below this zoom the plants' leaf cards are left out (`Scenery.setMap`). */
export const PLANT_MAP_ZOOM = perM(2.5);
