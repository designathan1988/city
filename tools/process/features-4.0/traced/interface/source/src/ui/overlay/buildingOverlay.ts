// The code of src/ui/overlay/buildingOverlay.ts that is the feature's (the rest of the file is shared).

// src/ui/overlay/buildingOverlay.ts:44-225 slice of src/ui/overlay/buildingOverlay.ts#drawBuildingOverlay (shared): the feature's statements; "…" marks code left out
export function drawBuildingOverlay(ctx: CanvasRenderingContext2D, input: BuildingOverlayInput): void {
  // … 52 lines
  if (sel) {
    // … 23 lines
    if (v) {
      // … 4 lines
      if (input.stage === 'shape' && volumeSides(v).length <= 8) {
        for (const side of volumeSides(v)) {
          // … 7 lines
          const label = `${formatDecimal(f.length * METERS_PER_UNIT, 1)} m`;
          // … 1 lines
          const width = ctx.measureText(label).width + 10;
          // … 2 lines
          ctx.roundRect(at.x - width / 2, at.y - 9, width, 18, 5);
          // … 4 lines
          ctx.fillText(label, at.x, at.y);
        }
      }
    // … 28 lines
    }
  }
// … 47 lines
}
