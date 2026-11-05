// The code of src/peopleLab/main.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/main.ts:47-777 slice of src/peopleLab/main.ts#startLab (shared): the feature's statements; "…" marks code left out
function startLab(): void {
  // … 2 lines
  const first = randomPerson(1, 11, { body: { age: ageFromYears(26), gender: 0.06 } });
  // … 1 lines
  let person: LabPerson = { spec: { ...first, name: '', body: { ...first.body, weight: 0.4, muscle: 0.4, proportions: 1 }, features: variedFace('female', () => 0.5), look: { ...first.look, makeup: 0, hairCut: 'long01', hairStyle: 'long', outfit: 'female_elegantsuit01', outfitTint: 0x3b6fd1, footwear: 'elvs_flatshoe_pointy1', brows: 'eyebrow010', extras: (first.look.extras ?? []).filter((x) => communityItem(x)?.kind !== 'glasses') } }, style: ROADCRAFT };
  // … 11 lines
  let clipName: LibraryClipName = 'idle';
  // … 207 lines
  const layers = createPersonLayers();
  let appearance: SkinAppearance | null = null;
  // … 80 lines
  let clips: Awaited<ReturnType<typeof loadRocketboxClips>> | null = null;
  // … 73 lines
  const PANELS = makePanels({
    get person() { return person; }, get showAll() { return showAll; }, get activity() { return activity; }, get pose() { return pose; }, get clip() { return clipName; },
    // … 1 lines
    setSpec, setLook: (patch) => setLook(patch as Loose<PersonLook>), setBody, setFeature,
    // … 13 lines
    setOutfitStyle: (s: OutfitStyle, tint?: number | null) => {
      person = { ...person, outfitStyle: s, ...(tint !== undefined ? { spec: { ...person.spec, look: { ...person.spec.look, outfitTint: tint } } } : {}) };
    // … 1 lines
    },
    // … 3 lines
    setSex: (v) => {
      // … 1 lines
      const look = person.spec.look;
      // … 1 lines
      const outfitSex = look.outfit ? (communityItem(look.outfit)?.sex ?? (look.outfit.startsWith('female') ? 'female' : 'male')) : 'any';
      setSpec({ body: { ...person.spec.body, gender: v === 'female' ? 0.06 : 0.92 }, features: variedFace(v, () => 0.5),
        look: { ...look, hairCut: fits(look.hairCut) ? look.hairCut! : NICE_HAIR[v][0], ...(outfitSex === 'any' || outfitSex === v ? {} : { outfit: v === 'female' ? 'female_elegantsuit01' : 'male_casualsuit02', footwear: v === 'female' ? 'elvs_flatshoe_pointy1' : 'shoes02' }) } });
    // … 1 lines
    },
  });
  // … 64 lines
  save.onclick = async () => {
    if (!person.spec.name.trim()) { nameInput.focus(); say('Dê um nome para a pessoa antes de salvar'); return; }
    // … 1 lines
    roster = [...roster.filter((p) => p.spec.id !== saved.spec.id), saved];
    thumbs[saved.spec.id] = snapshot();
    // … 2 lines
    say(`${saved.spec.name} salva (${savedBytes(saved)} bytes)`);
  };
  // … 1 lines
  dice.onclick = () => { const keepName = person.spec.name; person = freshPerson(Math.floor(Math.random() * 1e6)); person = { ...person, spec: { ...person.spec, name: keepName } }; render(); void fitFace(); };
  exportBtn.onclick = () => {
    const blob = new Blob([JSON.stringify(rosterOf(roster), null, 1)], { type: 'application/json' });
    const a = el('a', { href: URL.createObjectURL(blob), download: 'roadcraft-people.json' });
  // … 2 lines
  };
  importBtn.onclick = () => {
    // … 1 lines
    input.onchange = async () => {
      // … 2 lines
      try {
        // … 1 lines
        roster = [...roster.filter((p) => !people.some((q) => q.spec.id === p.spec.id)), ...people];
      // … 1 lines
      } catch (error) { console.warn('[people-lab] the file is no roster', error); say('Arquivo inválido'); }
    };
  // … 1 lines
  };
  // … 202 lines
  (window as unknown as { peopleLab: unknown }).peopleLab = { get person() { return person; }, buildNow() { busy.classList.add('on'); buildAt = performance.now(); }, get builds() { return generation; }, setStyle, frame, get built() { return built; },
    pick(tabId: string) { tab = tabId; render(); }, posing, async bakePictures() {
      // … 3 lines
      for (const sex of ['female', 'male'] as const) {
        person = { ...person, spec: { ...person.spec, body: { ...person.spec.body, gender: sex === 'female' ? 0.06 : 0.92 } } };
      // … 1 lines
      }
    // … 3 lines
    }, get camera() { return camera; }, sideView() { const d = camera.position.distanceTo(controls.target); camera.position.copy(controls.target).add(new Vector3(d, 0, 0)); }, pictures, withLook(patch: Partial<PersonLook>) { return { ...person, spec: { ...person.spec, look: { ...person.spec.look, ...patch } } }; }, spec(patch: Partial<PersonSpec>) { setSpec(patch); }, fitFace, async measure() { return (await builder.proportions()).measure(person.spec); }, async body() { return measureBody((await builder.setup()).assets, await builder.shape(person)); }, async sweep(slider: string) { const f = await builder.proportions(); return [-1, 0, 1].map((v) => f.measure({ ...person.spec, features: { ...person.spec.features, [slider]: v } })); }, look(patch: Record<string, unknown>) { setLook(patch as Partial<PersonLook>); },
    // … 5 lines
    }, features(patch: Record<string, number>) { setSpec({ features: { ...person.spec.features, ...patch } }); }, openSculptor, closeSculptor, get sculptor() { return sculptor; }, act(a: string | undefined, p: PoseName) { activity = a; pose = p; render(); } };
}

// src/peopleLab/main.ts:83-89 slice of src/peopleLab/main.ts#startLab.freshPerson (shared): the feature's statements; "…" marks code left out
  function freshPerson(seed: number): LabPerson {
    // … 1 lines
    const spec = randomPerson(id, seed + id);
    const hairs = NICE_HAIR[spec.body.gender < 0.5 ? 'female' : 'male'];
    const hairCut = hairs[Math.floor(Math.random() * hairs.length)]!;
    return { spec: { ...spec, name: '', features: variedFace(spec.body.gender < 0.5 ? 'female' : 'male', Math.random), look: { ...spec.look, makeup: 0, hairCut, ...(spec.body.gender < 0.5 ? { brows: 'eyebrow010' } : {}) } }, style: ROADCRAFT };
  }

// src/peopleLab/main.ts:91-91 slice of src/peopleLab/main.ts#startLab.female (shared): the feature's statements; "…" marks code left out
  const female = (): boolean => person.spec.body.gender < 0.5;

// src/peopleLab/main.ts:94-98 slice of src/peopleLab/main.ts#startLab.setSpec (shared): the feature's statements; "…" marks code left out
  function setSpec(patch: Partial<PersonSpec>, slider?: Slider): void {
    person = { ...person, spec: { ...person.spec, ...patch } };
  // … 2 lines
  }

// src/peopleLab/main.ts:130-130 slice of src/peopleLab/main.ts#startLab.setBody (shared): the feature's statements; "…" marks code left out
  function setBody(k: keyof MacroParams, v: number): void { setSpec({ body: { ...person.spec.body, [k]: v } }, bodySlider(k)); }

// src/peopleLab/main.ts:134-137 slice of src/peopleLab/main.ts#startLab.setLook (shared): the feature's statements; "…" marks code left out
  function setLook(patch: Loose<PersonLook>): void {
    const merged: Loose<PersonLook> = { ...person.spec.look, ...patch };
    setSpec({ look: defined<PersonLook>(merged) });
  }

// src/peopleLab/main.ts:138-138 slice of src/peopleLab/main.ts#startLab.setFeature (shared): the feature's statements; "…" marks code left out
  function setFeature(k: string, v: number): void { setSpec({ features: { ...person.spec.features, [k]: v } }, featureSlider(k)); }

// src/peopleLab/main.ts:139-139 slice of src/peopleLab/main.ts#startLab.setStyle (shared): the feature's statements; "…" marks code left out
  function setStyle(s: CartoonStyle): void { person = { ...person, style: s }; rebuild(); }

// src/peopleLab/main.ts:298-351 slice of src/peopleLab/main.ts#startLab.runBuild (shared): the feature's statements; "…" marks code left out
  async function runBuild(): Promise<void> {
    // … 5 lines
    try {
      // … 10 lines
      const skin = await dressPerson(next, effective(target).spec, layers, paintedSkins, !!person.sculpts?.some((x) => x.base === person.spec.look.outfit));
      // … 4 lines
      if (mine !== generation) { releaseSkinAppearance(skin); dispose(next.scene); return; }
      if (appearance) releaseSkinAppearance(appearance);
      appearance = skin;
      // … 7 lines
      void loadRocketboxClips(sexNow).then((loaded) => {
        if (built !== next || !next.meshes[0]) return;
        clips = loaded;
        startClip();
      }, (error: unknown) => { console.warn('[people-lab] the motion library could not be loaded; the lab poses instead', error); });
    // … 10 lines
    } catch (error) {
    // … 4 lines
    }
  }

// src/peopleLab/main.ts:381-381 slice of src/peopleLab/main.ts#startLab.kindOf (shared): the feature's statements; "…" marks code left out
  const kindOf = (name: string): WardrobeItem['kind'] | undefined => communityItem(name)?.kind;

// src/peopleLab/main.ts:384-387 slice of src/peopleLab/main.ts#startLab.setExtra (shared): the feature's statements; "…" marks code left out
  function setExtra(kinds: readonly WardrobeItem['kind'][], name: string): void {
    const kept = (person.spec.look.extras ?? []).filter((x) => !kinds.includes(kindOf(x)!));
    setLook({ extras: name === 'none' ? kept : [...kept, name] });
  }

// src/peopleLab/main.ts:388-388 slice of src/peopleLab/main.ts#startLab.extraOf (shared): the feature's statements; "…" marks code left out
  const extraOf = (kinds: readonly WardrobeItem['kind'][]): string => (person.spec.look.extras ?? []).find((x) => kinds.includes(kindOf(x)!)) ?? 'none';

// src/peopleLab/main.ts:403-426 slice of src/peopleLab/main.ts#startLab.render (shared): the feature's statements; "…" marks code left out
  function render(): void {
    // … 1 lines
    nameInput.value = person.spec.name;
  // … 20 lines
  }

// src/peopleLab/main.ts:455-455 slice of src/peopleLab/main.ts#startLab.fits (shared): the feature's statements; "…" marks code left out
      const fits = (n: string | undefined): boolean => !n || n === 'none' || (HAIR_CATALOGUE[v] as readonly string[]).includes(n) || communityItem(n)?.sex === 'any';

// src/peopleLab/main.ts:508-524 slice of src/peopleLab/main.ts#startLab.renderGallery (shared): the feature's statements; "…" marks code left out
  function renderGallery(): void {
    gallery.replaceChildren(...roster.map((p) => {
      const card = el('div', { cls: `card${p.spec.id === person.spec.id ? ' on' : ''}` },
        el('img', { src: thumbs[p.spec.id] ?? '', alt: '' }), p.spec.name || 'Sem nome');
      // … 1 lines
      x.onclick = (e) => {
        // … 1 lines
        if (!confirm(`Apagar ${p.spec.name || 'esta pessoa'}?`)) return;
        roster = roster.filter((q) => q.spec.id !== p.spec.id);
        delete thumbs[p.spec.id];
      // … 1 lines
      };
      card.append(x);
      card.onclick = () => { person = structuredClone(p); posing.load(person.motion); activity = undefined; pose = 'idle'; render(); rebuild(0); renderGallery(); };
      return card;
    }));
  }

// src/peopleLab/main.ts:595-604 slice of src/peopleLab/main.ts#startLab.openCarver (shared): the feature's statements; "…" marks code left out
  async function openCarver(): Promise<void> {
    // … 4 lines
    carver = new BodySculptor(body.positions, start, body.faces, body.faceGroup, body.skinGroup, body.skin, person.spec.look.skin);
  // … 3 lines
  }

// src/peopleLab/main.ts:623-640 slice of src/peopleLab/main.ts#startLab.openSculptor (shared): the feature's statements; "…" marks code left out
  async function openSculptor(name = person.spec.look.hairCut ?? ''): Promise<void> {
    const cut = name;
    // … 1 lines
    const item = cut && cut !== 'none' ? await builder.wornPiece(person, cut) : null;
    if (!item) { say('Escolha uma peça primeiro'); return; }
    // … 1 lines
    const n = item.pack.refs.length / 3;
    const hair = cut === person.spec.look.hairCut;
    const prior = hair ? (person.sculpt?.base === cut ? person.sculpt : undefined) : person.sculpts?.find((x) => x.base === cut);
    const start = prior ? decodeSculpt(prior, n) : null;
    sculpting = cut;
    // … 1 lines
    sculptor = new HairSculptor(item.pack, body, start, hair ? person.spec.look.hair : (person.spec.look.outfitTint ?? 0x8a96a3), person.spec.look.skin, hair ? null : item.texture);
    // … 2 lines
    frame(hair ? 'face' : 'body');
  // … 1 lines
  }

// src/peopleLab/main.ts:642-657 slice of src/peopleLab/main.ts#startLab.closeSculptor (shared): the feature's statements; "…" marks code left out
  function closeSculptor(keep: boolean): void {
    // … 2 lines
    if (keep) {
      // … 1 lines
      person = sculpting === person.spec.look.hairCut ? { ...person, sculpt: piece }
        : { ...person, sculpts: [...(person.sculpts ?? []).filter((x) => x.base !== sculpting), piece] };
    }
  // … 7 lines
  }

// src/peopleLab/main.ts:666-666 slice of src/peopleLab/main.ts#startLab.sliderOf (shared): the feature's statements; "…" marks code left out
  const sliderOf = (name: string): Slider => (name.startsWith('body:') ? bodySlider(name.slice(5) as keyof MacroParams) : featureSlider(name));

// src/peopleLab/main.ts:712-748 slice of src/peopleLab/main.ts#startLab.loop (shared): the feature's statements; "…" marks code left out
  function loop(): void {
    // … 6 lines
    setCloth(cloth, person.spec.look.outfitTint, person.outfitStyle);
    // … 3 lines
    if (built) {
      if (player?.playing) player.apply(t); else poser?.apply(pose, t);
      // … 10 lines
      const face = posing.faceNow() ?? faceAt(person.spec.id, t, activity, person.spec.mood ?? 0);
      for (const m of built.meshes as SkinnedMesh[]) {
        if (m.morphTargetInfluences && m.morphTargetDictionary) {
          applyLabFace(m.morphTargetInfluences, m.morphTargetDictionary, face);
        // … 2 lines
        }
      }
    }
  // … 4 lines
  }
