// What the feature uses of src/render/people/proceduralCrowd.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/proceduralCrowd.ts:231-231 type ProcClip
/** What a procedural person plays: walking, standing, running, sprinting for their life, cowering, photographing. */
export type ProcClip = 'walk' | 'idle' | 'run' | 'sprint' | 'cower' | 'photo' | 'getUp' | 'duck' | 'hurtWalk' | 'hurtRun' | 'nervous' | 'hit' | 'staggerBack' | 'riseFront' | 'riseBack' | 'crawl';

// src/render/people/proceduralCrowd.ts:233-261 interface ProceduralPerson
export interface ProceduralPerson {
  readonly spec: PersonSpec;
  readonly band: AgeBand;
  readonly sex: WalkSex;
  /** Their row in the class's palette, face and wound textures. */
  readonly row: number;
  /** Standing height over the class body's, the instance's scale. */
  readonly scale: number;
  /** Metres. */
  readonly height: number;
  readonly items: readonly string[];
  /** Their grown hairstyle's item (`hair:...`) and colour, for its strands close up. */
  readonly grown: string | null;
  readonly hairColour: Color;
  /** How far each of their joints is from the class body's, metres (their baked body's, `crowdBodies.ts`). */
  readonly joints: Float32Array;
  /** Where they stand and face; what they play. Set by the caller each frame (a zero scale hides them). */
  readonly matrix: Matrix4;
  clip: ProcClip;
  phase: number;
  /** The clip they are leaving, still playing under the new one at `weight` */
  fade?: { clip: ProcClip; phase: number; weight: number } | undefined;
  /** What they are doing, as the face shows it (`faceAt`): 'talk', 'panic'... */
  activity?: string | undefined;
  /** Limbs (or the head) lost to shots: their bones closed at the joint they were torn from. Set by the caller. */
  lost?: readonly Severable[] | undefined;
  /** How tall they stand on the screen, pixels: their level of detail. Set by the caller each frame; unset, they are drawn in full. */
  pixels?: number | undefined;
}

// src/render/people/proceduralCrowd.ts:263-276 interface ProceduralStats
interface ProceduralStats {
  readonly classes: number;
  readonly people: number;
  readonly pieces: number;
  readonly draws: number;
  readonly items: number;
  readonly vertices: number;
  readonly textureBytes: number;
  readonly bakeMs: number;
  /** People drawn at each level this frame (0 the closest). */
  readonly levels: readonly number[];
  /** Triangles drawn this frame, the shadow's not counted. */
  readonly triangles: number;
}

// src/render/people/proceduralCrowd.ts:878-929 interface ProceduralCrowd
export interface ProceduralCrowd {
  readonly group: Group;
  add(spec: PersonSpec): Promise<ProceduralPerson>;
  /** Every person's level, pose and place: once a frame, after setting `matrix`, `clip`, `phase` (and `pixels`). */
  /** `strands`: how many of the nearest people get their hair's strands (the tier's, `quality.ts` hairStrandPeople). */
  update(eye?: Vector3, time?: number, shadows?: boolean, strands?: number): void;
  /** The skeletons worked out on the GPU (`crowdAnimation.ts`): once a frame, after `update`, before the scene is drawn. */
  renderPalettes(renderer: WebGLRenderer): void;
  /** The ragdolls' side (`agents.ts`, `ragdoll.ts`): a person's skeleton */
  readonly ragdoll: {
    skeleton(person: ProceduralPerson): { names: string[]; parents: number[]; inverses: Matrix4[]; local: Matrix4; bind: Matrix4 } | null;
    pose(person: ProceduralPerson): Float32Array | null;
    /** A pose of a clip (`idle`, or `getUp` played once: its phase held at the end), in the person's row. */
    standing(person: ProceduralPerson, phase: number, clip?: 'idle' | 'getUp' | 'riseFront' | 'riseBack' | 'crawl'): Float32Array | null;
    hold(person: ProceduralPerson, palette: Float32Array | null): void;
    /** A clip's length, seconds. */
    duration(person: ProceduralPerson, clip: ProcClip): number;
    /** A bullet hole in this part of them, blood soaking out of it from now on. */
    wound(person: ProceduralPerson, part: BodyPart): void;
    /** The jolt of a bullet (Euphoria's shot: the spine giving way along the shot for a moment, then recovering) */
    jolt(person: ProceduralPerson, dirX: number, dirY: number, strength: number): void;
    /** The layers over the clip now (a jolt and its weight), for probes. */
    layers(person: ProceduralPerson): string[];
    /** A wounded posture laid over their clip (null takes it off): bent over by `hunch` radians, a hand on the wound by `reach` (0-1). */
    posture(person: ProceduralPerson, pose: { hunch: number; reach: number; part: BodyPart; cover?: number } | null): void;
    /** Blood all over them (a body shot to pieces). */
    drench(person: ProceduralPerson): void;
    /** Their wounds gone (the person drawn as somebody new). */
    heal(person: ProceduralPerson): void;
    /** Another of this person - their row's shape, face, clothes and colours - for a piece of them torn off */
    twin(person: ProceduralPerson): ProceduralPerson | null;
    untwin(twin: ProceduralPerson): void;
    /** Burnt black (a bomb's direct hit), skin, clothes and hair; given back by `hold(person, null)`. */
    char(person: ProceduralPerson, on: boolean): void;
  };
  clear(): void;
  clipDuration(person: ProceduralPerson, clip?: ProcClip): number;
  /** Ground one walk cycle covers at the person's scale, metres (of `clip`, their own by default). */
  stride(person: ProceduralPerson, clip?: ProcClip): number;
  stats(): ProceduralStats;
  /** Whether the last `update` drew anybody (each class casts through its shadow proxy): the shadow map's moving casters. */
  readonly casting: boolean;
  /** Diagnosis: the shape the GPU sums for a person at base vertices, against the rig's exact one, metres. */
  probe(person: ProceduralPerson, vertices: readonly number[]): { v: number; linear: number[]; exact: number[] }[];
  readonly people: readonly ProceduralPerson[];
  /** Drops an item and its pieces, so it is built again on next use (a hairstyle being edited). */
  forget(name: string): void;
  /** Every class made ahead, one at a time: its rig on the people's worker (`classRigInWorker`), the page only putting it together */
  warmClasses(): Promise<void>;
  /** Every class and hairstyle built here, packed for the cook (`proceduralCook.ts`, `scripts/cook-people.mjs`). */
  cook(): Promise<Map<string, ArrayBuffer>>;
}

// src/render/people/proceduralCrowd.ts:951-2531 function createProceduralCrowd
export function createProceduralCrowd(options: { hair?: boolean; /** World units per metre (the game's are 2.5). */ unit?: number } = {}): ProceduralCrowd;
