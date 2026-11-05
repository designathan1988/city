// The code of src/peopleLab/faceRig.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/faceRig.ts:40-65 slice of src/peopleLab/faceRig.ts#attachFace (shared): the feature's statements; "…" marks code left out
export async function attachFace(built: BuiltPerson, current: () => boolean): Promise<boolean> {
  // … 3 lines
  const shapes = await expressionShapes();
  // … 4 lines
  for (const [name, delta] of Object.entries(shapes)) {
    const positions = new Float32Array(base.length);
    for (let i = 0; i < base.length; i++) positions[i] = base[i]! + delta[i]!;
    const attribute = new BufferAttribute(built.morph(positions), 3);
    attribute.name = name;
    added.push(attribute);
    if (performance.now() - last > 12) {
      await new Promise((r) => requestAnimationFrame(r));
      if (!current()) return false;
      last = performance.now();
    }
  }
// … 4 lines
}

// src/peopleLab/faceRig.ts:68-81 slice of src/peopleLab/faceRig.ts#applyLabFace (shared): the feature's statements; "…" marks code left out
export function applyLabFace(influences: number[], dictionary: Record<string, number>, face: Readonly<Record<string, number>>, strength = 1): void {
  // … 1 lines
  for (const name in face) {
    // … 3 lines
    const mix = CHANNELS[name];
    if (!mix) continue;
    for (const unit in mix) {
      const index = dictionary[unit];
      if (index !== undefined) influences[index] = Math.min(1.5, influences[index]! + mix[unit]! * amount * strength);
    }
  }
}
