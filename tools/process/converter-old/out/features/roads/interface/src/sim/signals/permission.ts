// What the feature uses of src/sim/signals/permission.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/signals/permission.ts:11-26 function signalHolds
/** `mustStopAtSignal` for one vehicle and connector, with the amber decision held once it is made. */
export function signalHolds(
  v: { amberStop: string | null; rorCredit: boolean; driver: DriverParams; v: number },
  connector: string,
  state: SignalState,
  turn: TurnKind,
  distanceToStop: number,
): boolean;
