// What the feature uses of src/render/optimizationsApply.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/optimizationsApply.ts:39-45 interface OptimizerDeps
export interface OptimizerDeps {
  readonly renderer: WebGLRenderer;
  readonly scene: Scene;
  readonly environment: SceneEnvironment;
  /** The rivers and lakes of the plates shown. */
  waterMeshes(): readonly Object3D[];
}

// src/render/optimizationsApply.ts:47-76 interface Optimizer
export interface Optimizer {
  readonly values: Optimizations;
  /** The options taken: whether the lit programs must be built again, and the shadows drawn again. */
  set(next: Optimizations): { readonly programs: boolean; readonly shadows: boolean };
  /** The pixels a metre spans, as the buildings' levels are chosen by (`buildingLod`). */
  readonly buildingPx: number;
  /** The screen's height the terrain's levels are chosen by, times this (`terrainLod`). */
  readonly terrainPixels: number;
  /** A shadow texel, as the buildings' parts are cast by (`castTexel`). */
  readonly castTexel: number;
  /** The first cascade the buildings cast into by their masses alone (`castMasses`). */
  readonly coarseFrom: number;
  readonly agentShadows: boolean;
  readonly focusDetail: boolean;
  readonly exhaust: boolean;
  readonly wear: boolean;
  /** Whether this frame does it (`shadowEvery`, `reliefEvery`, `uploadEvery`): each asked once a frame, its own count. */
  shadowDue(): boolean;
  reliefDue(): boolean;
  uploadsDue(): boolean;
  /** The post chain's switches as the optimisations leave them: the street light off with Basic shadows (`canyonLight.ts` reads the cascades through a `sampler2DShadow`, which a Basic map, not compared, cannot be read by: GL_INVALID_OPERATION). The same object otherwise. */
  postSwitches(switches: PostSwitches): PostSwitches;
  /** Before the frame's drawing: the water's material swapped. */
  beforeRender(): void;
  /** After it: the water given back; every `REWALK_FRAMES`, what came in given the options. True when programs must be built (a material's precision changed). */
  afterRender(): boolean;
  /** What the options hold now (the lab's report). */
  report(): Readonly<Record<string, number>>;
  dispose(): void;
}

// src/render/optimizationsApply.ts:78-262 function createOptimizer
export function createOptimizer(deps: OptimizerDeps): Optimizer;
