// The code of src/sim/people/casualties.ts that is the feature's (the rest of the file is shared).

// src/sim/people/casualties.ts:46-50 function recordWound: the feature's own
export function recordWound(w: SimWorld, id: number, part: BodyPart, fromX: number, fromY: number): void {
  let list = WOUNDS.get(w);
  if (!list) WOUNDS.set(w, (list = []));
  if (list.length < 256) list.push({ id, part, fromX, fromY });
}

// src/sim/people/casualties.ts:62-66 function recordCasualty: the feature's own
export function recordCasualty(w: SimWorld, c: Casualty): void {
  let list = RECORDS.get(w);
  if (!list) RECORDS.set(w, (list = []));
  list.push(c);
}
