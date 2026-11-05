// What the feature uses of src/core/rerootMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/rerootMap.ts:5-5 variable GONE
/** A PERSISTENT MAP BY REROOTING: every version stays readable as it was, a new version costs what it writes. */
// Baker's persistent arrays with rerooting ("shallow binding"; Conchon & Filliatre, "A Persistent Union-Find Data Structure", 2007)

/** A key absent from a version (a diff's entry). */
const GONE: unique symbol = Symbol('gone');

// src/core/rerootMap.ts:6-6 type Slot
type Slot<V> = V | typeof GONE;

// src/core/rerootMap.ts:9-104 class RerootMap
/** One version of a map: the root holds the values; any other holds its differences from the version it points to. */
export class RerootMap<K, V> { … }

// src/core/rerootMap.ts:11-11 field RerootMap.data
/** Root: the values (never `GONE`). Otherwise: this version's value of each key where it differs from `up`'s. */
  private data: Map<K, Slot<V>> = new Map();

// src/core/rerootMap.ts:13-13 field RerootMap.up
/** The version this one is read through; null for the root. */
  private up: RerootMap<K, V> | null = null;

// src/core/rerootMap.ts:19-27 method RerootMap.get
/** `V` must not be `undefined`: an absent key reads as undefined. */
  get(key: K): V | undefined;
