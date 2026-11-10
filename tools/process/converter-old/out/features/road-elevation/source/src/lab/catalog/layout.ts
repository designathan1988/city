// The code of src/lab/catalog/layout.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layout.ts:89-91 slice of src/lab/catalog/layout.ts#road (shared): the feature's statements; "…" marks code left out
function road(doc: RoadDoc, type: number, x0: number, y0: number, x1: number, y1: number, structure: RoadStructure = 'ground', direction: SegmentDirection = 'both'): void {
  doc.addSegment(doc.addNode({ x: x0, y: y0 }).id, doc.addNode({ x: x1, y: y1 }).id, type, null, 0, direction, null, structure);
}

// src/lab/catalog/layout.ts:111-136 slice of src/lab/catalog/layout.ts#surfacesOf (shared): the feature's statements; "…" marks code left out
async function surfacesOf(net: Network, materials: SceneMaterials, groundMaterial: Material, paint: Map<string, Material>): Promise<Group> {
  // … 1 lines
  const elevation = buildRoadElevation(net, flat);
  // … 6 lines
  const steps = roadSurfaceSteps(net, elevation, materials, flat, reuse, groundMaterial, tiler);
  for (;;) {
    const step = steps.next();
    if (step.done) { step.value.adopt(); break; }
    if (step.value === 'wait') await new Promise((done) => setTimeout(done, 0));
  }
  // … 7 lines
  const details = buildStructureDetails(net, elevation, flat, materials, groundMaterial);
  group.add(details.group);
// … 1 lines
}
