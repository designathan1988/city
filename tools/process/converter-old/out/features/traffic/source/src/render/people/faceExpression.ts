// The code of src/render/people/faceExpression.ts that is the feature's (the rest of the file is shared).

// src/render/people/faceExpression.ts:122-139 function applyFace: the feature's own
export function applyFace(influences: number[], dictionary: Record<string, number>, face: FaceWeights): void {
  influences.fill(0);
  const channels = dictionary['faceBlink'] !== undefined;
  for (const name in face) {
    const amount = face[name]!;
    if (channels || GAZE.includes(name)) {
      const index = dictionary[name];
      if (index !== undefined) influences[index] = Math.min(1, amount);
      continue;
    }
    const mix = CHANNELS[name];
    if (!mix) continue;
    for (const unit in mix) {
      const index = dictionary[unit];
      if (index !== undefined) influences[index] = Math.min(1, influences[index]! + mix[unit]! * amount);
    }
  }
}
