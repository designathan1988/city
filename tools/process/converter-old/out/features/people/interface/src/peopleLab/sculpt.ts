// What the feature uses of src/peopleLab/sculpt.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/sculpt.ts:16-21 interface HairSculpt
/** A sculpt, saved: the base item and its vertices' offset moves (`encodeSculpt`). */
export interface HairSculpt {
  /** The hairstyle sculpted: a catalogue item or a grown `proc:` one. */
  readonly base: string;
  /** Base64 of [count u32][index u32, dx dy dz int16 in 1/4096 offset units] for every vertex moved. */
  readonly deltas: string;
}

// src/peopleLab/sculpt.ts:43-56 function decodeSculpt
export function decodeSculpt(sculpt: HairSculpt, vertexCount: number): Float32Array;

// src/peopleLab/sculpt.ts:59-63 function sculptedPack
/** The item with the sculpt's offsets added: the same refs and weights, so it is fitted to any body as the game fits it (`fitProxy`). */
export function sculptedPack(pack: ProxyPack, delta: Float32Array): ProxyPack;

// src/peopleLab/sculpt.ts:66-73 function axisScales
/** `fitProxy`'s axis scales on this body: a position move of d decimetres is an offset move of d / scale. */
function axisScales(pack: ProxyPack, base: Float32Array): [number, number, number];

// src/peopleLab/sculpt.ts:75-81 interface SculptBody
export interface SculptBody {
  /** The person's base mesh as shaped (decimetres), the body's quads, and which of them are skin. */
  readonly positions: Float32Array;
  readonly faces: Uint16Array;
  readonly faceGroup: Uint8Array;
  readonly skinGroup: number;
}

// src/peopleLab/sculpt.ts:83-455 class HairSculptor
export class HairSculptor { … }

// src/peopleLab/sculpt.ts:200-204 method HairSculptor.delta
/** The sculpt as moves of the item's offsets. */
  delta(): Float32Array;

// src/peopleLab/sculpt.ts:211-215 method HairSculptor.reset
reset(): void;

// src/peopleLab/sculpt.ts:242-280 method HairSculptor.refresh
private refresh(): void;
