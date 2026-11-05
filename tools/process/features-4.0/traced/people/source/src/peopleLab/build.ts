// The code of src/peopleLab/build.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/build.ts:22-23 slice of src/peopleLab/build.ts#hairBase (shared): the feature's statements; "…" marks code left out
const hairBase = (a: PeopleAssets, mo: Morpher): HairBase => ({ positions: mo.base, vertexCount: a.mesh.vertexCount,
  bodyRange: a.bodyRange, joints: a.mesh.joints, weights: a.mesh.weights, boneNames: a.mesh.boneNames, faces: a.mesh.faces });

// src/peopleLab/build.ts:26-26 slice of src/peopleLab/build.ts#labStyle (shared): the feature's statements; "…" marks code left out
const labStyle = (style: CartoonStyle): CartoonStyle => ({ ...style, eyes: 1 });

// src/peopleLab/build.ts:36-47 slice of src/peopleLab/build.ts#BuiltPerson (shared): the feature's statements; "…" marks code left out
export interface BuiltPerson {
  // … 9 lines
  readonly reshape?: (positions: Float32Array) => Reshaped;
}

// src/peopleLab/build.ts:51-51 slice of src/peopleLab/build.ts#LabBuilder.morpher (shared): the feature's statements; "…" marks code left out
  private morpher: Morpher | null = null;

// src/peopleLab/build.ts:52-52 slice of src/peopleLab/build.ts#LabBuilder.assets (shared): the feature's statements; "…" marks code left out
  private assets: PeopleAssets | null = null;

// src/peopleLab/build.ts:54-54 slice of src/peopleLab/build.ts#LabBuilder.grown (shared): the feature's statements; "…" marks code left out
  private readonly grown = new Map<string, ProxyItem>();

// src/peopleLab/build.ts:56-66 slice of src/peopleLab/build.ts#LabBuilder.grownHair (shared): the feature's statements; "…" marks code left out
  private grownHair(style: HairStyle, a: PeopleAssets, mo: Morpher): ProxyItem {
    const key = JSON.stringify(style);
    let item = this.grown.get(key);
    if (!item) {
      // Plain-coloured cards, no strand texture: the solid, sculpted hair of The Sims ("Maxis Match").
      item = { pack: generateHair(style, hairBase(a, mo)), texture: null, transparent: false, textureFile: null };
      if (this.grown.size > 24) this.grown.clear();
      this.grown.set(key, item);
    }
    return item;
  }

// src/peopleLab/build.ts:71-75 slice of src/peopleLab/build.ts#LabBuilder.proportions (shared): the feature's statements; "…" marks code left out
  async proportions(): Promise<FaceProportions> {
    // … 1 lines
    this.faces ??= new FaceProportions(a, mo, (spec) => mo.shape(spec.body, spec.features));
  // … 1 lines
  }

// src/peopleLab/build.ts:78-82 slice of src/peopleLab/build.ts#LabBuilder.setup (shared): the feature's statements; "…" marks code left out
  async setup(): Promise<{ assets: PeopleAssets; morpher: Morpher }> {
    this.assets ??= await loadPeopleAssets();
    this.morpher ??= new Morpher(this.assets.packs);
    return { assets: this.assets, morpher: this.morpher };
  }

// src/peopleLab/build.ts:85-92 slice of src/peopleLab/build.ts#LabBuilder.hairItem (shared): the feature's statements; "…" marks code left out
  async hairItem(person: LabPerson): Promise<ProxyItem | null> {
    // … 1 lines
    const cut = person.spec.look.hairCut;
    if (!cut || cut === 'none') return null;
    // … 1 lines
    if (cut === GROOM) return loadProxyItem('bob02');
    return cut.startsWith(PROC_PREFIX) && person.hair ? this.grownHair(person.hair, a, mo) : loadProxyItem(cut);
  }

// src/peopleLab/build.ts:95-108 slice of src/peopleLab/build.ts#LabBuilder.studioBody (shared): the feature's statements; "…" marks code left out
  async studioBody(person: LabPerson): Promise<StudioBody> {
    // … 3 lines
    for (const [lo, hi] of a.bodyRange) for (let v = lo; v <= hi; v++) skin.push(v);
    const m = a.mesh;
    const head = m.boneNames.indexOf('head'), neck = m.boneNames.indexOf('neck_01');
    const anchors = skin.filter((v) => {
      // … 1 lines
      for (let k = 0; k < 4; k++) { const j = m.joints[v * 4 + k]; if (j === head || j === neck) w += m.weights[v * 4 + k]! / 65535; }
      return w >= 0.5;
    });
    return { positions, faces: m.faces, faceGroup: m.faceGroup, skinGroup: m.faceGroups.indexOf('body'), skin, anchors };
  }

// src/peopleLab/build.ts:112-115 slice of src/peopleLab/build.ts#LabBuilder.item (shared): the feature's statements; "…" marks code left out
  async item(person: LabPerson, name: string): Promise<ProxyItem | null> {
    if (name === person.spec.look.hairCut) return this.hairItem(person);
    try { return await loadProxyItem(name); } catch (error) { console.warn('[people-lab] no such item to sculpt', name, error); return null; }
  }

// src/peopleLab/build.ts:119-130 slice of src/peopleLab/build.ts#LabBuilder.shaped (shared): the feature's statements; "…" marks code left out
  private shaped(person: LabPerson, name: string, item: ProxyItem, body: Float32Array, a: PeopleAssets): ProxyItem {
    const hair = name === person.spec.look.hairCut, outfit = name === person.spec.look.outfit;
    const shape = hair ? person.hairShape : outfit ? person.outfitShape : undefined;
    if (!shape || isPlain(shape)) return item;
    // … 1 lines
    for (const [lo, hi] of a.bodyRange) for (let v = lo; v <= hi; v++) skin.push(v);
    const surface = new GroomBody(body, skin, a.mesh.faces, a.mesh.faceGroup, a.mesh.faceGroups.indexOf('body'));
    // … 2 lines
    return { ...item, pack: hair ? shapedPack(item.pack, body, shape, out(0.02))
      : shapedPack(item.pack, body, { ...shape, length: Math.min(1, shape.length) }, out(0.1), skin, true) };
  }

// src/peopleLab/build.ts:133-138 slice of src/peopleLab/build.ts#LabBuilder.wornPiece (shared): the feature's statements; "…" marks code left out
  async wornPiece(person: LabPerson, name: string): Promise<ProxyItem | null> {
  // … 4 lines
  }

// src/peopleLab/build.ts:141-144 slice of src/peopleLab/build.ts#LabBuilder.sculptBody (shared): the feature's statements; "…" marks code left out
  async sculptBody(person: LabPerson): Promise<SculptBody> {
    // … 1 lines
    return { positions: await this.shape(person), faces: a.mesh.faces, faceGroup: a.mesh.faceGroup, skinGroup: a.mesh.faceGroups.indexOf('body') };
  }

// src/peopleLab/build.ts:147-150 slice of src/peopleLab/build.ts#LabBuilder.shape (shared): the feature's statements; "…" marks code left out
  async shape(person: LabPerson): Promise<Float32Array> {
    // … 1 lines
    return sculpted(cartoonBody(mo.shape(person.spec.body, person.spec.features), a.mesh, labStyle(person.style)), person.body);
  }

// src/peopleLab/build.ts:152-199 slice of src/peopleLab/build.ts#LabBuilder.build (shared): the feature's statements; "…" marks code left out
  async build(person: LabPerson): Promise<BuiltPerson> {
    // … 2 lines
    const proxies = new Map<string, ProxyItem>();
    try {
      for (const [name, item] of await Promise.all(wornItems(spec.look).map(async (n) =>
        [n, n === GROOM ? await loadProxyItem('bob02') : n.startsWith(PROC_PREFIX) && person.hair ? this.grownHair(person.hair, a, mo) : await loadProxyItem(n)] as const))) proxies.set(name, item);
    } catch (error) {
      // … 2 lines
      proxies.clear();
    }
    const body = sculpted(cartoonBody(mo.shape(spec.body, spec.features), a.mesh, labStyle(person.style)), person.body);
    // … 2 lines
    const cut = spec.look.hairCut, outfit = spec.look.outfit;
    for (const name of [cut, outfit]) {
      const item = name ? proxies.get(name) : undefined;
      if (name && item) proxies.set(name, this.shaped(person, name, item, body, a));
    }
    // … 1 lines
    for (const piece of [...(cut && sculpt?.base === cut ? [sculpt] : []), ...(person.sculpts ?? [])]) {
      const item = proxies.get(piece.base);
      if (item) proxies.set(piece.base, { ...item, pack: sculptedPack(item.pack, decodeSculpt(piece, item.pack.refs.length / 3)) });
    }
    const sex = spec.body.gender < 0.5 ? 'female' : 'male';
    const input = {
      proxies,
      // … 1 lines
      data: a.mesh, skeleton: a.skeleton, bodyRange: a.bodyRange,
      positions: body, look: spec.look,
      capture: captureBind(sex), captureAxes: captureBindRotations(sex),
    };
    await personSimplifier;
    const rig = createPersonRig(input);
    await attachFacialMorphs(input, rig, channelShapes(await expressionShapes()));
    // … 2 lines
    rig.scene.traverse((o) => {
      if (!(o instanceof SkinnedMesh)) return;
      meshes.push(o);
      waits.push(o.geometry.userData['lodReady'] as Promise<unknown>);
    });
    // … 3 lines
    return { scene: rig.scene, meshes, triangles, shape: body, morphs: Object.keys(rig.mesh.morphTargetDictionary ?? {}).length, ...(rig.morph ? { morph: rig.morph } : {}), ...(rig.reshape ? { reshape: rig.reshape } : {}) };
  }
