// What the feature uses of src/sim/people/view.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/people/view.ts:6-6 type PersonAgeClass
/** A person as everything outside the pedestrian engine sees them. */

export type PersonAgeClass = 'child' | 'adult' | 'elder';

// src/sim/people/view.ts:7-7 type PersonGender
export type PersonGender = 'f' | 'm';

// src/sim/people/view.ts:8-8 type PartyArchetype
type PartyArchetype = 'solo' | 'family' | 'couple' | 'friends' | 'colleagues' | 'elders' | 'tourists';

// src/sim/people/view.ts:11-16 interface PartyView
/** The group someone walks with. Its id is its first member's; members are `id + rank`. */
export interface PartyView {
  readonly id: number;
  readonly size: number;
  readonly archetype: PartyArchetype;
  readonly hasChild: boolean;
}

// src/sim/people/view.ts:19-30 type GestureKind
/** What someone standing is doing with their hands and face. */
export type GestureKind = 'look' | 'phone' | 'talk' | 'bench'
  // Something a person stops a moment to do (`people.ts` pauses).
  | 'read' | 'drink' | 'photo' | 'wave' | 'headphones' | 'bag' | 'dance' | 'cheer' | 'crouch' | 'laugh'
  | 'argue' | 'umbrella' | 'trolley' | 'knock' | 'eat' | 'work'
  // A trip and a fall: down, a few seconds on the ground, up again.
  | 'fall'
  // Shot, a first wound: doubled over a moment, a hand to it, up again.
  | 'flinch'
  // Somebody they were walking with shot: down beside them, crying, then away.
  | 'mourn'
  // Gravely wounded, dragging themself off on hands and knees, bleeding.
  | 'crawl';

// src/sim/people/view.ts:31-31 type GesturePhase
type GesturePhase = 'approach' | 'hold' | 'step' | 'turn' | 'sitDown' | 'seated' | 'standUp' | 'leave';

// src/sim/people/view.ts:32-42 interface GestureView
interface GestureView {
  kind: GestureKind;
  phase: GesturePhase;
  /** Seconds into the phase: a sit-down or stand-up clip is played to it. */
  t: number;
  /** How long the whole gesture lasts, seconds, where it has an end (a pause). */
  hold?: number;
  /** A fall: the point it was knocked from (a punch, a shove, a blast) - the body goes down away from it. */
  fromX?: number;
  fromY?: number;
}

// src/sim/people/view.ts:49-49 type PedGround
/** What the body stands on, which sets the height it is drawn at. */
type PedGround = 'footway' | 'crossing' | 'open';

// src/sim/people/view.ts:51-51 type Severable
/** What a body can lose: an arm or a leg (each side), or the head. */
export type Severable = 'armL' | 'armR' | 'legL' | 'legR' | 'head';

// src/sim/people/view.ts:53-53 type BodyPart
/** Where a shot strikes a body. */
export type BodyPart = 'head' | 'torso' | Severable;

// src/sim/people/view.ts:55-102 interface PedView
export interface PedView {
  readonly id: number;
  /** Terrified, running from a blow (`impact`): the face screams. */
  panic?: boolean;
  /** A limb lost to a blow, the rest of the walk on without it (and bleeding). */
  maimed?: 'armL' | 'armR' | 'legL' | 'legR';
  /** Every limb lost (to shots, `PedestrianEngine.shot`), drawn gone; `maimed` is the first of them. */
  lost?: readonly Severable[];
  /** Wounded and bleeding (shot): blood drips where they go. */
  bleeding?: boolean;
  /** Shot and hurt: where, and how badly (grave: half their health gone or a limb lost). It sets how they move. */
  wound?: { part: BodyPart; grave: boolean };
  /** Position and heading at the end of this tick, and at the end of the last. */
  x: number;
  y: number;
  heading: number;
  readonly prev: { x: number; y: number; heading: number };
  /** Walking speed, and how fast the heading is turning (rad/s). */
  v: number;
  turnV: number;
  /** Seconds since this person appeared: their animation clock. */
  age: number;
  /** How they walk, when not as everybody does: unsteady, late at night. */
  style?: 'drunk' | undefined;
  /** Walking hand in hand: which hand holds the partner's. */
  hand?: 'L' | 'R' | undefined;
  /** Carrying something in both arms in front: a box. */
  carry?: 'box' | undefined;

  readonly ageClass: PersonAgeClass;
  readonly gender: PersonGender;
  party: PartyView;
  rank: number;

  ground: PedGround;
  /** The road the ground belongs to, for its elevation; undefined off the road network. */
  segment: SegmentId | undefined;
  /** Which walkway and which way along it. */
  stretch: string;
  /** Under way along a path, not paused, waiting or crossing. */
  walking: boolean;
  /** Seconds standing at a kerb waiting to cross; 0 when not. */
  kerbWait: number;
  /** The crossing waited for, while `kerbWait > 0`. */
  waitingFor: string | null;
  /** What the person is doing in place, or null. */
  gesture: GestureView | null;
}

// src/sim/people/view.ts:105-110 function personHash
/** Integer avalanche over an id: how a person's appearance and idle habits are drawn */
export function personHash(id: number): number;
