// What the feature uses of src/render/citizenPlays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenPlays.ts:9-9 variable GPU_PLAYS
/** WHAT A COOKED BODY PLAYS, AS THE GPU'S PALETTE PASS BLENDS IT (`people/crowdAnimation.ts`), AND THE SAME ON THE CPU */

/** Plays the palette pass blends a person (`crowdAnimation.ts` PLAYS): more, and the person is blended on the CPU. */
export const GPU_PLAYS = 4;

// src/render/citizenPlays.ts:24-24 variable ATLAS_TEXELS
/** Texels a bone takes in an atlas row (twelve floats: `crowdAnimation.ts` ATLAS_TEXELS). */
const ATLAS_TEXELS = 3;

// src/render/citizenPlays.ts:26-26 variable ATLAS_SLACK
/** Room laid past the clips baked so far, a share of them: the clips baked on first use go in without laying the atlas again. */
const ATLAS_SLACK = 0.25;
