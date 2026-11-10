// The code of src/render/bakePool.ts that is the feature's (the rest of the file is shared).

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

// src/render/bakePool.ts:174-177 function bakeBodyInWorker: the feature's own
export async function bakeBodyInWorker(basis: Float32Array, vertexCount: number, shapes: number, c: Float32Array, stride: number): Promise<Float32Array | null> {
  const reply = await run({ body: { basis, vertexCount, shapes, c, stride } });
  return reply?.pixels ?? null;
}

// src/render/bakePool.ts:180-183 function personFaceInWorker: the feature's own
export async function personFaceInWorker(job: PersonFaceJob): Promise<{ readonly tall: number; readonly level0: number; readonly face: Float32Array | null } | null> {
  const reply = await run({ face: job }, [], true);
  return reply?.heights ? { tall: reply.heights[0]!, level0: reply.heights[1]!, face: reply.face ?? null } : null;
}

// src/render/bakePool.ts:186-189 function classRigInWorker: the feature's own
export async function classRigInWorker(sex: Sex, band: AgeBand): Promise<ClassRigMade | null> {
  const reply = await run({ klass: { sex, band } }, [], true);
  return reply?.klass ?? null;
}

// src/render/bakePool.ts:192-195 function wearInWorker: the feature's own
export async function wearInWorker(sex: Sex, band: AgeBand, pack: ProxyPack): Promise<WornPiece | null> {
  const reply = await run({ wear: { sex, band, pack } }, [], true);
  return reply?.worn ?? null;
}
