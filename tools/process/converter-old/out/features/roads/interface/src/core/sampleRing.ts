// What the feature uses of src/core/sampleRing.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/sampleRing.ts:2-56 class SampleRing
/** The last `capacity` samples of a measure (ms), in a ring: nothing allocated per sample, so it can be fed every frame. */
export class SampleRing { … }

// src/core/sampleRing.ts:69-69 variable STUTTER_MS
/** Two periods of a 59 Hz display: a frame that took longer was seen as a stutter (NVIDIA, "Analysing Stutter"). */
export const STUTTER_MS = 2 * (1000 / 59);
