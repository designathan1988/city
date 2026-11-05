// The code of src/sim/host/protocol.ts that is the feature's (the rest of the file is shared).

// src/sim/host/protocol.ts:30-30 slice of src/sim/host/protocol.ts#VEHICLES (shared): the feature's statements; "…" marks code left out
const VEHICLES = FLEET_CEILING;

// src/sim/host/protocol.ts:31-31 slice of src/sim/host/protocol.ts#PEDS (shared): the feature's statements; "…" marks code left out
const PEDS = PED_CEILING;

// src/sim/host/protocol.ts:33-33 slice of src/sim/host/protocol.ts#SLOT (shared): the feature's statements; "…" marks code left out
export const SLOT = HEAD + VEHICLES * VEHICLE_STRIDE + PEDS * PED_STRIDE;

// src/sim/host/protocol.ts:34-34 slice of src/sim/host/protocol.ts#vehicleBase (shared): the feature's statements; "…" marks code left out
export const vehicleBase = (slot: number): number => slot * SLOT + HEAD;

// src/sim/host/protocol.ts:35-35 slice of src/sim/host/protocol.ts#pedBase (shared): the feature's statements; "…" marks code left out
export const pedBase = (slot: number): number => slot * SLOT + HEAD + VEHICLES * VEHICLE_STRIDE;

// src/sim/host/protocol.ts:36-36 slice of src/sim/host/protocol.ts#headBase (shared): the feature's statements; "…" marks code left out
export const headBase = (slot: number): number => slot * SLOT;

// src/sim/host/protocol.ts:37-37 slice of src/sim/host/protocol.ts#MAX_VEHICLES (shared): the feature's statements; "…" marks code left out
export const MAX_VEHICLES = VEHICLES;

// src/sim/host/protocol.ts:38-38 slice of src/sim/host/protocol.ts#MAX_PEDS (shared): the feature's statements; "…" marks code left out
export const MAX_PEDS = PEDS;

// src/sim/host/protocol.ts:50-52 slice of src/sim/host/protocol.ts#makeSharedFrames (shared): the feature's statements; "…" marks code left out
export function makeSharedFrames(): SharedFrames {
  return { data: new SharedArrayBuffer(3 * SLOT * 8), control: new SharedArrayBuffer(CONTROL_WORDS * 4) };
}

// src/sim/host/protocol.ts:55-64 slice of src/sim/host/protocol.ts#VehicleRich (shared): the feature's statements; "…" marks code left out
export interface VehicleRich {
  // … 3 lines
  readonly kerbStop: KerbStop | null;
// … 4 lines
}

// src/sim/host/protocol.ts:76-80 slice of src/sim/host/protocol.ts#SignalLights (shared): the feature's statements; "…" marks code left out
export interface SignalLights {
  // … 2 lines
  readonly states: readonly (readonly [number, SignalState])[];
}

// src/sim/host/protocol.ts:87-105 slice of src/sim/host/protocol.ts#ToSim (shared): the feature's statements; "…" marks code left out
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

// src/sim/host/protocol.ts:154-165 slice of src/sim/host/protocol.ts#FromSim (shared): the feature's statements; "…" marks code left out
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
