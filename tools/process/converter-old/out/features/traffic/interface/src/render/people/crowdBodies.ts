// What the feature uses of src/render/people/crowdBodies.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/crowdBodies.ts:7-7 variable BODIES
/** The bodies of a class baked once (`proceduralCrowd.ts`) */
const BODIES = 16;

// src/render/people/crowdBodies.ts:9-9 variable BODY_WIDTH
/** Texels across the bodies texture (the shader's divisor). */
export const BODY_WIDTH = 4096;

// src/render/people/crowdBodies.ts:10-10 variable WIDTH
const WIDTH = BODY_WIDTH;

// src/render/people/crowdBodies.ts:12-29 interface ClassBodies
export interface ClassBodies {
  readonly texture: Texture;
  /** The shape basis the bodies are baked from, the only copy kept (shared with the workers where memory can be shared). */
  readonly basis: Float32Array;
  readonly joints: DataTexture;
  /** Texels between the first vertices of two bodies. */
  readonly stride: number;
  readonly count: number;
  /** The bodies' bytes on the GPU. */
  readonly bytes: number;
  /** The body to draw a person of these coefficients with (one made if room is left), its joints, and when its vertices are written (a new body's baked in a worker). */
  pick(coefficients: Float32Array): { index: number; joints: Float32Array; ready: Promise<void> };
  /** The bodies made since the last frame sent up: before anybody draws them. */
  flush(renderer: WebGLRenderer): void;
  /** Every body made baked and sent up again (a WebGL context restored). */
  restore(): void;
  dispose(): void;
}
