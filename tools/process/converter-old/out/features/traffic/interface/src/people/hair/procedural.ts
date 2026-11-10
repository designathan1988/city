// What the feature uses of src/people/hair/procedural.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/hair/procedural.ts:6-39 interface HairStyle
/** Procedural hairstyles, as MakeHuman hair items */

export interface HairStyle {
  readonly name: string;
  /** Strand length from the root, decimetres. */
  readonly length: number;
  /** The parting across the head, -1 (right) .. 1 (left); 0 a centre parting. */
  readonly part: number;
  /** How far the hair stands off the skull, decimetres. */
  readonly volume: number;
  /** How hard each step bends down, 0..1. */
  readonly gravity: number;
  readonly wave?: { readonly amplitude: number; readonly wavelength: number };
  readonly fringe?: { readonly length: number; readonly width: number };
  /** Gathered at a point of the head, then a tail (plaited when `braid`) or a bun; at two points */
  readonly gather?: { readonly at: readonly [number, number, number]; readonly tail: number; readonly bun?: number; readonly braid?: boolean; readonly twin?: boolean };
  /** Each strand wound in a helix: corkscrew curls. */
  readonly curl?: { readonly radius: number; readonly pitch: number };
  /** All the hair brushed towards one side, -1 (right) .. 1 (left). */
  readonly sweep?: number;
  /** Brushed straight back from the brow (a slick). */
  readonly back?: boolean;
  /** The front lifted up and back off the brow (a quiff), decimetres. */
  readonly quiff?: number;
  /** Worn with a headband (`acc:headband`, `generateHeadband`). */
  readonly headband?: boolean;
  /** Length by region of the head, as multipliers of `length` - front, crown, temples, back of the head, nape */
  readonly regions?: { readonly front?: number; readonly crown?: number; readonly temple?: number; readonly occipital?: number; readonly nape?: number };
  /** Guides (clumps), and the width of each clump's card, decimetres. */
  readonly clumps: number;
  readonly cardWidth: number;
  /** Two cards a clump for body; short crops lie flat with one. */
  readonly layers: 1 | 2;
  /** The texture's strands: 'straight' or 'wavy'. */
  readonly strands: 'straight' | 'wavy';
}

// src/people/hair/procedural.ts:42-71 variable HAIR_STYLES
/** The styles by name; the hair editor (`people-lab.html?editor`) adds its own. */
export const HAIR_STYLES: Record<string, HairStyle> = {
  longStraight: { name: 'longStraight', length: 4.6, part: 0.25, volume: 0.1, gravity: 0.55, clumps: 230, cardWidth: 0.42, layers: 2, strands: 'straight' },
  longWavy: { name: 'longWavy', length: 4.4, part: -0.3, volume: 0.16, gravity: 0.5, wave: { amplitude: 0.07, wavelength: 1.3 }, clumps: 230, cardWidth: 0.44, layers: 2, strands: 'wavy' },
  midLayered: { name: 'midLayered', length: 3.0, part: 0.35, volume: 0.14, gravity: 0.5, wave: { amplitude: 0.06, wavelength: 1.4 }, clumps: 220, cardWidth: 0.42, layers: 2, strands: 'straight' },
  bob: { name: 'bob', regions: { front: 0.95, crown: 0.95, temple: 1.05, occipital: 1, nape: 0.85 }, length: 1.9, part: 0.3, volume: 0.12, gravity: 0.6, clumps: 220, cardWidth: 0.4, layers: 2, strands: 'straight' },
  bobFringe: { name: 'bobFringe', length: 1.8, part: 0, volume: 0.12, gravity: 0.6, fringe: { length: 1.25, width: 0.55 }, clumps: 220, cardWidth: 0.4, layers: 2, strands: 'straight' },
  ponytail: { name: 'ponytail', length: 3.4, part: 0, volume: 0.05, gravity: 0.5, gather: { at: [0, 7.75, -0.55], tail: 3.2 }, clumps: 200, cardWidth: 0.36, layers: 2, strands: 'straight' },
  bun: { name: 'bun', length: 2.0, part: 0, volume: 0.05, gravity: 0.5, gather: { at: [0, 8.05, -0.45], tail: 0, bun: 0.42 }, clumps: 200, cardWidth: 0.36, layers: 2, strands: 'straight' },
  ponytailFringe: { name: 'ponytailFringe', length: 3.4, part: 0, volume: 0.05, gravity: 0.5, fringe: { length: 1.2, width: 0.55 }, gather: { at: [0, 7.75, -0.55], tail: 3.2 }, clumps: 210, cardWidth: 0.34, layers: 2, strands: 'straight' },
  longFringe: { name: 'longFringe', length: 4.6, part: 0, volume: 0.1, gravity: 0.55, fringe: { length: 1.25, width: 0.55 }, clumps: 240, cardWidth: 0.4, layers: 2, strands: 'straight' },
  longHeadband: { name: 'longHeadband', length: 4.6, part: 0, volume: 0.08, gravity: 0.55, back: true, headband: true, clumps: 230, cardWidth: 0.42, layers: 2, strands: 'straight' },
  shoulderBob: { name: 'shoulderBob', length: 2.6, part: 0.25, volume: 0.12, gravity: 0.6, clumps: 230, cardWidth: 0.42, layers: 2, strands: 'straight' },
  fade: { name: 'fade', regions: { front: 1.1, crown: 1, temple: 0.35, occipital: 0.5, nape: 0.25 }, length: 0.75, part: 0.3, volume: 0.06, gravity: 0.1, clumps: 220, cardWidth: 0.32, layers: 1, strands: 'straight' },
  buzz: { name: 'buzz', length: 0.22, part: 0, volume: 0.02, gravity: 0, back: true, clumps: 220, cardWidth: 0.34, layers: 1, strands: 'straight' },
  slickedBack: { name: 'slickedBack', length: 1.5, part: 0, volume: 0.06, gravity: 0.15, back: true, clumps: 200, cardWidth: 0.36, layers: 1, strands: 'straight' },
  quiff: { name: 'quiff', regions: { front: 1.4, crown: 1.05, temple: 0.45, occipital: 0.6, nape: 0.35 }, length: 0.9, part: 0.3, volume: 0.07, gravity: 0.1, quiff: 0.35, clumps: 220, cardWidth: 0.34, layers: 1, strands: 'straight' },
  manBun: { name: 'manBun', length: 1.8, part: 0, volume: 0.05, gravity: 0.4, back: true, gather: { at: [0, 8.1, -0.5], tail: 0, bun: 0.32 }, clumps: 200, cardWidth: 0.34, layers: 2, strands: 'straight' },
  longMale: { name: 'longMale', length: 3.0, part: 0.1, volume: 0.1, gravity: 0.55, clumps: 220, cardWidth: 0.4, layers: 2, strands: 'straight' },
  curlyShort: { name: 'curlyShort', length: 0.8, part: 0, volume: 0.16, gravity: 0.1, curl: { radius: 0.06, pitch: 0.22 }, clumps: 260, cardWidth: 0.24, layers: 1, strands: 'wavy' },
  afro: { name: 'afro', length: 1.5, part: 0, volume: 0.85, gravity: 0.02, curl: { radius: 0.09, pitch: 0.24 }, clumps: 300, cardWidth: 0.26, layers: 2, strands: 'wavy' },
  shortCrop: { name: 'shortCrop', regions: { front: 1.2, crown: 1, temple: 0.55, occipital: 0.7, nape: 0.45 }, length: 0.55, part: 0.4, volume: 0.05, gravity: 0.1, clumps: 160, cardWidth: 0.34, layers: 1, strands: 'straight' },
  pigtails: { name: 'pigtails', length: 2.6, part: 0, volume: 0.05, gravity: 0.5, gather: { at: [0.62, 7.55, -0.2], tail: 2.4, twin: true }, clumps: 200, cardWidth: 0.34, layers: 2, strands: 'straight' },
  braid: { name: 'braid', length: 3.4, part: 0, volume: 0.05, gravity: 0.5, gather: { at: [0, 7.45, -0.6], tail: 3.6, braid: true }, clumps: 200, cardWidth: 0.32, layers: 2, strands: 'straight' },
  twinBraids: { name: 'twinBraids', length: 3, part: 0, volume: 0.05, gravity: 0.5, gather: { at: [0.6, 7.3, -0.25], tail: 3.2, braid: true, twin: true }, clumps: 200, cardWidth: 0.32, layers: 2, strands: 'straight' },
  curly: { name: 'curly', length: 2.3, part: 0, volume: 0.45, gravity: 0.3, curl: { radius: 0.16, pitch: 0.45 }, clumps: 260, cardWidth: 0.3, layers: 2, strands: 'wavy' },
  sideSwept: { name: 'sideSwept', length: 3.6, part: 0.85, volume: 0.12, gravity: 0.5, sweep: 0.7, clumps: 230, cardWidth: 0.42, layers: 2, strands: 'straight' },
  pixie: { name: 'pixie', length: 0.75, part: 0.6, volume: 0.1, gravity: 0.15, fringe: { length: 0.9, width: 0.6 }, sweep: 0.5, clumps: 220, cardWidth: 0.32, layers: 1, strands: 'straight' },
  topKnot: { name: 'topKnot', length: 2.2, part: 0, volume: 0.05, gravity: 0.4, gather: { at: [0, 8.55, 0.15], tail: 0, bun: 0.38 }, clumps: 200, cardWidth: 0.34, layers: 2, strands: 'straight' },
  shortSide: { name: 'shortSide', regions: { front: 1.25, crown: 1.05, temple: 0.55, occipital: 0.72, nape: 0.42 }, length: 0.95, part: 0.5, volume: 0.08, gravity: 0.15, clumps: 160, cardWidth: 0.34, layers: 1, strands: 'straight' },
};

// src/people/hair/procedural.ts:74-76 variable FEMALE_HAIR
/** Drawn for the crowd: straight hair only (the player, 2026-10-05: "make the hair straight, no afro") */
export const FEMALE_HAIR = ['longStraight', 'midLayered', 'bob', 'bobFringe', 'ponytail', 'bun',
  'pigtails', 'braid', 'twinBraids', 'sideSwept', 'pixie', 'topKnot',
  'ponytailFringe', 'longFringe', 'longHeadband', 'shoulderBob'] as const;

// src/people/hair/procedural.ts:77-77 variable MALE_HAIR
export const MALE_HAIR = ['shortCrop', 'shortSide', 'fade', 'buzz', 'slickedBack', 'quiff', 'manBun', 'longMale'] as const;

// src/people/hair/procedural.ts:79-89 interface HairBase
export interface HairBase {
  /** The base mesh, decimetres. */
  readonly positions: Float32Array;
  readonly vertexCount: number;
  readonly bodyRange: readonly (readonly [number, number])[];
  readonly joints: Uint8Array;
  readonly weights: Uint16Array;
  readonly boneNames: readonly string[];
  /** Quads (a, b, c, d), d === c for a triangle. */
  readonly faces: Uint16Array;
}

// src/people/hair/procedural.ts:246-612 function generateHair
export function generateHair(style: HairStyle, base: HairBase, seed = 1, strands = false): ProxyPack;

// src/people/hair/procedural.ts:615-638 function generateHeadband
/** A headband: a band over the crown from ear to ear, a little forward of the top, standing on the hair */
export function generateHeadband(base: HairBase): ProxyPack;
