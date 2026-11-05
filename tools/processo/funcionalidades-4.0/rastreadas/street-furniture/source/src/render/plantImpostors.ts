// The code of src/render/plantImpostors.ts that is the feature's (the rest of the file is shared).

// src/render/plantImpostors.ts:470-476 slice of src/render/plantImpostors.ts#SceneryPictures (shared): the feature's statements; "…" marks code left out
export interface SceneryPictures {
  // … 1 lines
  treeAt(species: TreeSpecies): number;
  bushAt(kind: BushKind): number;
// … 2 lines
}

// src/render/plantImpostors.ts:479-483 slice of src/render/plantImpostors.ts#PlantModels (shared): the feature's statements; "…" marks code left out
interface PlantModels {
  readonly trees: Readonly<Record<TreeSpecies, BufferGeometry>>;
  readonly bushes: Readonly<Record<BushKind, BufferGeometry>>;
  readonly cards: Readonly<Partial<Record<TreeSpecies | BushKind, BufferGeometry>>>;
}

// src/render/plantImpostors.ts:487-511 slice of src/render/plantImpostors.ts#sceneryPictures (shared): the feature's statements; "…" marks code left out
export function sceneryPictures(kit: PlantModels): SceneryPictures {
  // … 1 lines
  if (!shared) {
    const models: ImpostorModel[] = [
      ...TREE_SPECIES.map((s) => ({ body: kit.trees[s], cards: kit.cards[s] ?? null, cardsCast: false })),
      ...BUSH_KINDS.map((k) => ({ body: kit.bushes[k], cards: kit.cards[k] ?? null, cardsCast: false })),
    ];
    sceneryShared.set(kit, shared = { set: createPlantImpostors(models, PICTURE_WIND, 'scenery'), users: 0 });
  }
// … 15 lines
}

// src/render/plantImpostors.ts:501-501 slice of src/render/plantImpostors.ts#sceneryPictures.treeAt (shared): the feature's statements; "…" marks code left out
    treeAt: (species) => TREE_SPECIES.indexOf(species),

// src/render/plantImpostors.ts:502-502 slice of src/render/plantImpostors.ts#sceneryPictures.bushAt (shared): the feature's statements; "…" marks code left out
    bushAt: (kind) => TREE_SPECIES.length + BUSH_KINDS.indexOf(kind),
