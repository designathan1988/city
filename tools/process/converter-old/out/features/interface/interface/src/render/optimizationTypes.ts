// What the feature uses of src/render/optimizationTypes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/optimizationTypes.ts:3-3 type OptValue
/** THE OPTIMISATIONS' SHARED WORDS (`optimizations.ts`, `optimizationsQuality.ts`): a value, a system, a choice. */

export type OptValue = string | number;

// src/render/optimizationTypes.ts:5-6 type OptSystem
export type OptSystem = 'renderer' | 'culling' | 'shadows' | 'post' | 'textures' | 'materials' | 'buildings' | 'terrain' | 'water' | 'roads'
  | 'vegetation' | 'people' | 'vehicles' | 'particles' | 'simulation' | 'uploads';

// src/render/optimizationTypes.ts:8-25 variable OPT_SYSTEM_TITLES
export const OPT_SYSTEM_TITLES: Readonly<Record<OptSystem, string>> = {
  renderer: 'Renderer',
  culling: 'Culling',
  shadows: 'Shadows',
  post: 'Post-processing',
  textures: 'Textures',
  materials: 'Materials and shaders',
  buildings: 'Buildings',
  terrain: 'Terrain',
  water: 'Water',
  roads: 'Roads',
  vegetation: 'Vegetation and street furniture',
  people: 'People',
  vehicles: 'Vehicles',
  particles: 'Particles',
  simulation: 'Simulation (CPU)',
  uploads: 'GPU uploads and pacing',
};

// src/render/optimizationTypes.ts:27-30 interface OptChoice
export interface OptChoice {
  readonly value: OptValue;
  readonly label: string;
}
