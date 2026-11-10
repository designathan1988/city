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
