// What the feature uses of src/people/body/proxy.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/body/proxy.ts:6-30 interface ProxyPack
/** One proxy as packed by the importer. */
export interface ProxyPack {
  readonly name: string;
  readonly kind: 'clothes' | 'hair' | 'eyebrows' | 'eyelashes' | 'eyes' | 'shoes' | CommunityKind;
  /** Base-mesh vertices the scale axes are measured between: [x0, x1, y0, y1, z0, z1]. */
  readonly scaleRefs: readonly number[];
  /** The axis lengths on the base mesh itself, decimetres. */
  readonly scaleBase: readonly [number, number, number];
  /** Per proxy vertex: three base vertices. */
  readonly refs: Uint32Array;
  /** Per proxy vertex: three weights. */
  readonly weights: Float32Array;
  /** Per proxy vertex: an offset, in units of the scale axes. */
  readonly offsets: Float32Array;
  /** Triangles. */
  readonly index: Uint32Array;
  /** Base-mesh vertices this proxy hides (the skin under a garment), or empty. */
  readonly deleteVerts: Uint32Array;
  /** Average colour of its texture, 0xRRGGBB, and how tintable it is. */
  readonly colour: number;
  /** Layer order: what is drawn over what. */
  readonly zDepth: number;
  readonly uvs?: Float32Array;
  /** Per vertex, how much of it is drawn, 0..1 (a generated hairline's feathering); all of it when absent. */
  readonly fade?: Float32Array;
}

// src/people/body/proxy.ts:120-125 interface ProxyTexture
/** A texture read back to pixels, small: only sampled once per vertex. */
export interface ProxyTexture {
  readonly width: number;
  readonly height: number;
  /** RGBA, row 0 at the top of the image. */
  readonly data: Uint8ClampedArray;
}

// src/people/body/proxy.ts:128-135 interface ProxyItem
/** A proxy ready to dress somebody: its geometry, and its texture's pixels when it has one. */
export interface ProxyItem {
  readonly pack: ProxyPack;
  readonly texture: ProxyTexture | null;
  /** Its texture has holes (hair strands, lashes): only the solid parts are drawn in the crowd. */
  readonly transparent: boolean;
  /** Its texture's file, for drawing it textured close up (`proxyUrl`), or null. */
  readonly textureFile: string | null;
}

// src/people/body/proxy.ts:159-196 function loadProxyItem
/** One proxy by name, fetched once (`public/models/people/proxies`). */
export function loadProxyItem(name: string): Promise<ProxyItem>;
