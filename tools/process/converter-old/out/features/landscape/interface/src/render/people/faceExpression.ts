// What the feature uses of src/render/people/faceExpression.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/faceExpression.ts:4-4 type FaceWeights
/** A person's face this instant. */
export type FaceWeights = Record<string, number>;

// src/render/people/faceExpression.ts:57-119 function faceAt
/** The face of person `seed` at `time` (s), doing `activity`, in a mood from -1 (low) to 1 (bright): channel weights. */
export function faceAt(seed: number, time: number, activity?: string, mood = 0): FaceWeights;
