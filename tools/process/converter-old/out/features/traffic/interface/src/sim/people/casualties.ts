// What the feature uses of src/sim/people/casualties.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/people/casualties.ts:8-40 interface Casualty
/** The people blows have struck, for the renderer (`render/ragdoll.ts`) */

/** Somebody a blow killed or knocked down: who they were (for their body) and the blast that threw them. */
export interface Casualty {
  readonly x: number;
  readonly y: number;
  readonly heading: number;
  /** Killed, killed and torn apart, or knocked down (gets up again). */
  readonly kind: 'dead' | 'torn' | 'knocked';
  /** Seconds since. */
  t: number;
  readonly id: number;
  readonly gender: PersonGender;
  readonly ageClass: PersonAgeClass;
  readonly party: PartyView;
  readonly blastX: number;
  readonly blastY: number;
  /** 0 at the edge of the reach, 1 at the blow's centre. */
  readonly power: number;
  /** Limbs (or the head) already lost when they fell: the body on the ground lacks them too. */
  readonly lost?: readonly Severable[];
  /** Knocked down for this long before getting up (seconds); a leg lost keeps them down for good. */
  readonly lieFor?: number;
  /** Down for good but alive: writhing where they lie, bleeding (`render/ragdoll.ts` writhe). */
  readonly crawl?: boolean;
  /** Bled to death where they lay: the body already down goes still, not thrown again. */
  readonly faded?: boolean;
  /** Limbs (or the head) that came off at this blow: thrown off the body, each a piece of its own. */
  readonly severed?: readonly Severable[];
  /** Burnt black (right under a bomb). */
  readonly charred?: boolean;
  /** Shot, and where: a bullet's small push there, the body falling with its muscles still working (`ragdoll.ts` tone). */
  readonly struck?: BodyPart;
  /** Shot open at the belly (a grave trunk wound, two or three rounds in it): the guts out (`ragdoll.ts` openBelly). */
  readonly opened?: boolean;
}

// src/sim/people/casualties.ts:42-42 variable RECORDS
const RECORDS = new WeakMap<SimWorld, Casualty[]>();

// src/sim/people/casualties.ts:43-43 variable WOUNDS
const WOUNDS = new WeakMap<SimWorld, { id: number; part: BodyPart; fromX: number; fromY: number }[]>();

// src/sim/people/casualties.ts:53-59 function takeWounds
/** The wounds recorded since the last call (`recordWound`), taken. */
export function takeWounds(w: SimWorld): { id: number; part: BodyPart; fromX: number; fromY: number }[];
