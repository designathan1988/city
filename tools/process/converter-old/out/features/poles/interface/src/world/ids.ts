// What the feature uses of src/world/ids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ids.ts:8-8 type SegmentId
export type SegmentId = number & { readonly [SegmentIdBrand]: true };

// src/world/ids.ts:10-10 type PoleId
/** A utility pole: a mast carrying wires, and usually a lamp. */
export type PoleId = number & { readonly [PoleIdBrand]: true };

// src/world/ids.ts:12-12 type SpanId
/** A run of wire between two poles. */
export type SpanId = number & { readonly [SpanIdBrand]: true };
