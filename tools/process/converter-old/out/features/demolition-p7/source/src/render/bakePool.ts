// The code of src/render/bakePool.ts that is the feature's (the rest of the file is shared).

// src/render/bakePool.ts:137-138 function clipsOf: the feature's own
const clipsOf = (clips: readonly (BakedClip | null)[]): (ClipFrames | undefined)[] =>
  clips.map((c) => (c ? { ...c, ...(c.head ? { head: new Matrix4().fromArray(c.head) } : {}) } as ClipFrames : undefined));

// src/render/bakePool.ts:141-146 function bakeInWorker: the feature's own
export async function bakeInWorker(scene: Object3D, sex: WalkSex): Promise<(ClipFrames | undefined)[] | null> {
  if (!usable()) return null;
  const { nodes, transfer } = rigData(scene);
  const reply = await run({ nodes, sex }, transfer);
  return reply?.clips ? clipsOf(reply.clips) : null;
}

// src/render/bakePool.ts:149-151 function bakeRig: the feature's own
export function bakeRig(scene: Object3D): readonly RigNode[] {
  return rigData(scene).nodes;
}

// src/render/bakePool.ts:154-158 function bakeDeferredInWorker: the feature's own
export async function bakeDeferredInWorker(nodes: readonly RigNode[], sex: WalkSex, at: number, facts: ShuffleFacts): Promise<ClipFrames | null> {
  // The rig copied, not moved: the body's next clip is baked on it too.
  const reply = await run({ nodes, sex, at, facts: { duration: facts.duration, stride: facts.stride } });
  return reply?.clips ? clipsOf(reply.clips)[0] ?? null : null;
}

// src/render/bakePool.ts:161-164 function uncookInWorker: the feature's own
export async function uncookInWorker(url: string): Promise<DecodedPerson | null> {
  const reply = await run({ uncook: url });
  return reply?.person ?? null;
}

// src/render/bakePool.ts:167-171 function buildPersonInWorker: the feature's own
export async function buildPersonInWorker(person: PersonSpec, gender: 'f' | 'm', style: CartoonStyle, faces: boolean): Promise<DecodedPerson | null> {
  // On the people's worker: it holds the MakeHuman packs already (`Job.people`).
  const reply = await run({ build: { person, gender, style, faces } }, [], true);
  return reply?.person ?? null;
}

// src/render/bakePool.ts:198-201 function releaseIdleBakeWorkers: the feature's own
export function releaseIdleBakeWorkers(): void {
  releaseRequested = true;
  releaseIfIdle();
}
