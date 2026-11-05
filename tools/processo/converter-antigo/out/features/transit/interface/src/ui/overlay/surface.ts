// What the feature uses of src/ui/overlay/surface.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/overlay/surface.ts:5-67 class CanvasSurface
/** DO NOT ask the canvas for a context nor size its backing store to the screen (UI-11): nothing draws on it (the pointer's target and the screen's measure), and its opaque, desynchronized 2D store of the whole screen was memory and a compositor layer for nothing. */
export class CanvasSurface { … }
