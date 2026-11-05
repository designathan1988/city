// What the feature uses of src/sim/host/simHost.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simHost.ts:41-41 field SimHost.buildings
/** The buildings as traffic reads them, by the world's cells, written in the copy's diary. */
  private readonly buildings = new TrafficBuildingTable(this.doc.changes);

// src/sim/host/simHost.ts:44-44 field SimHost.meso
/** The meso traffic and its micro window: made before the world, so a window told early is kept for it. */
  private readonly meso = new Meso();

// src/sim/host/simHost.ts:56-56 field SimHost.governor
private readonly governor = new PopulationGovernor(1);

// src/sim/host/simHost.ts:67-67 field SimHost.odometer
private readonly odometer = new WheelOdometer();

// src/sim/host/simHost.ts:87-111 method SimHost.handle
async handle(message: ToSim): Promise<void>;

// src/sim/host/simHost.ts:113-144 method SimHost.init
private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void>;

// src/sim/host/simHost.ts:147-158 method SimHost.load
/** Another map: the replica made whole again, and nothing of the old simulation carried over. */
  private load(map: TrafficMap, bake: NetworkBake | null): void;

// src/sim/host/simHost.ts:161-199 method SimHost.frame
/** One frame of the page: the topology brought up, the city stepped by the wall time, the frame written for the page. */
  private frame(wall: number, hold: boolean, controls: SimControls): void;

// src/sim/host/simHost.ts:202-281 method SimHost.publish
/** The frame written into the slot neither the page reads nor was written last (a triple buffer), then made the latest. */
  private publish(alpha: number): void;

// src/sim/host/simHost.ts:321-323 method SimHost.tellEvery
/** @rebuild Every junction's lights told: the first frame, another map, or the signals' log forgotten. */
  private tellEvery(tell: (node: NodeId, controller: SignalController) => void): void;

// src/sim/host/simHost.ts:325-329 method SimHost.planId
private planId(c: SignalController): number;

// src/sim/host/simHost.ts:332-352 method SimHost.parked
/** The cars come into a bay or gone from one since the last frame, each once as it stands now; null when nothing changed (every car standing, when the log was let go for this reader). */
  private parked(): ParkedChanges | null;

// src/sim/host/simHost.ts:354-367 method SimHost.stats
private stats(): SimStats;

// src/sim/host/simHost.ts:370-401 method SimHost.act
/** The player's hand: a shot, a blow, a vehicle taken away; what the simulation answers. */
  private act(act: SimAct): unknown;

// src/sim/host/simHost.ts:437-441 function sameList
/** Two lists the same, item by item. */
function sameList<T>(a: readonly T[], b: readonly T[]): boolean;

// src/sim/host/simHost.ts:463-465 function archetypeMap
/** Each kind of vehicle's place in `ARCHETYPES`, as the parked cars' numbers carry it (`PK.archetype`): kept by the host, not loose in the module. */
function archetypeMap(): Map<string, number>;
