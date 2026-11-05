// What the feature uses of src/render/destruction.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/destruction.ts:22-22 variable COLLAPSE_BELOW
/** Share of the fragments left standing below which the rest comes down. */
const COLLAPSE_BELOW = 0.35;

// src/render/destruction.ts:24-38 interface Piece
interface Piece {
  readonly fragment: Fragment;
  /** Its index among its ruin's fragments (`StillData.slots`, `fragments`). */
  readonly index: number;
  /** Its own mesh, in the scene only while it moves (else drawn in its ruin's `still`), with a geometry of its own only then */
  readonly mesh: Mesh;
  readonly floor: number;
  falling: boolean;
  settled: boolean;
  /** Its ranges in the ruin's `still` mesh: (start, vertex count, material) triples. */
  readonly slots: Int32Array;
  v: Vector3;
  w: Vector3;
  age: number;
}

// src/render/destruction.ts:40-48 interface Ruin
interface Ruin {
  readonly pieces: Piece[];
  readonly floor: number;
  /** Every piece that does not move (standing, or landed), in one mesh; a piece in the air is blanked in it. */
  readonly still: Mesh;
  /** The buffer the still mesh draws: a piece's own geometry is cut from it when it flies. */
  readonly data: StillData;
  standing: number;
}

// src/render/destruction.ts:53-65 interface Destruction
export interface Destruction {
  readonly group: Group;
  /** Buildings now drawn as fragments here (and not by the building layer). */
  readonly ruined: ReadonlySet<number>;
  /** A blow on building `b` (its floor at `floor`) at world (x, y, z), force 1..10. */
  hit(b: Building, floor: number, x: number, y: number, z: number, strength: number, eye?: Vector3): boolean;
  /** Called when a building comes down later than its blow (its pieces were still being made in the worker). */
  onDown: ((id: number) => void) | null;
  /** Called when a building's pieces are ready and stand in for it (the layer stops drawing it). */
  onRuined: (() => void) | null;
  update(dt: number): void;
  dispose(): void;
}

// src/render/destruction.ts:67-409 function createDestruction
export function createDestruction(
  exhaust: Exhaust,
  chunkOf: (b: Building) => { chunk: BuildingChunk; kit: BuildingKit } | null,
): Destruction;
