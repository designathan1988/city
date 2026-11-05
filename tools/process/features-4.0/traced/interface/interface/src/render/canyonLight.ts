// What the feature uses of src/render/canyonLight.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/canyonLight.ts:37-46 interface CanyonLight
export interface CanyonLight {
  readonly pass: ShaderPass;
  /** @rebuild The view's centre (map x, y), the town's solids, and where they changed since the last call */
  follow(cx: number, cy: number, solids: (() => CameraSolids) | null, changed: readonly (readonly [number, number, number, number])[] | null): void;
  /** The ambient occlusion multiplied into the picture first (GTAO's denoised target, its blend intensity), or none. */
  setOcclusion(texture: Texture | null, intensity: number): void;
  /** The sun's direction (world, to the sun) and its share of the light on the ground. */
  setSun(direction: Vector3, directShare: number): void;
  dispose(): void;
}

// src/render/canyonLight.ts:198-378 function createCanyonLight
/** `sunsOf`: the sun's cascades, nearest first (`sunCascades.ts`); the two nearest are read. */
export function createCanyonLight(camera: Camera, depthOf: () => Texture | null, sunsOf: () => readonly DirectionalLight[] = () => []): CanyonLight;
