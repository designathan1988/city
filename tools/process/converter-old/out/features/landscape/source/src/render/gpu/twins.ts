// The code of src/render/gpu/twins.ts that is the feature's (the rest of the file is shared).

// src/render/gpu/twins.ts:50-53 slice of src/render/gpu/twins.ts#LandLodData (shared): the feature's statements; "…" marks code left out
interface LandLodData {
  // … 1 lines
  grid: LandGrid;
}

// src/render/gpu/twins.ts:58-110 slice of src/render/gpu/twins.ts#ground (shared): the feature's statements; "…" marks code left out
const ground: Twin = {
  // … 2 lines
  make(material, mesh) {
    // … 29 lines
    const rimAt = select(side.lessThan(int(2)), ivec2(select(side.equal(int(0)), int(0), int(LOD_PATCH)), v.x), ivec2(v.x, select(side.equal(int(2)), int(0), int(LOD_PATCH))));
    const at = select(inPatch, v.xy, rimAt) as unknown as Node<'ivec2'>;
    const a = corner(at), b = corner(v.xy.add(ivec2(0, 1))), c = corner(v.xy.add(ivec2(1, 1))), d = corner(v.xy.add(ivec2(1, 0)));
    const unmorphed = patch.w.equal(0).and(a.k.add(b.k).add(c.k).add(d.k).equal(0));
    const flip = select(unmorphed, texel(start.add(v.xy)).z.greaterThan(0.5), abs(a.p.y.sub(c.p.y)).add(0.5).lessThan(abs(b.p.y.sub(d.p.y))));
    // … 1 lines
    const which = select(flip, role, role.add(int(1)).bitAnd(int(3)));
    // … 3 lines
    const rimP = a.p.sub(vec3(0, select(role.equal(int(7)), hang, float(0)), 0));
    // … 2 lines
    twin.positionNode = select(inPatch, pick(a.p, b.p, c.p, d.p), rimP);
    twin.normalNode = transformNormalToView(varying(select(inPatch, pick(a.n, b.n, c.n, d.n), a.n)));
  // … 3 lines
  },
};

// src/render/gpu/twins.ts:69-69 slice of src/render/gpu/twins.ts#ground.texel (shared): the feature's statements; "…" marks code left out
    const texel = (c: Node<'ivec2'>): Node<'vec4'> => textureLoad(land, clampCorner(c, ivec2(0, 0), ivec2(grid.seg, grid.seg))) as unknown as Node<'vec4'>;

// src/render/gpu/twins.ts:71-71 slice of src/render/gpu/twins.ts#ground.point (shared): the feature's statements; "…" marks code left out
    const point = (c: Node<'ivec2'>, h: Node<'float'>): Node<'vec3'> => vec3(float(c.x).mul(grid.cell).sub(grid.half), h, float(c.y).mul(grid.cell).sub(grid.half));
