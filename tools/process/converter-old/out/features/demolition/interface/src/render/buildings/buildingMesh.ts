// What the feature uses of src/render/buildings/buildingMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/buildingMesh.ts:143-143 type Rgb
type Rgb = readonly [number, number, number];

// src/render/buildings/buildingMesh.ts:146-149 interface Paint
/** A surface's colour (linear) and the finish it is drawn with. */
interface Paint {
  readonly rgb: Rgb;
  readonly finish: Finish;
}

// src/render/buildings/buildingMesh.ts:377-473 class PartRows
/** A kind's instances as the arrays of its batch (`PartBatch`), written as each is placed: DO NOT make an object of each placement (its numbers boxed, every pane, frame and chair of a job). */
class PartRows { … }

// src/render/buildings/buildingMesh.ts:383-383 field PartRows.count
count = 0;

// src/render/buildings/buildingMesh.ts:468-472 method PartRows.batch
/** The batch of what was placed, in arrays of its own (the rows are written again for the next building). */
  batch(): PartBatch;

// src/render/buildings/buildingMesh.ts:476-476 type PartSink
/** Every part kind's rows, written again for each building (`ShellWriter`). */
type PartSink = Record<PartKind, PartRows>;

// src/render/buildings/buildingMesh.ts:478-478 type FurnitureSink
/** The furniture's rows by kind, made as a kind is first placed. */
type FurnitureSink = Partial<Record<FurnitureKind, PartRows>>;

// src/render/buildings/buildingMesh.ts:666-803 class Shell
/** Triangles in THREE's axes, one buffer set per finish; winding fixed per face. */
class Shell { … }

// src/render/buildings/buildingMesh.ts:675-675 field Shell.ground
/** The ground floor's height for the building being emitted: walls darken towards it */
  ground = Number.NaN;

// src/render/buildings/buildingMesh.ts:677-677 field Shell.decay
/** How run-down the building being emitted is (`Building.decay`): stains, peeling paint, grime. */
  decay = 0;

// src/render/buildings/buildingMesh.ts:704-775 method Shell.face
/** DO NOT lay only the first two triangles: a lantern's octagonal and 24-sided caps */
  face(points: readonly (readonly [number, number, number])[], n: readonly [number, number, number], c: Paint, shades?: readonly number[]): void;

// src/render/buildings/buildingMesh.ts:805-805 type V3
type V3 = [number, number, number];

// src/render/buildings/buildingMesh.ts:847-1042 class Emitter
class Emitter { … }

// src/render/buildings/buildingMesh.ts:1458-1463 function placeFurniture
/** One piece of furniture, as an instance of its model */
function placeFurniture(e: Emitter, f: Furniture, floorZ: number): void;

// src/render/buildings/buildingMesh.ts:3814-3818 function shellChunks
/** A shell's fragments by finish: the arrays it was laid in (every face but the far level's runs with openings). */
function shellChunks(shell: Shell): Partial<Record<Finish, ShellChunk>>;

// src/render/buildings/buildingMesh.ts:3834-3834 variable SHELL_PART_MATRIX
/** One instance's matrix, written over by each instance `shellPartFaces` lays. */
const SHELL_PART_MATRIX = new Float32Array(16);
