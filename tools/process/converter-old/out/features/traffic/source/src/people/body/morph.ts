// The code of src/people/body/morph.ts that is the feature's (the rest of the file is shared).

// src/people/body/morph.ts:102-111 method Morpher.coefficients: the feature's own
  coefficients(params: MacroParams): Float64Array {
    const K = this.packs.macro.components;
    const c = new Float64Array(K);
    for (const [name, w] of macroTargetWeights(this.packs.modifiers.macro, this.macroNames, params)) {
      const t = this.macroRow.get(name);
      if (!t) continue;
      for (let k = 0; k < K; k++) c[k] = (c[k] ?? 0) + w * (this.coeff[t.row * K + k] ?? 0);
    }
    return c;
  }

// src/people/body/morph.ts:400-411 function bodyHeight: the feature's own
export function bodyHeight(positions: Float32Array, bodyRange: readonly (readonly [number, number])[]): number {
  let lo = Infinity;
  let hi = -Infinity;
  for (const [a, b] of bodyRange) {
    for (let v = a; v <= b; v++) {
      const y = positions[v * 3 + 1] ?? 0;
      if (y < lo) lo = y;
      if (y > hi) hi = y;
    }
  }
  return hi - lo;
}
