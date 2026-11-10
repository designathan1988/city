// The code of src/lab/catalog/layers.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layers.ts:55-67 slice of src/lab/catalog/layers.ts#oneRoad (shared): the feature's statements; "…" marks code left out
function oneRoad(extra: (doc: RoadDoc, net: Network) => void, junction = false): { doc: RoadDoc; net: Network } {
  // … 5 lines
  let net = new Network(doc);
  net.rebuild();
  extra(doc, net);
  net = new Network(doc);
  net.rebuild();
  return { doc, net };
}

// src/lab/catalog/layers.ts:70-74 slice of src/lab/catalog/layers.ts#footwayOut (shared): the feature's statements; "…" marks code left out
function footwayOut(net: Network): number {
  const first = net.doc.segments.keys().next().value!;
  const ribbon = net.ribbons.get(first)!;
  return ribbon.road.width / 2 + ribbon.road.sidewalk / 2;
}

// src/lab/catalog/layers.ts:78-86 slice of src/lab/catalog/layers.ts#signItems.one (shared): the feature's statements; "…" marks code left out
  const one = (name: string, variation: string, extra: (doc: RoadDoc, net: Network) => void, junction = false): void => {
    const { net } = oneRoad(extra, junction);
    const layer = createSignLayer();
    layer.sync(net, buildRoadElevation(net, flat));
    cullClose(layer);
    add({ group: 'Signs', name, variation, origin: 'kit', source: 'src/render/signLayer.ts (createSignLayer, sync)', build: 'instanced',
      pieces: piecesOf(layer.group), object: shown(layer.group, cache), countKey: `sign:${name}`,
      notes: ['the post an instance of the posts\' run, the plate a quad of its sheet\'s mesh (its text painted on the sheet)'] });
  };

// src/lab/catalog/layers.ts:118-134 slice of src/lab/catalog/layers.ts#signalItems (shared): the feature's statements; "…" marks code left out
export function signalItems(add: Add, cache: BatchCache): void {
  // … 1 lines
  const centre = [...net.doc.nodes.values()].find((n) => n.incident.length === 4)!;
  const incident = centre.incident;
  const feed: SignalFeed = {
    // … 1 lines
    layout: (node: NodeId) => (node === centre.id ? { node, groups: [{ id: 0, segments: [incident[0]!, incident[1]!] }, { id: 1, segments: [incident[2]!, incident[3]!] }] } : undefined),
    // … 2 lines
    nodes: () => [centre.id],
  };
  // … 2 lines
  add({ group: 'Traffic signals', name: 'signalised junction', variation: `${incident.length} heads (post, arm, housing, visors, lenses, halos)`, origin: 'kit',
    // … 1 lines
    notes: ['one head on each approach of a signal group, where the world stands its post (signalPostPlace); the frame (post, arm, housing) one geometry an instance'] });
}

// src/lab/catalog/layers.ts:200-200 function farNetworkItems.flat: the feature's own
  const flat = (): number => 0;

// src/lab/catalog/layers.ts:238-262 slice of src/lab/catalog/layers.ts#transitItems (shared): the feature's statements; "…" marks code left out
export function transitItems(add: Add, cache: BatchCache): void {
  // … 18 lines
  lay('bus stop', 'on the footway\'s edge', (doc, net) => {
    const segment = net.doc.segments.keys().next().value!;
    const stop = addStop(emptyTransit(), { mode: 'bus', x: 0, y: footwayOut(net), segment });
  // … 1 lines
  });
}

// src/lab/catalog/layers.ts:239-246 slice of src/lab/catalog/layers.ts#transitItems.lay (shared): the feature's statements; "…" marks code left out
  const lay = (name: string, variation: string, make: (doc: RoadDoc, net: Network) => void): void => {
    const { doc } = oneRoad(make);
    const layer = createTransit();
    layer.sync(doc, () => 0, () => 0, () => false);
    cullClose(layer);
    add({ group: 'Transit', name, variation, origin: 'kit', source: 'src/render/transit.ts (createTransit, sync)', build: 'instanced',
      pieces: piecesOf(layer.group), object: shown(layer.group, cache), countKey: `transit:${name}` });
  };
