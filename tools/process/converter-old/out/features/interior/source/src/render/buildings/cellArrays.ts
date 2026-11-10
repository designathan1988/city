// The code of src/render/buildings/cellArrays.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellArrays.ts:165-170 slice of src/render/buildings/cellArrays.ts#CellArrays (shared): the feature's statements; "…" marks code left out
export interface CellArrays {
  // … 3 lines
  readonly furniture: Readonly<Partial<Record<FurnitureKind, InstanceArrays>>>;
}

// src/render/buildings/cellArrays.ts:173-178 slice of src/render/buildings/cellArrays.ts#KitBoxes (shared): the feature's statements; "…" marks code left out
export interface KitBoxes {
  // … 1 lines
  readonly furniture: Readonly<Partial<Record<FurnitureKind, readonly number[]>>>;
// … 2 lines
}

// src/render/buildings/cellArrays.ts:182-217 slice of src/render/buildings/cellArrays.ts#cellArrays (shared): the feature's statements; "…" marks code left out
export function cellArrays(chunks: readonly BuildingChunk[], selection: CellSelection, boxes: KitBoxes, groups?: readonly string[]): CellArrays {
  // … 21 lines
  const furniture: Partial<Record<FurnitureKind, InstanceArrays>> = {};
  // … 1 lines
  if (!ghost && selection.furniture && chunks.some((c) => c.furniture)) {
    for (const kind of FURNITURE_KINDS) {
      let count = 0;
      for (const chunk of chunks) count += chunk.furniture?.[kind]?.count ?? 0;
      const box = boxes.furniture[kind];
      if (count === 0 || !box) continue;
      const poses = furniturePoses(chunks, kind, count);
      furniture[kind] = { count, poses, bounds: instanceBounds(poses, count, box) };
    }
  }
  return { shells, parts, furniture };
}

// src/render/buildings/cellArrays.ts:333-345 function cellBytes: the feature's own
export function cellBytes(arrays: CellArrays): number {
  let bytes = 0;
  // An array let go once on the GPU (`gpuOnly`) counts nothing: the sum is the budget's, never a crash.
  for (const s of arrays.shells) bytes += (s.position?.byteLength ?? 0) + (s.normal?.byteLength ?? 0) + (s.paint?.byteLength ?? 0) + (s.index?.byteLength ?? 0) + (s.facade?.byteLength ?? 0) + (s.grid?.byteLength ?? 0) + (s.opening?.byteLength ?? 0);
  for (const list of [arrays.parts, arrays.furniture]) {
    for (const p of Object.values(list) as (InstanceArrays | undefined)[]) {
      if (!p) continue;
      bytes += p.poses?.byteLength ?? 0;
      for (const a of [p.instanceColor, p.litSlot, p.rooms, p.looks]) if (a) bytes += a.byteLength;
    }
  }
  return bytes;
}

// src/render/buildings/cellArrays.ts:459-482 slice of src/render/buildings/cellArrays.ts#partArrays (shared): the feature's statements; "…" marks code left out
function partArrays(chunks: readonly BuildingChunk[], kind: PartKind, count: number, glassy: boolean, coloured: boolean, looked: boolean): {
// … 1 lines
} {
  // … 3 lines
  const rooms = glassy ? new Uint16Array(count * ROOM_FLOATS) : undefined;
  // … 2 lines
  for (const chunk of chunks) {
    // … 8 lines
    if (rooms && batch.rooms) rooms.set(batch.rooms, at * ROOM_FLOATS);
  // … 2 lines
  }
  return { poses, ...(instanceColor ? { instanceColor } : {}), ...(litSlot ? { litSlot } : {}), ...(rooms ? { rooms } : {}), ...(looks ? { looks } : {}) };
}

// src/render/buildings/cellArrays.ts:485-495 slice of src/render/buildings/cellArrays.ts#furniturePoses (shared): the feature's statements; "…" marks code left out
function furniturePoses(chunks: readonly BuildingChunk[], kind: FurnitureKind, count: number): Float32Array {
  // … 2 lines
  for (const chunk of chunks) {
    const batch = chunk.furniture?.[kind];
    if (!batch) continue;
    poses.set(batch.poses, at * POSE_FLOATS);
    at += batch.count;
  }
// … 1 lines
}
