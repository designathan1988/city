// What the feature uses of src/people/spec.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/spec.ts:7-16 interface PersonSpec
/** A person: everything the Person Creator sets, and all a citizen needs to be drawn. */
export interface PersonSpec {
  readonly id: number;
  readonly name: string;
  /** Resting expression, -1 subdued through 0 neutral to 1 cheerful. */
  readonly mood?: number;
  readonly body: MacroParams;
  /** Regional sliders, -1..1, by slider name (`l-`/`r-` for one side). */
  readonly features: Readonly<Record<string, number>>;
  readonly look: PersonLook;
}

// src/people/spec.ts:18-18 type HairStyle
type HairStyle = 'none' | 'short' | 'long';

// src/people/spec.ts:19-19 type TopStyle
export type TopStyle = 'none' | 'tank' | 'tshirt' | 'longsleeve';

// src/people/spec.ts:20-20 type BottomStyle
export type BottomStyle = 'trousers' | 'shorts' | 'skirt';

// src/people/spec.ts:22-51 interface PersonLook
export interface PersonLook {
  /** Colours as 0xRRGGBB. */
  readonly skin: number;
  /** Optional editable pigment coordinates; skin remains the rendered sRGB colour. */
  readonly melanin?: number;
  readonly undertone?: number;
  readonly beard?: 'none' | 'stubble' | 'moustache' | 'beard';
  readonly makeup?: number;
  readonly eyes: number;
  readonly hair: number;
  readonly hairStyle: HairStyle;
  readonly top: TopStyle;
  readonly topColour: number;
  readonly bottom: BottomStyle;
  readonly bottomColour: number;
  readonly shoes: number;
  /** MakeHuman garments, fitted to the body (`people/body/proxy.ts`), by name: what the person wears when set. */
  readonly outfit?: string;
  readonly footwear?: string;
  /** A hairstyle, or 'none'. */
  readonly hairCut?: string;
  readonly brows?: string;
  readonly lashes?: string;
  /** A hat, or 'none'. */
  readonly hat?: string;
  /** A colour the outfit is dyed, or null for its own colours. */
  readonly outfitTint?: number | null;
  /** More community items worn with the outfit (`wardrobe.ts`) */
  readonly extras?: readonly string[];
}

// src/people/spec.ts:88-104 function wornItems
/** The items a look wears, by name: what has to be loaded to draw it. */
export function wornItems(look: PersonLook): string[];

// src/people/spec.ts:129-249 function randomPerson
/** A plausible stranger, the same one for the same seed: any sex, an age from a toddler to the very old */
export function randomPerson(id: number, seed: number, keep: { body?: Partial<PersonSpec['body']>; look?: Partial<PersonSpec['look']> } = {}): PersonSpec;
