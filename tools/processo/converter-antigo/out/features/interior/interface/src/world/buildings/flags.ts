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

// src/world/buildings/flags.ts:17-20 variable FLAG_COLOURS
/** The colours a flag's fields are picked from in the Builder. */
export const FLAG_COLOURS: readonly number[] = [
  0xffffff, 0x111418, 0xc8102e, 0xe5533d, 0xf2a900, 0xffd100, 0x009b3a, 0x00843d,
  0x0b5ea8, 0x002776, 0x6cace4, 0x5b2a86, 0x8a1538, 0x7b4a2a, 0x9aa0a6, 0xf28ab2,
];

// src/world/buildings/flags.ts:22-22 variable DEFAULT_FLAG
export const DEFAULT_FLAG: FlagDesign = { pattern: 'tricolourV', colours: [0x0b5ea8, 0xffffff, 0xc8102e] };
