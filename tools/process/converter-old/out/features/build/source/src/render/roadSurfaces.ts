// The code of src/render/roadSurfaces.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaces.ts:45-79 slice of src/render/roadSurfaces.ts#SurfaceReuse (shared): the feature's statements; "…" marks code left out
export interface SurfaceReuse {
  // … 32 lines
  readonly farBatches?: FarBlockBatches;
}

// src/render/roadSurfaces.ts:283-794 slice of src/render/roadSurfaces.ts#roadSurfaceSteps (shared): the feature's statements; "…" marks code left out
export function* roadSurfaceSteps(
// … 10 lines
): Generator<'wait' | void, RoadSurfaces, void> {
  // … 40 lines
  for (const pass of passes) {
    // … 298 lines
    for (const group of groups) {
      for (const [chunk, lists] of parts.get(group.name) ?? []) {
        // … 8 lines
        if (known && sameTiles(known.parts, lists)) {
        // … 4 lines
        } else {
          // … 10 lines
          mesh = mergeTiles(memberTiles.flat(), group.options, source,
            lock ? {
              // … 1 lines
              start: (block, arrays) => {
                // … 5 lines
                Promise.all(pieces.map((piece, i) => coarsen!(piece, locks![i]!))).then((made) => {
                  // … 2 lines
                  if (made.some((m) => m !== null)) {
                    // … 3 lines
                    if (reuse?.farBatches) reuse.farBatches.adopt(level);
                    else if (!near) void warmLooseAhead([level]);
                  } else if (!near) void warmLooseAhead([block]);
                }).catch((error: unknown) => {
                // … 3 lines
                }).finally(() => { coarsening--; const levels = levelsOf(block); if (levels) levels.pending = false; });
              },
            } : undefined);
        // … 38 lines
        }
      // … 6 lines
      }
    }
  // … 27 lines
  }
// … 33 lines
}
