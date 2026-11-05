// What the feature uses of src/render/people/faceExpression.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/faceExpression.ts:4-4 type FaceWeights
/** A person's face this instant. */
export type FaceWeights = Record<string, number>;

// src/render/people/faceExpression.ts:7-20 variable CHANNELS
/** Each channel as ARKit shapes. */
export const CHANNELS: Record<string, FaceWeights> = {
  faceBlink: { eyeBlinkLeft: 1, eyeBlinkRight: 1 },
  faceJoy: { cheekSquintLeft: 0.55, cheekSquintRight: 0.55, mouthSmileLeft: 0.75, mouthSmileRight: 0.75, eyeSquintLeft: 0.2, eyeSquintRight: 0.2 },
  faceSadness: { browInnerUp: 0.7, browDownLeft: 0.25, browDownRight: 0.25, mouthFrownLeft: 0.55, mouthFrownRight: 0.55, mouthShrugLower: 0.2 },
  faceAnger: { browDownLeft: 0.85, browDownRight: 0.85, eyeSquintLeft: 0.35, eyeSquintRight: 0.35, eyeWideLeft: 0.15, eyeWideRight: 0.15,
    mouthPressLeft: 0.5, mouthPressRight: 0.5, noseSneerLeft: 0.2, noseSneerRight: 0.2 },
  faceSurprise: { browInnerUp: 0.75, browOuterUpLeft: 0.75, browOuterUpRight: 0.75, eyeWideLeft: 0.6, eyeWideRight: 0.6, jawOpen: 0.3 },
  faceBrowRaise: { browInnerUp: 0.6, browOuterUpLeft: 0.5, browOuterUpRight: 0.5 },
  mouthOpen: { jawOpen: 0.5 }, // aa, DD, nn, kk
  mouthRound: { jawOpen: 0.3, mouthFunnel: 0.55 }, // O
  mouthPuckered: { jawOpen: 0.08, mouthPucker: 0.7 }, // U
  mouthSpread: { jawOpen: 0.15, mouthStretchLeft: 0.35, mouthStretchRight: 0.35, mouthSmileLeft: 0.1, mouthSmileRight: 0.1 }, // E, I, SS
  mouthShut: { mouthClose: 0.5, mouthPressLeft: 0.35, mouthPressRight: 0.35, mouthRollLower: 0.2 }, // PP, FF
};

// src/render/people/faceExpression.ts:22-22 variable GAZE
/** The eyeballs' turn, a channel of its own (`facialMorphs.ts`). */
const GAZE = ['lookLeft', 'lookRight'];

// src/render/people/faceExpression.ts:57-119 function faceAt
/** The face of person `seed` at `time` (s), doing `activity`, in a mood from -1 (low) to 1 (bright): channel weights. */
export function faceAt(seed: number, time: number, activity?: string, mood = 0): FaceWeights;
