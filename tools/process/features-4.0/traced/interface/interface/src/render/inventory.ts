// What the feature uses of src/render/inventory.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/inventory.ts:6-26 interface InventoryRow
/** THE INVENTORY OF EVERYTHING THE SCENE HOLDS */

/** One row: a kind of mesh in a layer. */
interface InventoryRow {
  /** The layer (the scene's child the mesh hangs under, then its child: `world / road-network`). */
  readonly layer: string;
  /** The kind of mesh, its name without the tile or hash suffix. */
  readonly kind: string;
  readonly meshes: number;
  /** Meshes visible and in the scene, the ones a frame may draw. */
  readonly shown: number;
  /** The instances of the instanced ones (a plain mesh counts one). */
  readonly instances: number;
  /** Vertices of the geometries (shared geometries once). */
  readonly vertices: number;
  /** Triangles drawn when every instance is: indices / 3 times instances. */
  readonly triangles: number;
  /** Bytes of the buffers on the GPU (vertex, index, instance matrices and colours), a shared buffer once. */
  readonly gpuBytes: number;
  /** Of those, the bytes a JavaScript array still holds (`mesh/gpuOnly.ts` lets the rest go). */
  readonly cpuBytes: number;
  readonly materials: number;
  readonly textures: number;
}

// src/render/inventory.ts:29-32 interface InventoryFlag
/** A figure that says a row is out of proportion. */
interface InventoryFlag {
  readonly row: InventoryRow;
  readonly what: string;
}

// src/render/inventory.ts:34-38 interface Inventory
export interface Inventory {
  readonly rows: readonly InventoryRow[];
  readonly flags: readonly InventoryFlag[];
  readonly totals: { meshes: number; vertices: number; triangles: number; gpuBytes: number; cpuBytes: number };
}

// src/render/inventory.ts:66-160 function sceneInventory
/** Counts the scene under `root`: a row per layer and kind of mesh, the flags, the totals. */
export function sceneInventory(root: Object3D): Inventory;

// src/render/inventory.ts:163-177 function formatInventory
/** The inventory as a table, the `top` largest rows first, then the flags. */
export function formatInventory(inventory: Inventory, top = 40): string;
