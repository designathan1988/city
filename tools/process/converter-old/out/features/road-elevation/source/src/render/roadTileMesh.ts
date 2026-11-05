// The code of src/render/roadTileMesh.ts that is the feature's (the rest of the file is shared).

// src/render/roadTileMesh.ts:14-25 slice of src/render/roadTileMesh.ts#SurfaceData (shared): the feature's statements; "…" marks code left out
export interface SurfaceData {
  // … 1 lines
  readonly field: FieldData;
// … 8 lines
}

// src/render/roadTileMesh.ts:38-74 slice of src/render/roadTileMesh.ts#readsOf (shared): the feature's statements; "…" marks code left out
function readsOf(data: SurfaceData, heights: PointHeights, missing: number[]): SurfaceReads {
  // … 6 lines
  const field = fieldOf(data.field, terrainAt);
  // … 1 lines
  {
    // … 1 lines
    for (let i = 0; i < data.field.ids.length; i++) {
      lines.set(data.field.ids[i]! as SegmentId, Polyline.fromFlat(data.field.lines.subarray(at, data.field.lineEnds[i]!)));
      at = data.field.lineEnds[i]!;
    }
  }
  // … 11 lines
  return {
    field,
  // … 6 lines
  };
}

// src/render/roadTileMesh.ts:39-44 slice of src/render/roadTileMesh.ts#readsOf.terrainAt (shared): the feature's statements; "…" marks code left out
  const terrainAt = (x: number, y: number): number => {
    const h = heights.get(x, y);
    if (h !== undefined) return h;
  // … 2 lines
  };

// src/render/roadTileMesh.ts:78-78 slice of src/render/roadTileMesh.ts#TileMesher.heights (shared): the feature's statements; "…" marks code left out
  private readonly heights = new PointHeights();

// src/render/roadTileMesh.ts:87-93 slice of src/render/roadTileMesh.ts#TileMesher.constructor (shared): the feature's statements; "…" marks code left out
  constructor(private readonly ask: MeshAsk) {
    this.reads = readsOf(ask.data, this.heights, this.missing);
  // … 4 lines
  }

// src/render/roadTileMesh.ts:96-112 slice of src/render/roadTileMesh.ts#TileMesher.need (shared): the feature's statements; "…" marks code left out
  need(): Float64Array {
    const seen = new PointHeights();
  // … 14 lines
  }

// src/render/roadTileMesh.ts:99-99 slice of src/render/roadTileMesh.ts#TileMesher.need.take (shared): the feature's statements; "…" marks code left out
    const take = (x: number, y: number): void => { if (!seen.add(x, y, 0)) out.push(x, y); };

// src/render/roadTileMesh.ts:115-121 slice of src/render/roadTileMesh.ts#TileMesher.give (shared): the feature's statements; "…" marks code left out
  give(points: Float64Array, heights: Float64Array): void {
    for (let i = 0; i < heights.length; i++) this.heights.add(points[i * 2]!, points[i * 2 + 1]!, heights[i]!);
  // … 4 lines
  }

// src/render/roadTileMesh.ts:124-127 slice of src/render/roadTileMesh.ts#TileMesher.renew (shared): the feature's statements; "…" marks code left out
  private renew(): void {
    this.reads = readsOf(this.ask.data, this.heights, this.missing);
  // … 1 lines
  }

// src/render/roadTileMesh.ts:130-158 slice of src/render/roadTileMesh.ts#TileMesher.run (shared): the feature's statements; "…" marks code left out
  run(beat?: () => void): { tiles: TileMeshes[] } | { need: Float64Array } {
    // … 1 lines
    for (let k = 0; k < this.shapes.length; k++) {
      // … 5 lines
      this.reads.field.remembering!(() => {
        for (const spec of this.specs) {
          const shape = shapes.get(spec.options.name);
          if (shape) tile.push([spec.options.name, meshTileShape(shape, spec.options)]);
        }
      });
    // … 3 lines
    }
    if (this.missing.length > 0) {
      const seen = new PointHeights();
      // … 1 lines
      for (let i = 0; i < this.missing.length; i += 2) {
        // … 1 lines
        if (!seen.add(x, y, 0)) out.push(x, y);
      }
    // … 1 lines
    }
  // … 1 lines
  }
