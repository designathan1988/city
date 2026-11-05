// The code of src/lab/catalog/layers.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layers.ts:78-86 slice of src/lab/catalog/layers.ts#signItems.one (shared): the feature's statements; "…" marks code left out
  const one = (name: string, variation: string, extra: (doc: RoadDoc, net: Network) => void, junction = false): void => {
    // … 1 lines
    const layer = createSignLayer();
    layer.sync(net, buildRoadElevation(net, flat));
    cullClose(layer);
    add({ group: 'Signs', name, variation, origin: 'kit', source: 'src/render/signLayer.ts (createSignLayer, sync)', build: 'instanced',
      pieces: piecesOf(layer.group), object: shown(layer.group, cache), countKey: `sign:${name}`,
      notes: ['the post an instance of the posts\' run, the plate a quad of its sheet\'s mesh (its text painted on the sheet)'] });
  };

// src/lab/catalog/layers.ts:118-134 slice of src/lab/catalog/layers.ts#signalItems (shared): the feature's statements; "…" marks code left out
export function signalItems(add: Add, cache: BatchCache): void {
  // … 10 lines
  const heads = createSignalHeads(new Scene());
  heads.sync(feed, net, true);
  add({ group: 'Traffic signals', name: 'signalised junction', variation: `${incident.length} heads (post, arm, housing, visors, lenses, halos)`, origin: 'kit',
    source: 'src/render/signals.ts (createSignalHeads, sync)', build: 'instanced', pieces: piecesOf(heads.group), object: shown(heads.group, cache), countKey: 'signals',
    notes: ['one head on each approach of a signal group, where the world stands its post (signalPostPlace); the frame (post, arm, housing) one geometry an instance'] });
}
