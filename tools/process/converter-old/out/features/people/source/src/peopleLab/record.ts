// The code of src/peopleLab/record.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/record.ts:14-34 slice of src/peopleLab/record.ts#LabPerson (shared): the feature's statements; "…" marks code left out
export interface LabPerson {
  readonly spec: PersonSpec;
  // … 1 lines
  readonly style: CartoonStyle;
  // … 1 lines
  readonly hair?: HairStyle;
// … 14 lines
}

// src/peopleLab/record.ts:56-80 slice of src/peopleLab/record.ts#compactPerson (shared): the feature's statements; "…" marks code left out
export function compactPerson(person: LabPerson): LabPerson {
  // … 2 lines
  for (const [name, value] of Object.entries(spec.features)) {
    const v = round(value, 2);
    if (v !== 0) features[name] = v;
  }
  const body = Object.fromEntries(Object.entries(spec.body).map(([k, v]) => [k, round(v)])) as unknown as PersonSpec['body'];
  const look = Object.fromEntries(Object.entries(spec.look)
    .filter(([, v]) => v !== undefined && !(Array.isArray(v) && v.length === 0))
    .map(([k, v]) => [k, typeof v === 'number' && !Number.isInteger(v) ? round(v) : v])) as unknown as PersonSpec['look'];
  const style = Object.fromEntries(Object.entries(person.style).map(([k, v]) => [k, round(v)])) as unknown as CartoonStyle;
  return {
    spec: { id: spec.id, name: spec.name, ...(spec.mood !== undefined ? { mood: round(spec.mood, 2) } : {}), body, features, look },
    style,
    ...(person.hair && spec.look.hairCut?.startsWith(PROC_PREFIX) ? { hair: person.hair } : {}),
    ...(person.sculpt && person.sculpt.base === spec.look.hairCut ? { sculpt: person.sculpt } : {}),
  // … 6 lines
  };
}

// src/peopleLab/record.ts:87-94 slice of src/peopleLab/record.ts#readRoster (shared): the feature's statements; "…" marks code left out
export function readRoster(raw: unknown): LabPerson[] {
  // … 3 lines
  return people.filter((p): p is LabPerson => typeof p === 'object' && p !== null
    && typeof (p as LabPerson).spec?.id === 'number' && typeof (p as LabPerson).spec?.look === 'object'
    && typeof (p as LabPerson).style?.head === 'number');
}

// src/peopleLab/record.ts:108-113 slice of src/peopleLab/record.ts#rosterMarkdown (shared): the feature's statements; "…" marks code left out
export function rosterMarkdown(people: readonly LabPerson[]): string {
  return ['# Pessoas criadas no lab', '', `Formato \`roadcraft-people/1\`, ${people.length} pessoas.`, '',
    ...people.map((p) => `- **${p.spec.name || 'Sem nome'}** (id ${p.spec.id}): ${Math.round(yearsFromAge(p.spec.body.age))} anos, `
      + `${p.spec.body.gender < 0.5 ? 'feminino' : 'masculino'}, cabelo \`${p.spec.look.hairCut ?? 'none'}\`, roupa \`${p.spec.look.outfit ?? 'sob medida'}\`, `
      + `${savedBytes(p)} bytes`)].join('\n');
}
