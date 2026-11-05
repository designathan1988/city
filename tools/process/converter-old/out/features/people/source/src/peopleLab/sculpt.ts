// The code of src/peopleLab/sculpt.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/sculpt.ts:59-63 slice of src/peopleLab/sculpt.ts#sculptedPack (shared): the feature's statements; "…" marks code left out
export function sculptedPack(pack: ProxyPack, delta: Float32Array): ProxyPack {
  const offsets = new Float32Array(pack.offsets);
  for (let i = 0; i < offsets.length && i < delta.length; i++) offsets[i] = offsets[i]! + delta[i]!;
  return { ...pack, offsets };
}

// src/peopleLab/sculpt.ts:66-73 slice of src/peopleLab/sculpt.ts#axisScales (shared): the feature's statements; "…" marks code left out
function axisScales(pack: ProxyPack, base: Float32Array): [number, number, number] {
// … 6 lines
}

// src/peopleLab/sculpt.ts:67-71 slice of src/peopleLab/sculpt.ts#axisScales.axis (shared): the feature's statements; "…" marks code left out
  const axis = (i: number): number => {
    const a = pack.scaleRefs[i * 2]!, b = pack.scaleRefs[i * 2 + 1]!;
    const d = Math.abs(base[a * 3 + i]! - base[b * 3 + i]!);
    return d > 0 ? d / Math.max(1e-6, pack.scaleBase[i]!) : 1;
  };

// src/peopleLab/sculpt.ts:107-197 slice of src/peopleLab/sculpt.ts#HairSculptor.constructor (shared): the feature's statements; "…" marks code left out
  constructor(private readonly pack: ProxyPack, body: SculptBody, start: Float32Array | null, colour: number, skinColour: number, texture: ProxyTexture | null = null) {
    // … 3 lines
    this.fitted = fitProxy(pack, body.positions);
    // … 4 lines
    this.work = new Float32Array(this.fitted);
    // … 5 lines
    const covered = new Set(pack.deleteVerts);
    for (let f = 0; f < body.faceGroup.length; f++) {
      // … 2 lines
      if (covered.has(a) || covered.has(b) || covered.has(c) || covered.has(d)) continue;
    // … 2 lines
    }
    // … 21 lines
    geometry.setIndex(new BufferAttribute(pack.index, 1));
    // … 2 lines
    if (texture && pack.uvs) {
      geometry.setAttribute('uv', new BufferAttribute(pack.uvs, 2));
      // White strands with the texture's alpha: the hair's own colour shows, the cards cut to strands.
      const pixels = new Uint8Array(texture.data.length);
      for (let i = 0; i < pixels.length; i += 4) { pixels[i] = pixels[i + 1] = pixels[i + 2] = 200 + (texture.data[i]! >> 3); pixels[i + 3] = texture.data[i + 3]!; }
      const map = new DataTexture(pixels, texture.width, texture.height, RGBAFormat);
      map.flipY = true;
      map.needsUpdate = true;
      // Its colour (grey strands) tinted by the hair's, its alpha cut out (alphaMap would read the green channel, not the alpha).
      map.colorSpace = SRGBColorSpace;
      material.map = map;
      material.alphaTest = 0.45;
    }
    // … 8 lines
    for (let v = 0; v < n; v++) {
      const key = `${Math.round(this.fitted[v * 3]! * 2000)},${Math.round(this.fitted[v * 3 + 1]! * 2000)},${Math.round(this.fitted[v * 3 + 2]! * 2000)}`;
      let id = keyed.get(key);
      if (id === undefined) { id = v; keyed.set(key, v); }
      this.weld[v] = id;
    }
    // … 1 lines
    for (let t = 0; t < pack.index.length; t += 3) {
      for (let k = 0; k < 3; k++) {
        const a = this.weld[pack.index[t + k]!]!, b = this.weld[pack.index[t + ((k + 1) % 3)]!]!;
        if (a === b) continue;
        (sets.get(a) ?? sets.set(a, new Set()).get(a)!).add(b);
        (sets.get(b) ?? sets.set(b, new Set()).get(b)!).add(a);
      }
    }
  // … 7 lines
  }

// src/peopleLab/sculpt.ts:200-204 slice of src/peopleLab/sculpt.ts#HairSculptor.delta (shared): the feature's statements; "…" marks code left out
  delta(): Float32Array {
    // … 1 lines
    for (let v = 0; v < out.length; v++) out[v] = (this.work[v]! - this.fitted[v]!) / this.scales[v % 3]!;
  // … 1 lines
  }

// src/peopleLab/sculpt.ts:211-215 slice of src/peopleLab/sculpt.ts#HairSculptor.reset (shared): the feature's statements; "…" marks code left out
  reset(): void {
    // … 1 lines
    this.work.set(this.fitted);
  // … 1 lines
  }

// src/peopleLab/sculpt.ts:242-280 slice of src/peopleLab/sculpt.ts#HairSculptor.refresh (shared): the feature's statements; "…" marks code left out
  private refresh(): void {
    const hair = this.pack.kind === 'hair';
    for (let v = 0; v < this.weld.length; v++) {
      const dx = this.work[v * 3]! - this.fitted[v * 3]!, dy = this.work[v * 3 + 1]! - this.fitted[v * 3 + 1]!, dz = this.work[v * 3 + 2]! - this.fitted[v * 3 + 2]!;
      const d = Math.hypot(dx, dy, dz);
      if (d < 1e-6) continue;
      if (d > HairSculptor.MAX_MOVE) {
        const k = HairSculptor.MAX_MOVE / d;
        this.work[v * 3] = this.fitted[v * 3]! + dx * k; this.work[v * 3 + 1] = this.fitted[v * 3 + 1]! + dy * k; this.work[v * 3 + 2] = this.fitted[v * 3 + 2]! + dz * k;
      }
      // … 1 lines
      const margin = hair ? 0.05 : 0.03;
      this.surface.collide(this.work, v * 3, margin, hair);
      // … 1 lines
      for (let k = 0; k < 2; k++) this.surface.collide(this.work, v * 3, margin, false);
    }
  // … 22 lines
  }
