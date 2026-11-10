// The code of src/render/mesh/hiddenFaces.ts that is the feature's (the rest of the file is shared).

// src/render/mesh/hiddenFaces.ts:99-129 function dropFacing: the feature's own
export function dropFacing(part: BufferGeometry, towards: Vector3, cos = 0.99): BufferGeometry {
  const index = part.index;
  if (!index) return part;
  const position = part.getAttribute('position');
  const a = new Vector3(), b = new Vector3(), c = new Vector3(), e = new Vector3(), f = new Vector3();
  const kept: number[] = [];
  for (let i = 0; i < index.count; i += 3) {
    a.fromBufferAttribute(position, index.getX(i));
    b.fromBufferAttribute(position, index.getX(i + 1));
    c.fromBufferAttribute(position, index.getX(i + 2));
    const n = e.subVectors(b, a).cross(f.subVectors(c, a)).normalize();
    if (n.dot(towards) > cos) continue;
    kept.push(index.getX(i), index.getX(i + 1), index.getX(i + 2));
  }
  if (kept.length === index.count) return part;
  const remap = new Int32Array(position.count).fill(-1);
  let used = 0;
  for (const v of kept) if (remap[v] === -1) remap[v] = used++;
  for (const [name, attribute] of Object.entries(part.attributes)) {
    const size = attribute.itemSize;
    const array = new Float32Array(used * size);
    for (let v = 0; v < remap.length; v++) {
      const to = remap[v]!;
      if (to >= 0) for (let k = 0; k < size; k++) array[to * size + k] = attribute.array[v * size + k]!;
    }
    part.setAttribute(name, new BufferAttribute(array, size, attribute.normalized));
  }
  part.setIndex(kept.map((v) => remap[v]!));
  part.clearGroups();
  return part;
}
