// The code of src/world/buildings/pads.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/pads.ts:316-327 function padShaper: the feature's own
export function padShaper(cut: PadShapeCut): Pick<BuildingPads, 'shapeAt'> {
  const lists = new Map<number, readonly Pad[]>();
  for (const [key, ids] of cut.cells) lists.set(key, Array.from(ids, (k) => cut.pads[k]!));
  const grading = new Grading();
  return {
    shapeAt(x, y, ground) {
      const list = lists.get(padCellKey(Math.floor(x / PAD_CELL), Math.floor(y / PAD_CELL)));
      if (!list) throw new Error(`no platforms were cut for (${x}, ${y})`);
      return shapeOverPads(list, cut.apron, x, y, ground, grading);
    },
  };
}
