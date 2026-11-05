// What the feature uses of src/render/gpu/twins.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpu/twins.ts:14-20 interface Twin
/** THE GAME'S OWN MATERIALS REWRITTEN IN NODES (TSL), for the WebGPU renderer (`MIGRACAO-WEBGPU.md`): each twin draws what its WebGL material
 *  draws, reading the SAME uniforms' objects (a colour or a direction changed in place is seen by both), and replaces it in the WebGPU frame
 *  only once `scripts/look.mjs` `compare` shows the two pictures equal. A material with no twin and no node equivalent is left out and named. */

interface Twin {
  /** Whether this twin draws that mesh's material. */
  matches(mesh: Mesh, material: Material): boolean;
  make(material: Material, mesh: Mesh): NodeMaterial;
  /** `faithful`: drawn as the WebGL material draws (the comparison says how near); `simple`: the right shape with a plain finish, to see the scene until its faithful twin comes (`MIGRACAO-WEBGPU.md` stage 2). */
  readonly kind: 'faithful' | 'simple';
}

// src/render/gpu/twins.ts:46-46 variable clampCorner
const clampCorner = clamp as unknown as (x: Node<'ivec2'>, low: Node<'ivec2'>, high: Node<'ivec2'>) => Node<'ivec2'>;

// src/render/gpu/twins.ts:47-47 variable integerAnd
const integerAnd = bitAnd as unknown as (a: Node<'ivec2'>, b: Node<'ivec2'>) => Node<'ivec2'>;

// src/render/gpu/twins.ts:50-53 interface LandLodData
/** What a plate's patches are placed with (`terrain.ts` patchMesh `userData.landLod`). */
interface LandLodData {
  uniforms: { uLand: { value: Texture }; uLodMorph: { value: Float32Array }; uLodSkirt: { value: Vector4 } };
  grid: LandGrid;
}

// src/render/gpu/twins.ts:58-110 variable ground
/** The ground's patches (`terrainLod.ts` lodVertexGlsl): placed, read from the land's texels and morphed exactly as WebGL does, so the
 *  relief stands where it stands; its finish one plain grass tone, lit (the splat, the paint, the water's edge and the lots wait for the
 *  ground's faithful twin, stage 2). */
const ground: Twin = {
  kind: 'simple',
  matches: (mesh) => mesh.userData['landLod'] !== undefined,
  make(material, mesh) {
    const { uniforms: u, grid } = mesh.userData['landLod'] as LandLodData;
    const origin = uniform((material.userData['terrainUniforms'] as { uTileOrigin: { value: Vector3 } }).uTileOrigin.value);
    const morphs = uniformArray(u.uLodMorph.value as unknown as number[], 'float');
    const skirt = uniform(u.uLodSkirt.value);
    const land = u.uLand.value;
    const patch = attribute('aLodPatch', 'vec4');
    const v = attribute('position', 'ivec3');
    const texel = (c: Node<'ivec2'>): Node<'vec4'> => textureLoad(land, clampCorner(c, ivec2(0, 0), ivec2(grid.seg, grid.seg))) as unknown as Node<'vec4'>;
    const normalOf = (t: Node<'vec4'>): Node<'vec3'> => vec3(t.x, sqrt(max(0, float(1).sub(t.x.mul(t.x)).sub(t.y.mul(t.y)))), t.y);
    const point = (c: Node<'ivec2'>, h: Node<'float'>): Node<'vec3'> => vec3(float(c.x).mul(grid.cell).sub(grid.half), h, float(c.y).mul(grid.cell).sub(grid.half));
    const start = ivec2(patch.xy), step = int(patch.z);
    // A patch's level, and a rim's side above it (`terrainLod.ts` `LandLod.add`).
    const level = int(patch.w).bitAnd(int(15)), side = int(patch.w).shiftRight(int(4));
    /** A corner of the patch, moved towards the next level's by its own distance (`landCorner`). */
    const corner = (g: Node<'ivec2'>): { p: Node<'vec3'>; n: Node<'vec3'>; k: Node<'float'> } => {
      const c = start.add(g.mul(step));
      const t = texel(c);
      const p = point(c, t.w);
      const slot = level.mul(2);
      const k = clamp(distance(p.add(origin), cameraPosition).sub(morphs.element(slot) as unknown as Node<'float'>).mul(morphs.element(slot.add(1)) as unknown as Node<'float'>), 0, 1);
      const odd = integerAnd(g, ivec2(1, 1));
      const to = start.add(g.sub(odd).mul(step));
      const next = texel(to);
      const moved = select(odd.x.add(odd.y).greaterThan(int(0)), k, float(0));
      return { p: mix(p, point(to, next.w), moved), n: mix(normalOf(t), normalOf(next), moved), k };
    };
    const role = v.z;
    const inPatch = role.lessThan(int(4));
    // A rim's corner along its side: west, east, north, south (x = 0, x = LOD_PATCH, y = 0, y = LOD_PATCH).
    const rimAt = select(side.lessThan(int(2)), ivec2(select(side.equal(int(0)), int(0), int(LOD_PATCH)), v.x), ivec2(v.x, select(side.equal(int(2)), int(0), int(LOD_PATCH))));
    const at = select(inPatch, v.xy, rimAt) as unknown as Node<'ivec2'>;
    const a = corner(at), b = corner(v.xy.add(ivec2(0, 1))), c = corner(v.xy.add(ivec2(1, 1))), d = corner(v.xy.add(ivec2(1, 0)));
    const unmorphed = patch.w.equal(0).and(a.k.add(b.k).add(c.k).add(d.k).equal(0));
    const flip = select(unmorphed, texel(start.add(v.xy)).z.greaterThan(0.5), abs(a.p.y.sub(c.p.y)).add(0.5).lessThan(abs(b.p.y.sub(d.p.y))));
    // Triangles over roles (0, 1, 2) and (0, 2, 3); turned by one where the cell splits along its other diagonal (`landVertex`).
    const which = select(flip, role, role.add(int(1)).bitAnd(int(3)));
    const pick = (x: Node<'vec3'>, y: Node<'vec3'>, z: Node<'vec3'>, w: Node<'vec3'>): Node<'vec3'> => select(which.equal(int(0)), x, select(which.equal(int(1)), y, select(which.equal(int(2)), z, w)));
    // The rim's strip: hung from its side's edge corners, down by the skirt (a rim is drawn only on a side that hangs one).
    const hang = select(side.equal(int(0)), skirt.x, select(side.equal(int(1)), skirt.y, select(side.equal(int(2)), skirt.z, skirt.w)));
    const rimP = a.p.sub(vec3(0, select(role.equal(int(7)), hang, float(0)), 0));
    const twin = new MeshStandardNodeMaterial({ roughness: 1, metalness: 0, envMapIntensity: (material as Material & { envMapIntensity?: number }).envMapIntensity ?? 1 });
    twin.name = 'terrain (nodes, simple)';
    twin.positionNode = select(inPatch, pick(a.p, b.p, c.p, d.p), rimP);
    twin.normalNode = transformNormalToView(varying(select(inPatch, pick(a.n, b.n, c.n, d.n), a.n)));
    // The WebGL ground's mean grass, read off its picture under the same light (`scripts/look.mjs` compare, 2026-10-21), linear.
    twin.colorNode = vec3(0.037, 0.054, 0.021);
    return twin;
  },
};
