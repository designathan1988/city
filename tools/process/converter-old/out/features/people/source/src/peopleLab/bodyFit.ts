// The code of src/peopleLab/bodyFit.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/bodyFit.ts:7-43 slice of src/peopleLab/bodyFit.ts#fitBody (shared): the feature's statements; "…" marks code left out
export async function fitBody(person: LabPerson, builder: LabBuilder): Promise<LabPerson> {
  // … 2 lines
  const target = BODY_CANON[person.spec.body.gender < 0.5 ? 'female' : 'male'];
  // … 19 lines
  const head = await fit1((h) => ({ ...p, style: { ...p.style, head: h } }), 'heads', target.heads, p.style.head, 0.85, 1.6);
  p = { ...p, style: { ...p.style, head } };
  // … 1 lines
  const sh = await fit1((v) => withF('measure-shoulder-dist-decr-incr', v), 'shoulders', target.shoulders, p.spec.features['measure-shoulder-dist-decr-incr'] ?? 0, -1, 1);
  p = withF('measure-shoulder-dist-decr-incr', sh);
  // … 1 lines
  if (Math.abs((await at(p)).shoulders - target.shoulders) > 0.02) {
    const tw = await fit1((v) => withF('torso-scale-horiz-decr-incr', v), 'shoulders', target.shoulders, p.spec.features['torso-scale-horiz-decr-incr'] ?? 0, -0.7, 0.7);
    p = withF('torso-scale-horiz-decr-incr', tw);
  }
  const nk = await fit1((v) => withF('measure-neck-circ-decr-incr', v), 'neck', target.neck, p.spec.features['measure-neck-circ-decr-incr'] ?? 0, -1, 1);
  p = withF('measure-neck-circ-decr-incr', nk);
  return p;
}

// src/peopleLab/bodyFit.ts:14-19 slice of src/peopleLab/bodyFit.ts#fitBody.at (shared): the feature's statements; "…" marks code left out
  const at = async (q: LabPerson): Promise<BodyMeasure> => {
    const key = JSON.stringify([q.style.head, q.spec.features['measure-shoulder-dist-decr-incr'], q.spec.features['measure-neck-circ-decr-incr'], q.spec.features['torso-scale-horiz-decr-incr']]);
    let m = shapes.get(key);
    if (!m) { m = await bodyOf(q); shapes.set(key, m); }
    return m;
  };

// src/peopleLab/bodyFit.ts:32-32 slice of src/peopleLab/bodyFit.ts#fitBody.withF (shared): the feature's statements; "…" marks code left out
  const withF = (k: string, v: number): LabPerson => ({ ...p, spec: { ...p.spec, features: { ...p.spec.features, [k]: v } } });
