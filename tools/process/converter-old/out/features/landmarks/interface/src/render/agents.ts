// What the feature uses of src/render/agents.ts (shared with the program): declarations as the source writes them, bodies left out.

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
