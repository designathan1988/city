// What the feature uses of src/render/vehicleModels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/vehicleModels.ts:29-29 type DoorSide
/** Vehicle bodies, built once per class from the class's own proportions. */

/** Side of the vehicle a door is on: -1 left (the driver's), +1 right (the kerb's). */
export type DoorSide = -1 | 1;

// src/render/vehicleModels.ts:31-51 interface DoorModel
export interface DoorModel {
  /** Index into the vehicle's door state. Two leaves of one bus door share it. */
  readonly index: number;
  readonly side: DoorSide;
  /** `hinge`: swings about a vertical axis at the front edge (cars, cabs). */
  readonly kind: 'hinge' | 'slide';
  readonly slide: number;
  /** Hinge line (or the closed leaf's origin): along the body and across it, world units. */
  readonly hingeX: number;
  readonly hingeZ: number;
  /** Door length from hinge to trailing edge, world units. */
  readonly length: number;
  /** Seat this door serves (index into `seats`), or -1. */
  readonly seat: number;
  /** Painted panel, in the door's own frame (origin on the hinge line, at road level). */
  readonly panel: BufferGeometry;
  /** The door's window, same frame. */
  readonly glass: BufferGeometry;
  /** The door card on its inside (cabin material), same frame; null on a bus leaf. */
  readonly card: BufferGeometry | null;
}

// src/render/vehicleModels.ts:53-71 interface SeatModel
export interface SeatModel {
  /** Hip point: along, across (Z, right positive), height above the road; world units. */
  readonly x: number;
  readonly z: number;
  readonly hipY: number;
  /** Headroom above the hip point to the inside of the roof, world units. */
  readonly headroom: number;
  /** Room ahead of the hip point for the legs: to the front bulkhead, or under the seat in front. */
  readonly legroom: number;
  /** Height of the cabin floor under this seat. */
  readonly floor: number;
  /** Room either side of the seat's centre line: to the door or glass, or to the next seat somebody can sit in. */
  readonly sideRoom: number;
  readonly driver: boolean;
  /** Row from the front, 0 for the driver's row: at a middle zoom only the front row is drawn. */
  readonly row: number;
  /** The posture the seat is made for (`riderPoses.ts`): `car`, reclined with the legs forward */
  readonly pose: SeatPose;
}

// src/render/vehicleModels.ts:73-73 type SeatPose
export type SeatPose = 'car' | 'cab' | 'chair';

// src/render/vehicleModels.ts:111-117 interface SteeringModel
/** Where a steering wheel sits and how it is inclined, in the vehicle frame. */
interface SteeringModel {
  readonly geometry: BufferGeometry;
  /** Seat (index into `seats`) whose occupant holds it. */
  readonly seat: number;
  /** Where the wheel is from that seat's hip point, and its tilt and radius (`riderPoses.ts`). */
  readonly wheel: WheelSpec;
}

// src/render/vehicleModels.ts:119-151 interface VehicleModel
export interface VehicleModel {
  /** Painted body with every door shut, doors and handles included. */
  readonly shell: BufferGeometry;
  /** Glazing with every door shut. */
  readonly glass: BufferGeometry;
  /** Painted body in pieces, for while a door moves: the door leaves are drawn separately. */
  readonly openShell: BufferGeometry;
  /** Fixed glazing only, same moment. */
  readonly openGlass: BufferGeometry;
  /** Unpainted exterior: bumpers, grille, wheel-arch liners, sills, mirrors' glass. */
  readonly trim: BufferGeometry;
  /** Cabin, doors shut: seats, dashboard, carpet, door cards. */
  readonly interior: BufferGeometry;
  /** The interior's parts by name: what each range of `interior`'s vertices is (`interiorSolids.ts`). */
  readonly solids: readonly InteriorSolid[];
  /** Cabin without the door cards, which then travel with their leaves. */
  readonly openInterior: BufferGeometry;
  /** The panel of the roof that may be glass (a panoramic roof), or null. */
  readonly roof: BufferGeometry | null;
  /** A second body colour: a truck's box. */
  readonly accent: BufferGeometry | null;
  /** One opaque, low-poly geometry for the far zoom: body, dark glass and wheels. */
  readonly far: BufferGeometry;
  readonly steering: SteeringModel | null;
  readonly doors: readonly DoorModel[];
  readonly seats: readonly SeatModel[];
  /** Lamps, as boxes in the vehicle frame: centre and size, world units. */
  readonly headlamps: readonly Lamp[];
  readonly taillamps: readonly Lamp[];
  readonly indicators: readonly (Lamp & { readonly side: DoorSide; readonly front: boolean })[];
  /** Number plates, and on a bus the destination blind. */
  readonly plates: readonly Lamp[];
}

// src/render/vehicleModels.ts:153-160 interface Lamp
export interface Lamp {
  readonly x: number;
  readonly y: number;
  readonly z: number;
  readonly sx: number;
  readonly sy: number;
  readonly sz: number;
}

// src/render/vehicleModels.ts:221-228 function merge
/** Merges parts into one indexed geometry with position, normal and colour: corners equal in all three shared, so a hard edge stays split. */
export function merge(parts: BufferGeometry[]): BufferGeometry;

// src/render/vehicleModels.ts:535-555 interface TwoWheelerModel
/** A motorcycle or a bicycle, in the same frame as a car, with the parts that move kept apart */
export interface TwoWheelerModel {
  /** Frame, tank, bodywork: painted. */
  readonly body: BufferGeometry;
  /** Seat, engine, exhaust, mudguards: unpainted. */
  readonly trim: BufferGeometry;
  /** Fork, handlebar and front lamp, in a frame whose origin is the steering head. */
  readonly steering: BufferGeometry;
  /** Where the steering head is: along the body, and its height. */
  readonly headX: number;
  readonly headY: number;
  /** Cranks and pedals, about the bottom bracket (bicycles only). */
  readonly cranks: BufferGeometry | null;
  readonly bracketX: number;
  readonly bracketY: number;
  /** Where the rider's pelvis bone sits: along the body and height above the road. */
  readonly seatX: number;
  readonly seatY: number;
  readonly fit: TwoWheelerFit;
  readonly headlamp: Lamp;
  readonly taillamp: Lamp;
}
