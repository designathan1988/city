// The code of src/world/walkingLines.ts that is the feature's (the rest of the file is shared).

// src/world/walkingLines.ts:81-83 slice of src/world/walkingLines.ts#LocalLines.polygons (shared): the feature's statements; "…" marks code left out
  private polygons(level: SurfaceLevel, deck: RoadStructure): MultiPoly {
    return union(this.ringsOf(level, deck));
  }

// src/world/walkingLines.ts:86-94 slice of src/world/walkingLines.ts#LocalLines.paving (shared): the feature's statements; "…" marks code left out
  paving(deck: RoadStructure): MultiPoly {
    let known = this.pavings.get(deck);
    if (!known) {
      // The footway level alone: `surfaces` unions all four levels to hand back one.
      known = this.polygons(Level.Sidewalk, deck);
      this.pavings.set(deck, known);
    }
    return known;
  }

// src/world/walkingLines.ts:100-115 slice of src/world/walkingLines.ts#LocalLines.onPaving (shared): the feature's statements; "…" marks code left out
  onPaving(deck: RoadStructure, p: Vec2): boolean {
    let polys = this.pavingRings.get(deck);
    if (!polys) {
      polys = this.paving(deck).map(([outerRing, ...holes]) => ({
        outer: outerRing ? pavingRing(outerRing) : null,
        holes: holes.map(pavingRing),
      }));
      this.pavingRings.set(deck, polys);
    }
    for (const poly of polys) {
      if (!poly.outer || !inRing(p, poly.outer)) continue;
      if (poly.holes.some((h) => inRing(p, h))) continue;
      return true;
    }
  // … 1 lines
  }

// src/world/walkingLines.ts:119-123 slice of src/world/walkingLines.ts#LocalLines.nearestOuter (shared): the feature's statements; "…" marks code left out
  nearestOuter(deck: RoadStructure, p: Vec2): { point: Vec2; distance: number } | null {
    let index = this.outerIndex.get(deck);
    if (!index) this.outerIndex.set(deck, index = new NearestEdge(this.outer(deck)));
    return index.nearest(p);
  }

// src/world/walkingLines.ts:126-135 slice of src/world/walkingLines.ts#LocalLines.outer (shared): the feature's statements; "…" marks code left out
  outer(deck: RoadStructure): Polyline[] {
    const known = this.edges.get(deck);
    if (known) return known;
    // … 1 lines
    for (const poly of this.paving(deck)) for (const ring of poly) {
      if (ring.length >= 3) lines.push(Polyline.fromPoints([...ring.map(([x, y]) => ({ x: x!, y: y! })), { x: ring[0]![0]!, y: ring[0]![1]! }]));
    }
    this.edges.set(deck, lines);
  // … 1 lines
  }

// src/world/walkingLines.ts:137-148 slice of src/world/walkingLines.ts#LocalLines.at (shared): the feature's statements; "…" marks code left out
  at(deck: RoadStructure, inset: number): Polyline[] {
    const key = `${deck}:${inset.toFixed(4)}`;
    const known = this.cache.get(key);
    if (known) return known;
    const kerbs = this.polygons(Level.Curb, deck);
    // … 1 lines
    for (const poly of kerbs) for (const ring of poly) paths.push(ring.map(([x, y]) => ({ x: x!, y: y! })));
    // … 2 lines
    this.cache.set(key, lines);
  // … 1 lines
  }

// src/world/walkingLines.ts:282-282 slice of src/world/walkingLines.ts#RingsOf (shared): the feature's statements; "…" marks code left out
export type RingsOf = (level: SurfaceLevel, deck: RoadStructure) => MultiPoly;

// src/world/walkingLines.ts:285-291 slice of src/world/walkingLines.ts#CornerEnd (shared): the feature's statements; "…" marks code left out
export interface CornerEnd {
  // … 4 lines
  readonly structure: RoadStructure;
}

// src/world/walkingLines.ts:375-378 slice of src/world/walkingLines.ts#CornerAsk (shared): the feature's statements; "…" marks code left out
export interface CornerAsk {
  readonly rings: readonly { readonly level: SurfaceLevel; readonly deck: RoadStructure; readonly polys: readonly (readonly Float64Array[])[] }[];
// … 1 lines
}
