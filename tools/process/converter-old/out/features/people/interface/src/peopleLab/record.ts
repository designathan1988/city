// What the feature uses of src/peopleLab/record.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/record.ts:12-12 variable RECORD_FORMAT
/** A PERSON MADE IN THE LAB, as saved: the game's own `PersonSpec` plus what the game does not yet keep per person */

export const RECORD_FORMAT = 'roadcraft-people/1';

// src/peopleLab/record.ts:14-34 interface LabPerson
export interface LabPerson {
  readonly spec: PersonSpec;
  /** Proportions in an animated film's style, per person (`cartoon.ts`). */
  readonly style: CartoonStyle;
  /** A hairstyle grown in the hair studio (`hair/procedural.ts`) when `spec.look.hairCut` is `proc:<name>`. */
  readonly hair?: HairStyle;
  /** The hairstyle reshaped with the sculptor, when `base` is the one worn (`spec.look.hairCut`). */
  readonly sculpt?: HairSculpt;
  /** Any worn piece reshaped with the sculptor (a garment, shoes, a hat), each by its item name (`sculpt.base`). */
  readonly sculpts?: readonly HairSculpt[];
  /** The rig as reshaped, the pose held and the animation made in the pose tab (`posing.ts`). */
  readonly motion?: Motion;
  /** The body and face sculpted with brushes (`bodySculpt.ts`): moves of the base mesh's vertices. */
  readonly body?: HairSculpt;
  /** The worn hairstyle's length, volume and waves (`hairShape.ts`). */
  readonly hairShape?: HairShape;
  /** The worn outfit's length and looseness (`hairShape.ts`, its wave unused). */
  readonly outfitShape?: HairShape;
  /** The worn outfit's pattern, accent colour and finish (`clothStyle.ts`); its main colour is `spec.look.outfitTint`. */
  readonly outfitStyle?: OutfitStyle;
}

// src/peopleLab/record.ts:36-39 interface LabRoster
export interface LabRoster {
  readonly format: typeof RECORD_FORMAT;
  readonly people: readonly LabPerson[];
}

// src/peopleLab/record.ts:42-42 variable PROC_PREFIX
/** The hair studio's hairstyles are named `proc:<id>`, never a catalogue item. */
export const PROC_PREFIX = 'proc:';

// src/peopleLab/record.ts:45-45 variable GROOM
/** An older hair studio's hair, as `spec.look.hairCut`: worn as the base it was nearest, a bob (that studio is gone). */
export const GROOM = 'groom';

// src/peopleLab/record.ts:48-48 variable ATELIER
/** The atelier's garment, as an extra worn by people saved while the atelier existed: taken off (`shapeKeys.ts` effective). */
export const ATELIER = 'atelier';

// src/peopleLab/record.ts:50-53 function round
const round = (x: number, places = 3): number =>;

// src/peopleLab/record.ts:56-80 function compactPerson
/** The person written small: numbers to 3 places, features at rest left out, nothing undefined. */
export function compactPerson(person: LabPerson): LabPerson;

// src/peopleLab/record.ts:82-84 function rosterOf
export function rosterOf(people: readonly LabPerson[]): LabRoster;

// src/peopleLab/record.ts:87-94 function readRoster
/** A roster read back (a file, the browser's storage); malformed entries are dropped, not guessed. */
export function readRoster(raw: unknown): LabPerson[];

// src/peopleLab/record.ts:97-97 function savedBytes
/** Bytes the person takes saved. */
export const savedBytes = (person: LabPerson): number => …;

// src/peopleLab/record.ts:108-113 function rosterMarkdown
/** The roster for a reader: one line a person. */
export function rosterMarkdown(people: readonly LabPerson[]): string;
