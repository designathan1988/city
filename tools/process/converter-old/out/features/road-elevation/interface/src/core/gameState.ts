// What the feature uses of src/core/gameState.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/gameState.ts:26-146 class GameState
export class GameState<V extends object> { … }

// src/core/gameState.ts:48-50 method GameState.get
get<K extends keyof V>(key: K): V[K];
