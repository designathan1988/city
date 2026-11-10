// The code of src/render/vehicleLevels.ts that is the feature's (the rest of the file is shared).

// src/render/vehicleLevels.ts:37-82 slice of src/render/vehicleLevels.ts#partLevels (shared): the feature's statements; "…" marks code left out
export async function partLevels(source: BufferGeometry, weld: Weld, copies = 1): Promise<(BufferGeometry | null)[]> {
  // … 12 lines
  const pool = lodPool();
  const made: PartLevels = pool ? await pool.part(part, weld) : partLevelArrays(part, weld);
  const shared = made.shared ? {
    position: attributeOf(made.shared.position), color: attributeOf(made.shared.color), normal: made.shared.normal ? attributeOf(made.shared.normal) : null,
  } : null;
  // … 1 lines
  for (const l of made.levels) {
    if (l === 'same') { levels.push(levels[levels.length - 1]!); continue; }
    if (l === null) { levels.push(null); continue; }
    const level = new BufferGeometry();
    if (l.position) {
      level.setAttribute('position', attributeOf(l.position));
      level.setAttribute('color', attributeOf(l.color!));
      level.setAttribute('normal', attributeOf(l.normal!));
    } else if (shared) {
      level.setAttribute('position', shared.position);
      level.setAttribute('color', shared.color);
      if (shared.normal) level.setAttribute('normal', shared.normal);
    }
    if (l.index) level.setIndex(new BufferAttribute(l.index, 1));
    level.boundingSphere = new Sphere(new Vector3(l.sphere[0], l.sphere[1], l.sphere[2]), l.sphere[3]);
    levels.push(level);
  }
// … 9 lines
}
