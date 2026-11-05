// What the feature uses of src/render/citizenCasting.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenCasting.ts:9-9 type Wardrobe
/** WHO IS DRAWN AS WHOM - the only door into the citizen roster. */

/** `traditional`: everyday dress of a tradition - a headscarf and tunic, a shalwar kameez, a thobe and keffiyeh. */
export type Wardrobe = 'casual' | 'smart-casual' | 'business' | 'sport-casual' | 'traditional';

// src/render/citizenCasting.ts:10-10 type AgeBand
type AgeBand = 'child' | 'young' | 'adult' | 'senior';

// src/render/citizenCasting.ts:11-11 type Sex
type Sex = 'f' | 'm';

// src/render/citizenCasting.ts:13-26 interface CitizenModel
export interface CitizenModel {
  readonly id: string;
  readonly wardrobe: Wardrobe;
  readonly ageBand: AgeBand;
  readonly gender: Sex;
  /** A visual variant uses the same licensed mesh and rig as this source. */
  readonly sourceId?: string;
  /** Clothing palette of a visual variant; zero keeps its source texture. */
  readonly look?: number;
  /** A MakeHuman person (`people/roster.ts`): built and rigged, not loaded. */
  readonly person?: PersonSpec;
  /** False for a body that cannot reach a two-wheeler's controls. */
  readonly rides?: boolean;
}

// src/render/citizenCasting.ts:29-31 variable CROWD
/** The street roster: MakeHuman people, each dressed for a wardrobe (`people/roster.ts`). */
export const CROWD: readonly CitizenModel[] = sceneryCast(makeRoster().map((entry) => ({
  id: entry.id, wardrobe: entry.wardrobe, ageBand: entry.ageBand, gender: entry.gender, person: entry.person, rides: entry.rides,
})));

// src/render/citizenCasting.ts:42-57 function sceneryCast
function sceneryCast(all: readonly CitizenModel[]): CitizenModel[];
