// What the feature uses of src/peopleLab/groom.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/groom.ts:8-17 interface StudioBody
/** A person's body for the editors that work on it (the hair builder, the atelier): positions (dm), quads, the skin's vertices, the head and neck's. */
export interface StudioBody {
  readonly positions: Float32Array;
  readonly faces: Uint16Array;
  readonly faceGroup: Uint8Array;
  readonly skinGroup: number;
  /** The skin's vertices (the body's own range). */
  readonly skin: readonly number[];
  /** Head and neck skin vertices: what worn hair is pinned to. */
  readonly anchors: readonly number[];
}

// src/peopleLab/groom.ts:20-251 class GroomBody
/** The body the hair lives on: its positions and normals (dm), the skin's vertices on a grid for collision, the scalp. */
export class GroomBody { … }
