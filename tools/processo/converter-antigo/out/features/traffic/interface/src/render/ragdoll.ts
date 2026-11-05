// What the feature uses of src/render/ragdoll.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/ragdoll.ts:14-33 interface RagdollCitizens
/** Ragdolls: people knocked over or killed by a blow, or tripping, their own bodies falling. */

export interface RagdollCitizens {
  capturedPose(id: number): { index: number; palette: Float32Array; transform: Matrix4 } | null;
  skeletonOf(index: number): { names: string[]; parents: number[]; inverses: Matrix4[]; local: Matrix4; bind: Matrix4 } | null;
  /** `charred`: burnt black (a bomb's direct hit). */
  drawPalette(index: number, palette: Float32Array, instance: Matrix4, charred?: boolean, alive?: boolean): void;
  clipPose(index: number, key: 'crouchUp' | 'idle' | 'getUpFront' | 'getUpBack' | 'crawl', phase: number): { palette: Float32Array; duration: number } | null;
  /** Bodies loaded now, for people with no pose of their own (indoors). */
  loadedIndices(): number[];
  /** The index to draw a piece torn off this person with (their twin, or the same body); null when none can be drawn. */
  twin?(index: number): number | null;
  /** A twin's index given back (its piece gone). */
  untwin?(index: number): void;
  /** Burns the person black (a bomb's direct hit). */
  char?(index: number): void;
  /** A bullet hole in this part of the person, bleeding into their clothes. */
  /** From world (fromX, fromY) when known: the shot's jolt through them too. */
  wound?(index: number, part: BodyPart, fromX?: number, fromY?: number): void;
  /** The person soaked in blood all over (shot to pieces). */
  drench?(index: number): void;
}

// src/render/ragdoll.ts:36-51 interface Occupant
/** Somebody inside a building a blow struck: thrown out of it from where they were. */
export interface Occupant {
  readonly id: number;
  readonly x: number;
  readonly y: number;
  /** Height they stood at (the floor they were on). */
  readonly z: number;
  readonly heading: number;
  readonly blastX: number;
  readonly blastY: number;
  readonly power: number;
  readonly kind: 'dead' | 'torn' | 'knocked';
  /** Burnt black (right under the blow, or in a burning car). */
  readonly charred?: boolean;
  /** The body they were drawn with (a driver, a rider): thrown as themself, not as somebody else. */
  readonly index?: number | null;
}

// src/render/ragdoll.ts:54-60 interface RagdollWall
/** A wall a body can strike: a ring on the ground (world x, y), up to a height. */
export interface RagdollWall {
  readonly ring: readonly { readonly x: number; readonly y: number }[];
  /** The highest it reaches. */
  readonly top: number;
  /** Its top at a point inside it (a roof's pitch); `top` all over when absent. */
  readonly roof?: (x: number, y: number) => number;
}

// src/render/ragdoll.ts:62-69 interface RagdollWorld
export interface RagdollWorld {
  /** Height of whatever a body lands on at world (x, y). */
  groundAt(x: number, y: number): number;
  /** The walls near a world point. */
  wallsNear(x: number, y: number, reach: number): RagdollWall[];
  /** Moves when the ground changes (an edit): the physics' ground made before is not given to a body that falls after. */
  groundSerial?(): number;
}

// src/render/ragdoll.ts:72-72 type GetUp
/** Somebody alive getting up at world (x, y) facing `heading`, taking `seconds` (`PeopleEngine.getUp`). */
export type GetUp = (id: number, x: number, y: number, heading: number, seconds: number, crawl?: boolean) => void;

// src/render/ragdoll.ts:85-85 variable BEL
/** In front of the pelvis: braces the torso against folding back and forth. */
const BEL = 19;

// src/render/ragdoll.ts:87-87 type PartName
type PartName = 'torso' | 'neck' | 'head' | 'lua' | 'lfa' | 'rua' | 'rfa' | 'lth' | 'lca' | 'lfo' | 'rth' | 'rca' | 'rfo';

// src/render/ragdoll.ts:88-88 type Limb
type Limb = 'larm' | 'rarm' | 'lleg' | 'rleg';

// src/render/ragdoll.ts:137-137 variable STEP
const STEP = 1 / 90;

// src/render/ragdoll.ts:185-185 interface Stick
interface Stick { readonly a: number; readonly b: number; readonly min: number; readonly max: number; broken: boolean }

// src/render/ragdoll.ts:187-187 interface Wall
interface Wall extends RagdollWall { readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number }

// src/render/ragdoll.ts:196-196 interface GroundTile
/** One tile of the physics' ground, the ground's serial it was made at, and the bodies on it. */
interface GroundTile { readonly key: number; readonly serial: number; readonly collider: JoltStatic; users: number }

// src/render/ragdoll.ts:199-220 interface Survivor
/** Somebody alive on the ground: falling, lying a moment, then getting up. */
interface Survivor {
  readonly id: number;
  phase: 'fall' | 'lie' | 'rise';
  t: number;
  lie: number;
  /** Down for good and hurt, writhing where they lie (`writhe`): seconds since they lay. */
  crawl?: { since: number };
  /** Where and which way they get up, and the lying bones blended from. */
  root?: Matrix4;
  from?: Matrix4[];
  clip: number;
  /** Whether the get-up clip is baked. */
  ready?: boolean;
  /** Getting up from lying (`rise`): the body moved through key poses from where it lies */
  keys?: { stages: { from: Vector3[]; to: Vector3[]; start: number; end: number; roll?: { axis: Vector3; at: Vector3; lift: number } }[] };
  pre?: number;
  blend?: number;
  /** The captured getting-up off the ground played (face down, face up), when there is one. */
  riseKey?: 'getUpFront' | 'getUpBack' | 'crawl';
  /** How far into it they go (onto hands and knees, to crawl: `writhe`); all the way when absent. */
  riseMax?: number;
}

// src/render/ragdoll.ts:222-291 interface Body
interface Body {
  readonly index: number;
  readonly scale: number;
  readonly p: Vector3[];
  readonly o: Vector3[];
  readonly inv: number[];
  readonly radius: number[];
  readonly ground: number[];
  readonly sticks: Stick[];
  readonly bonePart: PartName[];
  readonly parents: number[];
  readonly pos0: Vector3[];
  readonly rot0: Quaternion[];
  readonly scl0: Vector3[];
  readonly origin0: Record<PartName, Vector3>;
  readonly frame0: Record<PartName, Matrix4>;
  /** Limb bend normals carried from frame to frame, so a straight limb keeps its last bend. */
  readonly normals: Record<Limb, Vector3>;
  /** +1 or -1: which way side x spine points the body's front. */
  readonly front: number;
  /** Each foot's way where it stood, in its shin's frame (`ankles`): its pitch and its turn out of the leg's plane, radians. */
  ankle: Record<'lleg' | 'rleg', { pitch: number; yaw: number }>;
  readonly inverses: Matrix4[];
  /** Each bone's bind matrix (the inverse of its inverse). */
  readonly binds: Matrix4[];
  /** Each particle's bone (-1: none of its own), for poses read off the skeleton (`particlesOf`). */
  readonly jointBone: number[];
  /** local * bind^-1 and its inverse: between the drawn instance's space and a bone's matrix. */
  readonly rebind: Matrix4;
  readonly unbind: Matrix4;
  readonly walls: Wall[];
  torn: boolean;
  /** Parts lost before the fall (shot off, `Casualty.lost`): drawn closed at their joint. */
  lostParts: Set<PartName>;
  survivor: Survivor | undefined;
  /** A limb torn off, a body of its own (`detach`) */
  pin?: { readonly root: number; readonly keep: ReadonlySet<number> };
  /** Called when the body goes (a twin given back). */
  drop?: (() => void) | undefined;
  /** Burnt black (a bomb's direct hit). */
  charred?: boolean;
  /** Whose body it was (the casualty's id), for probes. */
  recordId?: number;
  /** Shots taken on the ground, all told and by part (`shootBody`). */
  hits?: number;
  partHits?: Partial<Record<BodyPart, number>>;
  /** Shot to a heap of meat (`mush`). */
  mush?: boolean;
  /** Opened at the belly, the guts out (`openBelly`). */
  opened?: boolean;
  /** Seconds left of joint friction (`joints`): a body just struck moves as a body, not a bundle of loose sticks */
  stiff?: number;
  /** Which piece each particle is on: the body itself, or a limb torn off. */
  comp: number[];
  palettes: Float32Array[];
  /** The body's own palettes, one a piece, written again every frame it moves (`palettes`, `risePalette`). */
  readonly palettePool: Float32Array[];
  readonly anchor: Matrix4;
  still: number;
  asleep: boolean;
  age: number;
  flying: number;
  pooled: boolean;
  /** Simulated by Jolt (`ragdollJolt.ts`): its parts; the colliders made for it */
  jolt?: JoltRagdoll | undefined;
  colliders?: { list: JoltStatic[]; tiles: GroundTile[]; users: number } | undefined;
  synced?: { p: Vector3[]; o: Vector3[] } | undefined;
  splats: number;
  spray: number;
}

// src/render/ragdoll.ts:346-366 interface RagdollProbe
/** One body as the weapons lab measures it (`Ragdolls.probe`). Distances in metres. */
export interface RagdollProbe {
  /** The person's id (alive or dead: the record's), or -1 for a piece torn off. */
  readonly id: number;
  readonly phase: string;
  readonly piece: boolean;
  readonly asleep: boolean;
  readonly charred: boolean;
  /** Particles: world x, y and height (world units). */
  readonly points: readonly (readonly [number, number, number])[];
  /** Deepest a particle sits under the ground it lies on (its radius counted), metres; 0 when none does. */
  readonly underGround: number;
  /** Deepest a particle sits inside a wall, a pole, a car (below its top), metres. */
  readonly inWall: number;
  /** Worst bone off its length, as a share of it (0: all exact). */
  readonly boneError: number;
  /** Fastest particle, metres a second. */
  readonly speed: number;
  /** Parts gone. */
  readonly lost: readonly string[];
  readonly hits: number;
}

// src/render/ragdoll.ts:368-399 interface Ragdolls
export interface Ragdolls {
  /** Takes in the casualties of blows (each once): bodies thrown from the pose they were last drawn in. */
  absorb(list: readonly Casualty[], citizens: RagdollCitizens, world: RagdollWorld): void;
  /** People inside a struck building, thrown out of it - through its walls and windows - from the floor they were on. */
  fling(list: readonly Occupant[], citizens: RagdollCitizens, world: RagdollWorld): void;
  /** A drop of blood on the ground at world (x, y) (a trail behind somebody bleeding). */
  drip(x: number, y: number, z: number, size: number): void;
  /** Called where a body is torn apart (world x, y, height z; the way it flew; how fast): the renderer throws its pieces. */
  onGore: ((x: number, y: number, z: number, dirX: number, dirY: number, speed: number, kind: 'torn' | 'dead') => void) | null;
  /** Somebody falling (a `fall` pause), from the pose they were last drawn in: tripping forwards, or knocked towards `away` */
  trip(id: number, heading: number, citizens: RagdollCitizens, world: RagdollWorld, away?: number | null): void;
  /** Whether somebody's own body is on the ground here, so the crowd does not draw them standing too. */
  hides(id: number): boolean;
  /** A shot along the line of sight from `a` to `b` (three's frame) at the bodies on the ground */
  shootBody(a: Vector3, b: Vector3): { alive: number; part: BodyPart } | 'hit' | null;
  /** Lets go of those who are up again once the simulation has them up too (`down` false). */
  release(down: (id: number) => boolean): void;
  update(dt: number, world: RagdollWorld): void;
  draw(citizens: RagdollCitizens, world: RagdollWorld): void;
  /** The blood the bodies left. */
  readonly decals: BloodDecal[];
  /** Bodies drawn, still moving, pieces, and the living among them (for probes). */
  stats(): { bodies: number; moving: number; pieces: number; living: number };
  /** Whether a body still moves or somebody alive is on the ground (`stats` moving or living), asked every frame. */
  active(): boolean;
  /** Each body's pelvis, phase and fate (for probes). */
  debug(): { pelvis: number[]; head: number[]; asleep: boolean; phase: string; torn: boolean }[];
  /** Everything about each body for a test bench (the weapons lab): its particles */
  probe(): RagdollProbe[];
  /** Every body, piece and stain gone (the weapons lab's clean slate). */
  clear(): void;
}

// src/render/ragdoll.ts:401-1956 function createRagdolls
export function createRagdolls(exhaust: Exhaust, getUp: GetUp, gore: Gore | null = null): Ragdolls;
