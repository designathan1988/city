// What the feature uses of src/render/memoryCounters.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/memoryCounters.ts:4-23 interface MemoryCounters
/** What the scene holds in memory, counted from the scene itself, in bytes */
export interface MemoryCounters {
  /** Distinct vertex and index buffers in the scene (an interleaved buffer once). */
  readonly buffers: number;
  /** Size of those buffers: what the GPU holds for them once drawn. */
  readonly geometryBytes: number;
  /** The part still held by a JavaScript array (not let go after upload, `mesh/gpuOnly.ts`). */
  readonly geometryCpuBytes: number;
  /** Buffers whose array was let go with no size kept (`mesh/roadResidency.ts` gives a far block's back and disposes it): not counted as held anywhere. */
  readonly releasedBuffers: number;
  /** Who holds `geometryCpuBytes`: the objects' names (the type when unnamed), most first, the first `TOP` of them. */
  readonly cpuByName: readonly (readonly [name: string, bytes: number])[];
  /** `geometryBytes` by every holder's name, for the plan's levels (`core/perfBudget.ts` `memoryGroup`). */
  readonly bytesByName: ReadonlyMap<string, number>;
  /** Distinct textures reachable from the scene's objects and materials, and the uniforms the renderer compiled them with. */
  readonly textures: number;
  /** Their bytes on the GPU, every level of the mip chain: a data texture's array (what is uploaded), a compressed chain's levels, an image at 4 bytes a texel (uploaded as RGBA8). */
  readonly textureBytes: number;
  /** `textureBytes` by holder: the first object reaching each texture and the texture's own name (`holder :: texture`), digits kept. */
  readonly textureBytesByName: ReadonlyMap<string, number>;
}

// src/render/memoryCounters.ts:27-30 interface CountOptions
export interface CountOptions {
  /** The uniforms a material was compiled with (`renderer.properties.get(material).uniforms`): the textures `onBeforeCompile` puts there are in no field of the material (`uLots`, `uDetailMap`: report TST-09). */
  readonly uniformsOf?: (material: object) => Record<string, { value: unknown }> | undefined;
}

// src/render/memoryCounters.ts:33-115 function countMemory
/** Counts every object under `root`, shown or not: a hidden mesh holds its memory all the same. */
export function countMemory(root: Object3D, options: CountOptions = {}): MemoryCounters;
