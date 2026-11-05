// What the feature uses of src/render/effectSwitches.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/effectSwitches.ts:6-6 variable POST_SWITCHES
/** THE EFFECT SWITCHES: every effect and layer of the drawing, each turned off to measure what it costs the GPU (the effects panel, `effectsWiring.ts`). */

/** The post chain's passes (`postprocess.ts`): skipped by the composer when off (three's EffectComposer, `pass.enabled === false`). */
export const POST_SWITCHES = ['ambientOcclusion', 'canyonLight', 'bloom', 'clouds', 'smaa', 'sharpen'] as const;

// src/render/effectSwitches.ts:8-8 variable LIGHT_SWITCHES
/** The light: shadows, the sky's reflection and each light. */
export const LIGHT_SWITCHES = ['shadows', 'shadowUpdate', 'environmentMap', 'skyFill', 'ambient', 'fillLight', 'pointLights', 'surfaceDetail'] as const;

// src/render/effectSwitches.ts:10-10 variable LAYER_SWITCHES
/** What is drawn, layer by layer: hidden only while the frame is drawn (what the game sets each frame stands). */
export const LAYER_SWITCHES = ['terrain', 'water', 'roads', 'buildings', 'vegetation', 'furniture', 'props', 'transit', 'people', 'farTraffic', 'signals', 'particles', 'weather'] as const;

// src/render/effectSwitches.ts:13-14 variable BUILDING_SWITCHES
/** The buildings, part by part and piece by piece (`buildingSwitches.ts`), their shadows and their lit windows. */
export const BUILDING_SWITCHES = ['bMasses', 'bFacadeLevel', 'bDetail', 'bParts', 'bSignatures', 'bFlags',
  'bShell', 'bGlass', 'bFrames', 'bFacadeDetails', 'bProps', 'bLots', 'bFurniture', 'bShadows', 'bNightWindows'] as const;

// src/render/effectSwitches.ts:16-16 type PostSwitch
export type PostSwitch = (typeof POST_SWITCHES)[number];

// src/render/effectSwitches.ts:17-17 type LightSwitch
export type LightSwitch = (typeof LIGHT_SWITCHES)[number];

// src/render/effectSwitches.ts:18-18 type LayerSwitch
export type LayerSwitch = (typeof LAYER_SWITCHES)[number];

// src/render/effectSwitches.ts:19-19 type BuildingSwitch
export type BuildingSwitch = (typeof BUILDING_SWITCHES)[number];

// src/render/effectSwitches.ts:20-20 type EffectSwitch
export type EffectSwitch = PostSwitch | LightSwitch | LayerSwitch | BuildingSwitch;

// src/render/effectSwitches.ts:21-21 type EffectSwitches
export type EffectSwitches = Readonly<Record<EffectSwitch, boolean>>;

// src/render/effectSwitches.ts:24-26 variable ALL_EFFECTS_ON
export const ALL_EFFECTS_ON: EffectSwitches = Object.freeze(Object.fromEntries(
  [...POST_SWITCHES, ...LIGHT_SWITCHES, ...LAYER_SWITCHES, ...BUILDING_SWITCHES].map((key) => [key, true]),
) as Record<EffectSwitch, boolean>);

// src/render/effectSwitches.ts:29-36 interface BuildingChoices
/** The buildings' settings that are not on or off. */
export interface BuildingChoices {
  /** Their own materials, one plain lit grey (Lambert), or one unlit (Basic): what their shading costs. */
  readonly material: 'normal' | 'lambert' | 'basic';
  /** The share of the view's budget of triangles their levels of detail fit (`frameBudget.ts`); 0: no budget at all. */
  readonly triangles: number;
  /** The most their cells' detail may hold on the GPU, MB (`BuildingLayer.setDetailBudget`); -1: as the game sets it. */
  readonly detailMemoryMB: number;
}

// src/render/effectSwitches.ts:37-37 variable DEFAULT_BUILDING_CHOICES
export const DEFAULT_BUILDING_CHOICES: BuildingChoices = Object.freeze({ material: 'normal', triangles: 1, detailMemoryMB: -1 });
