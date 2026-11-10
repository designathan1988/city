// The code of src/core/clipper.ts that is the feature's (the rest of the file is shared).

// src/core/clipper.ts:158-169 function opened: the feature's own
export function opened(polygons: MultiPoly, eps: number): MultiPoly {
  const paths = input(polygons);
  if (!paths.length) return [];
  const d = eps * SCALE;
  const grown = inflatePaths(inflatePaths(paths, -d, JoinType.Miter, EndType.Polygon, OPEN_MITER), d, JoinType.Miter, EndType.Polygon, OPEN_MITER);
  if (!grown.length) return [];
  const clip = new Clipper64();
  clip.addPaths(grown, PathType.Subject);
  const tree = new PolyTree64();
  if (!clip.execute(ClipType.Union, FillRule.NonZero, tree)) throw new Error('Polygon clipping failed (opening)');
  return output(tree);
}
