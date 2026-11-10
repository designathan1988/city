// What the feature uses of src/sim/people/view.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/people/view.ts:6-6 type PersonAgeClass
/** A person as everything outside the pedestrian engine sees them. */

export type PersonAgeClass = 'child' | 'adult' | 'elder';

// src/sim/people/view.ts:7-7 type PersonGender
export type PersonGender = 'f' | 'm';

// src/sim/people/view.ts:51-51 type Severable
/** What a body can lose: an arm or a leg (each side), or the head. */
export type Severable = 'armL' | 'armR' | 'legL' | 'legR' | 'head';

// src/sim/people/view.ts:53-53 type BodyPart
/** Where a shot strikes a body. */
export type BodyPart = 'head' | 'torso' | Severable;
