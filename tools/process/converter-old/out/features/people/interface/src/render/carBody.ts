// What the feature uses of src/render/carBody.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/carBody.ts:16-16 type CarStyle
/** CARS, BUILT THE WAY A CAR IS DESIGNED. */

export type CarStyle = 'hatch' | 'sedan' | 'wagon' | 'suv' | 'pickup' | 'van';

// src/render/carBody.ts:18-18 function M
const M = (metres: number): number => …;

// src/render/carBody.ts:30-30 variable DASH
const DASH = 0x1e2023;

// src/render/carBody.ts:147-147 type P2
type P2 = readonly [number, number];

// src/render/carBody.ts:151-158 function tint
function tint<T extends BufferGeometry>(g: T, hex: number): T;

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

// src/render/carBody.ts:382-958 function buildCarModel
export function buildCarModel(a: Archetype, styleName: CarStyle = (a.style ?? 'sedan') as CarStyle): VehicleModel;

// src/render/carBody.ts:1006-1018 function steeringWheel
/** A steering wheel: rim, three spokes and a boss, axis along X, centred on the origin. */
function steeringWheel(radius: number): BufferGeometry;
