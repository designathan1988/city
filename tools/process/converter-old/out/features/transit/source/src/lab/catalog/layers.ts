// The code of src/lab/catalog/layers.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layers.ts:238-262 slice of src/lab/catalog/layers.ts#transitItems (shared): the feature's statements; "…" marks code left out
export function transitItems(add: Add, cache: BatchCache): void {
  // … 8 lines
  lay('train track and station', `${LEG * 2} m of track`, (doc) => {
    const track = addTrack(emptyTransit(), 'train', [{ x: -LEG, y: m(40) }, { x: LEG, y: m(40) }]);
    const stop = addStop(track.data, { mode: 'train', x: 0, y: m(40), track: track.id });
    doc.setTransit(addLine(stop.data, 'train', [stop.id]).data);
  });
  lay('metro track and station', `${LEG * 2} m of track, its way down`, (doc, net) => {
    const track = addTrack(emptyTransit(), 'metro', [{ x: -LEG, y: m(10) }, { x: LEG, y: m(10) }]);
    const stop = addStop(track.data, { mode: 'metro', x: 0, y: m(10), track: track.id, entrance: { x: m(10), y: footwayOut(net) } });
    doc.setTransit(addLine(stop.data, 'metro', [stop.id]).data);
  });
  lay('bus stop', 'on the footway\'s edge', (doc, net) => {
    // … 1 lines
    const stop = addStop(emptyTransit(), { mode: 'bus', x: 0, y: footwayOut(net), segment });
    doc.setTransit(addLine(stop.data, 'bus', [stop.id]).data);
  });
}

// src/lab/catalog/layers.ts:239-246 slice of src/lab/catalog/layers.ts#transitItems.lay (shared): the feature's statements; "…" marks code left out
  const lay = (name: string, variation: string, make: (doc: RoadDoc, net: Network) => void): void => {
    // … 1 lines
    const layer = createTransit();
    layer.sync(doc, () => 0, () => 0, () => false);
    cullClose(layer);
    add({ group: 'Transit', name, variation, origin: 'kit', source: 'src/render/transit.ts (createTransit, sync)', build: 'instanced',
      pieces: piecesOf(layer.group), object: shown(layer.group, cache), countKey: `transit:${name}` });
  };
