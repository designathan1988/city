// What the feature uses of src/render/elements.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/elements.ts:51-54 interface Part
/** One part of a model: its geometry (one unit across, standing on y = 0) and its material. */
interface Part {
  readonly geometry: BufferGeometry;
  readonly material: MeshStandardMaterial;
}

// src/render/elements.ts:168-172 interface ElementKit
export interface ElementKit {
  /** The parts of each prop kind's variants. */
  readonly models: ReadonlyMap<ElementKind, readonly Part[][]>;
  dispose(): void;
}

// src/render/elements.ts:174-227 function loadElementKit
export async function loadElementKit(anisotropy: number): Promise<ElementKit>;
