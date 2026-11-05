// What the feature uses of src/editor/commit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/commit.ts:204-212 function solveOver
/** A solve of `net` over `ground`: from the game's last on the same natural ground, or from the last on the same index */
function solveOver(net: Network, ground: (x: number, y: number) => number, sampled: boolean, index: TerrainIndex | null,
  previous: RoadElevation | null): RoadElevation;
