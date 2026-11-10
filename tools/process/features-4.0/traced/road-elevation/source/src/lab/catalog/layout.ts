// The code of src/lab/catalog/layout.ts that is the feature's (the rest of the file is shared).

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
