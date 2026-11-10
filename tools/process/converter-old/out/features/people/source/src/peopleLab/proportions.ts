// The code of src/peopleLab/proportions.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/proportions.ts:90-103 slice of src/peopleLab/proportions.ts#FaceProportions.constructor (shared): the feature's statements; "…" marks code left out
  constructor(assets: PeopleAssets, private readonly morpher: Morpher,
    private readonly shapeOf: (spec: PersonSpec) => Float32Array) {
    const m = assets.mesh;
    const headBone = m.boneNames.indexOf('head');
    // … 1 lines
    for (let v = 0; v < m.vertexCount; v++) {
      for (let k = 0; k < 4; k++) if (m.joints[v * 4 + k] === headBone && m.weights[v * 4 + k]! > 40000) { head.push(v); break; }
    }
    // … 1 lines
    const eyeGroups = new Set([m.faceGroups.indexOf('helper-l-eye'), m.faceGroups.indexOf('helper-r-eye')]);
    // … 1 lines
    for (let f = 0; f < m.faceGroup.length; f++) if (eyeGroups.has(m.faceGroup[f]!)) for (let k = 0; k < 4; k++) eyes.add(m.faces[f * 4 + k]!);
  // … 1 lines
  }

// src/peopleLab/proportions.ts:106-121 slice of src/peopleLab/proportions.ts#FaceProportions.influenceOf (shared): the feature's statements; "…" marks code left out
  influenceOf(slider: string, spec: PersonSpec): Influence {
    // … 1 lines
    if (out) return out;
    const rest = this.morpher.shape(spec.body, {});
    const moved = this.morpher.shape(spec.body, { [slider]: 1 });
    out = new Float32Array(this.morpher.vertexCount);
    // … 1 lines
    for (let v = 0; v < out.length; v++) {
      const d = Math.hypot(moved[v * 3]! - rest[v * 3]!, moved[v * 3 + 1]! - rest[v * 3 + 1]!, moved[v * 3 + 2]! - rest[v * 3 + 2]!);
      out[v] = d;
      most = Math.max(most, d);
    }
    if (most > 0) for (let v = 0; v < out.length; v++) out[v] = out[v]! / most;
    this.influence.set(slider, out);
    return out;
  }

// src/peopleLab/proportions.ts:124-133 slice of src/peopleLab/proportions.ts#FaceProportions.landmark (shared): the feature's statements; "…" marks code left out
  private landmark(slider: string, spec: PersonSpec, side: 'left' | 'any'): number {
    const inf = this.influenceOf(slider, spec);
    const base = this.morpher.base;
    // … 1 lines
    for (let v = 0; v < inf.length; v++) {
      if (side === 'left' && base[v * 3]! <= 0.02) continue;
      if (inf[v]! > most) { most = inf[v]!; best = v; }
    }
    return best;
  }

// src/peopleLab/proportions.ts:135-173 slice of src/peopleLab/proportions.ts#FaceProportions.measure (shared): the feature's statements; "…" marks code left out
  measure(spec: PersonSpec): FaceMeasure {
    const p = this.shapeOf(spec);
  // … 36 lines
  }

// src/peopleLab/proportions.ts:215-253 slice of src/peopleLab/proportions.ts#measureBody (shared): the feature's statements; "…" marks code left out
export function measureBody(assets: PeopleAssets, p: Float32Array): BodyMeasure {
  const m = assets.mesh;
  // … 6 lines
  const bones = m.boneNames;
  // … 7 lines
  for (const [a, b] of assets.bodyRange) for (let v = a; v <= b; v++) skin.push(v);
// … 21 lines
}

// src/peopleLab/proportions.ts:218-222 slice of src/peopleLab/proportions.ts#measureBody.centre (shared): the feature's statements; "…" marks code left out
  const centre = (group: string): number => {
    // … 1 lines
    for (const [a, b] of m.vertexGroups[group] ?? []) for (let v = a; v <= b; v++) { s += y(v); c++; }
    return c ? s / c : 0;
  };

// src/peopleLab/proportions.ts:224-228 slice of src/peopleLab/proportions.ts#measureBody.main (shared): the feature's statements; "…" marks code left out
  const main = (v: number): string => {
    // … 1 lines
    for (let k = 0; k < 4; k++) if (m.weights[v * 4 + k]! > best) { best = m.weights[v * 4 + k]!; bone = m.joints[v * 4 + k]!; }
    return bones[bone] ?? '';
  };
