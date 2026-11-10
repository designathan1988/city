// What the feature uses of src/lab/inspector/raycast.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/inspector/raycast.ts:14-19 function rayAtPixel
/** The ray through a canvas pixel (CSS px from the canvas's corner), in three's world (x, height, -y); null where the view has no line of sight. */
export function rayAtPixel(ctx: LabContext, px: number, py: number): Ray | null;
