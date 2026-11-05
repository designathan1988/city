// What the feature uses of src/sim/routing/routeIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/routeIndex.ts:4-67 class RouteIndex
/** The vehicles whose route goes over each lane or movement, filed by the writers of `Vehicle.route` (`SimWorld.noteRoute`); generic, so it does not import the vehicle that the world imports. */
export class RouteIndex<V extends { … }

// src/sim/routing/routeIndex.ts:5-5 field RouteIndex.byLane
private readonly byLane = new Map<LaneletId, Set<V>>();

// src/sim/routing/routeIndex.ts:7-7 field RouteIndex.filed
/** The ids each vehicle is filed under: its route as last noted. */
  private readonly filed = new Map<V, readonly LaneletId[]>();

// src/sim/routing/routeIndex.ts:8-8 field RouteIndex.before
private readonly before = new Set<LaneletId>();

// src/sim/routing/routeIndex.ts:9-9 field RouteIndex.after
private readonly after = new Set<LaneletId>();

// src/sim/routing/routeIndex.ts:34-37 method RouteIndex.add
private add(id: LaneletId, v: V): void;

// src/sim/routing/routeIndex.ts:39-44 method RouteIndex.drop
private drop(id: LaneletId, v: V): void;

// src/sim/routing/routeIndex.ts:59-61 method RouteIndex.routing
/** The vehicles whose route goes over `id`, in no set order. */
  routing(id: LaneletId): ReadonlySet<V> | undefined;

// src/sim/routing/routeIndex.ts:70-73 function sameTail
/** Whether `tail` is `list` without its first. */
function sameTail(list: readonly LaneletId[], tail: readonly LaneletId[]): boolean;
