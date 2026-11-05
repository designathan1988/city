// What the feature uses of src/sim/vehicles/archetypes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/archetypes.ts:7-7 type VehicleShape
/** Vehicle classes, in world units. */

/** Body plan the renderer builds from. */
export type VehicleShape = 'car' | 'bus' | 'truck' | 'motorcycle' | 'bicycle';

// src/sim/vehicles/archetypes.ts:10-10 type BodyStyle
/** Silhouette of a `car`: where the roof ends and how the tail is cut. */
type BodyStyle = 'hatch' | 'sedan' | 'suv' | 'van';

// src/sim/vehicles/archetypes.ts:12-48 interface Archetype
export interface Archetype {
  readonly id: string;
  /** Which body plan the renderer assembles for this class. */
  readonly shape: VehicleShape;
  /** For a `car`, the silhouette it is drawn with. */
  readonly style?: BodyStyle;
  readonly length: number;
  readonly width: number;
  /** Overall height above the road surface, roof included. */
  readonly height: number;
  /** Greenhouse length as a fraction of the body length. */
  readonly cabinFraction: number;
  /** Greenhouse centre, forward of the body centre, as a fraction of length. */
  readonly cabinShift: number;
  /** Number of axles, which decides the wheel count and their spacing. */
  readonly axles: number;
  /** Radius of one road wheel. */
  readonly wheelRadius: number;
  /** Seats the renderer may fill with occupants, driver included. */
  readonly seats: number;
  /** Doors on each side, each serving the seat behind it (a car's body model has `2 * doorsPerSide` seats). */
  readonly doorsPerSide: number;
  /** Comfortable acceleration. */
  readonly a: number;
  /** Comfortable deceleration, used by the IDM interaction term. */
  readonly b: number;
  /** Emergency deceleration, used by the safe-speed cap. */
  readonly bEmergency: number;
  /** Desired time headway, in seconds. */
  readonly T: number;
  /** Standstill gap. */
  readonly s0: number;
  /** Multiplier on the road's speed limit. */
  readonly speedFactor: number;
  readonly weight: number;
  readonly palette: readonly string[];
}

// src/sim/vehicles/archetypes.ts:236-236 function bodyClassOfArchetype
/** The conflict-zone size class a body of this archetype is measured with. */
export const bodyClassOfArchetype = (a: Archetype): BodyClass => …;
