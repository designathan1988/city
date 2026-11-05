// What the feature uses of src/render/scenery.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/scenery.ts:61-61 variable LAMP_POOL_RADIUS
/** Radius of the bright core under a street lamp (the cone's foot). */
export const LAMP_POOL_RADIUS = m(4.6);

// src/render/scenery.ts:63-63 variable POOL_ALONG
/** The light a street lamp lays on the road: not a spotlight's disc but the long patch of a road luminaire's spread */
export const POOL_ALONG = 1.6;

// src/render/scenery.ts:66-66 variable LAMP_BEAM_RADIUS
/** A lamp's visible cone of light, drawn as Volumetric Light Beam draws one (saladgamer.com/vlb-doc/comp-lightbeam-sd/) */
/** The cone's radius at the ground: the bright core of the pool. */
export const LAMP_BEAM_RADIUS = LAMP_POOL_RADIUS * 0.8;

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

// src/render/scenery.ts:175-312 function createSceneryKit
/** Builds every model and material once, at renderer start. */
export function createSceneryKit(): SceneryKit;

// src/render/scenery.ts:315-344 function plantSamples
/** One of every plant as the gardens and the street draw them, with and without a tint of their own */
export function plantSamples(kit: SceneryKit): { readonly group: Group; readonly depth: Group; dispose(): void };

// src/render/scenery.ts:363-363 variable SHADOW_REACH
/** How far outside the view an instance is still drawn, world units */
export const SHADOW_REACH = m(28);

// src/render/scenery.ts:373-376 function foliageTint
/** A near-white multiplier, so no two plants of one species are identical. */
export function foliageTint(rng: Rng): Color;
