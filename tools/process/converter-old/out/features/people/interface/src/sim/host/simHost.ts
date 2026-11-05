// What the feature uses of src/sim/host/simHost.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simHost.ts:80-80 field SimHost.casualtiesSent
/** The casualties already told (the page keeps their clocks). */
  private readonly casualtiesSent = new WeakSet<Casualty>();

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

// src/sim/host/simHost.ts:332-352 method SimHost.parked
/** The cars come into a bay or gone from one since the last frame, each once as it stands now; null when nothing changed (every car standing, when the log was let go for this reader). */
  private parked(): ParkedChanges | null;

// src/sim/host/simHost.ts:370-401 method SimHost.act
/** The player's hand: a shot, a blow, a vehicle taken away; what the simulation answers. */
  private act(act: SimAct): unknown;

// src/sim/host/simHost.ts:444-444 variable PED_RICH
/** What a person is told with, by reference: told again when any of these changes. */
const PED_RICH = ['panic', 'maimed', 'lost', 'bleeding', 'wound', 'style', 'hand', 'carry', 'ageClass', 'gender', 'party', 'ground', 'stretch', 'waitingFor', 'gesture'] as const satisfies readonly (keyof PedView)[];

// src/sim/host/simHost.ts:446-448 function pedRichKey
function pedRichKey(p: PedView): readonly unknown[];

// src/sim/host/simHost.ts:451-454 function pedDiffers
/** Whether a person differs from its key (`pedRichKey`), field by field, with no key made. */
function pedDiffers(p: PedView, key: readonly unknown[]): boolean;

// src/sim/host/simHost.ts:457-460 function pedRich
/** DO NOT copy it a second time: posted in the same call (`publish`), the message copies it. */
function pedRich(p: PedView): PedRich;
