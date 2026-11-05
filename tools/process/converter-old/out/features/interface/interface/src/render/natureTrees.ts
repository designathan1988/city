// What the feature uses of src/render/natureTrees.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/natureTrees.ts:36-41 interface Variant
interface Variant {
  /** The model, one unit tall on its origin (the ground); the same at every distance. */
  readonly levels: readonly [BufferGeometry, BufferGeometry, BufferGeometry];
  /** Its foliage cards (`lowPolyTrees.ts`; a palm's frond strips), drawn with the same instances. */
  readonly cards: BufferGeometry | null;
}

// src/render/natureTrees.ts:43-53 interface NatureTreeKit
export interface NatureTreeKit {
  readonly variants: readonly Variant[];
  readonly material: MeshStandardMaterial;
  readonly depth: MeshDepthMaterial;
  readonly cardMaterial: MeshStandardMaterial;
  /** The cards' shadow pass (a palm's fronds only). */
  readonly cardDepth: MeshDepthMaterial;
  /** Each variety's far picture (`plantImpostors.ts`). */
  readonly pictures: PlantImpostors;
  dispose(): void;
}
