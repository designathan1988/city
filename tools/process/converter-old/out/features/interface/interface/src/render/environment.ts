// What the feature uses of src/render/environment.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/environment.ts:30-34 interface EnvironmentQuality
export interface EnvironmentQuality {
  /** Side of each of the sun's cascade maps (`sunCascades.ts`). */
  readonly shadowMapSize: number;
  readonly shadows: boolean;
}

// src/render/environment.ts:36-72 interface SceneEnvironment
export interface SceneEnvironment {
  /** The sun as the game reads it (colour, intensity, direction): the first of its cascades. */
  readonly sun: DirectionalLight;
  /** Towards the sun the shadows are cast by (`SunCascades.toSun`). */
  readonly toSun: Vector3;
  readonly skyColor: Color;
  /** Fits the sun's cascades to what the camera sees (`sunCascades.ts`). */
  follow(camera: Camera, target: Vector3, top: number, groundAt: (x: number, y: number) => number): void;
  /** Marks the cascades to draw this frame; true when any is (the renderer's `shadowMap.needsUpdate`). */
  shadowPass(input: ShadowPassInput): boolean;
  /** The cascades' passes since the last call (`SunCascades.takeStats`). */
  shadowStats(): ReturnType<SunCascades['takeStats']>;
  /** What moves and is drawn into the shadows (`sunCascades.ts` `Movers`). */
  setMovers(movers: readonly Movers[]): void;
  /** What chooses its casters by each cascade's texel (`SunCascades.setCasters`). */
  setCasters(casters: ((texel: number | null, cascade: number, half: number) => void) | null): void;
  /** The cascades' maps, for the video memory count. */
  shadowMaps(): (WebGLRenderTarget | null)[];
  setQuality(quality: EnvironmentQuality): void;
  /** The far cascades drawn at most every `n` passes (`SunCascades.setFarEvery`). */
  setFarCascadeEvery(n: number): void;
  /** The world box the sun's cascades cover, grown by `margin` (`SunCascades.coverBox`): false without shadows. */
  shadowCover(target: Box3, margin: number): boolean;
  /** The near cascade's texel, world units (`SunCascades.nearTexel`). */
  shadowNearTexel(): number;
  /** The cascades' maps made again for the renderer's shadow map type (`SunCascades.remakeMaps`). */
  remakeShadowMaps(): void;
  /** The time of day, minutes after midnight: the sun's path, the sky, the light; the moon at night. */
  setTimeOfDay(minutes: number): number;
  /** Smoke in the air, 0..1: the fog closes in and browns, the light dims. */
  setSmog(k: number): void;
  /** The weather on the light (`world/weather.ts`): how overcast */
  setWeather(overcast: number, flash: number): void;
  /** How much of the light on level ground comes straight from the sun, 0..1 */
  directShare(): number;
  dispose(): void;
}

// src/render/environment.ts:118-315 function createEnvironment
export function createEnvironment(
  scene: Scene,
  renderer: WebGLRenderer,
  quality: EnvironmentQuality,
): SceneEnvironment;
