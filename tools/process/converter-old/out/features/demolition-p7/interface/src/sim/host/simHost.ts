// What the feature uses of src/sim/host/simHost.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simHost.ts:80-80 field SimHost.casualtiesSent
/** The casualties already told (the page keeps their clocks). */
  private readonly casualtiesSent = new WeakSet<Casualty>();

// src/sim/host/simHost.ts:113-144 method SimHost.init
private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void>;

// src/sim/host/simHost.ts:161-199 method SimHost.frame
/** One frame of the page: the topology brought up, the city stepped by the wall time, the frame written for the page. */
  private frame(wall: number, hold: boolean, controls: SimControls): void;

// src/sim/host/simHost.ts:202-281 method SimHost.publish
/** The frame written into the slot neither the page reads nor was written last (a triple buffer), then made the latest. */
  private publish(alpha: number): void;
