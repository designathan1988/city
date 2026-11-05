// What the feature uses of src/render/groundCover.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundCover.ts:19-31 interface CoverPlacement
/** What stands on the open ground: rocks and boulders */

export interface CoverPlacement {
  readonly x: number;
  readonly y: number;
  /** Ground height under it. */
  readonly z: number;
  /** Across, world units. */
  readonly size: number;
  readonly yaw: number;
  /** 0..1: which variant, its tilt, its tint. */
  readonly seed: number;
  /** A stone: the rock of the land it lies on (granite when absent). */
  readonly rock?: GeologyKind;
}

// src/render/groundCover.ts:34-34 type ForestSpecies
/** The trees a painted forest grows. */
export type ForestSpecies = 'broadleaf' | 'broadleafTall' | 'conifer' | 'ipeYellow' | 'ipePink';

// src/render/groundCover.ts:38-40 interface TreePlacement
/** A tree: `size` is its height. */
export interface TreePlacement extends CoverPlacement {
  readonly species: ForestSpecies;
}

// src/render/groundCover.ts:43-46 interface TreeModel
/** One tree model: its body and its foliage cards (none on a conifer). */
export interface TreeModel {
  readonly body: BufferGeometry;
  readonly cards: BufferGeometry | null;
}

// src/render/groundCover.ts:48-60 interface GroundCoverKit
export interface GroundCoverKit {
  readonly rocks: readonly BufferGeometry[];
  readonly scrub: readonly BufferGeometry[];
  /** Per species, its variants; one unit tall, the crown some 0.6 across. */
  readonly trees: Readonly<Record<ForestSpecies, readonly TreeModel[]>>;
  readonly rockMaterial: MeshStandardMaterial;
  readonly scrubMaterial: MeshStandardMaterial;
  readonly treeMaterial: MeshStandardMaterial;
  readonly treeDepth: MeshDepthMaterial;
  /** The foliage cards' material (`lowPolyTrees.ts` leafCardMaterials). */
  readonly cardMaterial: MeshStandardMaterial;
  dispose(): void;
}
