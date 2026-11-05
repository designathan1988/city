// What the feature uses of src/world/ids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ids.ts:7-7 type NodeId
export type NodeId = number & { readonly [NodeIdBrand]: true };

// src/world/ids.ts:8-8 type SegmentId
export type SegmentId = number & { readonly [SegmentIdBrand]: true };
