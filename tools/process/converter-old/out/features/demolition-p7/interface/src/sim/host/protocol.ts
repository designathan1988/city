// What the feature uses of src/sim/host/protocol.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/protocol.ts:45-48 interface SharedFrames
/** The memory both sides hold: three slots (a triple buffer) and the control words. */
export interface SharedFrames {
  readonly data: SharedArrayBuffer;
  readonly control: SharedArrayBuffer;
}

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
