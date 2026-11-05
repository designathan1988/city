// What the feature uses of src/job.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/job.worker.ts:40-53 function adopt
/** A lane another thread holds the other end of (`jobPool.ts` jobChannel): its service served on the port; a service that cannot load says so on it (`laneFailed`: the pool falls back). */
function adopt({ adopt: service, port }: AdoptLane): void;

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
