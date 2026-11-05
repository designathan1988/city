// The code of src/render/roadStandIn.ts that is the feature's (the rest of the file is shared).

// src/render/roadStandIn.ts:77-108 slice of src/render/roadStandIn.ts#RoadStandIn.show (shared): the feature's statements; "…" marks code left out
  show(net: Network, heights: StandInHeights, segments: Iterable<SegmentId>, revision: number): void {
    // … 15 lines
    for (const id of nodes) {
      // … 5 lines
      const casing = levels.ringAt(Level.Casing), asphalt = levels.ringAt(Level.Asphalt);
      if (casing) fill(casing.flatten(), height, casingParts);
      if (asphalt) fill(asphalt.flatten(), height, asphaltParts);
    }
  // … 5 lines
  }
