// What the feature uses of src/sim/host/protocol.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/protocol.ts:83-83 interface TrainCar
/** A train drawn: its line's colour and each car's place. */
export interface TrainCar { readonly x: number; readonly y: number; readonly heading: number }

// src/sim/host/protocol.ts:84-84 interface TrainState
export interface TrainState { readonly colour: string; readonly metro: boolean; readonly cars: readonly TrainCar[] }
