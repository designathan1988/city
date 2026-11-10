// What the feature uses of src/render/gpuLedger.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpuLedger.ts:12-67 class GpuLedger
export class GpuLedger { … }

// src/render/gpuLedger.ts:70-70 variable gpuLedger
/** The game's one book of video memory. */
export const gpuLedger = new GpuLedger();

// src/render/gpuLedger.ts:88-92 interface GpuDevice
/** What the cap is worked out from: the system's memory (GB, null unknown), the GPU's name, the drawing buffer's pixels. */
export interface GpuDevice {
  readonly memoryGB: number | null;
  readonly renderer: string;
  readonly pixels: number;
}

// src/render/gpuLedger.ts:117-123 function deviceGpuCap
/** cap = max(512 MiB, min(device ceiling, 690 B x drawing-buffer pixels)); ceiling = system GB x 0.5 x 0.5 on an iGPU (Intel 000020962), 4 GB x 0.75 on a discrete card; per-pixel budget from MDN WebGL best practices. */
export function deviceGpuCap(device: GpuDevice): number;

// src/render/gpuLedger.ts:126-132 function readGpuDevice
/** The device as the page can read it: `navigator.deviceMemory`, the unmasked renderer string (`WEBGL_debug_renderer_info`; Firefox gives it as `RENDERER`). */
export function readGpuDevice(gl: WebGLRenderingContext | WebGL2RenderingContext, pixels: number, weak: WeakMachine = WEAK_MACHINE): GpuDevice;
