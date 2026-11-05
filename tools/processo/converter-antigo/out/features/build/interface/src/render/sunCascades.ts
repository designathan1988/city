// What the feature uses of src/render/sunCascades.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/sunCascades.ts:94-103 interface Movers
/** What moves and is drawn into the shadows: when it moves, every cascade it is drawn into is drawn again. */
export interface Movers {
  readonly roots: readonly Object3D[];
  moving(): boolean;
  /** Its roots never change size (what the wind sways in place): each one's reach measured once, not every frame. */
  readonly fixedSize?: boolean;
  /** The last cascade it is drawn into (cascades are 0 near to the farthest): the people and the vehicles stop at the mid one, where their shadows are under a texel's spread (the far cascades redraw a million of their triangles a frame). */
  readonly maxCascade?: number;
  /** Past `maxCascade` it casts as it stands, kept in the store with the still casters (what the wind sways: a far texel blurs the swing, never the tree), not left out. */
  readonly keptPast?: boolean;
}

// src/render/sunCascades.ts:105-121 interface ShadowPassInput
export interface ShadowPassInput {
  /** The world under the shadows changed (buildings streamed in, an edit): every cascade. */
  readonly changed: boolean;
  /** Effects in play (`playEffects.ts`): their pieces are in every cascade. */
  readonly effects: boolean;
  /** Nothing that casts and stands still changed since the last pass, as the caller knows it (the view, what landed, came or went): a cascade's store may stand for its town. Absent: it may not. */
  readonly still?: boolean;
  /** The view stands where it stood (the camera unmoved): a cascade drawn whole now is kept as its store though its casters changed, the next frames standing on it. Absent: only a still pass keeps its store. */
  readonly keep?: boolean;
  /** With `still` false: the world boxes holding every still caster that changed since the last pass (`renderer.ts` casterChanges, a few apart from each other) - each cascade's store is drawn again under them alone (`patched`), and stands untouched where they fall outside its square; null: a change with no box to tell, the stores thrown away as before. Absent: the stores thrown away. */
  readonly patch?: readonly Box3[] | null | undefined;
  /** The town is shown (the curtain is down, `renderer.ts` reveal): a cascade drawn whole is kept as its store whatever the view does, the
   *  frames after it scrolling and patching it. Absent: kept only as above (behind the curtain the town is hidden, and no store of it stands). */
  readonly opened?: boolean;
  /** The camera's frustum: what changed under the near cascade where no shadow of it reaches the view is patched later (`deferredNear`). Absent: every change patched at once. */
  readonly view?: Frustum | null;
}

// src/render/sunCascades.ts:123-155 interface SunCascades
export interface SunCascades {
  /** Cascade 0 is the sun the rest of the game reads (its colour, intensity and direction). */
  readonly lights: readonly DirectionalLight[];
  /** Towards the sun the cascades are cast by (unit; zero before the first `follow`): the held sun itself, never a light's position less its target (those move with every fit, and their difference by the last bits). */
  readonly toSun: Vector3;
  setMovers(movers: readonly Movers[]): void;
  /** What chooses its casters by size, told each cascade's texel and half side (m) before that cascade is drawn (a piece narrower than it is a smudge PCF spreads away), and null once the passes are done. */
  setCasters(casters: ((texel: number | null, cascade: number, half: number) => void) | null): void;
  /** Fits the cascades to what the camera sees of the ground and what stands on it */
  follow(camera: Camera, sunDirection: Vector3, groundAt: (x: number, y: number) => number, target: Vector3, top: number): boolean;
  /** Marks the cascades that must be drawn this frame; true when any is (the renderer's `shadowMap.needsUpdate`). */
  pass(input: ShadowPassInput): boolean;
  /** How the passes went since the last call (counts, then zeroed): each cascade's whole draws and draws over its store, and why a whole draw was asked (`scripts/layers.mjs`). */
  takeStats(): { readonly passes: number; readonly whole: readonly number[]; readonly over: readonly number[]; readonly why: Readonly<Record<string, number>> };
  /** `mapSize`: one size for every cascade, or a size for each: the near cascade carries the shadow detail, the far ones are filtered over a texel's spread (a 2048 far map held 12.6 M texels for 6.2 M pixels, 2026-10-20). */
  setEnabled(on: boolean, mapSize: number | readonly number[]): void;
  /** The far cascades (1 and on) drawn at most every `n` passes, what changed meanwhile kept for their turn (the lab's optimisations, `optimizationsApply.ts`); 1: every pass, as the game draws. */
  setFarEvery(n: number): void;
  /** The world box (three's axes) round the near cascades' squares (0 to `KEEP_CASCADES` - 1) through their depth ranges, grown by `margin`:
   *  what the layers keep their casters for whatever the camera frames (`cellInstances.ts` keep); false when none is placed. */
  coverBox(target: Box3, margin: number): boolean;
  /** The near cascade's texel, world units (0 before it is placed): what a caster under its cover must span to cast there (`castBySize`). */
  nearTexel(): number;
  /** The maps made again, for the renderer's shadow map type changed (`shadowMap.type`: PCF compares its depth, Basic reads it): every cascade drawn whole next. */
  remakeMaps(): void;
  /** Copies the sun's colour and intensity to the other cascades. */
  syncLight(): void;
  /** Every cascade's map, for the video memory count. */
  maps(): (WebGLRenderTarget | null)[];
  /** Puts back what a cascade's pass hid (`Scene.onAfterRender`). */
  restore(): void;
  dispose(): void;
}
