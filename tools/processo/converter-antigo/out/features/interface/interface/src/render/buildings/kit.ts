// What the feature uses of src/render/buildings/kit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/kit.ts:124-124 type Axes
/** Three lengths along a part model's own axes (x along the wall, y up, z out of it): metres once scaled by an instance's matrix. */
export type Axes = readonly [number, number, number];

// src/render/buildings/kit.ts:127-134 interface PartLevel
/** A part's geometry at one level of detail. */
export interface PartLevel {
  readonly geometry: BufferGeometry;
  readonly triangles: number;
  /** One face whose bars its material draws (`thinParts.ts` cards): it casts no shadow (its bars are under a texel where it is drawn). */
  readonly card?: true;
  /** What it leaves out of the finest, along the model's axes: its error, the largest axis once scaled. */
  readonly error: Axes;
}

// src/render/buildings/kit.ts:137-144 interface PartScreen
/** How a part kind is drawn by the error on the screen (`partCells.ts`). */
export interface PartScreen {
  /** Finest first, each leaving out more; the small props' simplified levels come in once the simplifier has loaded (`SIMPLIFIED`). */
  readonly levels: readonly PartLevel[];
  /** The model's box: an instance under a pixel across its diagonal is not drawn. */
  readonly extent: Axes;
  /** The pieces its shadow is made of (a frame's rails and stiles, a brise's blades): under a shadow texel across the middle side of each it casts none. */
  readonly pieces: readonly Axes[];
}

// src/render/buildings/kit.ts:146-179 interface BuildingKit
export interface BuildingKit {
  readonly geometry: Readonly<Record<PartKind, BufferGeometry>>;
  /** Each part kind's levels of detail, its size and its shadow's (`partCells.ts`). */
  readonly screen: Readonly<Record<PartKind, PartScreen>>;
  readonly material: Readonly<Record<PartKind, Material>>;
  /** The merged shell - walls, plinths, bands, roofs, steps - of every finish: one material, vertex coloured, each vertex's finish picking its maps (`finishes.ts`). */
  readonly shellMaterial: MeshStandardMaterial;
  /** The shell's parts where their finish's texture is under a pixel (`finishes.ts` createFarShellMaterial). */
  readonly farShell: MeshStandardMaterial;
  /** A broken building's pieces (`destruction.ts`): the shell's look, each vertex carrying its uv, colour and finish as its piece moves. */
  readonly ruinShell: MeshStandardMaterial;
  /** Ghost materials for the placement / drag preview, tinted by validity. */
  readonly ghostShell: MeshStandardMaterial;
  readonly ghostParts: MeshStandardMaterial;
  /** "Ocultar outros": the same shells, faded, for every building but the edited one. */
  readonly dimShell: MeshStandardMaterial;
  readonly dimParts: MeshStandardMaterial;
  /** Each finish's colour from afar (`finishes.ts` `finishAlbedo`), for the far level of the buildings. */
  readonly finishTone: Readonly<Record<Finish, readonly [number, number, number]>>;
  /** Buildings seen from afar, their windows drawn by the shader (`massing.ts`, `massingMaterial.ts`). */
  readonly massing: MeshStandardMaterial;
  /** The base's, the top level's (`massBase.ts`): each face shaded by its slope, a colour a triangle. */
  readonly massingBase: MeshStandardMaterial;
  /** The facade level (L1): the shells with their openings drawn by the shader, every finish (`facadeLevel.ts`). */
  readonly facade: MeshStandardMaterial;
  /** Parts that cast shadows; the small ones do not, to spare the shadow pass. */
  readonly castsShadow: ReadonlySet<PartKind>;
  /** The furniture models, built the first time an interior is drawn. */
  furniture(): { readonly geometry: Readonly<Record<FurnitureKind, BufferGeometry>>; readonly material: MeshStandardMaterial };
  setGhostValid(valid: boolean): void;
  /** Lights the windows from inside as night falls: 0 by day, 1 at night; `hour` */
  setNight(dark: number, hour?: number): void;
  dispose(): void;
}
