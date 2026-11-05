// What the feature uses of src/render/citizenGait.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenGait.ts:8-19 interface GaitClip
/** What a pedestrian's legs play, decided from what the simulation says the body is doing — and nothing else. */

export interface GaitClip {
  /** Intervals between baked frames. */
  readonly frames: number;
  readonly duration: number;
  readonly loop: boolean;
  /** Ground one cycle covers on this body at scale 1, metres. */
  readonly stride: number;
  /** Ground covered by each baked frame (walk start, stop), metres at scale 1. */
  readonly travel?: Float32Array;
  /** Angle turned by each baked frame (turns), radians, unsigned. */
  readonly yaw?: Float32Array;
}

// src/render/citizenGait.ts:21-25 type GaitClipName
export type GaitClipName =
  | 'walk' | 'walkElder' | 'walkSlow' | 'walkShuffle' | 'walkRest' | 'walkBack' | 'walkLeft' | 'walkRight' | 'run' | 'walkDrunk' | 'walkHandL' | 'walkHandR' | 'start' | 'stop' | 'turnLeft' | 'turnRight'
  | 'idle' | 'look' | 'phone' | 'talk' | 'listen' | 'sitDown' | 'sitIdle' | 'standUp'
  | 'read' | 'bag' | 'trolley' | 'umbrella' | 'cheer' | 'dance' | 'wave' | 'drink' | 'photo' | 'crouchDown' | 'crouchIdle' | 'crouchUp' | 'laugh' | 'angry' | 'argue' | 'knock' | 'headphones' | 'eatIdle' | 'workTable'
  | 'walkN1' | 'walkN2' | 'walkN3' | 'walkStroll' | 'walkCool' | 'walkFast';

// src/render/citizenGait.ts:26-26 type GaitClips
export type GaitClips = Readonly<Record<GaitClipName, GaitClip>>;

// src/render/citizenGait.ts:28-29 type Cycle
/** The walk and run cycles, which 'loco' blends on one phase. */
type Cycle = 'walk' | 'walkElder' | 'walkSlow' | 'walkShuffle' | 'walkRest' | 'walkBack' | 'walkLeft' | 'walkRight' | 'run' | 'walkDrunk'
  | 'walkHandL' | 'walkHandR' | WalkStyle;

// src/render/citizenGait.ts:30-30 type Single
type Single = Exclude<GaitClipName, Cycle>;

// src/render/citizenGait.ts:31-31 type PlayKey
type PlayKey = 'loco' | Single;

// src/render/citizenGait.ts:32-38 interface Play
interface Play {
  key: PlayKey;
  /** Fraction through the clip, 0..1. */
  phase: number;
  /** Distance (start, stop) or angle (turns) this play has covered. */
  acc: number;
}

// src/render/citizenGait.ts:41-85 interface Gait
/** One pedestrian's gait: what is playing and how the body is moving. */
export interface Gait {
  time: number;
  /** The simulation's heading, followed smoothly. */
  base: number;
  /** Drawn heading last frame. */
  drawn: number;
  /** Velocity of the drawn body, smoothed, m/s. */
  vx: number;
  vy: number;
  speed: number;
  /** Rate of change of `speed`, smoothed, m/s². */
  accel: number;
  /** Shared phase of every walk and the run, 0..1. */
  cycle: number;
  run: number;
  /** The two walk cycles blended at this pace, and the second one's weight. */
  walkA: Cycle;
  walkB: Cycle;
  walkW: number;
  /** Ground the legs cover per cycle at this pace on this body, metres. */
  stride: number;
  /** Physical travel relative to the drawn torso, radians; never writes to PedView. */
  direction: number;
  /** Actual pose amplitude below the shortest captured stride. */
  stepScale: number;
  backScale: number;
  leftScale: number;
  rightScale: number;
  /** Directional group weights, calibrated by their effective ground travel. */
  forwardWeight: number;
  backWeight: number;
  leftWeight: number;
  rightWeight: number;
  cur: Play;
  prev: Play | null;
  /** Weight of `cur` against `prev`, rising to 1 over `fadeTime`. */
  fade: number;
  fadeTime: number;
  /** Seconds the body has been turning slowly enough to stop the turn. */
  settling: number;
  /** A walk stop may begin: not again until the body has walked on or stood. */
  armed: boolean;
  /** The stop, or kerb wait, a look round has already been played for. */
  looked: unknown;
}

// src/render/citizenGait.ts:212-212 variable WALK_STYLES
/** The walks people walk in, one each for life (`walkStyle`): several Rocketbox captures of ordinary walking */
export const WALK_STYLES = ['walkN1', 'walkN2', 'walkN3', 'walkStroll', 'walkCool', 'walkFast'] as const;

// src/render/citizenGait.ts:213-213 type WalkStyle
type WalkStyle = (typeof WALK_STYLES)[number];

// src/render/citizenGait.ts:316-316 function gaitHeading
/** The heading to draw the body at. */
export const gaitHeading = (g: Gait): number => …;
