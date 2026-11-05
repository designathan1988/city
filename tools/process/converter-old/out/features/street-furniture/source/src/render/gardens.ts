// The code of src/render/gardens.ts that is the feature's (the rest of the file is shared).

// src/render/gardens.ts:19-35 slice of src/render/gardens.ts#GardenPlant (shared): the feature's statements; "…" marks code left out
export interface GardenPlant {
  // … 12 lines
  readonly species?: TreeSpecies;
// … 2 lines
}

// src/render/gardens.ts:60-65 slice of src/render/gardens.ts#gardenSpecies (shared): the feature's statements; "…" marks code left out
function gardenSpecies(roll: number): TreeSpecies {
// … 4 lines
}

// src/render/gardens.ts:71-71 slice of src/render/gardens.ts#KIND_KEYS (shared): the feature's statements; "…" marks code left out
const KIND_KEYS = [...TREE_SPECIES.map((s) => `tree:${s}`), ...BUSH_KINDS.map((k) => `bush:${k}`)];

// src/render/gardens.ts:72-72 slice of src/render/gardens.ts#KIND_OF (shared): the feature's statements; "…" marks code left out
const KIND_OF = new Map(KIND_KEYS.map((key, i) => [key, i]));

// src/render/gardens.ts:75-154 slice of src/render/gardens.ts#gardenPack (shared): the feature's statements; "…" marks code left out
export function gardenPack(list: readonly GardenPlant[]): ReadonlyMap<number, Pack> {
  const trees = new Map<TreeSpecies, Placement[]>(TREE_SPECIES.map((s) => [s, []]));
  const bushes = new Map<string, Placement[]>(BUSH_KINDS.map((k) => [k, []]));
  for (const p of list) {
    // … 1 lines
    switch (p.kind) {
      case 'tree': {
        // … 2 lines
        (trees.get(p.species ?? gardenSpecies(roll)) as Placement[]).push({
          x: p.x, y: p.y, z: p.z - m(0.05), yaw: rng.float() * Math.PI * 2,
          sx: p.h * spread, sy: p.h, sz: p.h * spread * (0.9 + rng.float() * 0.2), tint: foliageTint(rng),
        });
      // … 1 lines
      }
      case 'shrub': {
        // … 4 lines
        (bushes.get(!p.plain && rng.float() < 0.25 ? 'bushFlowering' : 'bush') as Placement[]).push({
          x: p.x, y: p.y, z: p.z - m(0.05), yaw: rng.float() * Math.PI * 2,
          sx: wide, sy: tall, sz: wide * (0.9 + rng.float() * 0.2), tint: foliageTint(rng),
        });
      // … 1 lines
      }
      case 'hedge': {
        // … 5 lines
        for (let i = 0; i < n; i++) {
          // … 1 lines
          (bushes.get('hedge') as Placement[]).push({
            x: p.x + cx * t, y: p.y + cy * t, z: p.z - m(0.05), yaw: p.yaw,
            sx: (step / 1.9) * 1.25, sy: p.h, sz: p.d / 0.9, tint,
          });
        }
      // … 1 lines
      }
      case 'flowers': {
        // … 3 lines
        for (let i = 0; i < Math.min(n, 24); i++) {
          // … 2 lines
          (bushes.get('bushFlowering') as Placement[]).push({
            x: p.x + cx * u - cy * v, y: p.y + cy * u + cx * v, z: p.z + m(0.12), yaw: rng.float() * Math.PI * 2,
            sx: h * 1.1, sy: h, sz: h * 1.1,
          });
        }
      // … 1 lines
      }
    }
  }
  // … 19 lines
  for (const [species, placed] of trees) pack(`tree:${species}`, placed);
  for (const [kind, placed] of bushes) pack(`bush:${kind}`, placed);
// … 1 lines
}

// src/render/gardens.ts:168-186 slice of src/render/gardens.ts#gardenKinds (shared): the feature's statements; "…" marks code left out
function gardenKinds(kit: SceneryKit, pictures: SceneryPictures): KindSpec[] {
  // … 13 lines
  return [
    ...TREE_SPECIES.map((s) => kind(`garden-trees-${s}`, kit.trees[s], kit.foliage, kit.foliageDepth, kit.cards[s], kit.treeCards, kit.treeCardsDepth, pictures.treeAt(s))),
    ...BUSH_KINDS.map((k) => kind(`garden-bushes-${k}`, kit.bushes[k], kit.shrubs, kit.shrubsDepth, kit.cards[k], kit.shrubCards, kit.shrubCardsDepth, pictures.bushAt(k))),
  ];
}

// src/render/gardens.ts:189-189 slice of src/render/gardens.ts#gardenPictures (shared): the feature's statements; "…" marks code left out
const gardenPictures = (pictures: SceneryPictures): number[] => [...TREE_SPECIES.map((s) => pictures.treeAt(s)), ...BUSH_KINDS.map((k) => pictures.bushAt(k))];

// src/render/gardens.ts:233-244 slice of src/render/gardens.ts#GardenCells (shared): the feature's statements; "…" marks code left out
export interface GardenCells {
  readonly layer: PropLayer;
// … 9 lines
}

// src/render/gardens.ts:246-274 slice of src/render/gardens.ts#createGardenCells (shared): the feature's statements; "…" marks code left out
export function createGardenCells(kit: SceneryKit): GardenCells {
  // … 2 lines
  const layer = createPropLayer({
    name: 'gardens', version: 1, kinds: gardenKinds(kit, pictures), margin: SHADOW_REACH,
    // What a garden's place reads: its building, and the ground under each plant (`gardenLists.ts` read the same).
    changes: ['buildings', 'elevation', 'ground'], pad: GARDEN_SLACK,
    pictures: pictures.set, pictureOf: gardenPictures(pictures),
    place: (rect) => ({ packs: from ? gardenPacksAbout(from.buildings, from.groundAt, rect) : new Map(), growing: false }),
  });
  return {
    layer,
    get meshes() { return layer.meshes; },
    get triangles() { return layer.triangles; },
  // … 13 lines
  };
}

// src/render/gardens.ts:260-265 slice of src/render/gardens.ts#createGardenCells.source (shared): the feature's statements; "…" marks code left out
    source(buildings, groundAt) {
      // … 3 lines
      if (again) { layer.near.clear(); for (const f of layer.far) f.clear(); }
    },

// src/render/gardens.ts:266-266 slice of src/render/gardens.ts#createGardenCells.follow (shared): the feature's statements; "…" marks code left out
    follow(journal) { layer.follow(journal); },

// src/render/gardens.ts:267-267 slice of src/render/gardens.ts#createGardenCells.setMap (shared): the feature's statements; "…" marks code left out
    setMap(map) { layer.setMap(map); },

// src/render/gardens.ts:268-268 slice of src/render/gardens.ts#createGardenCells.cull (shared): the feature's statements; "…" marks code left out
    cull(frustum, view) { layer.cull(frustum, view); },

// src/render/gardens.ts:269-272 slice of src/render/gardens.ts#createGardenCells.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      layer.dispose();
    // … 1 lines
    },
