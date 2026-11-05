// What the feature uses of src/render/citizenWalk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenWalk.ts:8-20 interface WalkFile
/** The official Rocketbox neutral walks — a man's (m_walk_neutral, captured on Male_Adult_01) and a woman's */
interface WalkFile {
  readonly duration: number;
  /** A cycle that repeats (the walk); a one-shot holds its last frame. */
  readonly loop?: boolean;
  readonly bones: readonly string[];
  readonly bind: readonly { readonly q: readonly number[]; readonly p: readonly number[] }[];
  readonly bindLowest: number;
  readonly frames: readonly {
    readonly q: readonly (readonly number[])[] | Int16Array;
    readonly pelvis: readonly number[];
    readonly lowest: number;
  }[];
}

// src/render/citizenWalk.ts:33-42 interface Source
interface Source {
  readonly file: WalkFile;
  /** Bind rotations, inverted once for all bodies. */
  readonly bindInverse: readonly Quaternion[];
  readonly pelvisBind: Vector3;
  /** Each bone's world rotation averaged over the cycle: the centre its swing is about. */
  readonly mean: readonly Quaternion[];
  /** Mean pelvis position over the cycle. */
  readonly pelvisMean: Vector3;
}

// src/render/citizenWalk.ts:44-71 function sourceOf
/** Prepares a capture for transfer. */
function sourceOf(file: WalkFile, bindFrom: WalkFile = file): Source;

// src/render/citizenWalk.ts:72-75 variable WALKS
const WALKS = {
  male: { ...(JSON.parse(maleSource) as WalkFile), loop: true },
  female: { ...(JSON.parse(femaleSource) as WalkFile), loop: true },
} as const;

// src/render/citizenWalk.ts:76-76 variable SOURCES
const SOURCES = { male: sourceOf(WALKS.male), female: sourceOf(WALKS.female) } as const;

// src/render/citizenWalk.ts:77-77 type WalkSex
export type WalkSex = keyof typeof SOURCES;

// src/render/citizenWalk.ts:287-302 type LibraryClipName
/** The clips of the Rocketbox library, by the name the renderer plays them by. */
export type LibraryClipName =
  | 'start' | 'stop' | 'run' | 'walkSlow'
  | 'turnLeft' | 'turnRight' | 'turnLeft180' | 'turnRight180'
  | 'idle' | 'look' | 'phone' | 'talk' | 'listen'
  | 'sitDown' | 'sitIdle' | 'standUp'
  // Things people do (`sim/people/view.ts` GestureKind).
  | 'read' | 'bag' | 'trolley' | 'umbrella' | 'cheer' | 'dance' | 'wave' | 'drink' | 'photo'
  | 'crouchDown' | 'crouchIdle' | 'crouchUp' | 'laugh' | 'angry' | 'argue' | 'knock' | 'headphones'
  | 'eatIdle' | 'workTable' | 'walkDrunk' | 'runFast'
  | 'walkN1' | 'walkN2' | 'walkN3' | 'walkStroll' | 'walkCool' | 'walkFast'
  // Hurt and afraid (`people/proceduralCrowd.ts`: the wounded's run and limp, a bystander standing scared).
  | 'runInjured' | 'walkInjured' | 'nervous'
  // Struck by a round (Quaternius's Universal Animation Library).
  | 'hitChest' | 'hitHead'
  // Off the ground, steps back, a crawl (CMU motion capture).
  | 'getUpFront' | 'getUpBack' | 'staggerBack' | 'crawl';

// src/render/citizenWalk.ts:305-317 interface LibraryClip
/** One clip of the library, decoded and ready to transfer. */
export interface LibraryClip {
  readonly name: LibraryClipName;
  readonly kind: 'forward' | 'cycle' | 'turn' | 'static' | 'sit';
  readonly loop: boolean;
  readonly duration: number;
  /** Ground covered by the whole clip (a cycle: one cycle), metres. */
  readonly advance: number;
  /** Angle turned by the whole clip, radians; positive turns left. */
  readonly turned: number;
  readonly travel: Float32Array;
  readonly yaw: Float32Array;
  readonly source: Source;
}

// src/render/citizenWalk.ts:318-318 type RocketboxLibrary
export type RocketboxLibrary = Readonly<Record<WalkSex, Readonly<Record<LibraryClipName, LibraryClip>>>>;

// src/render/citizenWalk.ts:319-319 type RocketboxClips
export type RocketboxClips = RocketboxLibrary[WalkSex];
