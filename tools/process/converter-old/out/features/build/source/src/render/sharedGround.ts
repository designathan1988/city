// The code of src/render/sharedGround.ts that is the feature's (the rest of the file is shared).

// src/render/sharedGround.ts:101-109 function groundAround: the feature's own
export function groundAround(data: SharedGroundData, rect: readonly [number, number, number, number]): SharedGroundData {
  const near = new Set<number>();
  const i0 = Math.floor(rect[0] / PLATE_SIZE + 0.5), i1 = Math.floor(rect[2] / PLATE_SIZE + 0.5);
  const j0 = Math.floor(rect[1] / PLATE_SIZE + 0.5), j1 = Math.floor(rect[3] / PLATE_SIZE + 0.5);
  for (let i = i0; i <= i1; i++) for (let j = j0; j <= j1; j++) near.add(plateOf(i * PLATE_SIZE, j * PLATE_SIZE));
  const plates = new Map<number, SharedArrayBuffer>(), away: number[] = [];
  for (const [key, buffer] of data.plates) if (near.has(key)) plates.set(key, buffer); else away.push(key);
  return { ...data, plates, away };
}
