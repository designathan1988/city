// What the feature uses of src/render/buildings/gpuBatchedMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/gpuBatchedMesh.ts:69-406 class GpuBatchedMesh
export class GpuBatchedMesh extends BatchedMesh { … }

// src/render/buildings/gpuBatchedMesh.ts:239-242 method GpuBatchedMesh.deleteGeometry
override deleteGeometry(geometryId: number): this;

// src/render/buildings/gpuBatchedMesh.ts:244-275 method GpuBatchedMesh.setGeometrySize
override setGeometrySize(maxVertexCount: number, maxIndexCount: number): void;

// src/render/buildings/gpuBatchedMesh.ts:280-284 method GpuBatchedMesh.usedCounts
/** The vertices and indices the live geometries reserve: what the batch holds once its holes are packed (`compact`). three counts its
   *  room from the end of the last geometry ever stored (r186 `unusedVertexCount`), so a deleted geometry's range stays taken: the
   *  facade batch of a large city grew from 121 MB to 373 MB touring its corners, every cell let go a hole no new one went into (2026-10-21). */
  usedCounts(): { readonly vertices: number; readonly indices: number };

// src/render/buildings/gpuBatchedMesh.ts:292-359 method GpuBatchedMesh.compact
/**
   * The live geometries packed to the front of storage of `maxVertexCount` vertices and `maxIndexCount` indices, the holes deleted
   * geometries left gone (three's `optimize`, done on the GPU): the vertices copied buffer to buffer (`copyBufferSubData`), the indices
   * read back once (`getBufferSubData`, WebGL 2.0 spec 5.14.3) and shifted by the distance their geometry moved, as three's own
   * `optimize` shifts its index array (r186) - WebGL 2 has no base vertex for the multi-draw, the indices hold their vertex's place.
   */
  compact(maxVertexCount: number, maxIndexCount: number): void;

// src/render/buildings/gpuBatchedMesh.ts:396-405 method GpuBatchedMesh.dispose
override dispose(): this;
