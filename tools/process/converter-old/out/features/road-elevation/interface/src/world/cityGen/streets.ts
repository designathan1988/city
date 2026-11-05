// What the feature uses of src/world/cityGen/streets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/streets.ts:30-35 interface StreetLine
export interface StreetLine {
  readonly level: number;
  readonly family: 0 | 1;
  points: Vec2[];
  closed: boolean;
}

// src/world/cityGen/streets.ts:48-57 method SampleGrid.near
near(p: Vec2, d: number): boolean;

// src/world/cityGen/streets.ts:185-185 interface Stretch
/** A street's stretch as filed: its ends, its street, and its place in the order a walk of every street meets it. */
interface Stretch { readonly line: StreetLine; readonly a: Vec2; readonly b: Vec2; readonly rank: number }

// src/world/cityGen/streets.ts:214-222 method StretchGrid.near
/** The stretches whose boxes may meet the box, in the order of a walk of every street. */
  near(x0: number, y0: number, x1: number, y1: number): Stretch[];
