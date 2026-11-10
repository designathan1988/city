// The code of src/editor/cityGenerator.ts that is the feature's (the rest of the file is shared).

// src/editor/cityGenerator.ts:46-62 slice of src/editor/cityGenerator.ts#greenCity (shared): the feature's statements; "…" marks code left out
export function greenCity(doc: RoadDoc, net: Network, seed: number): { squares: number; pieces: number } {
  // … 9 lines
  const pieces = furnitureFor(net, doc.segments.keys(), 'complete', doc.landscape.values(), power, false);
  // … 1 lines
  doc.addLandscapeMany(pieces.map((piece) => ({ kind: piece.kind, at: piece.at })));
  // … 2 lines
  return { squares: squares.length, pieces: pieces.length };
}
