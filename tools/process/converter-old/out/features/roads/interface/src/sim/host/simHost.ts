// What the feature uses of src/sim/host/simHost.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simHost.ts:39-39 field SimHost.net
private net = new Network(this.doc);

// src/sim/host/simHost.ts:113-144 method SimHost.init
private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void>;

// src/sim/host/simHost.ts:147-158 method SimHost.load
/** Another map: the replica made whole again, and nothing of the old simulation carried over. */
  private load(map: TrafficMap, bake: NetworkBake | null): void;

// src/sim/host/simHost.ts:161-199 method SimHost.frame
/** One frame of the page: the topology brought up, the city stepped by the wall time, the frame written for the page. */
  private frame(wall: number, hold: boolean, controls: SimControls): void;

// src/sim/host/simHost.ts:404-409 method SimHost.lotPlan
/** THE ZONING TOOL'S PROPOSAL (`editor/lotTool.ts`): the land along the streets cut into lots on this copy of the region, which the page's diary keeps as the page has it (sent before the question, `SimClient.lotPlan`) - planned again only where the roads and the lots changed (`world/lots.ts` LotPlanner, one for this network), DO NOT plan it on the page. Null while a tile's network is being read (its junctions would be solved here): asked again. */
  private lotPlan(): LotPlan | null;

// src/sim/host/simHost.ts:412-416 method SimHost.ask
/** The panels' questions, answered off the simulation as it stands (`simAnswers.ts`). */
  private ask(ask: SimAsk): unknown;
