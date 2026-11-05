// The code of src/peopleLab/panels.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/panels.ts:19-56 slice of src/peopleLab/panels.ts#PanelContext (shared): the feature's statements; "…" marks code left out
export interface PanelContext {
  // … 4 lines
  readonly clip: LibraryClipName;
  // … 2 lines
  setSpec(patch: Partial<PersonSpec>): void;
  setLook(patch: { [K in keyof PersonLook]?: PersonLook[K] | undefined }): void;
  setBody(k: keyof MacroParams, v: number): void;
  // … 1 lines
  setStyle(s: CartoonStyle): void;
  // … 2 lines
  act(activity: string | undefined, pose: PoseName, clip: LibraryClipName): void;
  // … 15 lines
  extraOf(kinds: readonly WardrobeItem['kind'][]): string;
  setExtra(kinds: readonly WardrobeItem['kind'][], name: string): void;
// … 4 lines
}

// src/peopleLab/panels.ts:79-321 slice of src/peopleLab/panels.ts#makePanels (shared): the feature's statements; "…" marks code left out
export function makePanels(c: PanelContext): Record<string, Groups> {
  // … 70 lines
  return {
    pessoa: [
      ['tipo', 'Tipo', () => [
        seg([['female', 'Mulher'], ['male', 'Homem']] as const, sex(), (v) => c.setSex(v)),
        pics(BODIES.map((pr) => ({ id: pr.label, label: pr.label, variant: { ...m(), spec: { ...m().spec, ...withBody(m().spec.body, m().spec.features, pr) } } })), null,
          (id) => c.setSpec(withBody(b(), p().spec.features, BODIES.find((x) => x.label === id)!)), 'body'),
      ]],
      ['medidas', 'Medidas', () => [
        slider('Idade', 3, 90, 1, Math.round(yearsFromAge(b().age)), (v) => c.setBody('age', ageFromYears(v)), (v) => `${v} a`),
        slider('Altura', 0, 1, 0.01, b().height, (v) => c.setBody('height', v)),
        slider('Peso', 0, 1, 0.01, b().weight, (v) => c.setBody('weight', v)),
        slider('Músculos', 0, 1, 0.01, b().muscle, (v) => c.setBody('muscle', v)),
        slider('Proporções', 0, 1, 0.01, b().proportions, (v) => c.setBody('proportions', v)),
        ...(sex() === 'female' ? [slider('Busto', 0, 1, 0.01, b().cupsize, (v) => c.setBody('cupsize', v)), slider('Firmeza', 0, 1, 0.01, b().firmness, (v) => c.setBody('firmness', v))] : []),
      ]],
      ['esculpir', 'Esculpir', () => [el('div', { cls: 'bar' }, iconButton('sculpt', 'Esculpir corpo e rosto', () => c.openCarver()))]],
      ['tronco', 'Tronco', () => sliderList(BODY_SLIDERS['tronco']!)],
      ['bracos', 'Braços', () => sliderList(BODY_SLIDERS['bracos']!)],
      ['pernas', 'Pernas', () => sliderList(BODY_SLIDERS['pernas']!)],
      ['pele', 'Pele', () => [
        swatches(SKIN, p().spec.look.skin, (col) => c.setLook({ skin: col, melanin: undefined, undertone: undefined })),
        el('div', { cls: 'gap' }),
        slider('Africana', 0, 1, 0.01, b().african, (v) => shares('african', v)),
        slider('Asiática', 0, 1, 0.01, b().asian, (v) => shares('asian', v)),
        slider('Europeia', 0, 1, 0.01, b().caucasian, (v) => shares('caucasian', v)),
      ]],
    ],
    rosto: [
      ['rosto', 'Rosto', () => {
        const f = p().spec.features;
        const fit = iconButton('style', 'Proporção do cânone', () => c.fitFace());
        return [
          pics(FACES.map((face) => ({ id: face.label, label: face.label, variant: withFeatures(withFace(mf(), face)) })), null,
            (id) => c.setSpec({ features: withFace(f, FACES.find((x) => x.label === id)!) }), 'face'),
          fit,
        ];
      }],
      ['cabeca', 'Cabeça', () => {
        const f = p().spec.features;
        return [
          pics(HEAD_SHAPES.map(([k, label]) => {
            const next: Record<string, number> = { ...mf() };
            for (const [h] of HEAD_SHAPES) delete next[h];
            next[k] = 0.35;
            return { id: k, label, variant: withFeatures(next) };
          }), HEAD_SHAPES.find(([k]) => (f[k] ?? 0) > 0)?.[0], (id) => {
            const next: Record<string, number> = { ...f };
            for (const [h] of HEAD_SHAPES) delete next[h];
            next[id] = 0.35;
            c.setSpec({ features: next });
          }, 'face'),
          el('div', { cls: 'gap' }),
          ...FACE[0]![1].map(([k, label]) => featureSlider(k, label)),
        ];
      }],
      ['olhos', 'Olhos', () => [...partGroup('eyes')(), el('div', { cls: 'gap' }), swatches(EYE_COLOURS.concat([0x2e8b57, 0x6a9fd8, 0x8b5a2b]), p().spec.look.eyes, (col) => c.setLook({ eyes: col }))]],
      ['sobrancelhas', 'Sobrancelhas', () => [
        pics([...WARDROBE.brows, ...itemsOf('eyebrows')].map((n) => ({ id: n, label: nameOf(n), variant: withLook({ brows: n }) })), p().spec.look.brows, (id) => c.setLook({ brows: id }), 'brows'),
        ...partGroup('brows')(),
      ]],
      ['nariz', 'Nariz', partGroup('nose')],
      ['boca', 'Boca', partGroup('mouth')],
      ['bochechas', 'Bochechas', partGroup('cheeks')],
      ['queixo', 'Queixo', partGroup('jaw')],
      ['orelhas', 'Orelhas', partGroup('ears')],
      ['esculpir', 'Esculpir', () => [el('div', { cls: 'bar' }, iconButton('sculpt', 'Esculpir rosto', () => c.openCarver()))]],
      ['maquiagem', 'Maquiagem', () => [slider('Intensidade', 0, 1, 0.01, p().spec.look.makeup ?? 0, (v) => c.setLook({ makeup: v }))]],
    ],
    estilo: [
      ['estilo', 'Estilo', () => {
        // … 2 lines
        return [
          pics(STYLE_PRESETS.map(([label, preset]) => ({ id: label, label, variant: { ...m(), style: preset } })),
            STYLE_PRESETS.find(([, pr]) => (Object.keys(pr) as (keyof CartoonStyle)[]).every((k) => Math.abs(pr[k] - s[k]) < (k === 'head' ? 0.08 : 0.01)))?.[0],
            (id) => c.setStyle(STYLE_PRESETS.find(([l]) => l === id)![1]), 'body'),
          el('div', { cls: 'gap' }),
          slider('Cabeça', 0.8, 1.8, 0.01, s.head, (v) => set('head', v)),
          slider('Pernas', 0.7, 1.15, 0.01, s.legs, (v) => set('legs', v)),
          slider('Mãos', 0.8, 1.4, 0.01, s.hands, (v) => set('hands', v)),
          slider('Pés', 0.8, 1.4, 0.01, s.feet, (v) => set('feet', v)),
        ];
      }],
    ],
    cabelo: [
      ['penteado', 'Penteado', () => {
        // The catalogue's whole hairstyles by kind, then shaped: length, volume and waves (`hairShape.ts`) and the sculptor - made hair, no seams.
        const list = c.showAll ? [...HAIR_CATALOGUE[sex()], ...HAIR_CATALOGUE[sex() === 'female' ? 'male' : 'female']] : HAIR_CATALOGUE[sex()];
        const kind = (n: string): 'fringe' | 'tied' | 'loose' => (HAIR_FRINGED.includes(n) ? 'fringe' : HAIR_TIED.includes(n) ? 'tied' : 'loose');
        const cut = p().spec.look.hairCut;
        const hs = c.hairShape();
        const shape = el('button', { cls: 'toggle', title: 'Esculpir este penteado' }, icon('sculpt', 16));
        const dropHair = el('button', { cls: 'toggle', title: 'Desfazer escultura do penteado' }, icon('trash', 16));
        dropHair.onclick = () => c.dropSculpt();
        shape.onclick = () => c.openSculptor();
        const bald = !cut || cut === 'none';
        return [
          el('div', { cls: 'bar' }, c.ui.iconSeg([['all', 'all', 'Todos'], ['loose', 'loose', 'Solto'], ['fringe', 'fringe', 'Com franja'], ['tied', 'tied', 'Preso']] as const, hairKind, (v) => { hairKind = v; }), allToggle(), ...(bald ? [] : [shape]), ...(!bald && p().sculpt?.base === cut ? [dropHair] : [])),
          pics(['none', ...list.filter((n) => hairKind === 'all' || kind(n) === hairKind)].map((n) => ({ id: n, label: n === 'none' ? 'Careca' : HAIR_LABELS[n] ?? nameOf(n), variant: withLook({ hairCut: n }) })),
            cut, (id) => c.setLook({ hairCut: id }), 'head'),
          ...(bald ? [] : [
            slider('Comprimento', 0.5, 1.6, 0.01, hs.length, (v) => c.setHairShape({ ...c.hairShape(), length: v }), (v) => `${Math.round(v * 100)}%`),
            slider('Volume', -0.4, 2, 0.05, hs.volume * 10, (v) => c.setHairShape({ ...c.hairShape(), volume: v / 10 }), (v) => `${v.toFixed(1)} cm`),
            slider('Ondas', 0, 1.2, 0.05, hs.wave * 10, (v) => c.setHairShape({ ...c.hairShape(), wave: v / 10 }), (v) => `${v.toFixed(1)} cm`),
          ]),
        ];
      }],
      ['cor', 'Cor', () => [swatches(HAIR_COLOURS, p().spec.look.hair, (col) => c.setLook({ hair: col }))]],
      ['barba', 'Barba', () => [extraPics('beard', 'Sem barba')]],
      ['chapeu', 'Chapéu', () => [
        pics(['none', ...WARDROBE.hats, ...itemsOf('hat', { street: true })].map((n) => ({ id: n, label: n === 'none' ? 'Sem' : nameOf(n), variant: withLook({ hat: n }) })),
          p().spec.look.hat ?? 'none', (id) => c.setLook({ hat: id }), 'head'),
      ]],
    ],
    roupas: child() ? [
      ['roupa', 'Roupa', () => {
        const l = p().spec.look;
        return [
          c.ui.iconSeg([['tank', 'tube', 'Regata'], ['tshirt', 'tshirt', 'Camiseta'], ['longsleeve', 'longsleeve', 'Manga longa']] as const, l.top === 'none' ? 'tshirt' : l.top, (v) => c.setLook({ top: v })),
          swatches(CLOTH, l.topColour, (col) => c.setLook({ topColour: col })),
          c.ui.iconSeg([['shorts', 'shorts', 'Bermuda'], ['trousers', 'trousers', 'Calça'], ['skirt', 'skirt', 'Saia']] as const, l.bottom, (v) => c.setLook({ bottom: v })),
          swatches(CLOTH, l.bottomColour, (col) => c.setLook({ bottomColour: col })),
        ];
      }],
    ] : [
      ['ajustar', 'Ajustar', () => {
        // The outfit worn, as Create-a-Style dressed any garment: its colour and a pattern in a second colour, its finish, then shortened
        // below the waist (as Mutable clips a hem) and looser or closer. Colour and pattern are drawn at once; the cut rebuilds.
        const os = c.outfitShape(), st = c.outfitStyle() ?? PLAIN_STYLE;
        const put = (patch: Partial<OutfitStyle>, tint?: number | null): void => c.setOutfitStyle({ ...(c.outfitStyle() ?? PLAIN_STYLE), ...patch }, tint);
        const patterned = st.pattern !== 'plain';
        return [
          swatches(CLOTH, p().spec.look.outfitTint ?? -1, (col) => put({}, col)),
          c.ui.iconSeg<ClothPattern>([['plain', 'plain', 'Liso'], ['stripes', 'stripes', 'Listras'], ['gingham', 'gingham', 'Xadrez'], ['dots', 'dots', 'Poá'],
            ['denim', 'denim', 'Sarja (jeans)'], ['rib', 'rib', 'Canelado']], st.pattern, (v) => put({ pattern: v })),
          ...(patterned ? [
            swatches(CLOTH, st.accent, (col) => put({ accent: col })),
            slider('Escala', 3, 40, 0.5, st.scale, (v) => put({ scale: v }), (v) => `${Math.round(100 / v)} cm`),
            slider('Intensidade', 0, 1, 0.01, st.amount, (v) => put({ amount: v }), (v) => `${Math.round(v * 100)}%`),
          ] : []),
          slider('Textura', 0, 1, 0.01, st.detail, (v) => put({ detail: v }), (v) => `${Math.round(v * 100)}%`),
          slider('Brilho', 0, 1, 0.01, st.shine, (v) => put({ shine: v }), (v) => `${Math.round(v * 100)}%`),
          slider('Comprimento', 0.6, 1, 0.01, Math.min(1, os.length), (v) => c.setOutfitShape({ ...c.outfitShape(), length: v }), (v) => `${Math.round(v * 100)}%`),
          slider('Folga', -0.3, 1.5, 0.05, os.volume * 10, (v) => c.setOutfitShape({ ...c.outfitShape(), volume: v / 10 }), (v) => `${v.toFixed(1)} cm`),
        ];
      }],
      ['looks', 'Looks', () => [
        pics(LOOKS.map((lk) => ({ id: lk.label, label: lk.label, variant: withLook(sex() === 'female' ? lk.female : lk.male) })), null,
          (id) => { const lk = LOOKS.find((x) => x.label === id)!; c.setLook(sex() === 'female' ? lk.female : lk.male); }, 'body'),
      ]],
      ['inteira', 'Conjuntos', () => [allToggle(), outfitPics([...WARDROBE.outfits[sex()], ...opts('suit'), ...opts('dress')]), ...carvePiece(p().spec.look.outfit, 'Editar peça')]],
      ['cima', 'Parte de cima', () => [allToggle(), outfitPics(opts('top')), ...carvePiece(p().spec.look.outfit, 'Editar peça')]],
      ['baixo', 'Parte de baixo', () => [c.ui.iconSeg([['bottom', 'trousers', 'Calças e shorts'], ['skirt', 'skirt', 'Saias']] as const, lowerKind, (v) => { lowerKind = v; }), ...(c.pictures.baking ? [extraPics('bottom', 'Nenhuma'), extraPics('skirt', 'Nenhuma')] : [extraPics(lowerKind, 'Nenhuma')])]],
      ['sapatos', 'Sapatos', () => [
        pics(SHOES_CATALOGUE[sex()].map((n) => ({ id: n, label: nameOf(n), variant: withLook({ footwear: n }) })), p().spec.look.footwear, (id) => c.setLook({ footwear: id }), 'feet'),
        ...carvePiece(p().spec.look.footwear, 'Editar sapatos'),
      ]],
      ['oculos', 'Óculos', () => [extraPics('glasses', 'Sem')]],
      ['joias', 'Joias', () => [extraPics('jewelry', 'Sem')]],
    ],
    expressao: [
      ['acoes', 'Ações', () => [
        el('div', { cls: 'grid' }, ...MOODS.map(([label, act, pose, clip]) => {
          const t = el('button', { cls: `tile${act === c.activity && clip === c.clip ? ' on' : ''}` }, label);
          t.onclick = () => c.act(act, pose, clip);
          return t;
        })),
      ]],
      ['humor', 'Humor', () => [slider('Humor', -1, 1, 0.01, p().spec.mood ?? 0, (v) => c.setMood(v))]],
    ],
  };
}

// src/peopleLab/panels.ts:88-93 slice of src/peopleLab/panels.ts#makePanels.pics (shared): the feature's statements; "…" marks code left out
  const pics = (entries: readonly Pic[], current: string | null | undefined, pick: (id: string) => void, framing: Framing): HTMLElement =>
    // … 1 lines
    picker(memory, `${framing}:${entries.map((e) => e.id).join(',')}`, entries, current, (e) => {
      // … 1 lines
      return c.pictures.image(`${framing}:${JSON.stringify([v.spec.body, v.spec.features, v.spec.look, v.style, v.sculpt?.deltas.length])}`, v, framing);
    }, (id) => { pick(id); c.render(); }, c.pictures.baking);

// src/peopleLab/panels.ts:96-96 slice of src/peopleLab/panels.ts#makePanels.withLook (shared): the feature's statements; "…" marks code left out
  const withLook = (patch: Partial<PersonLook>): LabPerson => ({ ...m(), spec: { ...m().spec, look: { ...m().spec.look, ...patch } } });

// src/peopleLab/panels.ts:99-99 slice of src/peopleLab/panels.ts#makePanels.mf (shared): the feature's statements; "…" marks code left out
  const mf = (): Readonly<Record<string, number>> => m().spec.features;

// src/peopleLab/panels.ts:101-101 slice of src/peopleLab/panels.ts#makePanels.featureSlider (shared): the feature's statements; "…" marks code left out
  const featureSlider = (k: string, label = sliderLabel.get(k) ?? k): HTMLElement => slider(label, -1, 1, 0.01, p().spec.features[k] ?? 0, (v) => c.setFeature(k, v));

// src/peopleLab/panels.ts:114-114 slice of src/peopleLab/panels.ts#makePanels.b (shared): the feature's statements; "…" marks code left out
  const b = (): LabPerson['spec']['body'] => p().spec.body;

// src/peopleLab/panels.ts:122-131 slice of src/peopleLab/panels.ts#makePanels.partGroup (shared): the feature's statements; "…" marks code left out
  const partGroup = (id: string): (() => Node[]) => () => {
    // … 1 lines
    const f = p().spec.features;
    return [
      pics(part.presets.map((pr) => ({ id: pr.label, label: pr.label, variant: withFeatures(withPart(mf(), part, pr)) })), currentPreset(f, part),
        (pid) => c.setSpec({ features: withPart(f, part, part.presets.find((x) => x.label === pid)!) }), FRAMES[part.id] ?? 'face'),
      el('div', { cls: 'gap' }),
      ...part.sliders.filter((k) => sliderLabel.has(k)).map((k) => featureSlider(k)),
    ];
  };

// src/peopleLab/panels.ts:133-134 slice of src/peopleLab/panels.ts#makePanels.outfitPics (shared): the feature's statements; "…" marks code left out
  const outfitPics = (names: readonly string[]): HTMLElement =>
    pics(names.map((n) => ({ id: n, label: nameOf(n), variant: withLook({ outfit: n }) })), p().spec.look.outfit, (id) => c.setLook({ outfit: id }), 'body');

// src/peopleLab/panels.ts:142-142 slice of src/peopleLab/panels.ts#makePanels.opts (shared): the feature's statements; "…" marks code left out
  const opts = (kind: WardrobeItem['kind']): readonly string[] => itemsOf(kind, { street: !c.showAll, sex: sex() });

// src/peopleLab/panels.ts:143-143 slice of src/peopleLab/panels.ts#makePanels.child (shared): the feature's statements; "…" marks code left out
  const child = (): boolean => yearsFromAge(b().age) < 16;

// src/peopleLab/panels.ts:144-148 slice of src/peopleLab/panels.ts#makePanels.extraPics (shared): the feature's statements; "…" marks code left out
  const extraPics = (kind: WardrobeItem['kind'], none: string): HTMLElement =>
    pics(['none', ...itemsOf(kind, { street: true, sex: sex() })].map((n) => ({ id: n, label: n === 'none' ? none : nameOf(n),
      // A lower garment over the mannequin's whole-body suit would not show: shown under a top alone.
      variant: withLook({ extras: n === 'none' ? [] : [n], ...(kind === 'bottom' || kind === 'skirt' ? { outfit: itemsOf('top', { street: true, sex: sex() })[0] } : {}) }) })),
      c.extraOf([kind]), (id) => c.setExtra([kind], id), kind === 'beard' ? 'jaw' : kind === 'glasses' ? 'eyes' : kind === 'jewelry' ? 'head' : 'body');

// src/peopleLab/panels.ts:221-221 slice of src/peopleLab/panels.ts#makePanels.set (shared): the feature's statements; "…" marks code left out
        const set = (k: keyof CartoonStyle, v: number): void => c.setStyle({ ...s, [k]: v });
