// What the feature uses of src/sim/host/protocol.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/protocol.ts:15-18 variable V
/** THE SIMULATION'S WORKER AND THE PAGE (`simHost.ts` there, `simClient.ts` here): what goes between them */

/** A vehicle's numbers as the worker writes them each tick (Float64, `VEHICLE_STRIDE` a vehicle). */
export const V = {
  id: 0, archetype: 1, free: 2, bx: 3, by: 4, bangle: 5, x: 6, y: 7, angle: 8, yawBefore: 9, yawNow: 10, jump: 11,
  v: 12, prevV: 13, age: 14, segment: 15, indicator: 16, curvature: 17, odometer: 18, seats: 19, slain: 20, lot: 21, inSegment: 22,
} as const;

// src/sim/host/protocol.ts:19-19 variable VEHICLE_STRIDE
export const VEHICLE_STRIDE = 23;

// src/sim/host/protocol.ts:24-24 variable PED_STRIDE
export const PED_STRIDE = 14;

// src/sim/host/protocol.ts:27-27 variable HEAD
const HEAD = 8;

// src/sim/host/protocol.ts:30-30 variable VEHICLES
/** Most vehicles a slot holds: the traffic's ceiling. */
const VEHICLES = FLEET_CEILING;

// src/sim/host/protocol.ts:31-31 variable PEDS
const PEDS = PED_CEILING;

// src/sim/host/protocol.ts:33-33 variable SLOT
/** One slot's Float64s: the head, then the vehicles, then the pedestrians. */
export const SLOT = HEAD + VEHICLES * VEHICLE_STRIDE + PEDS * PED_STRIDE;

// src/sim/host/protocol.ts:34-34 function vehicleBase
export const vehicleBase = (slot: number): number => …;

// src/sim/host/protocol.ts:35-35 function pedBase
export const pedBase = (slot: number): number => …;

// src/sim/host/protocol.ts:36-36 function headBase
export const headBase = (slot: number): number => …;

// src/sim/host/protocol.ts:37-37 variable MAX_VEHICLES
export const MAX_VEHICLES = VEHICLES;

// src/sim/host/protocol.ts:38-38 variable MAX_PEDS
export const MAX_PEDS = PEDS;

// src/sim/host/protocol.ts:42-42 variable CONTROL_WORDS
export const CONTROL_WORDS = 2;

// src/sim/host/protocol.ts:45-48 interface SharedFrames
/** The memory both sides hold: three slots (a triple buffer) and the control words. */
export interface SharedFrames {
  readonly data: SharedArrayBuffer;
  readonly control: SharedArrayBuffer;
}

// src/sim/host/protocol.ts:50-52 function makeSharedFrames
export function makeSharedFrames(): SharedFrames;

// src/sim/host/protocol.ts:55-64 interface VehicleRich
/** What a vehicle carries that is not a number of the tick: told when it changes. */
export interface VehicleRich {
  readonly color: string;
  readonly archetype: string;
  readonly doors: readonly number[];
  readonly kerbStop: KerbStop | null;
  readonly people: readonly number[];
  readonly peopleAge: readonly (PersonAgeClass | undefined)[];
  /** Off the road: its pose a tick ago and now (the numbers carry them too), its lot. */
  readonly freeLot: number | null;
}

// src/sim/host/protocol.ts:67-67 type PedRich
/** What a pedestrian carries that is not a number of the tick (`P`): told when it changes. */
export type PedRich = Omit<PedView, 'id' | 'x' | 'y' | 'heading' | 'prev' | 'v' | 'turnV' | 'age' | 'segment' | 'walking' | 'kerbWait' | 'rank'>;

// src/sim/host/protocol.ts:70-73 interface SignalLayout
/** A signalised junction's heads by group (`render/signals.ts`): which roads each group lights. */
export interface SignalLayout {
  readonly node: NodeId;
  readonly groups: readonly { readonly id: number; readonly segments: readonly SegmentId[] }[];
}

// src/sim/host/protocol.ts:76-80 interface SignalLights
/** A junction's lights now: its stage, and each group's colour. */
export interface SignalLights {
  readonly node: NodeId;
  readonly stage: string;
  readonly states: readonly (readonly [number, SignalState])[];
}

// src/sim/host/protocol.ts:83-83 interface TrainCar
/** A train drawn: its line's colour and each car's place. */
export interface TrainCar { readonly x: number; readonly y: number; readonly heading: number }

// src/sim/host/protocol.ts:84-84 interface TrainState
export interface TrainState { readonly colour: string; readonly metro: boolean; readonly cars: readonly TrainCar[] }

// src/sim/host/protocol.ts:87-105 type ToSim
/** The page to the worker: never the whole map nor a building's record, only what traffic reads (`agents/trafficBuildings.ts`). */
export type ToSim =
  | { readonly type: 'init'; readonly frames: SharedFrames; readonly map: TrafficMap; readonly seed: number; readonly zones: ReadonlyMap<string, Float64Array | null>; readonly zonesPrefix: string | null; readonly ambient: 'view' | 'edges'; readonly bake?: NetworkBake | null }
  | { readonly type: 'load'; readonly map: TrafficMap; readonly bake?: NetworkBake | null }
  /** The SAME map sent whole again, when the diary cannot say what changed (its entries let go, a change without ids): the replica brought up to it, the simulation kept running (SIM-18). */
  | { readonly type: 'resync'; readonly map: TrafficMap }
  /** The page's job lanes for the lots' bays and the conflict pairs (`jobPool.ts` jobChannel), before `init`: the worker opens no pool of its own. */
  | { readonly type: 'lanes'; readonly bays: readonly MessagePort[]; readonly conflicts: readonly MessagePort[] }
  /** The page's worker serving those lanes failed: their pool falls back to working here. */
  | { readonly type: 'laneLost'; readonly service: 'bays' | 'conflicts'; readonly why: string }
  /** The roads', lots' and transit's records the diary named (`delta`), and where the buildings it named are read (worked out as traffic reads them in the worker). */
  | { readonly type: 'delta'; readonly delta: Restoration; readonly buildings: readonly BuildingSent[] }
  | { readonly type: 'frame'; readonly wall: number; readonly hold: boolean; readonly controls: SimControls }
  | { readonly type: 'reset' }
  /** The micro window's centre and radius (`meso/meso.ts`); `whole`: the page's copy of the meso links is new, the next delta carries every link (`meso/memory.ts`). */
  | { readonly type: 'window'; readonly x: number; readonly y: number; readonly r: number; readonly whole: boolean }
  | { readonly type: 'act'; readonly id: number; readonly act: SimAct }
  | { readonly type: 'ask'; readonly id: number; readonly ask: SimAsk }
  /** The zoning tool's proposed lots planned on the worker's copy of the region (`SimHost.lotPlan`), answered with `answer`: a `LotPlan`, or null when not now. */
  | { readonly type: 'lotPlan'; readonly id: number };

// src/sim/host/protocol.ts:108-124 interface SimControls
/** What the player sets of the simulation, sent each frame (a few numbers). */
export interface SimControls {
  readonly speed: number;
  readonly paused: boolean;
  readonly traffic: boolean;
  readonly pedestrians: boolean;
  readonly trafficIntensity: number;
  readonly pedestrianIntensity: number;
  readonly demandMultiplier: number;
  readonly maxShare: number;
  /** Where the population starts (`populationGovernor.ts` cpuPopulationShare): taken before the first step is measured. */
  readonly startShare?: number;
  readonly focus: SimFocus | null;
  /** The city's time of day set to these minutes after midnight (a saved map reopened), told once. */
  readonly clock?: number;
  /** A map opening behind its curtain (`SceneHandle.opening`): its traffic's topology, bays and steps wait for it to be shown (`simHost.ts` frame). */
  readonly opening?: boolean;
}

// src/sim/host/protocol.ts:126-132 interface SimFocus
export interface SimFocus {
  readonly x: number;
  readonly y: number;
  readonly r: number;
  readonly detail: boolean;
  readonly view: { readonly ex: number; readonly ey: number; readonly dx: number; readonly dy: number; readonly cos: number; readonly far: number } | null;
}

// src/sim/host/protocol.ts:135-141 type SimAct
/** The player's hand on the simulation (the Actions): answered with `acted`. */
export type SimAct =
  | { readonly kind: 'shot'; readonly ped: number; readonly part: BodyPart; readonly fromX: number; readonly fromY: number }
  | { readonly kind: 'impact'; readonly x: number; readonly y: number; readonly kill: number; readonly scare: number }
  | { readonly kind: 'removeVehicle'; readonly id: number }
  | { readonly kind: 'strand'; readonly id: number }
  | { readonly kind: 'removeVehiclesNear'; readonly x: number; readonly y: number; readonly r: number }
  | { readonly kind: 'getUp'; readonly id: number; readonly x: number; readonly y: number; readonly heading: number; readonly seconds: number; readonly crawl?: boolean };

// src/sim/host/protocol.ts:144-148 type SimAsk
/** A question the panels ask (the inspector, the junction panel): answered with `answer`. */
export type SimAsk =
  | { readonly kind: 'segment'; readonly id: SegmentId }
  | { readonly kind: 'junction'; readonly node: NodeId }
  | { readonly kind: 'greenWave'; readonly node: NodeId }
  | { readonly kind: 'nodeLanes'; readonly node: NodeId };

// src/sim/host/protocol.ts:151-151 interface Wound
/** A bullet wound on somebody (`people/casualties.ts` recordWound), told once. */
export interface Wound { readonly id: number; readonly part: BodyPart; readonly fromX: number; readonly fromY: number }

// src/sim/host/protocol.ts:154-165 type FromSim
/** The worker to the page. */
export type FromSim =
  | { readonly type: 'ready' }
  /** A message the worker could not handle: told, so the page reports it and does not wait for a frame never coming. */
  | { readonly type: 'failed'; readonly message: string }
  | { readonly type: 'tick'; readonly slot: number; readonly vehicles: readonly (readonly [number, VehicleRich])[]; readonly peds: readonly (readonly [number, PedRich])[];
      readonly signals: readonly SignalLights[]; readonly layout: { readonly all: boolean; readonly junctions: readonly SignalLayout[] } | null; readonly trains: readonly TrainState[];
      readonly parked: ParkedChanges | null; readonly stats: SimStats;
      /** The meso links changed since the last tick (null: none), their buffers handed over (`meso/memory.ts`). */
      readonly meso?: MesoDelta | null;
      readonly wounds: readonly Wound[]; readonly casualties: readonly Casualty[] }
  | { readonly type: 'acted'; readonly id: number; readonly result: unknown }
  | { readonly type: 'answer'; readonly id: number; readonly answer: unknown };

// src/sim/host/protocol.ts:168-168 interface ParkedChanges
/** The cars come into a bay or gone from one, `PARKED_STRIDE` numbers each (fields `PK`), in a buffer handed over; `whole`: every car standing, those not in it gone. */
export interface ParkedChanges { readonly whole: boolean; readonly cars: Float64Array }

// src/sim/host/protocol.ts:170-170 variable PK
/** A parked car's numbers: its id, gone (1) or standing (0), where, its lot (-1: none), its kind (`ARCHETYPES` index) and its colour (0xrrggbb). */
export const PK = { id: 0, gone: 1, x: 2, y: 3, angle: 4, lot: 5, archetype: 6, color: 7 } as const;

// src/sim/host/protocol.ts:171-171 variable PARKED_STRIDE
export const PARKED_STRIDE = 8;

// src/sim/host/protocol.ts:174-188 interface SimStats
/** The simulation's own numbers for the status bar, the monitor and the population governor. */
export interface SimStats {
  readonly vehicles: number;
  readonly people: number;
  readonly completedTrips: number;
  readonly entryDemandLost: number;
  readonly controllers: number;
  readonly lotsMoving: number;
  /** The topology the worker's simulation stands at (a token, not a product key). */
  readonly topology: number;
  readonly issues: readonly { readonly code: string; readonly subject: string; readonly detail: string; readonly tick: number }[];
  readonly populationShare: number;
  readonly stepMs: number;
  /** Left empty: the worker keeps the zones it measured itself (`SimHost.stats`); `main.ts` zonesDue reads it until it goes. */
  readonly measured: readonly (readonly [string, Float64Array | null])[];
}
