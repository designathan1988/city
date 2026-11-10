// The code of src/sim/host/protocol.ts that is the feature's (the rest of the file is shared).

// src/sim/host/protocol.ts:55-64 slice of src/sim/host/protocol.ts#VehicleRich (shared): the feature's statements; "…" marks code left out
export interface VehicleRich {
  // … 5 lines
  readonly peopleAge: readonly (PersonAgeClass | undefined)[];
// … 2 lines
}

// src/sim/host/protocol.ts:67-67 slice of src/sim/host/protocol.ts#PedRich (shared): the feature's statements; "…" marks code left out
export type PedRich = Omit<PedView, 'id' | 'x' | 'y' | 'heading' | 'prev' | 'v' | 'turnV' | 'age' | 'segment' | 'walking' | 'kerbWait' | 'rank'>;

// src/sim/host/protocol.ts:135-141 slice of src/sim/host/protocol.ts#SimAct (shared): the feature's statements; "…" marks code left out
export type SimAct =
  | { readonly kind: 'shot'; readonly ped: number; readonly part: BodyPart; readonly fromX: number; readonly fromY: number }
  | { readonly kind: 'impact'; readonly x: number; readonly y: number; readonly kill: number; readonly scare: number }
  | { readonly kind: 'removeVehicle'; readonly id: number }
  | { readonly kind: 'strand'; readonly id: number }
  | { readonly kind: 'removeVehiclesNear'; readonly x: number; readonly y: number; readonly r: number }
  | { readonly kind: 'getUp'; readonly id: number; readonly x: number; readonly y: number; readonly heading: number; readonly seconds: number; readonly crawl?: boolean };

// src/sim/host/protocol.ts:151-151 slice of src/sim/host/protocol.ts#Wound (shared): the feature's statements; "…" marks code left out
export interface Wound { readonly id: number; readonly part: BodyPart; readonly fromX: number; readonly fromY: number }

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
