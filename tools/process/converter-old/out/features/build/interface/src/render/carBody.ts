// What the feature uses of src/render/carBody.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/carBody.ts:16-16 type CarStyle
/** CARS, BUILT THE WAY A CAR IS DESIGNED. */

export type CarStyle = 'hatch' | 'sedan' | 'wagon' | 'suv' | 'pickup' | 'van';

// src/render/carBody.ts:18-18 function M
const M = (metres: number): number => …;

// src/render/carBody.ts:29-29 variable CARPET
const CARPET = 0x26282c;

// src/render/carBody.ts:30-30 variable DASH
const DASH = 0x1e2023;

// src/render/carBody.ts:31-31 variable SEAT_FABRIC
const SEAT_FABRIC = 0x4a4f57;

// src/render/carBody.ts:33-33 variable CARD
const CARD = 0x3b3e44;

// src/render/carBody.ts:42-54 variable CAR_PACKAGE
/** Where a person sits, relative to the seat's HIP POINT, metres */
const CAR_PACKAGE = {
  /** Hip point over the floor (the heel point's floor), every car seat. */
  hipOverFloor: 0.27,
  /** Heel point: forward of the hip and below it, on the floor. */
  heelForward: 0.62,
  /** Recline of the seat back from vertical, radians. */
  recline: 0.42,
  /** Cushion: its top sits this far under the hip point, and reaches this far forward. */
  cushionDrop: 0.1,
  cushionReach: 0.42,
  /** A rear passenger's heels: forward of their hip, on the floor under the front seat's back edge. */
  rearHeelForward: 0.43,
} as const;

// src/render/carBody.ts:61-61 variable UPRIGHT_CUSHION_REACH
/** How far an upright seat's cushion (a van's or a truck's cab, a bus's) reaches ahead of the hip point, metres */
export const UPRIGHT_CUSHION_REACH = 0.3;

// src/render/carBody.ts:147-147 type P2
type P2 = readonly [number, number];

// src/render/carBody.ts:151-158 function tint
function tint<T extends BufferGeometry>(g: T, hex: number): T;

// src/render/carBody.ts:161-188 function merge
/** Merges parts into one indexed geometry with position, normal and colour (white where none): corners equal in all three shared, so a hard edge stays split. */
function merge(parts: BufferGeometry[]): BufferGeometry;

// src/render/carBody.ts:190-191 function box
const box = (sx: number, sy: number, sz: number, x: number, y: number, z: number): BufferGeometry => …;

// src/render/carBody.ts:194-198 function bar
/** A bar between two points, `thick` square in section. */
function bar(a: Vector3, b: Vector3, thick: number, thickB = thick): BufferGeometry;

// src/render/carBody.ts:201-218 function roundedSlab
/** A rectangle rounded in plan, `height` tall, bottom at y. */
function roundedSlab(lx: number, lz: number, radius: number, height: number, x: number, y: number, z: number): BufferGeometry;

// src/render/carBody.ts:221-229 function extrude
/** A side profile extruded across, bevelled, centred on z. */
function extrude(points: readonly P2[], width: number, bevel: number, z = 0): BufferGeometry;

// src/render/carBody.ts:378-380 function seatIndex
/** The seat a door serves and a seat's number: the driver's side first, front to back, then the kerb side. */
function seatIndex(side: DoorSide, row: number, perSide: number): number;

// src/render/carBody.ts:382-958 function buildCarModel
export function buildCarModel(a: Archetype, styleName: CarStyle = (a.style ?? 'sedan') as CarStyle): VehicleModel;

// src/render/carBody.ts:963-990 function seatShell
/** A seat's side profile extruded across: cushion under the hip point, backrest behind it. */
function seatShell(hipX: number, hipY: number, floor: number, width: number, z: number, recline: number,
  backHeight: number, fabric: number, reach: number = CAR_PACKAGE.cushionReach): BufferGeometry[];

// src/render/carBody.ts:997-1003 function headrest
function headrest(x: number, y: number, z: number, width = M(0.26)): BufferGeometry[];
