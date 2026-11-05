// What the feature uses of src/render/interiorSolids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/interiorSolids.ts:4-31 type InteriorSolidKind
/** THE SOLIDS OF A VEHICLE'S INTERIOR, BY NAME. */
export type InteriorSolidKind =
  /** The floor, the carpet, a boot floor. */
  | 'floor'
  /** The dashboard, its binnacle and its screen. */
  | 'dash'
  /** Between the front seats, or the engine cover between a cab's seats. */
  | 'console'
  /** The gear lever. */
  | 'lever'
  /** The steering column. */
  | 'column'
  /** The pedals and their arms. */
  | 'pedal'
  /** A seat's cushion, back, bolsters and runner, or a bench. */
  | 'seat'
  | 'headrest'
  /** A partition or a back wall: a van's or a cab's bulkhead, a bus driver's screen. */
  | 'wall'
  /** The trim on the inside of a door or a side, up to the waist. */
  | 'card'
  /** The parcel shelf, the boot's cover. */
  | 'shelf'
  /** A bus's inner wall under the windows. */
  | 'sill'
  /** The podium over a bus's rear wheels the seats stand on. */
  | 'podium'
  /** A bus's stanchions and ceiling rails. */
  | 'pole';

// src/render/interiorSolids.ts:34-41 interface InteriorSolid
/** One closed piece of the cabin (a box, an extruded profile, a bar), named */
export interface InteriorSolid {
  readonly kind: InteriorSolidKind;
  /** The seats (indices into `VehicleModel.seats`) it belongs to: a seat's own shell, a bench's two places, the driver's pedals; empty for the cabin's. */
  readonly seats: readonly number[];
  /** Its range of the merged interior's index (three to a triangle). */
  readonly start: number;
  readonly count: number;
}
