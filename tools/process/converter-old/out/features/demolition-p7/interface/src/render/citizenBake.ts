// What the feature uses of src/render/citizenBake.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenBake.ts:15-15 variable WALK
/** The Rocketbox neutral walk of the body's sex, exactly as captured. */
export const WALK = 0;

// src/render/citizenBake.ts:17-17 variable WALK_ELDER
/** The same walk with an older person's shorter step and quieter arms. */
export const WALK_ELDER = 1;

// src/render/citizenBake.ts:19-19 variable WALK_SHUFFLE
/** The Rocketbox slow walk with its swing shrunk: short steps, for inching along. */
export const WALK_SHUFFLE = 2;

// src/render/citizenBake.ts:21-26 variable LIBRARY
/** The Rocketbox library clips, baked after the walks in this order. */
const LIBRARY = [
  'start', 'stop', 'run', 'turnLeft', 'turnRight',
  'idle', 'look', 'phone', 'talk', 'listen', 'sitDown', 'sitIdle', 'standUp', 'walkSlow', 'walkDrunk',
  'read', 'bag', 'trolley', 'umbrella', 'cheer', 'dance', 'wave', 'drink', 'photo', 'crouchDown', 'crouchIdle', 'crouchUp', 'laugh', 'angry', 'argue', 'knock', 'headphones', 'eatIdle', 'workTable',
  'walkN1', 'walkN2', 'walkN3', 'walkStroll', 'walkCool', 'walkFast',
] as const satisfies readonly LibraryClipName[];

// src/render/citizenBake.ts:27-27 type Played
export type Played = (typeof LIBRARY)[number];

// src/render/citizenBake.ts:28-29 variable LIBRARY_AT
export const LIBRARY_AT = Object.fromEntries(LIBRARY.map((name, i) => [name, WALK_SHUFFLE + 1 + i])) as
  Readonly<Record<Played, number>>;

// src/render/citizenBake.ts:30-30 variable DIRECTIONAL
const DIRECTIONAL = ['walkRest', 'walkBack', 'walkLeft', 'walkRight'] as const;

// src/render/citizenBake.ts:31-32 variable DIRECTIONAL_AT
const DIRECTIONAL_AT = Object.fromEntries(DIRECTIONAL.map((name, i) => [name, WALK_SHUFFLE + 1 + LIBRARY.length + i])) as
  Readonly<Record<(typeof DIRECTIONAL)[number], number>>;

// src/render/citizenBake.ts:34-38 variable GAIT_AT
/** Where each clip the gait plays (`citizenGait.ts`) is baked. */
export const GAIT_AT: Readonly<Record<GaitClipName, number>> = {
  ...LIBRARY_AT, ...DIRECTIONAL_AT, walk: WALK, walkElder: WALK_ELDER, walkShuffle: WALK_SHUFFLE,
  walkHandL: WALK_SHUFFLE + 1 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length,
  walkHandR: WALK_SHUFFLE + 2 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length,
};

// src/render/citizenBake.ts:40-41 variable RIDER_AT
/** People in and on vehicles (`riderPoses.ts`) */
export const RIDER_AT = Object.fromEntries(RIDER_CLIPS.map((clip, i) => [clip.key, WALK_SHUFFLE + 1 + LIBRARY.length + DIRECTIONAL.length + i])) as
  Readonly<Record<RiderClipKey, number>>;

// src/render/citizenBake.ts:46-46 variable CARRIED
/** The clips somebody carrying a box plays, baked again with both arms holding it in front */
const CARRIED = ['walk', 'walkShuffle', 'idle', 'turnLeft', 'turnRight', 'walkBack', 'walkLeft', 'walkRight'] as const satisfies readonly GaitClipName[];

// src/render/citizenBake.ts:47-49 variable CARRY_AT
export const CARRY_AT = Object.fromEntries(CARRIED.map((name, i) =>
  [name, WALK_SHUFFLE + 3 + LIBRARY.length + DIRECTIONAL.length + RIDER_CLIPS.length + i])) as
  Readonly<Partial<Record<GaitClipName, number>>>;

// src/render/citizenBake.ts:51-51 type CitizenClipKey
/** Anything `drawClip` can play. */
export type CitizenClipKey = RiderClipKey | 'walk' | Played;

// src/render/citizenBake.ts:55-76 interface ClipFrames
export interface ClipFrames {
  /** Twelve affine values per bone and frame; the fixed fourth row is restored for the GPU. */
  data: Float32Array;
  /** Intervals between baked frames; `data` holds `frames + 2` rows (the last repeated). */
  frames: number; duration: number;
  /** Ground one cycle covers on this body at scale 1, metres. */
  stride: number;
  /** Height of the pelvis above the model origin in the first frame, metres. */
  pelvisY: number;
  /** The pelvis's offset from the origin in plan in the first frame (model +X left, +Z forward), metres. */
  pelvisX: number;
  pelvisZ: number;
  loop: boolean;
  /** Ground covered by each baked frame on this body at scale 1, metres (start, stop). */
  travel?: Float32Array;
  /** Angle turned by each baked frame, radians, unsigned (turns). */
  yaw?: Float32Array;
  /** A rider's head bone in the first frame, in the model's frame: where a helmet goes. */
  head?: Matrix4;
  /** Per baked frame, the right then the left hand bone, in the model's frame (16 + 16): where a held thing goes. */
  hands?: Float32Array;
}

// src/render/citizenBake.ts:94-97 function afterFrame
/** Resolves just after the next frame has been drawn: one slice of work per frame. */
export function afterFrame(): Promise<void>;

// src/render/citizenBake.ts:100-105 interface BakeRig
/** One copy of a body to bake on, and the way back to its rest pose. */
export interface BakeRig {
  readonly rig: Object3D;
  readonly mesh: SkinnedMesh;
  /** Puts every node back where the asset has it, as a fresh copy would be. */
  reset(): void;
}

// src/render/citizenBake.ts:347-347 type Deferred
export type Deferred = Map<number, { readonly make: () => Promise<ClipFrames>; readonly standIn: number }>;

// src/render/citizenBake.ts:350-350 type LibraryOf
/** The motion library, asked only when a clip is baked here: DO NOT decode it (~5 MB of JSON a sex) where every clip comes baked. */
export type LibraryOf = () => Promise<RocketboxClips>;

// src/render/citizenBake.ts:353-353 interface ShuffleFacts
/** What the directional walks are timed by: the shuffle's length and stride (`WALK_SHUFFLE`). */
export interface ShuffleFacts { readonly duration: number; readonly stride: number }

// src/render/citizenBake.ts:367-392 function deferredClips
/** The clips baked on first use, each with what stands in for it until then: the same here and in a worker (`bakeDeferred`). */
function deferredClips(body: BakeRig, sex: WalkSex, library: LibraryOf, facts: () => ShuffleFacts): Deferred;

// src/render/citizenBake.ts:381-383 function deferredClips.carried
const carried = (name: (typeof CARRIED)[number], make: () => Promise<ClipFrames>): void =>;

// src/render/citizenBake.ts:400-430 function bake
export async function bake(scene: Object3D, sex: WalkSex, library: LibraryOf,
  given?: readonly (ClipFrames | undefined)[]): Promise<{ clips: ClipFrames[]; helmet: Matrix4 | null; deferred: Deferred }>;

// src/render/citizenBake.ts:455-472 interface RigNode
/** One node of a rig: bone, skinned mesh or plain node, its parent and rest transform. */
export interface RigNode {
  readonly name: string;
  readonly kind: 'bone' | 'skinned' | 'node';
  readonly parent: number;
  readonly p: readonly number[];
  readonly q: readonly number[];
  readonly s: readonly number[];
  /** A skinned mesh: its bones (node indices), their inverses, its bind, and the vertex data the bakes read (feet, head). */
  readonly skin?: {
    readonly bones: readonly number[];
    readonly inverses: Float32Array;
    readonly bind: readonly number[];
    readonly bindMode: string;
    readonly position: Float32Array;
    readonly skinIndex: Float32Array;
    readonly skinWeight: Float32Array;
  };
}
