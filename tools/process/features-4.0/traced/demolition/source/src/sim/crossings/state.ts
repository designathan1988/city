// The code of src/sim/crossings/state.ts that is the feature's (the rest of the file is shared).

// src/sim/crossings/state.ts:33-35 function emptyCrossingState: the feature's own
export function emptyCrossingState(length: number): CrossingState {
  return { length, occupants: [], waitingFrom: 0, waitingTo: 0, demand: false, longestWait: 0 };
}
