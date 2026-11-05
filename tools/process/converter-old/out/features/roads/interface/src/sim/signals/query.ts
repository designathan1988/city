// What the feature uses of src/sim/signals/query.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/signals/query.ts:5-5 type SignalState
export type SignalState = 'green' | 'amber' | 'red';

// src/sim/signals/query.ts:6-6 type WalkState
export type WalkState = 'walk' | 'flash' | 'dont';

// src/sim/signals/query.ts:9-16 function signalStateFor
/** Signal state for one approach group. */
export function signalStateFor(c: SignalController, group: GroupId): SignalState;

// src/sim/signals/query.ts:40-50 function pedestrianSignalState
/** Walk indicator for one crossing. */
export function pedestrianSignalState(
  c: SignalController,
  crossing: CrossingId,
  crossingLength: number,
): WalkState;
