// What the feature uses of src/world/ids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ids.ts:2-2 variable NodeIdBrand
/** Branded id types. */
declare const NodeIdBrand: unique symbol;

// src/world/ids.ts:3-3 variable SegmentIdBrand
declare const SegmentIdBrand: unique symbol;

// src/world/ids.ts:4-4 variable PoleIdBrand
declare const PoleIdBrand: unique symbol;

// src/world/ids.ts:7-7 type NodeId
export type NodeId = number & { readonly [NodeIdBrand]: true };

// src/world/ids.ts:8-8 type SegmentId
export type SegmentId = number & { readonly [SegmentIdBrand]: true };

// src/world/ids.ts:10-10 type PoleId
/** A utility pole: a mast carrying wires, and usually a lamp. */
export type PoleId = number & { readonly [PoleIdBrand]: true };
