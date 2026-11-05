// The code of src/lab/catalog/layout.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layout.ts:37-50 slice of src/lab/catalog/layout.ts#piecesOf (shared): the feature's statements; "…" marks code left out
export function piecesOf(root: Object3D): PieceSpec[] {
  // … 1 lines
  root.traverse((o) => { const coarse = (o as Mesh).isMesh ? levelsOf(o as Mesh)?.coarse : undefined; if (coarse) far.add(coarse); });
  // … 1 lines
  root.traverse((o) => {
    // … 4 lines
    const coarse = (o as Mesh).isMesh ? levelsOf(o as Mesh)?.coarse : undefined;
    out.push({ label: o.name || o.type, geometry: holder.geometry, material: holder.material ?? null, copies, ...(coarse ? { levels: [coarse.geometry] } : {}) });
  });
// … 1 lines
}

// src/lab/catalog/layout.ts:53-81 slice of src/lab/catalog/layout.ts#shown (shared): the feature's statements; "…" marks code left out
export function shown(root: Object3D, cache: BatchCache): Object3D {
  // … 4 lines
  root.traverse((o) => { const coarse = (o as Mesh).isMesh ? levelsOf(o as Mesh)?.coarse : undefined; if (coarse) far.add(coarse); });
// … 22 lines
}

// src/lab/catalog/layout.ts:89-91 slice of src/lab/catalog/layout.ts#road (shared): the feature's statements; "…" marks code left out
function road(doc: RoadDoc, type: number, x0: number, y0: number, x1: number, y1: number, structure: RoadStructure = 'ground', direction: SegmentDirection = 'both'): void {
  doc.addSegment(doc.addNode({ x: x0, y: y0 }).id, doc.addNode({ x: x1, y: y1 }).id, type, null, 0, direction, null, structure);
}

// src/lab/catalog/layout.ts:111-136 function surfacesOf: the feature's own
async function surfacesOf(net: Network, materials: SceneMaterials, groundMaterial: Material, paint: Map<string, Material>): Promise<Group> {
  const flat = (): number => 0;
  const elevation = buildRoadElevation(net, flat);
  const tiler: RoadTiler = {
    tiles: () => Promise.reject(new Error('the tiles are built on the page')),
    check: () => {},
    coarsen: (arrays, lock) => Promise.resolve(coarseArrays(structuredClone(arrays), lock)),
  };
  const reuse = { tiles: new Map(), dependsOn: () => 0, paint, chunks: new Map(), inputs: new SurfaceInputs(), near: () => true } as unknown as SurfaceReuse;
  const steps = roadSurfaceSteps(net, elevation, materials, flat, reuse, groundMaterial, tiler);
  for (;;) {
    const step = steps.next();
    if (step.done) { step.value.adopt(); break; }
    if (step.value === 'wait') await new Promise((done) => setTimeout(done, 0));
  }
  const group = new Group();
  group.name = 'roads';
  for (const { mesh } of reuse.chunks.values()) {
    group.add(mesh);
    const coarse = levelsOf(mesh)?.coarse;
    if (coarse) group.add(coarse);
  }
  const details = buildStructureDetails(net, elevation, flat, materials, groundMaterial);
  group.add(details.group);
  return group;
}
