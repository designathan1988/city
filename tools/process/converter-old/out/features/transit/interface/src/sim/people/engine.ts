// What the feature uses of src/sim/people/engine.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/people/engine.ts:56-65 interface Boarder
/** Somebody passing between a vehicle seat and the footway. */
export interface Boarder {
  readonly seed: number;
  readonly gender: PersonGender;
  readonly ageClass: PersonAgeClass;
  /** Where they stand on the footway (getting out) or come from (getting in), world units. */
  readonly footX: number;
  readonly footY: number;
  /** Heading they face while standing on the footway. */
  readonly footHeading: number;
}
