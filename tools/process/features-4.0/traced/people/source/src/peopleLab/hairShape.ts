// The code of src/peopleLab/hairShape.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/hairShape.ts:30-89 slice of src/peopleLab/hairShape.ts#shapedPack (shared): the feature's statements; "…" marks code left out
export function shapedPack(pack: ProxyPack, base: Float32Array, s: HairShape, outOfBody?: (p: number[]) => void, rebindTo?: readonly number[], garment = false): ProxyPack {
  const P = fitProxy(pack, base);
  const n = P.length / 3;
  // … 1 lines
  for (let i = 1; i < P.length; i += 3) { lo = Math.min(lo, P[i]!); hi = Math.max(hi, P[i]!); }
  // … 9 lines
  const pivot = garment ? lo + (hi - lo) * 0.25 : hi - (hi - lo) * 0.45, span = Math.max(1e-4, hi - lo);
  const offsets = new Float32Array(pack.offsets);
  const refs = rebindTo ? new Uint32Array(pack.refs) : pack.refs, weights = rebindTo ? new Float32Array(pack.weights) : pack.weights;
  const nearest = rebindTo ? nearestThree(base, rebindTo) : null;
  const scale = axisScales(pack, base);
  for (let v = 0; v < n; v++) {
    const x = P[v * 3]!, y = P[v * 3 + 1]!, z = P[v * 3 + 2]!;
    let nx = x, ny = y, nz = z;
    // Length: the hanging part stretched or drawn up from the pivot.
    if (y < pivot) ny = pivot - (pivot - y) * s.length;
    // Out from the head: from its middle above it, level below it.
    let ox = x - cx, oy = y > cy ? y - cy : 0, oz = z - cz;
    const ol = Math.hypot(ox, oy, oz) || 1;
    ox /= ol; oy /= ol; oz /= ol;
    const down = Math.max(0, Math.min(1, (hi - y) / span));
    // Volume: more towards the ends (the roots stay on the scalp).
    const lift = s.volume * (0.15 + 0.85 * down);
    nx += ox * lift; ny += oy * lift; nz += oz * lift;
    // Waves: side to side round the head (across the way out), along the hanging part, growing from the pivot.
    if (s.wave > 0 && ny < pivot) {
      const along = (pivot - ny) / WAVE_PERIOD;
      const amount = Math.sin(along * Math.PI * 2) * s.wave * Math.min(1, along * 1.5);
      const tx = -oz, tz = ox, tl = Math.hypot(tx, tz) || 1;
      nx += tx / tl * amount; nz += tz / tl * amount;
    }
    if (outOfBody && (nx !== x || ny !== y || nz !== z)) { const q = [nx, ny, nz]; outOfBody(q); nx = q[0]!; ny = q[1]!; nz = q[2]!; }
    // Moved far (a hem let down past the knee), a garment's vertex is pinned again to the skin nearest where it is now, as MakeHuman binds a
    // piece: kept on its old thigh vertices it would follow the thigh while the knee bends through it.
    if (nearest && Math.hypot(nx - x, ny - y, nz - z) > 0.04) {
      const best = nearest([nx, ny, nz]);
      const w = best.map(([d]) => 1 / (Math.sqrt(d) + 0.05)), total = w.reduce((a, b) => a + b, 0);
      let fx = 0, fy = 0, fz = 0;
      for (let k = 0; k < 3; k++) {
        const r = best[k]![1];
        refs[v * 3 + k] = r; weights[v * 3 + k] = w[k]! / total;
        fx += base[r * 3]! * w[k]! / total; fy += base[r * 3 + 1]! * w[k]! / total; fz += base[r * 3 + 2]! * w[k]! / total;
      }
      offsets[v * 3] = (nx - fx) / scale[0]; offsets[v * 3 + 1] = (ny - fy) / scale[1]; offsets[v * 3 + 2] = (nz - fz) / scale[2];
      continue;
    }
    offsets[v * 3] = offsets[v * 3]! + (nx - x) / scale[0];
    offsets[v * 3 + 1] = offsets[v * 3 + 1]! + (ny - y) / scale[1];
    offsets[v * 3 + 2] = offsets[v * 3 + 2]! + (nz - z) / scale[2];
  }
  return { ...pack, offsets, refs, weights };
}

// src/peopleLab/hairShape.ts:92-99 slice of src/peopleLab/hairShape.ts#axisScales (shared): the feature's statements; "…" marks code left out
function axisScales(pack: ProxyPack, base: Float32Array): [number, number, number] {
// … 6 lines
}

// src/peopleLab/hairShape.ts:93-97 slice of src/peopleLab/hairShape.ts#axisScales.axis (shared): the feature's statements; "…" marks code left out
  const axis = (i: number): number => {
    const a = pack.scaleRefs[i * 2]!, b = pack.scaleRefs[i * 2 + 1]!;
    const d = Math.abs(base[a * 3 + i]! - base[b * 3 + i]!);
    return d > 0 ? d / Math.max(1e-6, pack.scaleBase[i]!) : 1;
  };
