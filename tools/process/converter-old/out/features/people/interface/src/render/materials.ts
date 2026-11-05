// What the feature uses of src/render/materials.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/materials.ts:11-33 interface SceneMaterials
/** Every material the scene uses, its surfaces baked at build time (`bake/roadRecipes.ts`) and shared. */

export interface SceneMaterials {
  readonly asphalt: MeshStandardMaterial;
  readonly asphaltRaised: MeshStandardMaterial;
  readonly footway: MeshStandardMaterial;
  readonly kerb: MeshStandardMaterial;
  readonly verge: MeshStandardMaterial;
  readonly deck: MeshStandardMaterial;
  /** The deck's concrete again, for the parapets. */
  readonly parapet: MeshStandardMaterial;
  readonly concrete: MeshStandardMaterial;
  readonly steel: MeshStandardMaterial;
  /** World units one tile of each surface covers, for UV generation. */
  readonly scale: {
    readonly asphalt: number;
    readonly footway: number;
    readonly kerb: number;
    readonly verge: number;
    readonly deck: number;
  };
  /** Switches the close-zoom detail layer on every material that has one. */
  setDetail(enabled: boolean): void;
  dispose(): void;
}
