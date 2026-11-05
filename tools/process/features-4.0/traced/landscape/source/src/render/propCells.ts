// The code of src/render/propCells.ts that is the feature's (the rest of the file is shared).

// src/render/propCells.ts:130-134 slice of src/render/propCells.ts#nearSquareOf (shared): the feature's statements; "…" marks code left out
export function nearSquareOf(v: PropView, largest: number): PropSquare {
  const reach = reachFor(v, largest, PICTURE_PX * (1 - HYSTERESIS));
  // … 1 lines
  return { cx, cy, k: reach < 0 ? -1 : Math.min(MOST_NEAR, Math.ceil(reach / PROP_CELL)) };
}

// src/render/propCells.ts:143-163 slice of src/render/propCells.ts#PropLayerSpec (shared): the feature's statements; "…" marks code left out
export interface PropLayerSpec {
  // … 11 lines
  readonly pictures: PlantImpostors | null;
// … 7 lines
}

// src/render/propCells.ts:264-275 slice of src/render/propCells.ts#createPropLayer.note (shared): the feature's statements; "…" marks code left out
  const note = (packs: ReadonlyMap<number, Pack>): void => {
    for (const [kind, pack] of packs) {
      // … 1 lines
      const reach = picture !== null && picture !== undefined && spec.pictures ? spec.pictures.reach[picture]! : spec.kinds[kind]!.levels?.reach;
      if (reach === undefined) continue;
      // … 2 lines
      const size = 2 * reach * most;
      if (picture !== null && picture !== undefined) largestPicture = Math.max(largestPicture, size);
      else if (classOf[kind] === FarClass.small) largestSmall = Math.max(largestSmall, size);
    }
  };

// src/render/propCells.ts:310-436 slice of src/render/propCells.ts#createPropLayer.batches (shared): the feature's statements; "…" marks code left out
  const batches = (cls: FarClass, key: CellKey, rect: ChangeRect, onUploaded: () => void): { block: FarBlock; arrays: number } => {
    // … 47 lines
    if (pictures && plantKinds.length) {
      const fill = (from: ReadonlyMap<number, Pack>): Record<string, Float32Array> => {
        // … 4 lines
        for (const kind of plantKinds) {
          // … 2 lines
          const picture = spec.pictureOf[kind]!, s = pictures.spheres[picture]!;
          const bx2 = picture % pictures.blocks.x, by2 = Math.floor(picture / pictures.blocks.x);
          // … 2 lines
          for (let i = 0; i < pack.count; i++) {
            spheres.set([s.centre.x, s.centre.y, s.centre.z, s.radius], (at + i) * 4);
            blocks[(at + i) * 2] = bx2; blocks[(at + i) * 2 + 1] = by2;
          }
        // … 1 lines
        }
      // … 1 lines
      };
      // … 7 lines
      const quad = pictures.quads[0]!;
      model.setIndex(quad.index);
      model.setAttribute('position', quad.getAttribute('position'));
      model.setAttribute('normal', quad.getAttribute('normal'));
      // … 2 lines
      const { sphere, largest } = boundsOf(plantKinds.map((kind) => {
        const s = pictures.spheres[spec.pictureOf[kind]!]!;
        return { pack: packs.get(kind)!, radius: s.radius, centre: s.centre.length() };
      }));
      const meshSpec: KindMesh = {
        name: `${spec.name}-far-cells`, geometry: quad, material: pictures.material, depth: pictures.depth,
        castShadow: true, receiveShadow: true, cards: false, far: true, made: pictures.attach,
      };
    // … 4 lines
    }
    // … 29 lines
    for (const mesh of meshes) (mesh.material === spec.pictures?.material ? pictured : holder).add(mesh);
  // … 1 lines
  };

// src/render/propCells.ts:548-589 slice of src/render/propCells.ts#createPropLayer.show (shared): the feature's statements; "…" marks code left out
    show(square, view, deadline) {
      // … 24 lines
      far.forEach((f, cls) => {
        // … 12 lines
        const pictured = !spec.pictures || spec.pictures.ready();
        for (const [key, block] of heldFar[cls]!) { block.group.visible = wanted.has(key); block.pictured.visible = pictured; }
      });
    },

// src/render/propCells.ts:591-595 slice of src/render/propCells.ts#createPropLayer.cull (shared): the feature's statements; "…" marks code left out
    cull(frustum, view) {
      // … 1 lines
      spec.pictures?.setView(view, screenHalf());
    // … 1 lines
    },
