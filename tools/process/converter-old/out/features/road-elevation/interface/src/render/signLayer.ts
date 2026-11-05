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
