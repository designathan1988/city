// The code of src/render/people/crowdBodies.ts that is the feature's (the rest of the file is shared).

// src/render/people/crowdBodies.ts:32-32 function canShare: the feature's own
const canShare = (): boolean => typeof SharedArrayBuffer !== 'undefined' && (globalThis as { crossOriginIsolated?: boolean }).crossOriginIsolated === true;

// src/render/people/crowdBodies.ts:35-114 function createClassBodies: the feature's own
export function createClassBodies(shapePixels: Float32Array, vertexCount: number, shapes: number,
  jointBasis: Float32Array, bones: number): ClassBodies {
  const rowsPerBody = Math.ceil(vertexCount / WIDTH);
  const stride = rowsPerBody * WIDTH;
  // DO NOT sum a new body's vertices on the page: the basis shared once with the workers, each new body baked there.
  // DO NOT keep the caller's copy beside the shared one (two of ~6 MB a class): the shared one is the basis from here on.
  // (No inner function reads the caller's array: V8 would keep it in the closures' shared context.)
  let shared: Float32Array | null = null;
  if (canShare()) {
    shared = new Float32Array(new SharedArrayBuffer(shapePixels.byteLength));
    shared.set(shapePixels);
  }
  const basis = shared ?? shapePixels;
  // DO NOT keep the bodies on the CPU: each is baked from the shape basis, sent up, and baked again only on a restore.
  const rows = createGpuRows(WIDTH, rowsPerBody * BODIES);
  const jointPixels = new Float32Array(bones * 4 * BODIES);
  const joints = new DataTexture(jointPixels, bones, BODIES, RGBAFormat, FloatType);
  joints.minFilter = joints.magFilter = NearestFilter;
  joints.needsUpdate = true;
  const coefficients: Float32Array[] = [];
  const jointsOf: Float32Array[] = [];

  /** Body `b`'s every vertex move, baked from its coefficients here, sent up at the next flush (a restore). */
  const bake = (b: number): void => {
    rows.write(b * rowsPerBody, bodyPixels(basis, vertexCount, shapes, coefficients[b]!, stride));
  };
  /** Each body's vertices written (`pick`'s `ready`). */
  const written: Promise<void>[] = [];

  /** A body made from coefficients `c`: its vertices baked (in a worker where one can), every joint's move. */
  const make = (c: Float32Array): number => {
    const b = coefficients.length;
    coefficients.push(c.slice());
    written[b] = (shared ? bakeBodyInWorker(shared, vertexCount, shapes, c.slice(), stride) : Promise.resolve(null))
      .then((pixels) => { if (pixels) rows.write(b * rowsPerBody, pixels); else bake(b); });
    const d = new Float32Array(bones * 3);
    for (let i = 0; i < bones; i++) for (let a = 0; a < 3; a++) {
      let s = 0;
      for (let k = 0; k < shapes; k++) s += (c[k] ?? 0) * jointBasis[(i * shapes + k) * 3 + a]!;
      d[i * 3 + a] = s;
      jointPixels[(b * bones + i) * 4 + a] = s;
    }
    jointsOf.push(d);
    joints.addUpdateRange(b * bones * 4, bones * 4);
    joints.needsUpdate = true;
    return b;
  };

  return {
    get texture() { return rows.texture; },
    basis,
    joints,
    stride,
    get count() { return coefficients.length; },
    get bytes() { return rows.bytes; },
    pick(c) {
      let best = -1, bestD = Infinity;
      for (let b = 0; b < coefficients.length; b++) {
        const own = coefficients[b]!;
        let d = 0;
        for (let k = 0; k < shapes; k++) { const e = (c[k] ?? 0) - own[k]!; d += e * e; }
        if (d < bestD) { bestD = d; best = b; }
      }
      // A new body while there is room and nobody this close is made yet.
      if (coefficients.length < BODIES && (best < 0 || bestD > 1e-6)) best = make(c);
      return { index: best, joints: jointsOf[best]!, ready: written[best]! };
    },
    flush(renderer) { rows.flush(renderer); },
    restore() {
      rows.reset();
      for (let b = 0; b < coefficients.length; b++) bake(b);
      joints.clearUpdateRanges();
      joints.needsUpdate = true;
    },
    dispose() {
      rows.dispose();
      joints.dispose();
    },
  };
}
