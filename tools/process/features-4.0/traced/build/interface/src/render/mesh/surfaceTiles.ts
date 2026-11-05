// What the feature uses of src/render/mesh/surfaceTiles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/surfaceTiles.ts:55-89 function mergeTiles
/** DO NOT pass a source that returns `parts`: the closure would hold every tile's arrays for as long as the mesh lives. */
export function mergeTiles(parts: readonly Tile[], options: Pick<SurfaceMeshOptions,
  'name' | 'material' | 'castShadow' | 'receiveShadow'>, source: () => readonly Tile[],
  far?: { readonly lock: readonly [number, number, number, number]; readonly near: boolean; readonly start?: (mesh: Mesh, arrays: BlockArrays) => void;
    /** The other surfaces' blocks' boxes, for a block of several (`BlockLevels.also`). */
    readonly also?: readonly (readonly [number, number, number, number])[];
    /** The road block's cell (`BlockLevels.cell`). */
    readonly cell?: string }): Mesh | null;
