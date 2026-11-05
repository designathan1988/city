// What the feature uses of src/render/signLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/signLayer.ts:19-26 interface Stand
/** DO NOT build every sign of the map again */

/** One plate standing: where, on what ground, what it says, which way it faces. */
interface Stand {
  readonly x: number;
  readonly y: number;
  readonly ground: number;
  readonly type: SignType;
  readonly text: string;
  readonly yaw: number;
}

// src/render/signLayer.ts:29-32 interface SignValue
/** A key's signs and the ground they read: a change there stands them again. */
export interface SignValue {
  readonly stands: readonly Stand[];
  readonly reads: ChangeRect;
}

// src/render/signLayer.ts:34-38 interface SignSource
export interface SignSource {
  readonly changes: ChangeJournal;
  readonly net: Network;
  readonly elevation: RoadElevation;
}

// src/render/signLayer.ts:41-41 variable ITEM_READS
/** How far round a placed sign or a street's name the footway it stands on is looked for (`streetChain`'s reach). */
const ITEM_READS = m(30);

// src/render/signLayer.ts:43-44 function unite
const unite = (a: ChangeRect | null, b: ChangeRect): ChangeRect => …;

// src/render/signLayer.ts:45-45 function around
const around = (x: number, y: number, r: number): ChangeRect => …;

// src/render/signLayer.ts:46-46 function grow
const grow = (r: ChangeRect, pad: number): ChangeRect => …;

// src/render/signLayer.ts:49-54 function ribbonBox
/** The box a segment's ribbon covers, with its casing and footways. */
function ribbonBox(net: Network, id: SegmentId): ChangeRect | null;

// src/render/signLayer.ts:56-56 function isSignItem
const isSignItem = (item: LandscapeItem): boolean => …;

// src/render/signLayer.ts:59-59 function nodeKey
/** A junction's key and a sign item's: one number (`CellFiling` files numbers), even and odd. */
const nodeKey = (id: number): number => …;

// src/render/signLayer.ts:60-60 function itemKey
const itemKey = (id: number): number => …;

// src/render/signLayer.ts:73-79 method SignProduct.allKeys
/** @rebuild Every junction and every sign item of the map: read only when everything changed (a load, the diary forgotten). */
  allKeys(source: SignSource): Iterable<number>;

// src/render/signLayer.ts:81-110 method SignProduct.dirty
dirty(change: Change, out: Set<number>, source: SignSource): void;

// src/render/signLayer.ts:137-152 function junctionSigns
/** A junction's signs (`signsAtNode`), reading its segments' ground. */
function junctionSigns({ net, elevation }: SignSource, node: NodeId): SignValue | undefined;

// src/render/signLayer.ts:155-196 function itemSigns
/** A placed sign's plate, or a street name's plates at both ends of every segment of its street. */
function itemSigns({ net, elevation }: SignSource, id: number): SignValue | undefined;

// src/render/signLayer.ts:219-232 interface SignLayerKept
export interface SignLayerKept {
  readonly group: Group;
  /** Brings the signs up to the diary; the keys stood again. */
  sync(net: Network, elevation: RoadElevation): number;
  /** A post and a plate as the layer draws them, for their programs to be compiled before the first sign stands */
  warm(): Mesh[];
  /** The posts the camera can see (`InstanceRuns.cull`), every frame. */
  cull(frustum: Frustum, view: Matrix4): void;
  /** A WebGL context restored: the plates' sheets painted again (their pixels were only on the GPU). */
  refill(): void;
  /** The GPU copy a sheet already sent takes its new plates by (`SignSheets.useCopier`, VIA-04). */
  useCopier(copy: SheetCopier): void;
  dispose(): void;
}

// src/render/signLayer.ts:239-440 function createSignLayer
export function createSignLayer(): SignLayerKept;
