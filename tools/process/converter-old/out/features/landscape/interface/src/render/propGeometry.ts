// What the feature uses of src/render/propGeometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/propGeometry.ts:29-29 type Rgb
/** Procedural geometry for everything that stands on the ground. */

type Rgb = readonly [number, number, number];

// src/render/propGeometry.ts:31-34 function rgb
const rgb = (hex: number): Rgb =>;

// src/render/propGeometry.ts:144-144 type TreeSpecies
export type TreeSpecies = 'broadleaf' | 'broadleafTall' | 'conifer' | 'ipeYellow' | 'ipePink';

// src/render/propGeometry.ts:145-145 variable TREE_SPECIES
export const TREE_SPECIES: readonly TreeSpecies[] = ['broadleaf', 'broadleafTall', 'conifer', 'ipeYellow', 'ipePink'];

// src/render/propGeometry.ts:148-150 variable TREE_KIND
/** Each street and garden species as the game's one tree style grows it (`lowPolyTrees.ts`). */
const TREE_KIND: Readonly<Record<TreeSpecies, LowPolyKind>> = {
  broadleaf: 'oak', broadleafTall: 'broadleafTall', conifer: 'cypress', ipeYellow: 'ipeYellow', ipePink: 'ipePink',
};

// src/render/propGeometry.ts:157-157 type BushKind
export type BushKind = 'bush' | 'bushFlowering' | 'hedge';

// src/render/propGeometry.ts:158-158 variable BUSH_KINDS
export const BUSH_KINDS: readonly BushKind[] = ['bush', 'bushFlowering', 'hedge'];

// src/render/propGeometry.ts:233-233 variable LAMP_HEIGHT
/** A residential lighting column is 8 to 10 m with a 1.5 to 2.5 m outreach. */
export const LAMP_HEIGHT = m(9);

// src/render/propGeometry.ts:234-234 variable LAMP_OUTREACH
export const LAMP_OUTREACH = m(2.1);

// src/render/propGeometry.ts:236-236 variable LAMP_METAL
/** Galvanised grey, not near-black. */
const LAMP_METAL = rgb(0x8a918e);

// src/render/propGeometry.ts:237-237 variable LAMP_PLINTH
const LAMP_PLINTH = rgb(0x5d6461);

// src/render/propGeometry.ts:298-298 variable WOOD
const WOOD = rgb(0x8d6540);

// src/render/propGeometry.ts:299-299 variable IRON
const IRON = rgb(0x262b2a);
