// The code of src/peopleLab/polish.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/polish.ts:11-28 slice of src/peopleLab/polish.ts#clearScalpPaint (shared): the feature's statements; "…" marks code left out
function clearScalpPaint(mesh: SkinnedMesh, look: PersonLook): void {
  // … 6 lines
  const skin = new Color(look.skin);
  for (let i = body.start; i < body.start + body.count; i++) {
    // … 4 lines
    colour.setXYZ(v, skin.r, skin.g, skin.b);
  }
// … 2 lines
}

// src/peopleLab/polish.ts:122-130 slice of src/peopleLab/polish.ts#solidHair (shared): the feature's statements; "…" marks code left out
function solidHair(mesh: SkinnedMesh, look: PersonLook): void {
  // … 4 lines
  const hair = new Color(look.hair);
  for (let v = 0; v < hairMask.count; v++) if (Math.round(hairMask.getX(v)) === HAIR_SLOT) colour.setXYZ(v, hair.r, hair.g, hair.b);
// … 1 lines
}

// src/peopleLab/polish.ts:192-205 slice of src/peopleLab/polish.ts#polishPerson (shared): the feature's statements; "…" marks code left out
export function polishPerson(meshes: readonly SkinnedMesh[], look: PersonLook, sculpted = false): void {
  for (const mesh of meshes) {
    // … 6 lines
    clearScalpPaint(mesh, look);
    solidHair(mesh, look);
  // … 2 lines
  }
}
