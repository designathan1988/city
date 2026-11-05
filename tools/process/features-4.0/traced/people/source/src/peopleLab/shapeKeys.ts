// The code of src/peopleLab/shapeKeys.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/shapeKeys.ts:7-18 slice of src/peopleLab/shapeKeys.ts#effective (shared): the feature's statements; "…" marks code left out
export function effective(person: LabPerson): LabPerson {
  // … 1 lines
  const p = person.spec.look.extras?.includes(ATELIER)
    ? { ...person, spec: { ...person.spec, look: { ...person.spec.look, extras: person.spec.look.extras.filter((x) => x !== ATELIER) } } } : person;
  if (yearsFromAge(p.spec.body.age) >= 16) {
    if (p.spec.look.outfit) return p;
    return { ...p, spec: { ...p.spec, look: { ...p.spec.look, outfit: p.spec.body.gender < 0.5 ? 'female_casualsuit02' : 'male_casualsuit02', footwear: 'shoes01' } } };
  }
  const { outfit: _o, footwear: _f, extras: _e, ...look } = p.spec.look;
  void _o; void _f; void _e;
  return { ...p, spec: { ...p.spec, look } };
}

// src/peopleLab/shapeKeys.ts:21-27 slice of src/peopleLab/shapeKeys.ts#buildKey (shared): the feature's statements; "…" marks code left out
export function buildKey(p: LabPerson): string {
  // … 4 lines
  return JSON.stringify({ ...rest, spec: { ...e.spec, name: '', mood: 0, features: null, body: e.spec.body.gender < 0.5, look: { ...e.spec.look, outfitTint: null } } });
}

// src/peopleLab/shapeKeys.ts:30-30 slice of src/peopleLab/shapeKeys.ts#shapeOf (shared): the feature's statements; "…" marks code left out
export const shapeOf = (p: LabPerson): string => JSON.stringify([p.spec.body, p.spec.features, p.style, p.body ?? null]);

// src/peopleLab/shapeKeys.ts:35-36 slice of src/peopleLab/shapeKeys.ts#bodySlider (shared): the feature's statements; "…" marks code left out
export const bodySlider = (k: keyof MacroParams): Slider => ({ key: `body:${k}`, range: [0, 1], get: (p) => p.spec.body[k],
  set: (p, v) => ({ ...p, spec: { ...p.spec, body: { ...p.spec.body, [k]: v } } }) });

// src/peopleLab/shapeKeys.ts:37-38 slice of src/peopleLab/shapeKeys.ts#featureSlider (shared): the feature's statements; "…" marks code left out
export const featureSlider = (k: string): Slider => ({ key: `feature:${k}`, range: [-1, 1], get: (p) => p.spec.features[k] ?? 0,
  set: (p, v) => ({ ...p, spec: { ...p.spec, features: { ...p.spec.features, [k]: v } } }) });
