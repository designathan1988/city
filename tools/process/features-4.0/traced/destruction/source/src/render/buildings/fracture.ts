// The code of src/render/buildings/fracture.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/fracture.ts:105-115 function modelArrays: the feature's own
function modelArrays(source: BufferGeometry): { position: Float32Array; normal: Float32Array; uv: Float32Array | null; colour: Float32Array | null } {
  let known = MODEL_ARRAYS.get(source);
  if (!known) {
    const geo = source.index ? source.toNonIndexed() : source;
    const arr = (name: string): Float32Array | null => { const at = geo.getAttribute(name); return at ? Float32Array.from(at.array as ArrayLike<number>) : null; };
    known = { position: arr('position')!, normal: arr('normal')!, uv: arr('uv'), colour: arr('color') };
    if (geo !== source) geo.dispose();
    MODEL_ARRAYS.set(source, known);
  }
  return known;
}

// src/render/buildings/fracture.ts:118-161 function prepareFracture: the feature's own
export function prepareFracture(
  chunk: BuildingChunk,
  kit: BuildingKit,
  slabs: readonly { readonly corners: readonly Vector3[]; readonly y: number }[],
  seed: number,
  furniture?: Partial<Record<string, { poses: Float32Array; count: number }>>,
): { input: FractureInput; materials: Material[] } {
  const materials: Material[] = [];
  const index = new Map<Material, number>();
  const materialIndex = (mat: Material): number => {
    let i = index.get(mat);
    if (i === undefined) { i = materials.length; materials.push(mat); index.set(mat, i); }
    return i;
  };
  const shells: FractureInput['shells'][number][] = [];
  // Every finish in the ruin's one material, each vertex its finish id, uv and colour carried as its piece moves (`finishes.ts`).
  const shellMat = materialIndex(kit.ruinShell);
  for (const [finish, part] of Object.entries(chunk.shells) as [Finish, NonNullable<BuildingChunk['shells'][Finish]>][]) {
    shells.push({ mat: shellMat, finish: finishId(finish), position: part.position, normal: part.normal, paint: part.paint, index: part.index });
  }
  const parts: FractureInput['parts'][number][] = [];
  const model = (source: BufferGeometry, mat: number, batch: { poses: Float32Array; count: number; colours?: Float32Array | null }): void => {
    const arrays = modelArrays(source);
    // Each instance's matrix as the GPU makes it from its pose (`partPose.ts`): a ruin's pieces are laid in the world.
    parts.push({ mat, ...arrays, instColours: batch.colours ?? null, matrices: poseMatrices(batch.poses, batch.count), count: batch.count });
  };
  for (const [kind, batch] of Object.entries(chunk.parts) as [PartKind, BuildingChunk['parts'][PartKind]][]) {
    if (!batch.count || kind === 'water') continue;
    model(kit.geometry[kind], materialIndex(kit.material[kind]), batch);
  }
  if (furniture) {
    const fk = kit.furniture();
    const mat = materialIndex(fk.material);
    for (const [kind, batch] of Object.entries(furniture)) {
      const source = (fk.geometry as Record<string, BufferGeometry>)[kind];
      if (batch && batch.count && source) model(source, mat, batch);
    }
  }
  return {
    input: { shells, parts, slabMat: shellMat, slabFinish: finishId('concrete'), seed,
      slabs: slabs.map((sl) => ({ corners: sl.corners.map((c) => [c.x, c.y, c.z] as const), y: sl.y })) },
    materials,
  };
}

// src/render/buildings/fracture.ts:173-202 function fragmentGeometry: the feature's own
export function fragmentGeometry(data: StillData, index: number): BufferGeometry {
  const slots = data.slots[index]!;
  const [cx, cy, cz] = data.fragments[index]!.centre;
  let total = 0;
  for (let s = 0; s < slots.length; s += 3) total += slots[s + 1]!;
  const position = new Float32Array(total * 3), normal = new Float32Array(total * 3), colour = new Float32Array(total * 3);
  const uv = new Float32Array(total * 2), decay = new Float32Array(total), finish = new Float32Array(total);
  const g = new BufferGeometry();
  let at = 0;
  for (let s = 0; s < slots.length; s += 3) {
    const start = slots[s]!, count = slots[s + 1]!;
    for (let i = start; i < start + count; i++, at++) {
      position[at * 3] = data.position[i * 3]! - cx; position[at * 3 + 1] = data.position[i * 3 + 1]! - cy; position[at * 3 + 2] = data.position[i * 3 + 2]! - cz;
    }
    normal.set(data.normal.subarray(start * 3, (start + count) * 3), (at - count) * 3);
    colour.set(data.colour.subarray(start * 3, (start + count) * 3), (at - count) * 3);
    uv.set(data.uv.subarray(start * 2, (start + count) * 2), (at - count) * 2);
    decay.set(data.decay.subarray(start, start + count), at - count);
    finish.set(data.finish.subarray(start, start + count), at - count);
    g.addGroup(at - count, count, slots[s + 2]!);
  }
  g.setAttribute('position', new BufferAttribute(position, 3));
  g.setAttribute('normal', new BufferAttribute(normal, 3));
  g.setAttribute('color', new BufferAttribute(colour, 3));
  g.setAttribute('uv', new BufferAttribute(uv, 2));
  g.setAttribute('aDecay', new BufferAttribute(decay, 1));
  g.setAttribute(FINISH_ATTRIBUTE, new BufferAttribute(finish, 1));
  g.boundingSphere = new Sphere(new Vector3(), data.fragments[index]!.radius);
  return g;
}
