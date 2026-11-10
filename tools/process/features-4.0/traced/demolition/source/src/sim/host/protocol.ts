// The code of src/sim/host/protocol.ts that is the feature's (the rest of the file is shared).

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
