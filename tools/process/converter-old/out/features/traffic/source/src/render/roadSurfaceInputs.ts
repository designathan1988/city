// The code of src/render/roadSurfaceInputs.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaceInputs.ts:93-139 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.refresh (shared): the feature's statements; "…" marks code left out
  refresh(net: Network): void {
    // … 31 lines
    if (near.length) {
      for (const id of net.segmentCells.idsMeetingAny(near)) {
        // … 1 lines
        if (ribbon && parksAlong(net, ribbon)) segs.add(id);
      }
    }
  // … 8 lines
  }

// src/render/roadSurfaceInputs.ts:400-448 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.placeSegment (shared): the feature's statements; "…" marks code left out
  private placeSegment(net: Network, id: SegmentId, silent = false): void {
    // … 15 lines
    if (ribbon) {
      // … 19 lines
      const gates = this.gates && parksAlong(net, ribbon) ? this.gates(ribbon.full, ribbon.road.width / 2) : null;
      for (const line of parkingLinesOf(net, ribbon, gates)) {
        strokeQuads(parking, { points: line.points, width: line.width, color: line.color, dash: line.dash, dashOffset: 0 });
      }
    }
  // … 7 lines
  }
