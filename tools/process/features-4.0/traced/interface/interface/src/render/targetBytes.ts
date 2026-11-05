// What the feature uses of src/render/targetBytes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/targetBytes.ts:15-34 function renderTargetBytes
/** The bytes a render target holds: its textures, their multisampled buffers, and its depth. */
export function renderTargetBytes(target: Pick<RenderTarget, 'width' | 'height' | 'samples' | 'textures' | 'depthBuffer' | 'depthTexture'>): number;

// src/render/targetBytes.ts:37-46 function renderTargetsBytes
/** The total of several targets, a target shared counted once; null entries are skipped. */
export function renderTargetsBytes(targets: Iterable<Parameters<typeof renderTargetBytes>[0] | null | undefined>): number;
