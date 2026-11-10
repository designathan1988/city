// The code of src/render/scenery.ts that is the feature's (the rest of the file is shared).

// src/render/scenery.ts:131-172 slice of src/render/scenery.ts#SceneryKit (shared): the feature's statements; "…" marks code left out
export interface SceneryKit {
  readonly trees: Record<TreeSpecies, BufferGeometry>;
  readonly bushes: Record<BushKind, BufferGeometry>;
  // … 1 lines
  readonly cards: Partial<Record<TreeSpecies | BushKind, BufferGeometry>>;
  // … 4 lines
  readonly furniture: Record<Exclude<FurnitureKind, 'streetTree' | 'shrub'>, BufferGeometry>;
// … 31 lines
}

// src/render/scenery.ts:175-312 slice of src/render/scenery.ts#createSceneryKit (shared): the feature's statements; "…" marks code left out
export function createSceneryKit(): SceneryKit {
  const treeModels = TREE_SPECIES.map((s) => [s, treeParts(s)] as const);
  const bushModels = BUSH_KINDS.map((k) => [k, bushParts(k)] as const);
  const trees = Object.fromEntries(treeModels.map(([s, p]) => [s, p.body])) as Record<TreeSpecies, BufferGeometry>;
  const bushes = Object.fromEntries(bushModels.map(([k, p]) => [k, p.body])) as Record<BushKind, BufferGeometry>;
  const cards: Partial<Record<TreeSpecies | BushKind, BufferGeometry>> = {};
  for (const [kind, parts] of [...treeModels, ...bushModels]) if (parts.cards) cards[kind] = parts.cards;
  // … 1 lines
  for (const g of [...Object.values(trees), ...Object.values(bushes), ...Object.values(cards)]) g?.computeBoundingSphere();
  // … 2 lines
  const furniture = {
    lamp: lampGeometry(),
    bin: binGeometry(),
    bench: benchGeometry(),
    hydrant: hydrantGeometry(),
    postbox: postboxGeometry(),
    phone: phoneGeometry(),
    drain: drainGeometry(),
  };
  const lampLens = lampLensGeometry();
  const lampFar = lampFarGeometry();
  lampLens.computeBoundingBox();
  const lensBox = lampLens.boundingBox!;
  const lens = { reach: (lensBox.min.x + lensBox.max.x) / 2, height: (lensBox.min.y + lensBox.max.y) / 2 };
  const treePit = treePitGeometry();
  const tuft = grassTuftGeometry();
  const flower = wildflowerGeometry();
  // … 16 lines
  const poolMap = lightPoolTexture();
  // … 28 lines
  const geometries: BufferGeometry[] = [
    ...Object.values(trees),
    ...Object.values(bushes),
    ...Object.values(cards) as BufferGeometry[],
    ...Object.values(furniture),
    lampLens,
    lampFar,
    treePit,
    tuft,
    flower,
    pool,
    beam,
  ];
  // … 2 lines
  return {
    trees,
    bushes,
    cards,
    // … 5 lines
    lampLens,
    lampFar,
    treePit,
    tuft,
    flower,
  // … 34 lines
  };
}

// src/render/scenery.ts:315-344 slice of src/render/scenery.ts#plantSamples (shared): the feature's statements; "…" marks code left out
export function plantSamples(kit: SceneryKit): { readonly group: Group; readonly depth: Group; dispose(): void } {
  // … 2 lines
  for (const species of TREE_SPECIES) {
    models.push([kit.trees[species], kit.foliage, kit.foliageDepth]);
    const cards = kit.cards[species];
    if (cards) models.push([cards, kit.treeCards, kit.treeCardsDepth]);
  }
  for (const kind of BUSH_KINDS) {
    models.push([kit.bushes[kind], kit.shrubs, kit.shrubsDepth]);
    const cards = kit.cards[kind];
    if (cards) models.push([cards, kit.shrubCards, kit.shrubCardsDepth]);
  }
// … 16 lines
}
