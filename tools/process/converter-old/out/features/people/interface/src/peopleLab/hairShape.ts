// What the feature uses of src/peopleLab/hairShape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/hairShape.ts:10-17 interface HairShape
/** HAIR AND CLOTHES SHAPED BY PARAMETERS: a catalogue hairstyle (hand-made, its quality kept) lengthened or shortened, given volume and waves by sliders, the
 *  way the parametric character creators do it - Daz's and Character Creator's per-hairstyle morphs, inZOI's root-to-tip length - computed here
 *  from the piece itself, so every catalogue hairstyle has them. Garments take the same two of them: their length below the waist (Mutable's
 *  clip-and-morph does a hem so) and their looseness (Roblox's puffiness). The result is a move of the item's own offsets (as the sculptor's), so
 *  it fits any body as the game fits the item. */

export interface HairShape {
  /** Length of the hanging part: 1 as made, below 1 shorter, above longer. */
  readonly length: number;
  /** Volume (dm): the hair stood off the head, more towards the ends. */
  readonly volume: number;
  /** Waves (dm): side to side along the hanging part, growing from the root. */
  readonly wave: number;
}

// src/peopleLab/hairShape.ts:22-22 variable WAVE_PERIOD
/** One wave along the hair (dm). */
const WAVE_PERIOD = 0.9;

// src/peopleLab/hairShape.ts:24-26 function isPlain
export function isPlain(s: HairShape | undefined): boolean;

// src/peopleLab/hairShape.ts:30-89 function shapedPack
/** The item with the shape applied on this body (`base`: the base mesh as shaped, dm); `outOfBody` pushes a moved point back out of the skin (a
 *  skirt made longer over the knees, hair made longer over the shoulders, a closer fit). */
export function shapedPack(pack: ProxyPack, base: Float32Array, s: HairShape, outOfBody?: (p: number[]) => void, rebindTo?: readonly number[], garment = false): ProxyPack;

// src/peopleLab/hairShape.ts:92-99 function axisScales
/** `fitProxy`'s axis scales on this body: a position move of d dm is an offset move of d / scale. */
function axisScales(pack: ProxyPack, base: Float32Array): [number, number, number];
