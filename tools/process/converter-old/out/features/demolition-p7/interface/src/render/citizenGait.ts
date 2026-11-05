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

// src/render/citizenGait.ts:201-201 variable ADULT_WALKS
/** Walk cycles for a body, slowest first. */
const ADULT_WALKS = ['walkShuffle', 'walkSlow', 'walk'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:202-202 variable ELDER_WALKS
const ELDER_WALKS = ['walkShuffle', 'walkSlow', 'walkElder'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:204-204 variable DRUNK_WALKS
/** Somebody unsteady on their feet: the stagger in place of the stride. */
const DRUNK_WALKS = ['walkShuffle', 'walkSlow', 'walkDrunk'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:206-206 variable HAND_L_WALKS
/** Walking hand in hand: the walk with the hand on the partner's side held out to them. */
const HAND_L_WALKS = ['walkHandL'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:207-207 variable HAND_R_WALKS
const HAND_R_WALKS = ['walkHandR'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:209-209 variable CARRY_WALKS
/** Carrying a box: the clips the arms are baked holding it in (`riggedCitizens` CARRY_AT). */
const CARRY_WALKS = ['walkShuffle', 'walk'] as const satisfies readonly Cycle[];

// src/render/citizenGait.ts:212-212 variable WALK_STYLES
/** The walks people walk in, one each for life (`walkStyle`): several Rocketbox captures of ordinary walking */
export const WALK_STYLES = ['walkN1', 'walkN2', 'walkN3', 'walkStroll', 'walkCool', 'walkFast'] as const;

// src/render/citizenGait.ts:213-213 type WalkStyle
type WalkStyle = (typeof WALK_STYLES)[number];

// src/render/citizenGait.ts:217-217 variable styleWalks
/** A person's walks, slowest first: the shuffle, the slow walk, their own, and the brisk walk on top. */
const styleWalks = new WeakMap<GaitClips, Map<WalkStyle, readonly Cycle[]>>();

// src/render/citizenGait.ts:267-267 variable TURN_STEPS
/** Body turning rate, rad/s, at which the feet have to step round. */
const TURN_STEPS = 0.35;

// src/render/citizenGait.ts:269-269 variable TURN_STEPS_STANDING
/** The same for a body standing or creeping, which no stride carries round. */
const TURN_STEPS_STANDING = 0.2;

// src/render/citizenGait.ts:271-271 variable STEPPED_TURN_RATE
/** Turning rate, rad/s, a fully played turn clip carries a standing body round at. */
const STEPPED_TURN_RATE = 3.2;

// src/render/citizenGait.ts:273-273 variable FEET_LEAD_RATE
/** Yaw rate, rad/s, a turn clip steps at before the body has begun to follow it. */
const FEET_LEAD_RATE = 1.2;

// src/render/citizenGait.ts:275-275 variable LAG_STEPS
/** Angle, rad, a standing body may lag where it is facing before it steps round. */
const LAG_STEPS = 0.12;

// src/render/citizenGait.ts:277-277 variable TURN_IN_PLACE
/** Drawn speed below which a turn that fast is stepped with the turn clips. */
const TURN_IN_PLACE = 0.3;

// src/render/citizenGait.ts:279-279 variable TURN_SETTLE
/** Seconds a turn clip is held once the turning has stopped. */
const TURN_SETTLE = 0.35;

// src/render/citizenGait.ts:281-281 variable TURN_WINDUP
/** Angle a turn clip is entered at: into its steps, not its wind-up. */
const TURN_WINDUP = 0.05;

// src/render/citizenGait.ts:283-283 variable TURN_FOOT
/** Half the width between the feet, metres: how far each foot travels as the body pivots. */
const TURN_FOOT = 0.3;

// src/render/citizenGait.ts:285-285 variable STOP_BELOW
/** Walking speed, m/s, below which a braking walker plays the walk stop, and the braking it needs. */
const STOP_BELOW = 0.6;

// src/render/citizenGait.ts:286-286 variable STOP_DECEL
const STOP_DECEL = 0.35;

// src/render/citizenGait.ts:288-288 variable START_BELOW
/** Speed, m/s, below which setting off plays the walk start. */
const START_BELOW = 1.0;

// src/render/citizenGait.ts:290-290 variable CLIP_FLOOR
/** Slowest share of its own rate a start or stop is played at. */
const CLIP_FLOOR = 0.75;

// src/render/citizenGait.ts:292-292 variable STOP_SETTLE
/** Rate a stop the body has already come out of finishes at, as a share of its own. */
const STOP_SETTLE = 1.25;

// src/render/citizenGait.ts:294-294 variable STRIDE_EXPONENT
/** How stride follows pace below the slowest capture (stride ∝ pace^0.6, as measured in people). */
const STRIDE_EXPONENT = 0.6;

// src/render/citizenGait.ts:296-296 variable GAIT_FADE
/** Crossfades, seconds: between steps of the gait, into a turn, and between anything else. */
const GAIT_FADE = 0.18;

// src/render/citizenGait.ts:297-297 variable TURN_FADE
const TURN_FADE = 0.22;

// src/render/citizenGait.ts:298-298 variable FADE
const FADE = 0.3;

// src/render/citizenGait.ts:299-299 variable RUN_TIME
const RUN_TIME = 0.6;

// src/render/citizenGait.ts:301-301 variable VELOCITY_TIME
/** Smoothing of the drawn velocity and of its rate of change, seconds. */
const VELOCITY_TIME = 0.12;

// src/render/citizenGait.ts:302-302 variable ACCEL_TIME
const ACCEL_TIME = 0.2;

// src/render/citizenGait.ts:304-304 variable JUMP
/** A tick's displacement faster than this is a re-seat, not a step, m/s. */
const JUMP = 4;

// src/render/citizenGait.ts:387-392 variable ACTIVITY
/** What a person stopped to do plays (the gestures of `sim/people/view.ts`). */
const ACTIVITY: Partial<Record<string, Single>> = {
  // Reading: the book held up in both hands, as a phone is (the library's newspaper clip only carries one by the side).
  read: 'phone', bag: 'bag', trolley: 'trolley', umbrella: 'umbrella', cheer: 'cheer', dance: 'dance',
  wave: 'wave', drink: 'drink', photo: 'photo', laugh: 'laugh', argue: 'argue', knock: 'knock',
  headphones: 'headphones', eat: 'eatIdle', work: 'workTable',
};

// src/render/citizenGait.ts:466-466 variable STILL_BELOW
/** Below this drawn speed a body is standing (m/s, as everything in the gait is - `drawnVelocity`) */
const STILL_BELOW = 0.05;
