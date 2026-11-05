// The code of src/render/streetFurnitureLayer.ts that is the feature's (the rest of the file is shared).

// src/render/streetFurnitureLayer.ts:29-29 slice of src/render/streetFurnitureLayer.ts#ROAD_REACH (shared): the feature's statements; "…" marks code left out
const ROAD_REACH = ACCESS_REACH + MAX_ITEM_RADIUS + m(1);

// src/render/streetFurnitureLayer.ts:62-67 slice of src/render/streetFurnitureLayer.ts#FurnitureSource (shared): the feature's statements; "…" marks code left out
export interface FurnitureSource {
  // … 2 lines
  readonly elevation: RoadElevation;
// … 1 lines
}

// src/render/streetFurnitureLayer.ts:94-144 slice of src/render/streetFurnitureLayer.ts#furnitureStores (shared): the feature's statements; "…" marks code left out
export function furnitureStores(kit: SceneryKit, pictures: SceneryPictures): RunStore[] {
  // … 13 lines
  const f = kit.furniture;
  const stores: RunStore[] = [
    // A column a dozen pixels tall drawn with its coarse model (`lampFarGeometry`).
    prop('street-lights', f.lamp, kit.props, true, true, undefined, true, kit.lampFar),
    // The lens is lit from inside; it neither casts nor takes a shadow.
    prop('street-light-lamps', kit.lampLens, kit.posedGlow, false),
    prop('street-light-pools', kit.pool, kit.posedPoolGlow, false, false, 3),
    prop('street-light-beams', kit.beam, kit.posedBeamGlow, false, false, 4),
    prop('street-bins', f.bin, kit.props, true, true, undefined, true),
    prop('benches', f.bench, kit.props, true, true, undefined, true),
    prop('hydrants', f.hydrant, kit.props, true, true, undefined, true),
    prop('post-boxes', f.postbox, kit.props, true, true, undefined, true),
    prop('phones', f.phone, kit.props, true, true, undefined, true),
    prop('drains', f.drain, kit.props, true, true, undefined, true),
    prop('tree-pits', kit.treePit, kit.props, false, true, undefined, true),
  ];
  const far = pictures.set;
  // … 12 lines
  for (const s of TREE_SPECIES) {
    plant(`street-trees-${s}`, kit.trees[s], kit.foliage, kit.foliageDepth, kit.cards[s], kit.treeCards, kit.treeCardsDepth, pictures.treeAt(s));
  }
  for (const k of BUSH_KINDS) {
    plant(`street-bushes-${k}`, kit.bushes[k], kit.shrubs, kit.shrubsDepth, kit.cards[k], kit.shrubCards, kit.shrubCardsDepth, pictures.bushAt(k));
  }
  return stores;
}

// src/render/streetFurnitureLayer.ts:126-136 slice of src/render/streetFurnitureLayer.ts#furnitureStores.plant (shared): the feature's statements; "…" marks code left out
  const plant = (name: string, body: RunStore['geometry'], material: RunStore['material'], depth: MeshDepthMaterial,
    // … 1 lines
    stores.push({
      // … 1 lines
      levels: { reach: far.reach[picture]!, far: PICTURE_PX, view: far.setView, ready: () => far.ready() },
      with: [
        // … 2 lines
        { name: `${name}-far`, geometry: far.quads[picture]!, material: far.material, depth: far.depth, castShadow: true, receiveShadow: true, cards: false, far: true, made: far.attach },
      ],
    }) - 1;

// src/render/streetFurnitureLayer.ts:167-231 slice of src/render/streetFurnitureLayer.ts#furniturePlace (shared): the feature's statements; "…" marks code left out
function furniturePlace(net: Network, elevation: RoadElevation, placed: LandscapeItem, lens: LampLens): FurniturePlace | undefined {
  const item = furnitureOf(net, placed);
  if (!item) return undefined;
  // … 5 lines
  const base = elevation.onSegment(item.segment, item.x, item.y) + footwayRiseAt(net, item.x, item.y);
  // … 1 lines
  const inward = angleOf({ x: -item.outward.x, y: -item.outward.y });
  const facing = angleOf({ x: item.outward.y, y: -item.outward.x });
  const at = { x: item.x, y: item.y, z: base, sx: 1, sy: 1, sz: 1 };
  switch (item.kind) {
    case 'lamp': {
      put(MODEL.lamp, { ...at, yaw: inward });
      put(MODEL.lens, { ...at, yaw: inward });
      // The light falls over the road and the footway behind the lamp: across, the carriageway's width
      const road = net.ribbons.get(item.segment)?.road;
      const across = Math.max(LAMP_POOL_RADIUS, (road ? road.width : m(7)) * 0.55 + m(2.5));
      const centre = Math.max(LAMP_OUTREACH * 0.85, across - m(3));
      put(MODEL.pool, { x: item.x - item.outward.x * centre, y: item.y - item.outward.y * centre, z: base + m(0.012),
        yaw: inward, sx: across, sy: 1, sz: LAMP_HEIGHT * POOL_ALONG });
      // And the cone it is lit by, from the lens down to the road under it.
      put(MODEL.beam, { x: item.x - item.outward.x * lens.reach, y: item.y - item.outward.y * lens.reach, z: base + lens.height,
        yaw: 0, sx: LAMP_BEAM_RADIUS, sy: lens.height + FOOTWAY_RISE, sz: LAMP_BEAM_RADIUS });
      break;
    }
    case 'bin':
      put(MODEL.bin, { ...at, yaw: item.seed * Math.PI * 2 });
      break;
    case 'bench': {
      // Its seat faces where the layout says somebody sitting looks.
      const faces = item.faces ?? { x: -item.outward.x, y: -item.outward.y };
      put(MODEL.bench, { ...at, yaw: angleOf({ x: -faces.y, y: faces.x }) });
      break;
    }
    case 'postbox':
    case 'hydrant':
    case 'phone':
    case 'drain':
      // Built facing -Z; turned so the phone's shell opens to the footway and the drain's grate lies in the gutter on the road
      put(MODEL[item.kind], { ...at, yaw: facing + Math.PI });
      break;
    case 'streetTree': {
      put(MODEL.treePit, { ...at, yaw: angleOf(item.along) });
      // Planted young, it grows to its full height over a few days.
      const rng = new Rng(Math.floor(item.seed * 0xffffff));
      const species = streetSpecies(rng.float());
      const tree = { x: item.x, y: item.y, z: base + m(0.03), yaw: rng.float() * Math.PI * 2,
        sx: 0.85 + rng.float() * 0.2, sy: 1, sz: 0.85 + rng.float() * 0.2, tint: foliageTint(rng), h: STREET_TREE_MIN + item.seed * STREET_TREE_RANGE };
      put(treeStore(species), tree);
      break;
    }
    case 'shrub': {
      const rng = new Rng(Math.floor(item.seed * 0xffffff));
      const h = m(0.8) + rng.float() * m(0.4);
      const kind: BushKind = rng.float() < 0.4 ? 'bushFlowering' : 'bush';
      const shrub = { x: item.x, y: item.y, z: base - m(0.05), yaw: rng.float() * Math.PI * 2, sx: 1.05, sy: 1, sz: 1.05, tint: foliageTint(rng), h };
      put(bushStore(kind), shrub);
      break;
    }
  }
  return { x: placed.x, y: placed.y, planted: item.planted, placed: out };
}

// src/render/streetFurnitureLayer.ts:252-260 slice of src/render/streetFurnitureLayer.ts#PlacedProduct.constructor (shared): the feature's statements; "…" marks code left out
  constructor(
    // … 2 lines
    private readonly wanted: (item: LandscapeItem) => boolean,
    private readonly make: (item: LandscapeItem, source: FurnitureSource) => V | undefined,
  // … 3 lines
  }

// src/render/streetFurnitureLayer.ts:263-268 slice of src/render/streetFurnitureLayer.ts#PlacedProduct.allKeys (shared): the feature's statements; "…" marks code left out
  allKeys(source: FurnitureSource): Iterable<number> {
    // … 1 lines
    for (const item of source.net.doc.landscape.values()) if (this.wanted(item)) out.push(item.id);
  // … 2 lines
  }

// src/render/streetFurnitureLayer.ts:270-288 slice of src/render/streetFurnitureLayer.ts#PlacedProduct.dirty (shared): the feature's statements; "…" marks code left out
  dirty(change: Change, out: Set<number>, source: FurnitureSource): void {
    // … 5 lines
    if (change.kind === 'landscape' && change.ids) {
      for (const id of change.ids) {
        const item = source.net.doc.landscape.get(id);
        if (!item || this.wanted(item)) out.add(id);
      }
    // … 1 lines
    }
    // … 3 lines
    for (const item of source.net.doc.landscapeInAny(areas)) if (this.wanted(item)) out.add(item.id);
  // … 1 lines
  }

// src/render/streetFurnitureLayer.ts:290-293 slice of src/render/streetFurnitureLayer.ts#PlacedProduct.compute (shared): the feature's statements; "…" marks code left out
  compute(key: number, source: FurnitureSource): V | undefined {
    const item = source.net.doc.landscape.get(key);
    return item && this.wanted(item) ? this.make(item, source) : undefined;
  }

// src/render/streetFurnitureLayer.ts:316-319 slice of src/render/streetFurnitureLayer.ts#MEADOW_STORES (shared): the feature's statements; "…" marks code left out
const MEADOW_STORES = (kit: SceneryKit): RunStore[] => [
  { name: 'grass-tufts', geometry: kit.tuft, material: kit.grass, castShadow: false, receiveShadow: true, tinted: true, cards: false },
  { name: 'wildflowers', geometry: kit.flower, material: kit.flowers, castShadow: false, receiveShadow: true, tinted: true, cards: false },
];

// src/render/streetFurnitureLayer.ts:322-331 slice of src/render/streetFurnitureLayer.ts#meadowProduct (shared): the feature's statements; "…" marks code left out
export function meadowProduct(kit: SceneryKit): Derived<FurnitureSource, number, MeadowPlace> {
  const stores = MEADOW_STORES(kit);
  return new Derived(new PlacedProduct<MeadowPlace>('long grass', {
    landscape: 0, elevation: CLUMP_READ_REACH + m(1), ground: CLUMP_READ_REACH + m(1),
  }, (item) => item.kind === 'meadow', (item, source) => {
    const builder = new RunBuilder(stores);
    meadowInto(builder, source.elevation, source.groundAt, item);
    return { x: item.x, y: item.y, run: builder.build() };
  }, (a, b) => a.x === b.x && a.y === b.y && sameRun(a.run, b.run)));
}

// src/render/streetFurnitureLayer.ts:334-342 slice of src/render/streetFurnitureLayer.ts#meadowInto (shared): the feature's statements; "…" marks code left out
function meadowInto(builder: RunBuilder, elevation: RoadElevation, groundAt: (x: number, y: number) => number, item: LandscapeItem): void {
  const clump = grassClump(elevation, groundAt, item);
  // … 4 lines
  for (const tuft of clump.tufts) add(0, tuft);
  for (const tuft of clump.flowers) add(1, tuft);
}

// src/render/streetFurnitureLayer.ts:336-339 slice of src/render/streetFurnitureLayer.ts#meadowInto.add (shared): the feature's statements; "…" marks code left out
  const add = (store: number, tuft: Tuft): void => {
    builder.turned(store, tuft.x, tuft.z, -tuft.y, tuft.yaw, tuft.height * tuft.width, tuft.height, tuft.height * tuft.width,
      tuft.height * Math.max(1, tuft.width), tuft.tint);
  };

// src/render/streetFurnitureLayer.ts:345-355 slice of src/render/streetFurnitureLayer.ts#placeInto (shared): the feature's statements; "…" marks code left out
function placeInto(builder: RunBuilder, place: FurniturePlace, now: number): void {
  const growth = plantGrowth(place.planted, now);
  for (const p of place.placed) {
    // … 1 lines
    if (p.h > 0) {
      const height = p.h * growth;
      sx = height * p.sx; sy = height * p.sy; sz = height * p.sz;
    }
    builder.turned(p.store, p.x, p.z, -p.y, p.yaw, sx, sy, sz, Math.max(sx, sy, sz), p.tint ?? undefined);
  }
}

// src/render/streetFurnitureLayer.ts:358-374 slice of src/render/streetFurnitureLayer.ts#StreetFurnitureLayer (shared): the feature's statements; "…" marks code left out
export interface StreetFurnitureLayer {
  // … 9 lines
  sync(net: Network, elevation: RoadElevation, groundAt: (x: number, y: number) => number, minutes: number): number;
// … 5 lines
}

// src/render/streetFurnitureLayer.ts:377-378 slice of src/render/streetFurnitureLayer.ts#stillGrowing (shared): the feature's statements; "…" marks code left out
const stillGrowing = (planted: number | undefined, minutes: number): boolean =>
  planted !== undefined && !(Number.isFinite(minutes) && minutes - planted >= GROW_MINUTES);

// src/render/streetFurnitureLayer.ts:384-384 slice of src/render/streetFurnitureLayer.ts#isFurniture (shared): the feature's statements; "…" marks code left out
const isFurniture = (item: LandscapeItem): boolean => item.kind !== 'meadow' && item.kind !== 'sign' && item.kind !== 'streetname';

// src/render/streetFurnitureLayer.ts:400-470 slice of src/render/streetFurnitureLayer.ts#createStreetFurniture (shared): the feature's statements; "…" marks code left out
export function createStreetFurniture(kit: SceneryKit): StreetFurnitureLayer {
  const pictures = sceneryPictures(kit);
  const stores = furnitureStores(kit, pictures);
  // … 1 lines
  const pictureOf = stores.map((_, i): number | null => {
    const plant = i - FURNITURE_MODELS.length;
    if (plant < 0) return null;
    return plant < TREE_SPECIES.length ? pictures.treeAt(TREE_SPECIES[plant]!) : pictures.bushAt(BUSH_KINDS[plant - TREE_SPECIES.length]!);
  });
  // … 2 lines
  const layer = createPropLayer({
    name: 'street-furniture', version: 1, kinds: stores.map(kindOf), margin: SHADOW_REACH,
    // … 1 lines
    changes: ['landscape', 'roads', 'elevation'], pad: ROAD_REACH + FURNITURE_SPREAD,
    pictures: pictures.set, pictureOf,
    place: (rect) => (source ? furnitureAbout(source, stores, kit.lens, hour * 60, rect) : { packs: new Map(), growing: false }),
  });
  // … 1 lines
  const meadowStores = MEADOW_STORES(kit);
  const meadow = createInstanceRuns('grass', meadowStores, 0);
  const clumps = meadowProduct(kit);
  // … 2 lines
  return {
    // … 3 lines
    grass: meadow.group,
    get triangles() { return layer.triangles + meadow.triangles; },
  // … 39 lines
  };
}

// src/render/streetFurnitureLayer.ts:464-468 slice of src/render/streetFurnitureLayer.ts#createStreetFurniture.dispose (shared): the feature's statements; "…" marks code left out
    dispose() {
      // … 2 lines
      pictures.release();
    },
