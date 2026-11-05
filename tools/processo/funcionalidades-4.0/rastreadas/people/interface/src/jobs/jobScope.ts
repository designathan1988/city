// What the feature uses of src/jobs/jobScope.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobScope.ts:2-5 interface JobScope
/** What a service of the job system is given in place of a worker's own global (`jobWorker.ts`): its messages, and a way to answer. */
export interface JobScope {
  onmessage: ((event: MessageEvent) => void) | null;
  postMessage(message: unknown, transfer?: Transferable[]): void;
}

// src/jobs/jobScope.ts:8-10 type JobService
/** The services the job workers run, each a module with `serve(scope)`. */
export type JobService = 'growth' | 'bays' | 'conflicts' | 'cells' | 'fracture' | 'citizenBake' | 'ecology' | 'flatGround' | 'groundShape'
  | 'garmentPadding' | 'peopleLod' | 'roadTiles' | 'roadCoarse' | 'terrainLight' | 'landStroke' | 'derivedWriter' | 'save' | 'roadStations'
  | 'cityGen' | 'roadFar' | 'networkBake';

// src/jobs/jobScope.ts:15-15 interface AdoptLane
/** Page to job worker: serve `adopt` on `port`, one end of a channel whose other end another thread holds (`jobPool.ts` jobChannel): that thread talks to this worker straight, no page in between. */
export interface AdoptLane { readonly adopt: JobService; readonly port: MessagePort }
