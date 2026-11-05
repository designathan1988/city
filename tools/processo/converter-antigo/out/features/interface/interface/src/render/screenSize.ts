// What the feature uses of src/render/screenSize.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/screenSize.ts:4-8 interface DrawingBuffer
/** The renderer's drawing buffer as `screenSizer` sizes it (three's `WebGLRenderer`). */
export interface DrawingBuffer {
  getSize(target: Vector2): Vector2;
  getPixelRatio(): number;
  setDrawingBufferSize(width: number, height: number, pixelRatio: number): void;
}

// src/render/screenSize.ts:17-33 function screenSizer
/**
 * The screen sized for the canvas's CSS pixels and a pixel ratio: the drawing buffer made once (three's
 * `setPixelRatio` and `setSize` each make one), and none, nor the screen's targets (`fit`), when what it was
 * sized for has not changed. `chain`: what the targets belong to (the post chain): a new one is fitted even at
 * the same size. `fit` is also given the pixels at the whole ratio, for what must not follow the dynamic
 * resolution's share (the GPU's cap, `gpuLedger.ts`).
 */
export function screenSizer(gl: DrawingBuffer, whole: () => number,
  fit: (width: number, height: number, ratio: number, wholePixels: number) => void): (width: number, height: number, ratio: number, chain: object) => void;
