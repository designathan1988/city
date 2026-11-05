// What the feature uses of src/render/optimizationsQuality.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/optimizationsQuality.ts:6-6 type Field
/** EVERY FIELD OF THE TIER'S SETTINGS (`QualitySettings`, `quality.ts`) set by hand from the lab (`optimizations.ts`, ids `q.<field>`): "tier" (the tier's own, the default) or a value of its own, over the tier and over the other optimisations. A field nothing reads today says so: setting it changes nothing. */

type Field = keyof QualitySettings;

// src/render/optimizationsQuality.ts:54-54 type QualityOptId
export type QualityOptId = `q.${Field}`;
