// What the feature uses of src/lab/catalog/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/city.ts:379-407 function drawCallsByGroup
/** The scene's draw calls with every group on, and how many each layer and each part of the buildings takes (the frame with it off, subtracted). */
export async function drawCallsByGroup(): Promise<{ total: number; triangles: number; groups: Record<string, number> }>;
