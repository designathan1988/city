// What the feature uses of src/render/people/proceduralBake.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/proceduralBake.ts:19-19 variable SHAPES
/** What the procedural crowd (`proceduralCrowd.ts`) is cooked from (`npm run cook:people`, `proceduralCook.ts`) */

/** Principal components carried per body: 16 keep a body within about 1.5 cm of the full model. */
export const SHAPES = 16;

// src/render/people/proceduralBake.ts:22-22 variable FACE_WIDTH
/** Rows of `procFace` (the vertex-to-face index four a texel, the expressions, each person's face) are this wide; the index's own rows this wide. */
export const FACE_WIDTH = 2048;

// src/render/people/proceduralBake.ts:25-25 variable EXPR
/** The face's expression channels (`faceExpression.ts`: blink, joy, sadness, anger, surprise, brows, visemes), padded to 12. */
export const EXPR = Object.keys(CHANNELS).slice(0, 12);

// src/render/people/proceduralBake.ts:26-26 variable EXPR_SLOTS
export const EXPR_SLOTS = 12;

// src/render/people/proceduralBake.ts:28-28 type AgeBand
export type AgeBand = 'child' | 'young' | 'adult' | 'senior';

// src/render/people/proceduralBake.ts:38-45 interface BakeEnv
/** What the bake reads: the people assets and morpher, the shape components, the eyes. */
export interface BakeEnv {
  setup(): Promise<{ assets: PeopleAssets; morpher: Morpher }>;
  /** The first `SHAPES` principal components (filled by `setup`). */
  readonly components: readonly Float32Array[];
  eyes(): Promise<ProxyItem>;
  /** The assets and morpher once `setup` has loaded them. */
  readonly loaded: { assets: PeopleAssets; morpher: Morpher } | null;
}

// src/render/people/proceduralBake.ts:48-64 function createBakeEnv
/** The assets loaded once, shared by the bake and the crowd that draws with it. */
export function createBakeEnv(): BakeEnv;

// src/render/people/proceduralBake.ts:55-60 method createBakeEnv.setup
async setup();

// src/render/people/proceduralBake.ts:61-61 method createBakeEnv.eyes
eyes: () => …;

// src/render/people/proceduralBake.ts:86-87 function hairBase
/** The base mesh as a hairstyle is grown on it. */
export const hairBase = (a: PeopleAssets, mo: Morpher): HairBase => …;

// src/render/people/proceduralBake.ts:101-132 interface ClassData
/** What a class is made of besides its rig: computed here, or read from the cook. */
export interface ClassData {
  readonly shapePixels: Float32Array;
  readonly jointBasis: Float32Array;
  readonly faceIndexPixels: Float32Array;
  readonly faceList: Int32Array;
  readonly exprPixels: Float32Array;
  readonly walk: ClipFrames;
  readonly idle: ClipFrames;
  /** Running (from danger), and cowering crouched (struck with fear): the reactions' clips. */
  readonly run: ClipFrames;
  readonly cower: ClipFrames;
  /** Sprinting from a blow (GTA's peds flee at a flat-out run), and holding a phone up at it. */
  readonly sprint: ClipFrames;
  readonly photo: ClipFrames;
  /** Up off the ground from a crouch: what a body knocked down gets up with (`ragdoll.ts` rise). */
  readonly getUp: ClipFrames;
  /** Down into a crouch (a first wound doubles them over, `agents.ts` flinch). */
  readonly duck: ClipFrames;
  /** Walking hurt, a limp as captured (Rocketbox walk_bruised / walk_injured), and running hurt (run_injured). */
  readonly hurtWalk: ClipFrames;
  readonly hurtRun: ClipFrames;
  /** Standing scared, looking about (Rocketbox idle_nervous_01): a bystander's first moment after a shot. */
  readonly nervous: ClipFrames;
  /** Struck in the chest by a round, as captured (Quaternius Hit_Chest): the jerk of a hit. */
  readonly hit: ClipFrames;
  /** Knocked back a few steps, and up off the ground face down or face up (CMU captures). */
  readonly staggerBack: ClipFrames;
  readonly riseFront: ClipFrames;
  readonly riseBack: ClipFrames;
  /** On hands and knees, crawling (CMU 111_03). */
  readonly crawl: ClipFrames;
}

// src/render/people/proceduralBake.ts:150-164 function classRig
/** A class's rig: its body at the band's age, with only the eyes on it (no outfit, hair, brows, lashes or hat). */
export async function classRig(env: BakeEnv, sex: WalkSex, band: AgeBand): Promise<{ base: MacroParams; shape: Float32Array; eyes: ProxyItem; rig: PersonRig }>;

// src/render/people/proceduralBake.ts:167-273 function classData
/** A class's data on its rig: the shape basis, the clips, the joint basis, the face slots and the expressions. */
export async function classData(env: BakeEnv, sex: WalkSex, rig: PersonRig, shape: Float32Array): Promise<ClassData>;
