// The code of src/render/citizenBake.ts that is the feature's (the rest of the file is shared).

// src/render/citizenBake.ts:447-449 function gaitClips: the feature's own
export function gaitClips(clips: readonly ClipFrames[]): GaitClips {
  return Object.fromEntries(Object.entries(GAIT_AT).map(([name, at]) => [name, clips[at]!])) as unknown as GaitClips;
}

// src/render/citizenBake.ts:475-499 function rigData: the feature's own
export function rigData(scene: Object3D): { readonly nodes: RigNode[]; readonly transfer: ArrayBuffer[] } {
  const order: Object3D[] = [];
  scene.traverse((o) => order.push(o));
  const at = new Map(order.map((o, i) => [o, i] as const));
  const transfer: ArrayBuffer[] = [];
  const copy = (a: ArrayLike<number>): Float32Array => { const f = Float32Array.from(a); transfer.push(f.buffer as ArrayBuffer); return f; };
  const nodes = order.map((o): RigNode => {
    const base = { name: o.name, parent: o.parent && at.has(o.parent) ? at.get(o.parent)! : -1,
      p: o.position.toArray(), q: o.quaternion.toArray(), s: o.scale.toArray() };
    if (o instanceof SkinnedMesh) {
      const g = o.geometry;
      const inverses = new Float32Array(o.skeleton.boneInverses.length * 16);
      o.skeleton.boneInverses.forEach((m, i) => inverses.set(m.elements, i * 16));
      transfer.push(inverses.buffer as ArrayBuffer);
      return { ...base, kind: 'skinned', skin: {
        bones: o.skeleton.bones.map((b) => at.get(b) ?? -1), inverses, bind: [...o.bindMatrix.elements], bindMode: o.bindMode,
        position: copy(g.getAttribute('position').array as ArrayLike<number>),
        skinIndex: copy(g.getAttribute('skinIndex').array as ArrayLike<number>),
        skinWeight: copy(g.getAttribute('skinWeight').array as ArrayLike<number>),
      } };
    }
    return { ...base, kind: (o as Bone).isBone ? 'bone' : 'node' };
  });
  return { nodes, transfer };
}
