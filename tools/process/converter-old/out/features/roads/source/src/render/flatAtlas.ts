// The code of src/render/flatAtlas.ts that is the feature's (the rest of the file is shared).

// src/render/flatAtlas.ts:63-63 slice of src/render/flatAtlas.ts#FAR_RING (shared): the feature's statements; "…" marks code left out
const FAR_RING = Math.round(MAP_HALF / PLATE_SIZE - 0.5);

// src/render/flatAtlas.ts:231-960 slice of src/render/flatAtlas.ts#createFlatAtlas (shared): the feature's statements; "…" marks code left out
export function createFlatAtlas(anisotropy: number): TerrainSurface {
  // … 487 lines
  atlas = {
    // … 5 lines
    focus(x, y, reach) {
      // … 10 lines
      if (key !== focusKey) {
        // … 2 lines
        if (radius > 0) {
          // … 1 lines
          for (const k of platesOver([x - radius, y - radius, x + radius, y + radius])) {
            // … 2 lines
            if (Math.abs(r.cx) > MAP_HALF || Math.abs(r.cy) > MAP_HALF) continue;
          // … 1 lines
          }
        // … 2 lines
        }
      // … 1 lines
      }
    // … 15 lines
    },
  // … 190 lines
  };
// … 1 lines
}

// src/render/flatAtlas.ts:292-299 slice of src/render/flatAtlas.ts#createFlatAtlas.buildSlab (shared): the feature's statements; "…" marks code left out
  const buildSlab = (): void => {
    // … 4 lines
    slab.geometry = slabGeometry(-MAP_HALF, MAP_HALF, -MAP_HALF, MAP_HALF, PLATE_SIZE / FAR_QUADS, (x, y) => TERRAIN_BASE + reliefHeight(relief, x, y));
  // … 1 lines
  };

// src/render/flatAtlas.ts:390-397 slice of src/render/flatAtlas.ts#createFlatAtlas.takeStamps (shared): the feature's statements; "…" marks code left out
  const takeStamps = (p: Plate, doc: TerrainSource): boolean => {
    // … 1 lines
    if (p.source.terrainRevision !== 0 && sameList(sources, p.source.stampSources)) return false;
  // … 4 lines
  };

// src/render/flatAtlas.ts:399-406 slice of src/render/flatAtlas.ts#createFlatAtlas.takePaint (shared): the feature's statements; "…" marks code left out
  const takePaint = (p: Plate, doc: TerrainSource, edited: readonly ChangeRect[] | null): void => {
    // … 1 lines
    const next = plateDabs(p.source.paintSources, p.source.laid, reaching, edited);
    if (!next) return;
    p.source.paintSources = next;
    p.source.terrainPaint = next.map((d) => p.dabCopies.get(d) as PaintDab);
  // … 1 lines
  };
