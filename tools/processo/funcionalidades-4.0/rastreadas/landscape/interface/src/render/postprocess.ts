// What the feature uses of src/render/postprocess.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/postprocess.ts:28-57 interface PostChain
/** The post chain: ambient occlusion, then anti-aliasing */


export interface PostChain {
  readonly enabled: boolean;
  /** What the scene is drawn into (null: the screen): shaders are compiled ahead for it. */
  readonly target: WebGLRenderTarget | null;
  /** Settles once the chain's own programs (its passes') are built: a quality change holds the last picture until then (`renderer.ts` compileChanged). */
  readonly ready: Promise<void>;
  /** Every render target the chain holds (the scene's, the composer's two, the masks, the clouds', the occlusion's), for the video memory count (`targetBytes.ts`). */
  targets(): readonly WebGLRenderTarget[];
  render(delta: number): void;
  setSize(width: number, height: number, pixelRatio: number): void;
  /** How dark it is, 0 by day to 1 at night: the bloom is for lights in the dark (lamps, headlights, lit windows). */
  setNight(dark: number): void;
  /** How much of the light on the ground comes straight from the sun (`SceneEnvironment.directShare`) */
  setDirectShare(share: number): void;
  /** The view's centre (map x, y), the town's solids and where they changed since the last call (null: anywhere) */
  followTown(cx: number, cy: number, solids: (() => CameraSolids) | null, changed: readonly (readonly [number, number, number, number])[] | null): void;
  /** The sky the player set (`Atmosphere`) and the light it is lit by */
  setAtmosphere(atmosphere: Atmosphere, sun: Vector3, skyColor: Color, sunLight: Color,
    /** How far in front of the camera the view's equivalent eye stands (`uEyeShift`): 0 in perspective. */
    eyeShift?: number): void;
  /** The clouds the player placed (`world/clouds.ts`). */
  setPlacedClouds(clouds: readonly PlacedCloud[]): void;
  /** How far the wind has carried the clouds, on the map (`world/clouds.ts` driftedCloud). */
  setCloudDrift(x: number, y: number): void;
  /** The painted fog (`render/fogLayer.ts`): its map, the slab of ground it lies over, and the map's settings - or none. */
  setGroundFog(fog: { texture: Texture; low: number; high: number; density: number; window: readonly [number, number, number] } | null): void;
  /** Passes switched off from the effects panel (`effectSwitches.ts`): skipped by the composer from the next frame. */
  setSwitches(switches: PostSwitches): void;
  dispose(): void;
}

// src/render/postprocess.ts:60-60 interface PostPassHook
/** Told round every composer pass drawn (the lab's per-pass GPU timing, `lab/gpu`): set on the scene's `userData[POST_PASS_HOOK]`; absent, the passes draw as ever. */
export interface PostPassHook { before(pass: string): void; after(pass: string): void }

// src/render/postprocess.ts:61-61 variable POST_PASS_HOOK
export const POST_PASS_HOOK = 'postPassHook';

// src/render/postprocess.ts:64-72 interface Atmosphere
/** The player's sky (Landscape > Sky and weather) */
export interface Atmosphere {
  /** How much of the sky the old setting covered (0..1). */
  readonly clouds: number;
  readonly cloudBase: number;
  readonly cloudThickness: number;
  /** 0 none (the default: the map seen clear), 1 a thick mist. */
  readonly fog: number;
  readonly fogHeight: number;
}

// src/render/postprocess.ts:75-567 function createPostChain
export function createPostChain(
  renderer: WebGLRenderer,
  scene: Scene,
  camera: Camera,
  quality: QualitySettings,
  level: QualityLevel,
): PostChain;

// src/render/postprocess.ts:620-620 variable MAX_CLOUDS
/** Most cumulus clouds over the map at once, and the puffs each is built of. */
const MAX_CLOUDS = 24;

// src/render/postprocess.ts:621-621 variable CLOUD_PUFFS
const CLOUD_PUFFS = 7;

// src/render/postprocess.ts:642-657 function layClouds
/** Lays the clouds out: the map's own, each where the player put it or the sky's spreader laid it */
function layClouds(bounds: Vector4[], puffs: Vector4[], lives: number[], bases: number[], shows: number[], placed: readonly PlacedCloud[], drift: { x: number; y: number },
  about: { x: number; y: number }): number;

// src/render/postprocess.ts:660-672 function layOne
/** One cloud into slot `i`: its bounding sphere, its puffs (jittered by `id`), how grown it is (`life`). */
function layOne(i: number, id: number, x: number, z: number, base: number, size: number, yaw: number, life: number, grown: number,
  bounds: Vector4[], puffs: Vector4[], lives: number[]): void;
