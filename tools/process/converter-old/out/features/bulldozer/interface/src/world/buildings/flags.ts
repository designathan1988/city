// What the feature uses of src/world/buildings/flags.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/flags.ts:2-5 variable FLAG_PATTERNS
/** Flags: a generic, customizable flag flown from a mast (a spire, a lantern's mast, a flagpole). */
export const FLAG_PATTERNS = [
  'solid', 'bicolourH', 'bicolourV', 'tricolourH', 'tricolourV', 'cross', 'nordic',
  'diagonal', 'saltire', 'canton', 'stripes', 'border', 'disc', 'chevron',
] as const;

// src/world/buildings/flags.ts:6-6 type FlagPattern
type FlagPattern = (typeof FLAG_PATTERNS)[number];

// src/world/buildings/flags.ts:8-12 interface FlagDesign
export interface FlagDesign {
  readonly pattern: FlagPattern;
  /** Field, second and third colours (0xrrggbb). */
  readonly colours: readonly [number, number, number];
}
