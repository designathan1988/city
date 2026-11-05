// What the feature uses of src/job.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/job.worker.ts:6-6 type Serve
/** ONE WORKER OF THE JOB SYSTEM (`jobs/jobPool.ts`): at the root with the other entries, since it puts every service together: every service's ports in it, each service's module loaded on its first message */

type Serve = (scope: JobScope) => void;

// src/job.worker.ts:8-35 variable LOADERS
const LOADERS: Readonly<Record<JobService, () => Promise<{ serve: Serve }>>> = {
  growth: () => import('./growth.worker'),
  bays: () => import('./bays.worker'),
  conflicts: () => import('./conflictTable.worker'),
  cells: () => import('./render/buildings/cell.worker'),
  fracture: () => import('./render/buildings/fracture.worker'),
  citizenBake: () => import('./render/citizenBake.worker'),
  ecology: () => import('./render/ecology.worker'),
  flatGround: () => import('./render/flatGround.worker'),
  groundShape: () => import('./render/groundShape.worker'),
  garmentPadding: () => import('./render/people/garmentPadding.worker'),
  peopleLod: () => import('./render/people/lodWorker'),
  roadTiles: () => import('./render/roadTile.worker'),
  // The road blocks' far levels (`roadTilePool.ts` coarsen): the same worker, on the page's own workers.
  roadCoarse: () => import('./render/roadTile.worker'),
  roadStations: () => import('./render/roadStations.worker'),
  // The whole network's far roads at a large city's opening (`render/networkFarRoads.ts`).
  roadFar: () => import('./render/networkFarRoads.worker'),
  // The whole finite city built with the game's own generator, off the page (`cityGen.worker.ts`).
  cityGen: () => import('./cityGen.worker'),
  // A city's network solved while its curtain is up, taken by the page's build (`networkBake.worker.ts`).
  networkBake: () => import('./networkBake.worker'),
  terrainLight: () => import('./render/terrainLight.worker'),
  // A dab's work on a plate: its levels, its woods, its shore (`render/landStroke.ts`).
  landStroke: () => import('./render/landStroke.worker'),
  derivedWriter: () => import('./render/derivedWriter.worker'),
  save: () => import('./editor/save.worker'),
};

// src/job.worker.ts:37-37 variable host
const host = self as unknown as { onmessage: ((e: MessageEvent<ToJob | AdoptLane | CloseLane>) => void) | null; postMessage(message: FromJob, transfer?: Transferable[]): void };

// src/job.worker.ts:40-53 function adopt
/** A lane another thread holds the other end of (`jobPool.ts` jobChannel): its service served on the port; a service that cannot load says so on it (`laneFailed`: the pool falls back). */
function adopt({ adopt: service, port }: AdoptLane): void;

// src/job.worker.ts:55-55 interface Port
/** A port's scope once its service is loaded; the messages that came before, in order; the queued job it is on (`seq`); why its service never loaded. */
interface Port { scope: JobScope | null; readonly waiting: MessageEvent[]; seq: number | undefined; failed: string | null }

// src/job.worker.ts:56-56 variable ports
const ports = new Map<number, Port>();

// src/job.worker.ts:61-62 function answer
/** A port's answer, with the queued job it is for: a queue port runs one job at a time (`jobPool.ts` `submitJob`). */
const answer = (port: number, entry: Port, message: { readonly data?: unknown; readonly error?: string }, transfer: Transferable[] = []): void => …;

// src/job.worker.ts:64-94 statement <ExpressionStatement>
host.onmessage = (event) => {
  // Where this worker's store is, said by the page before any job (`jobPool.ts` `setJobWorkerSetup`, the desktop's saves).
  if (isStoreSetup(event.data)) { takeStoreSetup(event.data); return; }
  if ('adopt' in event.data) { adopt(event.data); return; }
  // DO NOT keep a lane's entry after the page let it go: the tile lanes set aside are made and let go again and again (`tileGenerator.ts`).
  if ('close' in event.data) { ports.delete(event.data.close); return; }
  const { port, service, data, seq } = event.data;
  let entry = ports.get(port);
  const message = new MessageEvent('message', { data });
  if (entry) {
    entry.seq = seq;
    // DO NOT hold a queued job behind a service that never loaded: the pool would wait for its answer for ever.
    if (entry.failed !== null) { answer(port, entry, { error: entry.failed }); return; }
    if (entry.scope) { entry.scope.onmessage?.(message); return; }
    entry.waiting.push(message);
    return;
  }
  entry = { scope: null, waiting: [message], seq, failed: null };
  ports.set(port, entry);
  const made = entry;
  void LOADERS[service]().then(({ serve }) => {
    const scope: JobScope = { onmessage: null, postMessage: (data, transfer) => answer(port, made, { data }, transfer ?? []) };
    serve(scope);
    made.scope = scope;
    for (const waiting of made.waiting.splice(0)) scope.onmessage?.(waiting);
  }, (error: unknown) => {
    made.failed = String(error);
    made.waiting.length = 0;
    answer(port, made, { error: made.failed });
  });
};
