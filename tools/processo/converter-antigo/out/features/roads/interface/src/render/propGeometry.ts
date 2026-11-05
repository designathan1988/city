// What the feature uses of src/render/propGeometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/propGeometry.ts:29-29 type Rgb
/** Procedural geometry for everything that stands on the ground. */

type Rgb = readonly [number, number, number];

// src/render/propGeometry.ts:36-40 interface Placement
interface Placement {
  readonly at?: readonly [number, number, number];
  readonly rotate?: readonly [number, number, number];
  readonly scale?: readonly [number, number, number];
}

// src/render/propGeometry.ts:46-91 function part
/** Normalises a primitive into the shared layout - indexed, position, normal and colour only. A colour read per face (it takes
 *  `face`) keeps each triangle's own corners, so each triangle is speckled on its own as before; every other part keeps its shared corners
 *  (a box 24 corners, not 36; a drum's side one ring a row): the same triangles, a third fewer vertices to shade
 *  ([meshoptimizer: vertex reuse](https://github.com/zeux/meshoptimizer#vertex-cache-optimization)). */
function part(
  geometry: BufferGeometry,
  /** A flat colour, or one per vertex given its position, normal and face centre. */
  color: Rgb | ((p: Vector3, n: Vector3, face: Vector3) => Rgb),
  placement: Placement = {},
): BufferGeometry;

// src/render/propGeometry.ts:95-106 function merge
/** The parts merged, the faces no one sees taken out first (`hiddenFaces.ts`): a leg's foot on the ground, a bar's underside in the soil,
 *  a plate's back against its box. `floor`: the model stands on y = 0 (a drain lies below it, in the gutter). */
function merge(parts: BufferGeometry[], floor = true): BufferGeometry;

// src/render/propGeometry.ts:137-140 function faceNoise
/** Per-face hash, so a crown is speckled leaf by leaf rather than smooth. */
function faceNoise(p: Vector3, seed: number): number;

// src/render/propGeometry.ts:144-144 type TreeSpecies
export type TreeSpecies = 'broadleaf' | 'broadleafTall' | 'conifer' | 'ipeYellow' | 'ipePink';

// src/render/propGeometry.ts:145-145 variable TREE_SPECIES
export const TREE_SPECIES: readonly TreeSpecies[] = ['broadleaf', 'broadleafTall', 'conifer', 'ipeYellow', 'ipePink'];

// src/render/propGeometry.ts:157-157 type BushKind
export type BushKind = 'bush' | 'bushFlowering' | 'hedge';

// src/render/propGeometry.ts:158-158 variable BUSH_KINDS
export const BUSH_KINDS: readonly BushKind[] = ['bush', 'bushFlowering', 'hedge'];

// src/render/propGeometry.ts:233-233 variable LAMP_HEIGHT
/** A residential lighting column is 8 to 10 m with a 1.5 to 2.5 m outreach. */
export const LAMP_HEIGHT = m(9);

// src/render/propGeometry.ts:234-234 variable LAMP_OUTREACH
export const LAMP_OUTREACH = m(2.1);

// src/render/propGeometry.ts:405-424 function drainGeometry
/** A kerb inlet ("boca de lobo"): an iron grate in the gutter, in front of a dark mouth cut into the kerb. */
export function drainGeometry(): BufferGeometry;

// src/render/propGeometry.ts:427-447 function treePitGeometry
/** A tree pit in the footway: a square of dark soil behind a steel grate, root at the footway surface. */
export function treePitGeometry(): BufferGeometry;
