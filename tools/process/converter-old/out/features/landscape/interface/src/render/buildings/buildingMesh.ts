// What the feature uses of src/render/buildings/buildingMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/buildingMesh.ts:377-473 class PartRows
/** A kind's instances as the arrays of its batch (`PartBatch`), written as each is placed: DO NOT make an object of each placement (its numbers boxed, every pane, frame and chair of a job). */
class PartRows { … }

// src/render/buildings/buildingMesh.ts:476-476 type PartSink
/** Every part kind's rows, written again for each building (`ShellWriter`). */
type PartSink = Record<PartKind, PartRows>;

// src/render/buildings/buildingMesh.ts:666-803 class Shell
/** Triangles in THREE's axes, one buffer set per finish; winding fixed per face. */
class Shell { … }

// src/render/buildings/buildingMesh.ts:2151-2151 variable LAWN_LATTICE
/** The open blocks of a building, laid on the ground */
/** The grid the terrain is drawn on, whose triangles a lawn's parts are seated over (`lawnGround.ts`). */
const LAWN_LATTICE = { cell: TERRAIN_CELL, half: TERRAIN_HALF } as const;

// src/render/buildings/buildingMesh.ts:2154-2332 function emitLots
/** `entrances`: the building's ways in as its body's emission worked them out (`emitBuilding`): DO NOT work its foundation out again for the flights' feet. */
function emitLots(b: Building, lots: readonly Volume[], withParts: boolean, groundAt: GroundAt, shell: Shell,
  parts: PartSink, buildingFloor?: number, pavedAt?: PavedAt,
  onLot?: (el: BuildingElement) => boolean, entrances?: readonly Entrance[]): LotGround | undefined;

// src/render/buildings/buildingMesh.ts:3716-3729 class ShellWriter
/** DO NOT grow a new shell's arrays for each building of a job from nothing (every one doubling its way up, the half let go, its chunk holding views of arrays twice its size): one shell written again for each, the arrays grown once to the largest, each chunk an exact copy. */
export class ShellWriter { … }
