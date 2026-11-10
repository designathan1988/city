// The code of src/render/partLevelsWork.ts that is the feature's (the rest of the file is shared).

// src/render/partLevelsWork.ts:113-122 function levelBuffers: the feature's own
export function levelBuffers(made: PartLevels): ArrayBuffer[] {
  const out = new Set<ArrayBuffer>();
  for (const a of [made.shared?.position, made.shared?.color, made.shared?.normal]) if (a) out.add(a.array.buffer as ArrayBuffer);
  for (const l of made.levels) {
    if (!l || l === 'same') continue;
    for (const a of [l.position, l.color, l.normal]) if (a) out.add(a.array.buffer as ArrayBuffer);
    if (l.index) out.add(l.index.buffer as ArrayBuffer);
  }
  return [...out];
}
