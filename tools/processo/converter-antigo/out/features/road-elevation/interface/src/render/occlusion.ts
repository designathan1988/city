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

// src/render/occlusion.ts:46-46 field OcclusionBuffer.near
private near = 0.1;
