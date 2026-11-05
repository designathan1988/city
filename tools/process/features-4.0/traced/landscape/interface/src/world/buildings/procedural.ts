// What the feature uses of src/world/buildings/procedural.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/procedural.ts:16-25 interface Signature
/** What the eye tells two buildings apart by. */
export interface Signature {
  /** The form of the mass (`house:ell`, `flats:u`, `office:setback`...). */
  readonly form: string;
  /** The main wall colour, 0xRRGGBB. */
  readonly colour: number;
  /** The main roof: its kind and colour. */
  readonly roof: string;
  /** Storeys of the tallest block. */
  readonly storeys: number;
}
