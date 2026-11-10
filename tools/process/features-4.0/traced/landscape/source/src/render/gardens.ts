// The code of src/render/gardens.ts that is the feature's (the rest of the file is shared).

// src/render/gardens.ts:75-154 slice of src/render/gardens.ts#gardenPack (shared): the feature's statements; "…" marks code left out
export function gardenPack(list: readonly GardenPlant[]): ReadonlyMap<number, Pack> {
  // … 2 lines
  for (const p of list) {
    // … 1 lines
    switch (p.kind) {
      case 'tree': {
        // … 2 lines
        (trees.get(p.species ?? gardenSpecies(roll)) as Placement[]).push({
          // … 1 lines
          sx: p.h * spread, sy: p.h, sz: p.h * spread * (0.9 + rng.float() * 0.2), tint: foliageTint(rng),
        });
      // … 1 lines
      }
      case 'shrub': {
        // … 4 lines
        (bushes.get(!p.plain && rng.float() < 0.25 ? 'bushFlowering' : 'bush') as Placement[]).push({
          // … 1 lines
          sx: wide, sy: tall, sz: wide * (0.9 + rng.float() * 0.2), tint: foliageTint(rng),
        });
      // … 1 lines
      }
      case 'hedge': {
        // … 4 lines
        const tint = foliageTint(rng);
        for (let i = 0; i < n; i++) {
          // … 1 lines
          (bushes.get('hedge') as Placement[]).push({
            // … 1 lines
            sx: (step / 1.9) * 1.25, sy: p.h, sz: p.d / 0.9, tint,
          });
        }
      // … 1 lines
      }
    // … 14 lines
    }
  }
// … 22 lines
}

// src/render/gardens.ts:168-186 slice of src/render/gardens.ts#gardenKinds (shared): the feature's statements; "…" marks code left out
function gardenKinds(kit: SceneryKit, pictures: SceneryPictures): KindSpec[] {
  const far = pictures.set;
  // … 12 lines
  return [
    ...TREE_SPECIES.map((s) => kind(`garden-trees-${s}`, kit.trees[s], kit.foliage, kit.foliageDepth, kit.cards[s], kit.treeCards, kit.treeCardsDepth, pictures.treeAt(s))),
    ...BUSH_KINDS.map((k) => kind(`garden-bushes-${k}`, kit.bushes[k], kit.shrubs, kit.shrubsDepth, kit.cards[k], kit.shrubCards, kit.shrubCardsDepth, pictures.bushAt(k))),
  ];
}

// src/render/gardens.ts:171-181 slice of src/render/gardens.ts#gardenKinds.kind (shared): the feature's statements; "…" marks code left out
  const kind = (name: string, body: BufferGeometry, material: Material, depth: MeshDepthMaterial, cards: BufferGeometry | undefined,
    cardMaterial: Material, cardDepth: MeshDepthMaterial, picture: number): KindSpec => ({
    // … 1 lines
    levels: { reach: far.reach[picture]!, far: PICTURE_PX, view: far.setView, ready: () => far.ready() },
    meshes: [
      // … 3 lines
      { name: `${name}-far`, geometry: far.quads[picture]!, material: far.material, depth: far.depth, castShadow: true, receiveShadow: true, cards: false, far: true, made: far.attach },
    ],
  });

// src/render/gardens.ts:189-189 slice of src/render/gardens.ts#gardenPictures (shared): the feature's statements; "…" marks code left out
const gardenPictures = (pictures: SceneryPictures): number[] => [...TREE_SPECIES.map((s) => pictures.treeAt(s)), ...BUSH_KINDS.map((k) => pictures.bushAt(k))];

// src/render/gardens.ts:246-274 slice of src/render/gardens.ts#createGardenCells (shared): the feature's statements; "…" marks code left out
export function createGardenCells(kit: SceneryKit): GardenCells {
  const pictures = sceneryPictures(kit);
  // … 1 lines
  const layer = createPropLayer({
    name: 'gardens', version: 1, kinds: gardenKinds(kit, pictures), margin: SHADOW_REACH,
    // … 2 lines
    pictures: pictures.set, pictureOf: gardenPictures(pictures),
  // … 1 lines
  });
// … 18 lines
}

// src/render/gardens.ts:269-272 slice of src/render/gardens.ts#createGardenCells.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      // … 1 lines
      pictures.release();
    },
