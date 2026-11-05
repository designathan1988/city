// The code of src/ui/roads/catalogPanel.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/catalogPanel.ts:28-28 function nameOf: the feature's own
const nameOf = (r: RoadTemplate): string => (r.nameKey ? t(r.nameKey) : r.name ?? r.id);

// src/ui/roads/catalogPanel.ts:29-30 function sameRoad: the feature's own
const sameRoad = (a: { type: number; profile: RoadProfileSpec }, b: { type: number; profile: RoadProfileSpec }): boolean =>
  a.type === b.type && sameProfile(a.profile, b.profile);

// src/ui/roads/catalogPanel.ts:33-35 function allCatalogRoads: the feature's own
function allCatalogRoads(): CatalogRoad[] {
  return [...catalogRoads(), ...savedTemplates().map((r) => ({ ...r, category: 'mine' as const }))];
}

// src/ui/roads/catalogPanel.ts:38-43 function pickedRoad: the feature's own
export function pickedRoad(host: Pick<CatalogHost, 'classIndex'>): CatalogRoad | null {
  const chosen = drawProfile();
  const roads = allCatalogRoads();
  if (chosen) return roads.find((r) => sameRoad(r, chosen)) ?? null;
  return roads.find((r) => r.id.startsWith('class:') && r.type === host.classIndex()) ?? null;
}

// src/ui/roads/catalogPanel.ts:46-50 function pickRoad: the feature's own
function pickRoad(road: RoadTemplate, host: CatalogHost): void {
  host.pickClass(road.type);
  const profile: DrawProfile | null = road.id.startsWith('class:') ? null : { name: nameOf(road), type: road.type, profile: road.profile };
  setDrawProfile(profile);
}

// src/ui/roads/catalogPanel.ts:65-153 function catalogStrip: the feature's own
export function catalogStrip(host: CatalogHost): HTMLElement {
  const picked = pickedRoad(host);
  const saved = savedTemplates();
  strip.category ??= picked?.category ?? 'streets';
  const category = strip.category;
  // A profile in hand that no card is (an edited one, or one picked from a road with the eyedropper, V8)
  const loose = picked ? null : drawProfile();
  const kept = strip.built;
  if (kept && kept.language === language() && kept.category === category && kept.saved === saved && kept.loose === loose) {
    kept.host = host;
    markPicked(kept, loose ? 'drawProfile' : picked?.id ?? null);
    return kept.root;
  }
  const roads = allCatalogRoads();
  const root = document.createElement('div');
  root.className = 'rc-catalog';

  const tabs = document.createElement('div');
  tabs.className = 'rc-tabs';
  tabs.setAttribute('role', 'tablist');
  for (const c of CATALOG_CATEGORIES) {
    const b = document.createElement('button');
    b.type = 'button';
    b.className = `rc-tab${c === category ? ' on' : ''}`;
    b.setAttribute('role', 'tab');
    b.setAttribute('aria-selected', String(c === category));
    b.dataset['category'] = c;
    const count = roads.filter((r) => r.category === c).length;
    b.innerHTML = '<span></span><em></em>';
    b.querySelector('span')!.textContent = t(`catalog.cat.${c}`);
    b.querySelector('em')!.textContent = String(count);
    b.onclick = () => { strip.category = c; built.host.refresh(); };
    tabs.append(b);
  }
  root.append(tabs);

  const grid = document.createElement('div');
  grid.className = 'rc-cards';
  const shown = roads.filter((r) => r.category === category);
  if (loose) shown.unshift({ id: 'drawProfile', name: loose.name, category, type: loose.type, profile: loose.profile });
  // One scale for every tile: the widest road offered.
  const span = Math.max(...roads.map((r) => r.profile.elements.reduce((sum, e) => sum + e.width, 0)));
  if (!shown.length) {
    const empty = document.createElement('div');
    empty.className = 'rc-empty';
    empty.textContent = t(category === 'mine' ? 'catalog.mineEmpty' : 'catalog.empty');
    grid.append(empty);
  }
  const swatches = new Map<string, string>();
  const cards: BuiltStrip['cards'][number][] = [];
  for (const road of shown) {
    const rt = profileRoad(road.profile, road.type);
    const card = document.createElement('div');
    card.className = 'rc-card';
    const main = document.createElement('button');
    main.type = 'button';
    main.className = 'rc-card-main';
    main.dataset['road'] = road.id;
    const lanes = road.profile.elements.filter((e) => e.kind === 'lane').length;
    const oneWay = !road.profile.elements.some((e) => e.kind === 'lane' && e.dir === 'backward') ||
      !road.profile.elements.some((e) => e.kind === 'lane' && e.dir === 'forward');
    main.title = t('catalog.tip', {
      name: nameOf(road), lanes: plural('palette.lanes.count', lanes), way: t(oneWay ? 'catalog.oneWay' : 'catalog.twoWay'), speed: road.profile.speedKmh,
    });
    main.innerHTML = '<img class="rc-art" alt="" draggable="false"><span class="rc-name"></span><span class="rc-meta"></span>';
    const swatchKey = `${JSON.stringify(road.profile)}|${span}`;
    const swatch = swatches.get(swatchKey) ?? kept?.swatches.get(swatchKey) ?? profileSwatch(road.profile, span, 124, 64);
    swatches.set(swatchKey, swatch);
    (main.querySelector('.rc-art') as HTMLImageElement).src = swatch;
    main.querySelector('.rc-name')!.textContent = nameOf(road);
    main.querySelector('.rc-meta')!.textContent = t('catalog.meta', {
      width: metresText(profileWidth(road.profile)),
      price: formatMoney(Math.round(pricePerUnit(rt) * UNITS_PER_METER)),
    });
    main.onclick = () => {
      pickRoad(road, built.host);
      if (profileEditorOpen()) closeProfileEditor();
      built.host.refresh();
    };
    card.append(main);
    grid.append(card);
    cards.push({ id: road.id, card, main, road });
  }
  root.append(grid);
  const built: BuiltStrip = { root, language: language(), category, saved, loose, cards, swatches, host };
  strip.built = built;
  markPicked(built, loose ? 'drawProfile' : picked?.id ?? null);
  return root;
}

// src/ui/roads/catalogPanel.ts:156-165 function markPicked: the feature's own
function markPicked(built: BuiltStrip, id: string | null): void {
  for (const { id: cardId, card, main, road } of built.cards) {
    const on = cardId === id;
    card.classList.toggle('on', on);
    main.setAttribute('aria-pressed', String(on));
    const custom = card.querySelector('.rc-custom');
    if (on && !custom) card.append(customButton(built, road));
    else if (!on) custom?.remove();
  }
}

// src/ui/roads/catalogPanel.ts:167-186 slice of src/ui/roads/catalogPanel.ts#customButton (shared): the feature's statements; "…" marks code left out
function customButton(built: BuiltStrip, road: CatalogRoad): HTMLButtonElement {
  // … 7 lines
  custom.onclick = () => {
    openProfileEditor({
      // … 5 lines
      onSaved: () => { strip.category = 'mine'; built.host.refresh(); },
    });
  };
// … 1 lines
}
