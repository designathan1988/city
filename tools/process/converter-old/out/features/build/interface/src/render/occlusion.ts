// What the feature uses of src/render/occlusion.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/occlusion.ts:25-34 interface OcclusionStats
export interface OcclusionStats {
  /** Volumes laid into the buffer this frame, and texels they covered. */
  occluders: number;
  texels: number;
  /** Boxes asked and found hidden this frame. */
  tested: number;
  hidden: number;
  /** ms the frame's buffer took to lay. */
  buildMs: number;
}

// src/render/occlusion.ts:57-87 method OcclusionBuffer.build
/** Lays this frame's occluders: the walls round the eye (`solids`), of the cells `drawn` says are drawn in their detail. `aspect`: the screen's width over its height; `near`: the camera's near plane. */
  build(camera: Camera, aspect: number, solids: CameraSolids, drawn: (x: number, y: number) => boolean, near: number): void;
