// What the feature uses of src/world/junctionTurns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junctionTurns.ts:8-16 class TurnPlan
/** DO NOT sweep a generated city's turns on the page: 25 s of one frame on the whole map (2026-10-15). */
export class TurnPlan { … }

// src/world/junctionTurns.ts:9-9 field TurnPlan.recording
recording = true;

// src/world/junctionTurns.ts:10-10 field TurnPlan.asks
readonly asks = new Map<string, TurnAsk>();

// src/world/junctionTurns.ts:12-12 field TurnPlan.surfaces
/** Each junction surface the asks stand on, once a key. DO NOT copy its rings into every ask: a junction has dozens of turns. */
  readonly surfaces = new Map<string, SurfaceRings>();

// src/world/junctionTurns.ts:13-13 field TurnPlan.chosen
readonly chosen = new Map<string, ChosenTurn>();

// src/world/junctionTurns.ts:19-83 class JunctionTurns
/** DO NOT go back to one memo for the whole map with a ceiling that empties it (`MEMO_LIMIT`, then `clear`) */
export class JunctionTurns { … }

// src/world/junctionTurns.ts:20-20 field JunctionTurns.made
private readonly made = new Map<string, ChosenTurn>();

// src/world/junctionTurns.ts:24-24 field JunctionTurns.deferred
/** How many turns this build left to be chosen off the page (`defer`): a movement that left one is shaped again once chosen. */
  deferred = 0;

// src/world/junctionTurns.ts:66-70 method JunctionTurns.settle
/** Done building: the build before is let go, so what this one did not ask for goes with it. */
  settle(): JunctionTurns;

// src/world/junctionTurns.ts:73-75 method JunctionTurns.entries
/** The turns it holds, in the order found: what a worker is sent and answers (`junctionBuild.ts`). */
  entries(): [string, ChosenTurn][];

// src/world/junctionTurns.ts:78-82 method JunctionTurns.of
/** A settled junction's turns from `entries` (a worker's answer, or what it is sent). */
  static of(entries: readonly (readonly [string, ChosenTurn])[]): JunctionTurns;
