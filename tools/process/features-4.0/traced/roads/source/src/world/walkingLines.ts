// The code of src/world/walkingLines.ts that is the feature's (the rest of the file is shared).

// src/world/walkingLines.ts:81-83 slice of src/world/walkingLines.ts#LocalLines.polygons (shared): the feature's statements; "…" marks code left out
  private polygons(level: SurfaceLevel, deck: RoadStructure): MultiPoly {
    return union(this.ringsOf(level, deck));
  }

// src/world/walkingLines.ts:86-94 slice of src/world/walkingLines.ts#LocalLines.paving (shared): the feature's statements; "…" marks code left out
  paving(deck: RoadStructure): MultiPoly {
    // … 1 lines
    if (!known) {
      // The footway level alone: `surfaces` unions all four levels to hand back one.
      known = this.polygons(Level.Sidewalk, deck);
      this.pavings.set(deck, known);
    }
    return known;
  }

// src/world/walkingLines.ts:137-148 slice of src/world/walkingLines.ts#LocalLines.at (shared): the feature's statements; "…" marks code left out
  at(deck: RoadStructure, inset: number): Polyline[] {
    // … 3 lines
    const kerbs = this.polygons(Level.Curb, deck);
    // … 1 lines
    for (const poly of kerbs) for (const ring of poly) paths.push(ring.map(([x, y]) => ({ x: x!, y: y! })));
  // … 4 lines
  }

// src/world/walkingLines.ts:282-282 slice of src/world/walkingLines.ts#RingsOf (shared): the feature's statements; "…" marks code left out
export type RingsOf = (level: SurfaceLevel, deck: RoadStructure) => MultiPoly;

// src/world/walkingLines.ts:375-378 slice of src/world/walkingLines.ts#CornerAsk (shared): the feature's statements; "…" marks code left out
export interface CornerAsk {
  readonly rings: readonly { readonly level: SurfaceLevel; readonly deck: RoadStructure; readonly polys: readonly (readonly Float64Array[])[] }[];
// … 1 lines
}
