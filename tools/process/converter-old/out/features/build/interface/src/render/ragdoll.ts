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
