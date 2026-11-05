// What the feature uses of src/people/roster.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/roster.ts:6-6 type RosterWardrobe
/** The street's people: a fixed roster of MakeHuman bodies, made once from seeds, that the crowd draws */
type RosterWardrobe = 'casual' | 'smart-casual' | 'business' | 'sport-casual' | 'traditional';

// src/people/roster.ts:7-7 type RosterAge
type RosterAge = 'child' | 'young' | 'adult' | 'senior';

// src/people/roster.ts:9-17 interface RosterEntry
export interface RosterEntry {
  readonly id: string;
  readonly wardrobe: RosterWardrobe;
  readonly ageBand: RosterAge;
  readonly gender: 'f' | 'm';
  readonly person: PersonSpec;
  /** Tall enough to reach a motorcycle's grips and pegs and a bicycle's pedals. */
  readonly rides: boolean;
}

// src/people/roster.ts:84-129 function makeRoster
export function makeRoster(): RosterEntry[];
