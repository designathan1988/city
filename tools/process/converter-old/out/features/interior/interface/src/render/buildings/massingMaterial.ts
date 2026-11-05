// What the feature uses of src/render/buildings/massingMaterial.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/massingMaterial.ts:11-11 variable ROOM_SEEN_GLSL
/** The room seen through a pane by day, as the detail shows it once the pane is a few pixels (`glassRoom.ts` paneTiny's mean), lit by `roomDaylight`. */
export const ROOM_SEEN_GLSL = 'vec3(0.42, 0.45, 0.47)';

// src/render/buildings/massingMaterial.ts:13-13 variable SILL
/** Where a window sits in its storey, from the floor (shares of the storey height). */
const SILL = 0.3;

// src/render/buildings/massingMaterial.ts:14-14 variable HEAD
const HEAD = 0.85;

// src/render/buildings/massingMaterial.ts:16-23 interface MassingUniforms
export interface MassingUniforms {
  /** Share of rooms lit at this hour (`kit.ts` `awakeShare`). */
  readonly roomsAwake: { value: number };
  /** How bright a lit window glows (0 by day), as the detailed panes' `emissiveIntensity`. */
  readonly massingNight: { value: number };
  /** The share of the room behind a pane seen by day (`kit.ts` roomDaylight), as the detailed panes show it. */
  readonly roomDaylight?: { value: number };
}

// src/render/buildings/massingMaterial.ts:29-50 function PULSE_GLSL
/** GLSL `<prefix>Pulse(x, w, a0, a1)`: a pulse [a0, a1) repeated every 1 seen through a pixel whose footprint is w periods, and `<prefix>Footprint(cell)`, that footprint per axis.
 * A tent filter of radius w (the pulse's second integral, Inigo Quilez "filterable procedurals"): its response sinc squared, the first side lobe at -26.5 dB against a box's -13 dB,
 * so the windows stay drawn while a bay holds 2 pixels and do not shimmer. DO NOT fade to the mean from 4 pixels a bay: the facades went flat and soft at mid distance (the player, 2026-10-23).
 * Taken on fract(x) (the filtered pulse repeats with it): the second integral of a far bay index loses the float's precision. Under 0.05 a pixel a period, the box of the first integral (a bay of 20 pixels rings at nothing; the tent's second difference would lose the float there). */
export function PULSE_GLSL(prefix: string): string;

// src/render/buildings/massingMaterial.ts:59-126 function createMassingMaterial
/** The masses' material; `base`, the top level's (`massBase.ts`): no normal and no facade of its own - each face shaded by its own slope (three's `flatShading`), each triangle in its last corner's colour (a `flat` varying), no window drawn (their mean is in the wall's colour). */
export function createMassingMaterial(uniforms: MassingUniforms, base = false): MeshStandardMaterial;
