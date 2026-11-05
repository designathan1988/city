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

// src/render/interiorSolids.ts:44-70 class CabinParts
/** The cabin's parts as they are added, each named; merged by the caller in the same order. */
export class CabinParts { … }

// src/render/interiorSolids.ts:45-45 field CabinParts.geometries
readonly geometries: BufferGeometry[] = [];

// src/render/interiorSolids.ts:49-56 method CabinParts.add
/** Adds parts of one kind, belonging to `seats` (a seat, several, or none). */
  add(kind: InteriorSolidKind, seats: number | readonly number[], ...parts: BufferGeometry[]): void;

// src/render/interiorSolids.ts:59-69 method CabinParts.solids
/** The solids as index ranges of the merge of `geometries` and then `after` (a closed interior's door cards): one a piece. */
  solids(after: readonly { kind: InteriorSolidKind; seats: readonly number[]; geometry: BufferGeometry }[] = []): InteriorSolid[];
