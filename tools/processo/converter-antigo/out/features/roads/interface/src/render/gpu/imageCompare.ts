// What the feature uses of src/render/gpu/imageCompare.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpu/imageCompare.ts:49-60 interface PictureDifference
export interface PictureDifference {
  /** The mean, over every pixel, of its largest channel difference, in 8-bit steps (0..255). */
  mean: number;
  /** The share of pixels, %, whose largest channel difference is over `threshold`. */
  over: number;
  /** The threshold, 8-bit steps. */
  threshold: number;
  /** The largest difference anywhere, 8-bit steps. */
  max: number;
  /** The difference as a picture: black where equal, brighter red the larger it is (x4), white where over the threshold's 8 times. */
  image: Uint8ClampedArray;
}
