// What the feature uses of src/render/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/utilities.ts:60-60 variable WIRE_SWING
/** How far a wire swings at mid-span in a full gust. */
const WIRE_SWING = m(0.35);

// src/render/utilities.ts:62-62 variable WIRE_RADIUS
/** Half-width of a drawn cable: thicker than life so it reads at play zoom. */
const WIRE_RADIUS = m(0.035);

// src/render/utilities.ts:64-64 variable WIRE_WIDTH
/** A drawn cable's width as the eye sees it: the mean width of the tube's square section turned every way (perimeter / pi). */
export const WIRE_WIDTH = (4 * Math.SQRT2 * WIRE_RADIUS) / Math.PI;

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

// src/render/utilities.ts:93-95 variable POLE_PART
/** The instanced parts of a pole, by index into `polePartSpecs` and `placePole`'s lists. */
const POLE_PART = {
  mast: 0, arm: 1, cap: 2, brace: 3, insulator: 4, can: 5, lampArm: 6, lampHead: 7, lens: 8, pool: 9, beam: 10,
} as const;

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

// src/render/utilities.ts:365-365 variable wireScreen
/** The screen the wires are drawn on: half its height in pixels, 0 before a layer has seen it (no fade). Set by `PoleLayer`. */
export const wireScreen = { uWirePixels: { value: 0 } };

// src/render/utilities.ts:368-416 variable WIRE_GLSL
/** A wire's vertex in the vertex shader, from its template and its instance (in three's axes) */
const WIRE_GLSL = `
        attribute vec3 aStart;
        attribute vec3 aEnd;
        attribute vec2 aSagPhase;
        vec3 wireAt(float t) {
          float u = t * 2.0 - 1.0;
          vec3 p = mix(aStart, aEnd, t);
          p.y -= aSagPhase.x * (1.0 - u * u);
          return p;
        }
        vec3 wireCorner(vec3 u, vec3 w, float c) {
          return c < 0.5 ? u : c < 1.5 ? w : c < 2.5 ? -u : -w;
        }
        // The share of its width a wire keeps where it is: all of it down to ${WIRE_FADE[0]} px wide, none at ${WIRE_FADE[1]} (pixels <= 0: all).
        float wireKeep(vec3 p, float pixels) {
          if (pixels <= 0.0) return 1.0;
          float depth = isPerspectiveMatrix(projectionMatrix) ? max(-(modelViewMatrix * vec4(p, 1.0)).z, 1e-4) : 1.0;
          float wide = ${WIRE_WIDTH.toFixed(6)} * pixels * projectionMatrix[1][1] / depth;
          return smoothstep(${WIRE_FADE[1].toFixed(6)}, ${WIRE_FADE[0].toFixed(6)}, wide);
        }
        vec3 wirePosition(out vec3 n, float pixels) {
          vec3 p = wireAt(position.x);
          float keep = wireKeep(p, pixels);
          if (position.z > 3.5) {
            // A strip across the curve's tangent, square to the ray from the eye; its normal leans out to its edges as a round cable's.
            vec3 d = aEnd - aStart;
            d.y += 4.0 * aSagPhase.x * (position.x * 2.0 - 1.0);
            d = normalize(d);
            vec3 ray = isPerspectiveMatrix(projectionMatrix) ? p - cameraPosition : -vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
            vec3 side = cross(d, ray);
            float sl = length(side);
            side = sl > 1e-6 ? side / sl : vec3(1.0, 0.0, 0.0);
            float s = position.z < 4.5 ? 1.0 : -1.0;
            n = normalize(normalize(cross(d, side)) + s * side);
            return p + side * (s * ${(WIRE_WIDTH / 2).toFixed(6)} * keep);
          }
          // The tube: the frame of the vertex's segment, from its chord.
          vec3 d = wireAt(max(position.x, position.y)) - wireAt(min(position.x, position.y));
          float dl = length(d);
          d /= dl > 0.0 ? dl : 1.0;
          vec3 u = vec3(-d.z, 0.0, d.x);
          float ul = length(u);
          u = u / (ul > 0.0 ? ul : 1.0) * ${WIRE_RADIUS.toFixed(6)};
          vec3 w = cross(u, d);
          float wl = length(w);
          w = w / (wl > 0.0 ? wl : 1.0) * ${WIRE_RADIUS.toFixed(6)};
          n = normalize(wireCorner(u, w, normal.x) + wireCorner(u, w, mod(normal.x + 1.0, 4.0)));
          return p + wireCorner(u, w, position.z) * keep;
        }`;

// src/render/utilities.ts:419-449 function wireMaterials
/** The wires' materials: lit with the wind, and the shadow's depth, both bending the template along each wire. */
function wireMaterials(): { readonly lit: MeshStandardMaterial; readonly depth: MeshDepthMaterial };

// src/render/utilities.ts:524-537 function poleGroundAt
/** Ground a pole stands on: the footway where there is one, the terrain where there is not. */
export function poleGroundAt(
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  /** The network, to stand a pole level with a footway laid flush (`footwayRiseAt`). */
  net?: Network,
): (x: number, y: number) => number;
