// What the feature uses of src/render/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/utilities.ts:64-64 variable WIRE_WIDTH
/** A drawn cable's width as the eye sees it: the mean width of the tube's square section turned every way (perimeter / pi). */
export const WIRE_WIDTH = (4 * Math.SQRT2 * WIRE_RADIUS) / Math.PI;

// src/render/utilities.ts:66-66 variable WIRE_TUBE_PIXELS
/** A cable at least this many pixels wide is drawn as the tube; a thinner one as a strip square to the eye. */
const WIRE_TUBE_PIXELS = 1.5;

// src/render/utilities.ts:68-68 variable WIRE_FADE
/** A cable keeps its width down to the first of these, in pixels, and none of it at the second: its coverage fades out (`WIRE_GLSL`). */
export const WIRE_FADE = [1 / 8, 1 / 32] as const;

// src/render/utilities.ts:73-77 interface Utilities
/** The overhead utility network on screen: poles, cross-arms, wires and lamps. */

export interface Utilities {
  readonly group: Object3D;
  readonly triangles: number;
  dispose(): void;
}

// src/render/utilities.ts:79-90 interface Placement
export interface Placement {
  x: number;
  y: number;
  /** Height above the ground, in world units. */
  z: number;
  yaw: number;
  /** Tilt of the part's local X axis up from level, radians (a brace, a lamp bracket). */
  roll?: number;
  sx: number;
  sy: number;
  sz: number;
}

// src/render/utilities.ts:96-96 variable POLE_PART_COUNT
export const POLE_PART_COUNT = 11;

// src/render/utilities.ts:99-102 interface PartLevel
/** A shape a part may be drawn with, and how far (world units) its outline strays from the part's own. */
export interface PartLevel {
  readonly geometry: BufferGeometry;
  readonly error: number;
}

// src/render/utilities.ts:105-118 interface PolePartSpec
/** One instanced part: its mesh's name, shape and material, and how it takes light and shadow. */
export interface PolePartSpec {
  readonly name: string;
  readonly geometry: BufferGeometry;
  readonly material: Material;
  readonly castShadow: boolean;
  readonly receiveShadow: boolean;
  readonly renderOrder: number;
  /** The light a lamp throws on the street (pool, beam): drawn at any size, seen at night from across the map. */
  readonly glows: boolean;
  /** How many times a lit surface's brightness it shines: a lens lit at night is seen smaller than a pixel (coverage times brightness). */
  readonly brightness: () => number;
  /** Its shapes, finest first: the first is `geometry`, the others coarser (`PoleLayer` picks by the error on the screen). */
  readonly levels: readonly PartLevel[];
}

// src/render/utilities.ts:123-147 function polePartSpecs
/** The parts in `POLE_PART` order: the catalogue's shapes and materials, the lamps' light from the scenery's kit. */
export function polePartSpecs(kit: SceneryKit): readonly PolePartSpec[];

// src/render/utilities.ts:125-126 function polePartSpecs.solid
const solid = (name: string, geometry: BufferGeometry, material: Material, levels: readonly PartLevel[] = [{ geometry, error: 0 }]): PolePartSpec => …;

// src/render/utilities.ts:127-128 function polePartSpecs.light
const light = (name: string, geometry: BufferGeometry, material: Material, renderOrder: number): PolePartSpec => …;

// src/render/utilities.ts:155-237 function placePole
/** One pole's parts, pushed onto `out` by `POLE_PART` */
export function placePole(
  pole: UtilityPole,
  top: number,
  frame: PoleArms,
  kerb: { x: number; y: number } | null,
  groundAt: (x: number, y: number) => number,
  out: Placement[][],
): void;

// src/render/utilities.ts:239-239 variable placing
const placing = new Object3D();

// src/render/utilities.ts:250-250 variable SPAN_WIRES
/** Wires a span carries: every course and offset of the arms. */
export const SPAN_WIRES = WIRE_COURSES.length * WIRE_OFFSETS.length;

// src/render/utilities.ts:252-252 variable WIRE_STRIDE
/** Numbers a wire's instance holds: its two ends in three's axes, its sag, its phase in the wind. */
export const WIRE_STRIDE = 8;

// src/render/utilities.ts:255-276 function spanWires
/** DO NOT build the wires' vertices on the CPU again. */
export function spanWires(
  span: UtilitySpan,
  a: { readonly x: number; readonly y: number; readonly top: number; readonly arm: { x: number; y: number } },
  b: { readonly x: number; readonly y: number; readonly top: number; readonly arm: { x: number; y: number } },
  out: Float32Array,
  at: number,
): void;

// src/render/utilities.ts:279-279 variable WIRE_HALF_WIDTH
/** Half-width of a drawn cable (`WIRE_GLSL`), for the wires' bounds. */
export const WIRE_HALF_WIDTH = WIRE_RADIUS;

// src/render/utilities.ts:282-288 interface WireTemplate
/** One way of drawing a wire, shared by every cell's wires: a template of (t along the wire, t at the other end of its segment, corner). */
interface WireTemplate {
  readonly position: BufferAttribute;
  readonly normal: BufferAttribute;
  readonly index: BufferAttribute;
  /** Chords it bends the wire into. */
  readonly segments: number;
}

// src/render/utilities.ts:328-328 variable WIRE_TEMPLATES
/** The ways of drawing a wire, finest first: the tube, then strips of 8, 4, 2 and 1 chords. Made once, at load. */
const WIRE_TEMPLATES: readonly WireTemplate[] = [tubeTemplate(), stripTemplate(8), stripTemplate(4), stripTemplate(2), stripTemplate(1)];

// src/render/utilities.ts:365-365 variable wireScreen
/** The screen the wires are drawn on: half its height in pixels, 0 before a layer has seen it (no fade). Set by `PoleLayer`. */
export const wireScreen = { uWirePixels: { value: 0 } };

// src/render/utilities.ts:452-467 interface UtilityParts
/** The pole layer's materials and shapes, one set for every build (the built layer and the run's preview). */
interface UtilityParts {
  readonly metal: MeshStandardMaterial;
  readonly timber: MeshStandardMaterial;
  readonly housing: MeshStandardMaterial;
  readonly wireMaterial: MeshStandardMaterial;
  readonly wireDepth: MeshDepthMaterial;
  readonly porcelain: MeshStandardMaterial;
  readonly steel: MeshStandardMaterial;
  /** The mast, finest first (`mastShape`). */
  readonly mastLevels: readonly PartLevel[];
  readonly capGeometry: CylinderGeometry;
  readonly insulatorGeometry: BufferGeometry;
  readonly canGeometry: CylinderGeometry;
  readonly boxGeometry: BoxGeometry;
  readonly lensGeometry: BufferGeometry;
}

// src/render/utilities.ts:587-587 variable PARTS
/** DO NOT make them lazily on the first build again: that is a material made in a frame. */
const PARTS: UtilityParts = makeUtilityParts();
