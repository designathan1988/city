// What the feature uses of src/render/agents.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/agents.ts:112-112 variable SIDES
const SIDES = [1, -1] as const;

// src/render/agents.ts:137-166 interface AgentRenderOptions
interface AgentRenderOptions {
  readonly pedestrianDetail?: 0 | 1 | 2;
  readonly pedestrianVisible?: (x: number, y: number, height: number) => boolean;
  /** Whether a vehicle of this reach, centred here, can be on screen or cast a shadow onto it. */
  readonly vehicleVisible?: (x: number, y: number, height: number, radius: number) => boolean;
  /** Whether such a sphere is wholly inside the view (every plane of the frustum farther than its radius): a cell of parked cars standing as drawn is not walked. */
  readonly vehicleInside?: (x: number, y: number, height: number, radius: number) => boolean;
  /** The ground where a vehicle `vehicleVisible` can pass stands (`frustumGround.ts`): only the vehicles there are asked of the client's grid; none given, every one. */
  readonly vehicleBox?: Readonly<GroundBox>;
  /** Zoom from which vehicle cabins and occupants are drawn. */
  readonly occupantZoom?: number;
  /** The tier's vehicle and people detail (`quality.ts`): none given, what every tier but the weak one draws. */
  readonly tier?: Pick<QualitySettings, 'vehicleErrorPixels' | 'vehicleWholeCap' | 'hairStrandPeople'>;
  /** Each motor vehicle drawn: where its tail smoke leaves from (`exhaust.ts`). */
  readonly exhaust?: (x: number, y: number, z: number, angle: number, length: number, speed: number, dusty: boolean) => void;
  /** The bodies of people killed by a blow (`ragdoll.ts`), drawn with the crowd's own meshes. */
  readonly ragdolls?: (citizens: RagdollCitizens) => void;
  /** Somebody whose own body lies on the ground (`ragdoll.ts`): not drawn standing as well. */
  readonly hiddenPed?: (id: number) => boolean;
  /** Where the camera is: the people nearest it get their hair's strands (`?bodies=proc`). */
  readonly eye?: Vector3;
  /** How tall something `height` high standing at (x, y, z) is on the screen, pixels */
  readonly personPixels?: (x: number, y: number, z: number, height: number) => number;
  /** Pixels a world unit across the screen at (x, y, z): each vehicle's level of detail. */
  readonly screenScale?: (x: number, y: number, z: number) => number;
  /** Pixels drawn per CSS pixel (the canvas's buffer over its size): a part's level is chosen by its error in pixels drawn. */
  readonly pixelRatio?: number;
  /** Whether shadows are drawn at all (the quality tier): the far ones' soft discs follow it. */
  readonly shadows?: boolean;
}

// src/render/agents.ts:168-198 interface AgentMeshes
export interface AgentMeshes {
  readonly meshes: readonly Object3D[];
  /** Whether the last `sync` switched the two-wheelers between casting into the shadow map and a soft disc under them */
  readonly castersChanged: boolean;
  /** Whether the last `sync` drew anything into the shadow map (vehicles, two-wheelers, people): the map's moving casters. */
  readonly casting: boolean;
  /** The records the page holds for the vehicles and walkers (by id, by cell, kept): bounded by the view, the region and the last seconds, never by the map or the path. */
  readonly held: { readonly vehicles: number; readonly cells: number; readonly kept: number; readonly people: number };
  sync(world: SimClient, net: Network, alpha: number, detailed: boolean, zoom?: number, options?: AgentRenderOptions): void;
  /** Every figure drawn last frame and the body it was cast as (`citizenCasting.ts`). */
  census(): ReturnType<ReturnType<typeof createRiggedCitizens>['census']>;
  /** The burnt-out shell of this vehicle's own body */
  carcass(vehicle: { readonly id: number; readonly archetype: Archetype }, intact?: boolean): BufferGeometry | null;
  /** How far each procedural person's drawn skeleton is from a body (the weapons lab's mesh probe) */
  meshProbe(): { id: number; stretch: number; stretchBone: string; scale: number; scaleBone: string; held: boolean; clip: string }[];
  /** What is posing a procedural person now (the weapons lab): the baked clip played */
  animProbe(id: number): { clip: string; time: number; duration: number; weight: number; paused: boolean; layers: string[]; source: 'animation' | 'physics' } | null;
  /** Plays `clip` on a person whatever they are doing (the lab's clip check); null gives them back. */
  forceClip(id: number, clip: ProcClip | null): void;
  /** The body a vehicle's driver or rider is drawn with, for their body when they are thrown out (`Occupant.index`). */
  driverBody(vehicle: SimVehicle, x: number, y: number): number | null;
  /** Called for each walker drawn bleeding (a limb lost), to drip blood where they go. */
  setBleed(fn: (id: number, x: number, y: number, z: number) => void): void;
  /** Lamps burn brighter than white after dark, so headlights and tail lights glow. */
  setNight(dark: number): void;
  /** The people's skeletons (procedural and cooked), worked out on the GPU: once a frame, after `sync`, before the scene is drawn. */
  renderPalettes(renderer: WebGLRenderer): void;
  /** Every body of the vehicles' people loaded now and kept loaded (`riggedCitizens.keepAll`): asked once the page has a scene to draw. */
  keepPeople(): void;
  dispose(): void;
}

// src/render/agents.ts:203-203 type ElevationAt
type ElevationAt = (x: number, y: number, segment?: SegmentId) => number;

// src/render/agents.ts:208-213 function agentHash
/** A cheap integer avalanche hash. */
export function agentHash(id: number): number;

// src/render/agents.ts:379-401 interface BodyPlan
/** Per-class geometry, derived once at module load. */
interface BodyPlan {
  readonly shape: VehicleShape;
  readonly length: number;
  readonly width: number;
  readonly height: number;
  readonly wheelRadius: number;
  /** Tyre width. */
  readonly tread: number;
  /** Axle positions along the body, front first. */
  readonly axleAlong: readonly number[];
  /** Lateral offset of a wheel from the centreline. Zero for two-wheelers. */
  readonly axleSide: number;
  readonly cabinLength: number;
  readonly cabinAlong: number;
  readonly seats: number;
  /** Seated torso centre, and the head above it. */
  readonly seatY: number;
  readonly headY: number;
  /** First seat row, and the step back to the next. */
  readonly seatAlong: number;
  readonly seatPitch: number;
  readonly seatSide: number;
}

// src/render/agents.ts:438-438 function planOf
const planOf = (a: Archetype): BodyPlan => …;

// src/render/agents.ts:447-449 interface KeptHolder
/** Who holds kept instances (`VehicleDraw`): each one's place in a part, moved when another kept one leaves. */
interface KeptHolder {
  readonly keptSlots: number[];
}

// src/render/agents.ts:452-478 interface Part
/** DO NOT write every vehicle's every instance each frame again: a parked car's stay where they were written (`kept`), only the frame's own are written and sent. */
interface Part {
  readonly mesh: InstancedMesh;
  /** Instances [0, kept) are the parked cars' standing still, written once; [kept, n) this frame's. */
  kept: number;
  /** Write cursor: back to `kept` at the top of every sync. */
  n: number;
  /** The holder of each kept instance and its place in the holder's list (`holders.length === kept`). */
  readonly holders: KeptHolder[];
  readonly holderAt: number[];
  /** Whether its instances are tinted (a part always is or never is): their colours go to the GPU with their matrices. */
  tinted: boolean;
  /** Instances changed since the GPU last took them, [lo, hi) (none while hi <= lo); the upload clears it (`onUpload`). */
  lo: number;
  hi: number;
  /** In the frame's list of parts drawn (`live`). */
  listed: boolean;
  /** Made to cast into the shadow map: a vehicle off the screen and in the shadows' reach writes only these (`castOnly`). */
  readonly casts: boolean;
  /** Instances drawn last frame: one gone changes the shadow as much as one moved. */
  shown: number;
  /** Whether it cast last frame: a part turned to cast, or not, changes the shadow too. */
  castLast: boolean;
  /** A body joined with its panel (`vehiclePanels.ts`): the panel's colour per instance, on the mesh's geometry; null for every other part. */
  panel: InstancedBufferAttribute | null;
  /** A body's glazing (`vehicleWindows.ts`): each instance's two windows' drop, on the mesh's geometry; null for every other part. */
  windows: InstancedBufferAttribute | null;
}

// src/render/agents.ts:517-520 interface Ladder
/** A part at each level (`vehicleLevels.ts`): `parts[k]` within `LEVEL_ERRORS[k - 1]`, `route[k]` the part drawn at level k (-1: none), the model's own until made. */
interface Ladder {
  readonly parts: readonly Part[];
  readonly route: number[];
}

// src/render/agents.ts:552-2596 function createAgentMeshes
/** `groundAt`: what a walker off the streets stands on (a yard, a terrace, the paving, the land); `roughGroundAt` */
export function createAgentMeshes(elevationAt: ElevationAt, onAssetsReady: () => void = () => {},
  groundAt?: (x: number, y: number) => number,
  lotAt?: (building: number, x: number, y: number) => number,
  roughGroundAt?: (x: number, y: number) => number): AgentMeshes;

// src/render/agents.ts:621-648 interface CarParts
/** One set of instanced meshes per car class, built from its own model (`vehicleModels.ts`) */
  interface CarParts {
    readonly model: VehicleModel;
    /** The body, glazing and cabin with every door shut: one instance each, at the level its size on the screen allows. */
    readonly shell: Ladder;
    readonly glass: Ladder;
    readonly interior: Ladder;
    /** The same while a door moves: the pieces, and each leaf on its own. */
    readonly openShell: Part;
    readonly openGlass: Part;
    readonly openInterior: Part;
    readonly doors: readonly Part[];
    readonly doorGlass: readonly Part[];
    readonly doorCards: readonly (Part | null)[];
    readonly trim: Ladder;
    /** The roof panel in paint, or in glass on a car with a panoramic roof. */
    readonly roof: Ladder | null;
    readonly roofGlass: Part | null;
    readonly accent: Ladder | null;
    /** The body, roof and accent of the middle level: the same, casting no shadow (a soft disc instead). */
    readonly shellLite: Ladder;
    readonly roofLite: Ladder | null;
    readonly accentLite: Ladder | null;
    readonly far: Part;
    readonly steering: Part | null;
    /** The body and its panel (the roof, or else the accent) in one draw, doors shut (`vehiclePanels.ts`); null for a body with no panel. */
    readonly panel: Ladder | null;
    readonly panelLite: Ladder | null;
  }

// src/render/agents.ts:651-656 function createAgentMeshes.bodyKey
/** Which body a vehicle is drawn with: its class and, for a car, the style its id picks. */
  const bodyKey = (vehicle: SimVehicle): string =>;

// src/render/agents.ts:817-823 interface TwoWheelerParts
/** A two-wheeler's parts, each at the level its size on the screen allows (`vehicleLevels.ts`), as a car's. */
  interface TwoWheelerParts {
    readonly model: TwoWheelerModel;
    readonly body: Ladder;
    readonly trim: Ladder;
    readonly steering: Ladder;
    readonly cranks: Ladder | null;
  }
