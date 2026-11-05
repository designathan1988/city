// The code of src/render/people/classRigMade.ts that is the feature's (the rest of the file is shared).

// src/render/people/classRigMade.ts:37-37 function canShare: the feature's own
const canShare = (): boolean => typeof SharedArrayBuffer !== 'undefined' && (globalThis as { crossOriginIsolated?: boolean }).crossOriginIsolated === true;

// src/render/people/classRigMade.ts:40-45 function sharedCopy: the feature's own
export function sharedCopy<T extends Float32Array | Float64Array | Int32Array>(a: T): T {
  if (!canShare() || a.buffer instanceof SharedArrayBuffer) return a;
  const c = new (a.constructor as new (b: SharedArrayBuffer) => T)(new SharedArrayBuffer(a.byteLength));
  c.set(a as never);
  return c;
}

// src/render/people/classRigMade.ts:49-62 function rigFromMade: the feature's own
export function rigFromMade(made: ClassRigMade, weights: ArrayLike<number>, joints: ArrayLike<number>): { rig: PersonRig; shape: Float32Array } {
  const scene = assemblePerson(made.person);
  let mesh: SkinnedMesh | undefined;
  scene.traverse((o) => { if (o instanceof SkinnedMesh && !mesh) mesh = o; });
  if (!mesh) throw new Error('a class rig without a body');
  mesh.geometry.userData['morphSource'] = made.morphSource;
  const positions = sharedCopy(made.deltas.positions);
  const rig: PersonRig = {
    scene, mesh, height: made.height,
    deltasData: { positions, lowestDm: made.deltas.lowestDm, bodyRange: made.deltas.bodyRange, linear: sharedCopy(made.deltas.linear) },
    deltasAt: (offset, vertices) => rigDeltasAt(rig.deltasData!, weights, joints, offset, vertices),
  };
  return { rig, shape: made.shape === made.deltas.positions ? positions : made.shape };
}
