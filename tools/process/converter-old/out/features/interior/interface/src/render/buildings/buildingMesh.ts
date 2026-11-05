// What the feature uses of src/render/buildings/buildingMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/buildingMesh.ts:128-134 interface BuildingMeshes
/** Buildings, as meshes. */

export interface BuildingMeshes {
  readonly group: Group;
  readonly triangles: number;
  /** Shown or hidden as a whole. */
  setVisible(visible: boolean): void;
  dispose(): void;
}

// src/render/buildings/buildingMesh.ts:143-143 type Rgb
type Rgb = readonly [number, number, number];

// src/render/buildings/buildingMesh.ts:146-149 interface Paint
/** A surface's colour (linear) and the finish it is drawn with. */
interface Paint {
  readonly rgb: Rgb;
  readonly finish: Finish;
}

// src/render/buildings/buildingMesh.ts:157-157 function paint
/** A building paint in its own colour: held to 70 % chroma the town read grey (the player, 2026-10-09). */
const paint = (m: MaterialSpec, shade = 1): Paint => …;

// src/render/buildings/buildingMesh.ts:166-166 variable WHITE_RGB
const WHITE_RGB: Rgb = [1, 1, 1];

// src/render/buildings/buildingMesh.ts:179-179 variable FLAT_DEPTH
/** The deepest opening the far level lays flat (its error, `massCells.ts` `FACADE_ERROR`): a loggia's deeper recess keeps its geometry. */
export const FLAT_DEPTH = m(0.3);

// src/render/buildings/buildingMesh.ts:197-197 variable PLINTH_RISE
/** How far a plinth's ledge and a door's threshold stand over the floor */
const PLINTH_RISE = LANDING_RISE;

// src/render/buildings/buildingMesh.ts:332-332 type Typed
type Typed = Float32Array | Int16Array | Uint16Array | Uint32Array | Uint8Array;

// src/render/buildings/buildingMesh.ts:335-374 class Grow
/** DO NOT lay a shell in number[]: 8 bytes a number and its regrowth, tens of MB of garbage a cell; a typed array that doubles as it fills, viewed at its length at the end */
class Grow<T extends Typed> { … }

// src/render/buildings/buildingMesh.ts:343-353 method Grow.room
/** The array, with room for `n` more numbers after `length` (a new one, twice as large, when full). */
  room(n: number): T;

// src/render/buildings/buildingMesh.ts:356-358 method Grow.view
/** What was written, without a copy. */
  view(): T;

// src/render/buildings/buildingMesh.ts:361-363 method Grow.copy
/** What was written, in an array of its own size (the arrays written again for the next building, `ShellWriter`). */
  copy(): T;

// src/render/buildings/buildingMesh.ts:377-473 class PartRows
/** A kind's instances as the arrays of its batch (`PartBatch`), written as each is placed: DO NOT make an object of each placement (its numbers boxed, every pane, frame and chair of a job). */
class PartRows { … }

// src/render/buildings/buildingMesh.ts:394-401 method PartRows.clear
/** Emptied for the next building, its arrays kept at their size. */
  clear(): void;

// src/render/buildings/buildingMesh.ts:404-431 method PartRows.add
/** One instance at (x, y, z) in world axes, turned `yaw`, sized (sx, sy, sz): its pose (`partPose.ts`), the kind's colour, no look, slot or room; its index. */
  add(x: number, y: number, z: number, yaw: number, sx: number, sy: number, sz: number): number;

// src/render/buildings/buildingMesh.ts:448-452 method PartRows.pane
/** A pane's light slot and the room behind it (`roomBehind.ts`, ROOM_FLOATS numbers as half floats), when known. */
  pane(i: number, slot: number, room: readonly number[] | null): void;

// src/render/buildings/buildingMesh.ts:455-459 method PartRows.paneFrame
/** A pane's frame, drawn by the pane's own material (`glassRoom.ts` PANE_FRAME_GLSL): its trim colour and its thickness from front to back. */
  paneFrame(i: number, r: number, g: number, b: number, thickness: number): void;

// src/render/buildings/buildingMesh.ts:462-465 method PartRows.paneRecess
/** A pane laid on the wall's face over its opening: how deep its glass stands behind it (`glassRoom.ts` PANE_RECESS_GLSL draws the reveals). */
  paneRecess(i: number, depth: number): void;

// src/render/buildings/buildingMesh.ts:476-476 type PartSink
/** Every part kind's rows, written again for each building (`ShellWriter`). */
type PartSink = Record<PartKind, PartRows>;

// src/render/buildings/buildingMesh.ts:478-478 type FurnitureSink
/** The furniture's rows by kind, made as a kind is first placed. */
type FurnitureSink = Partial<Record<FurnitureKind, PartRows>>;

// src/render/buildings/buildingMesh.ts:482-482 function furnitureRows
const furnitureRows = (sink: FurnitureSink, kind: FurnitureKind): PartRows => …;

// src/render/buildings/buildingMesh.ts:501-625 class ShellPart
/** One finish's share of a shell, written as laid in the GPU's layout (`ShellChunk`): flat-shaded, painted triangles, the uv worked out on the GPU. */
class ShellPart { … }

// src/render/buildings/buildingMesh.ts:536-543 method ShellPart.clear
/** Emptied for the next building, its arrays and its table kept at their size (`ShellWriter`). */
  clear(): void;

// src/render/buildings/buildingMesh.ts:574-591 method ShellPart.weld
/** The vertex just written at `vertices`: kept as a new one, or the one laid before with every number alike; its index. */
  weld(): number;

// src/render/buildings/buildingMesh.ts:666-803 class Shell
/** Triangles in THREE's axes, one buffer set per finish; winding fixed per face. */
class Shell { … }

// src/render/buildings/buildingMesh.ts:704-775 method Shell.face
/** DO NOT lay only the first two triangles: a lantern's octagonal and 24-sided caps */
  face(points: readonly (readonly [number, number, number])[], n: readonly [number, number, number], c: Paint, shades?: readonly number[]): void;

// src/render/buildings/buildingMesh.ts:805-805 type V3
type V3 = [number, number, number];

// src/render/buildings/buildingMesh.ts:807-807 variable UP
/** Straight up and down, and the two sides of a centre (-1, 1): DO NOT write them as literals in what lays every bay (a new array each time). */
const UP: Readonly<V3> = Object.freeze([0, 0, 1] as V3);

// src/render/buildings/buildingMesh.ts:808-808 variable DOWN
const DOWN: Readonly<V3> = Object.freeze([0, 0, -1] as V3);

// src/render/buildings/buildingMesh.ts:812-823 interface BayFace
/** A face of one bay: its frame in the building's local plan, and its storey. */
interface BayFace {
  /** Local start of the bay on the facade line, its along direction and outward normal. */
  ax: number;
  ay: number;
  tx: number;
  ty: number;
  nx: number;
  ny: number;
  z0: number;
  W: number;
  H: number;
}

// src/render/buildings/buildingMesh.ts:847-1042 class Emitter
class Emitter { … }

// src/render/buildings/buildingMesh.ts:1064-1064 type SlotAllocator
/** The light slot of a bay's window: the space on its floor behind it. */
/** Who gives a space its seed for its windows' light (`lightSlots.ts` `spaceSeed`; a test may give its own). */
export type SlotAllocator = (building: number, level: number, volume: number, x: number, y: number) => number;

// src/render/buildings/buildingMesh.ts:1066-1077 function slotOfBay
function slotOfBay(b: Building, v: Volume, bay: FacadeBay, spaces: ReturnType<typeof deriveSpaces>, slots: SlotAllocator): number;

// src/render/buildings/buildingMesh.ts:1082-1349 function emitBuilding
function emitBuilding(
  b: Building,
  groundAt: GroundAt,
  shell: Shell,
  parts: PartSink,
  pavedAt?: PavedAt,
  furniture: FurnitureSink = {},
  onLot?: (el: BuildingElement) => boolean,
  designedFloor?: number,
  slots: SlotAllocator = spaceSeed,
  /** The neighbours' blocks a party wall stands against (`partyWalls.ts`): a bay inside them is not laid. */
  covers: readonly Cover[] = [],
): Foundation;

// src/render/buildings/buildingMesh.ts:1351-1351 variable FLOOR_FINISH
const FLOOR_FINISH = paint({ finish: 'stone', colour: 0xb9b2a5 });

// src/render/buildings/buildingMesh.ts:1364-1364 variable WOOD_FLOOR
const WOOD_FLOOR = paint({ finish: 'wood', colour: 0xa27548 });

// src/render/buildings/buildingMesh.ts:1365-1365 variable PORCELAIN_FLOOR
const PORCELAIN_FLOOR = paint({ finish: 'stone', colour: 0xd9d3c7 });

// src/render/buildings/buildingMesh.ts:1368-1368 variable STEP
const STEP = paint({ finish: 'stone', colour: 0xc9c2b3 });

// src/render/buildings/buildingMesh.ts:1371-1455 function emitInterior
/** Inside a building cut open at level `cut` (the Construction tool's interior view) */
function emitInterior(e: Emitter, b: Building, floor: number, cut: number): void;

// src/render/buildings/buildingMesh.ts:1458-1463 function placeFurniture
/** One piece of furniture, as an instance of its model */
function placeFurniture(e: Emitter, f: Furniture, floorZ: number): void;

// src/render/buildings/buildingMesh.ts:1465-1465 variable PARTITION
const PARTITION = paint({ finish: 'plaster', colour: 0xece6da });

// src/render/buildings/buildingMesh.ts:1468-1468 function shaded
/** How much light a window reveal keeps: it sits in the wall's own shadow. */

const shaded = (c: Paint, k: number): Paint => …;

// src/render/buildings/buildingMesh.ts:1628-1831 function emitBay
function emitBay(
  e: Emitter,
  f: BayFace,
  bay: FacadeBay,
  wall: Paint,
  trim: Paint,
  awning: Color,
  /** What the bays either side of it hold, along the same storey (none past the side's ends). */
  left: BayComponent | undefined,
  right: BayComponent | undefined,
  /** An entrance whose flight is set into the building: the opening becomes a porch this deep. */
  recess = 0,
  geometry?: FacadeGeometry,
): void;

// src/render/buildings/buildingMesh.ts:2154-2332 function emitLots
/** `entrances`: the building's ways in as its body's emission worked them out (`emitBuilding`): DO NOT work its foundation out again for the flights' feet. */
function emitLots(b: Building, lots: readonly Volume[], withParts: boolean, groundAt: GroundAt, shell: Shell,
  parts: PartSink, buildingFloor?: number, pavedAt?: PavedAt,
  onLot?: (el: BuildingElement) => boolean, entrances?: readonly Entrance[]): LotGround | undefined;

// src/render/buildings/buildingMesh.ts:2335-2339 function lotGroundAgain
/** A building's lot ground made again from its recipe, as `emitLots` made it */
export function lotGroundAgain(b: Building, recipe: LotGroundRecipe, pavedAt?: PavedAt): LotGround | undefined;

// src/render/buildings/buildingMesh.ts:2342-2362 function lotGroundOf
/** The ground of a building's lots as drawn: each lot's surface, and the level a parking bay outside every lot is laid at. */
function lotGroundOf(b: Building, surfaces: readonly LotPlane[], level: number | undefined, lots: boolean): LotGround | undefined;

// src/render/buildings/buildingMesh.ts:3694-3696 function fliesFlag
/** Whether a building flies a flag: a spire or a lantern on its roof with a flag (`flagOf`). */
export function fliesFlag(b: Building): boolean;

// src/render/buildings/buildingMesh.ts:3709-3713 function letGoEmitted
/** DO NOT hold a job's every building's furniture and bays until the job is done (a dense cell's 15 MB of furniture and 9 MB of bays as JS objects in the worker's heap, `cellWork.ts`): let go once the building is emitted, the blocks it was emitted from (`emitChunk`'s body, its closed blocks); asked again, made again the same. */
export function letGoEmitted(b: Building): void;

// src/render/buildings/buildingMesh.ts:3716-3729 class ShellWriter
/** DO NOT grow a new shell's arrays for each building of a job from nothing (every one doubling its way up, the half let go, its chunk holding views of arrays twice its size): one shell written again for each, the arrays grown once to the largest, each chunk an exact copy. */
export class ShellWriter { … }

// src/render/buildings/buildingMesh.ts:3723-3728 method ShellWriter.next
/** The shell and the rows emptied for the next building. */
  next(): Shell;

// src/render/buildings/buildingMesh.ts:3735-3778 function emitChunk
/** `far`: the building's far level (L1) - walls as runs of bays with their windows drawn by the shader, no parts (`facadeLevel.ts`). */
/** `apart`: the elements' boxes, pavings and rails as instances of the shell's parts (`shellParts.ts`); false lays them in the shell face by face, as the parts draw them (the specs compare). */
/** `party`: the job's buildings as one another's neighbours (`partyWalls.ts`): the bays inside a neighbour are not laid, and each floor is worked out once. */
/** `writer`: one shell written again for each building of a job (`ShellWriter`): its arrays grow once, and each chunk copies out what is its own. */
export function emitChunk(b: Building, groundAt: GroundAt, pavedAt?: PavedAt, naturalAt: GroundAt = groundAt, slots: SlotAllocator = spaceSeed, far = false, apart = true,
  party?: PartyWalls, writer?: ShellWriter): BuildingChunk;

// src/render/buildings/buildingMesh.ts:3781-3806 function chunkSummary
/** What the page keeps of a building (`cellWork.ts` `ChunkSummary`), as `emitChunk` makes it, without laying it: its lots' ground recipe and its signature floor; a building that flies a flag is laid whole, the flags being where its roof puts them. */
export function chunkSummary(b: Building, groundAt: GroundAt, pavedAt?: PavedAt, naturalAt: GroundAt = groundAt, party?: PartyWalls): Pick<BuildingChunk, 'flags' | 'lotGround' | 'signatureFloor'>;

// src/render/buildings/buildingMesh.ts:3809-3811 function shellChunkOf
/** A part's arrays viewed at their length, no copy. */
const shellChunkOf = (part: ShellPart, copy: boolean): ShellChunk => …;

// src/render/buildings/buildingMesh.ts:3814-3818 function shellChunks
/** A shell's fragments by finish: the arrays it was laid in (every face but the far level's runs with openings). */
function shellChunks(shell: Shell): Partial<Record<Finish, ShellChunk>>;

// src/render/buildings/buildingMesh.ts:3821-3831 function runChunks
/** The far level's runs of bays with openings by finish, each vertex with its window grid; undefined when none was laid. */
function runChunks(shell: Shell): Partial<Record<Finish, RunChunk>> | undefined;

// src/render/buildings/buildingMesh.ts:3907-3924 function kitBoxes
/** @rebuild Worked out once per kit; every later ask returns the boxes kept. */
export function kitBoxes(kit: BuildingKit): KitBoxes;

// src/render/buildings/buildingMesh.ts:3943-4145 function meshesOf
/** A batch's meshes from its arrays (`cellArrays.ts`), made here or in a worker */
export function meshesOf(arrays: CellArrays, kit: BuildingKit, ghost: boolean, dim: boolean, refill?: () => CellArrays): BuildingMeshes;

// src/render/buildings/buildingMesh.ts:4132-4143 method meshesOf.dispose
dispose();

// src/render/buildings/buildingMesh.ts:4154-4154 function boxOf
const boxOf = (b: Bounds): Box3 => …;

// src/render/buildings/buildingMesh.ts:4155-4155 function sphereOf
const sphereOf = (b: Bounds): Sphere => …;

// src/render/buildings/buildingMesh.ts:4158-4169 function buildBuildingMeshes
/** Meshes for buildings, emitted afresh (the preview; tests). */
export function buildBuildingMeshes(
  buildings: Iterable<Building>,
  groundAt: GroundAt,
  kit: BuildingKit,
  ghost = false,
  pavedAt?: PavedAt,
  naturalAt: GroundAt = groundAt,
): BuildingMeshes;

// src/render/buildings/buildingMesh.ts:4175-4196 function interiorFurniture
/** The furniture of every floor of a building, as it stands inside: what a blow throws out of it when it breaks */
export function interiorFurniture(b: Building, floor: number): Partial<Record<FurnitureKind, PartBatch>>;
