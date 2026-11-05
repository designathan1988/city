// What the feature uses of src/render/people/lodPort.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/lodPort.ts:7-13 interface LodPool
/** The simplifier pool as a body's levels ask it (`personRig.ts`): handed in by the page (`lodPool.ts`); none in a worker, which simplifies on its own thread */
/** DO NOT import the pool where a person is built: the bake worker builds people (`personBuild.ts`), and the pool's import of the job system made a cycle through `job.worker.ts`. */

export interface LodPool {
  levels(req: Omit<LodRequest, 'id'>): Promise<LodLevel[]>;
  index(req: Omit<IndexRequest, 'id' | 'kind'>): Promise<Uint32Array | null>;
  ladder(req: Omit<LadderRequest, 'id' | 'kind'>): Promise<Uint32Array[]>;
  /** A vehicle part's levels, welded, simplified and creased there (`partLevelsWork.ts`). */
  part(part: PartArrays, weld: Weld): Promise<PartLevels>;
}
