// The code of src/render/people/proceduralBake.ts that is the feature's (the rest of the file is shared).

// src/render/people/proceduralBake.ts:74-83 function packFromRecord: the feature's own
export function packFromRecord(r: Record<string, PackValue>): ProxyPack {
  const uvs = r['uvs'], fade = r['fade'];
  return {
    name: r['name'] as string, kind: r['kind'] as ProxyPack['kind'], scaleRefs: r['scaleRefs'] as number[],
    scaleBase: r['scaleBase'] as unknown as [number, number, number], refs: r['refs'] as Uint32Array,
    weights: r['weights'] as Float32Array, offsets: r['offsets'] as Float32Array, index: r['index'] as Uint32Array,
    deleteVerts: r['deleteVerts'] as Uint32Array, colour: r['colour'] as number, zDepth: r['zDepth'] as number,
    ...(uvs instanceof Float32Array ? { uvs } : {}), ...(fade instanceof Float32Array ? { fade } : {}),
  };
}

// src/render/people/proceduralBake.ts:90-98 function hairCards: the feature's own
export async function hairCards(env: BakeEnv, style: HairStyle): Promise<ProxyPack> {
  const cooked = await loadProcedural(`hair-${style.name}`);
  if (cooked) return packFromRecord(cooked);
  const { assets: a, morpher: mo } = await env.setup();
  const at = performance.now();
  const pack = generateHair(style, hairBase(a, mo));
  measureHitch(`hitch:person/hair ${style.name}`, at);
  return pack;
}

// src/render/people/proceduralBake.ts:140-147 function classFromRecord: the feature's own
export const classFromRecord = (r: Record<string, PackValue>): ClassData => ({
  shapePixels: r['shapePixels'] as Float32Array, jointBasis: r['jointBasis'] as Float32Array,
  faceIndexPixels: r['faceIndexPixels'] as Float32Array, faceList: r['faceList'] as Int32Array,
  exprPixels: r['exprPixels'] as Float32Array, walk: clipOf('walk', r), idle: clipOf('idle', r),
  run: clipOf('run', r), cower: clipOf('cower', r), sprint: clipOf('sprint', r), photo: clipOf('photo', r), getUp: clipOf('getUp', r), duck: clipOf('duck', r), hurtWalk: clipOf('hurtWalk', r),
  hurtRun: clipOf('hurtRun', r), nervous: clipOf('nervous', r), hit: clipOf('hit', r),
  staggerBack: clipOf('staggerBack', r), riseFront: clipOf('riseFront', r), riseBack: clipOf('riseBack', r), crawl: clipOf('crawl', r),
});
