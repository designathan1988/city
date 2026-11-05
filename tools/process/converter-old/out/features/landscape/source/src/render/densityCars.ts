// The code of src/render/densityCars.ts that is the feature's (the rest of the file is shared).

// src/render/densityCars.ts:52-52 slice of src/render/densityCars.ts#TILE_RADIUS (shared): the feature's statements; "…" marks code left out
const TILE_RADIUS = Math.hypot(TILE / 2, TILE / 2, (TERRAIN_MAX_HEIGHT - TERRAIN_MIN_HEIGHT) / 2 + m(80));

// src/render/densityCars.ts:206-224 slice of src/render/densityCars.ts#createDensityCars.tileAt (shared): the feature's statements; "…" marks code left out
  function tileAt(cx: number, cy: number): CarTile {
    // … 3 lines
    const sphere = new Sphere(new Vector3((cx + 0.5) * TILE, (TERRAIN_MAX_HEIGHT + TERRAIN_MIN_HEIGHT) / 2, -(cy + 0.5) * TILE), TILE_RADIUS);
    // … 1 lines
    const geometry = tileGeometry(made, TILE_START, sphere);
    const mesh = new Mesh(geometry, material);
    mesh.name = `densityCars-${cx},${cy}`;
    pinned(mesh);
    mesh.castShadow = false;
    mesh.receiveShadow = true;
    // … 1 lines
    mesh.visible = false;
    tile = Object.assign(made, { mesh, geometry, n: 0, slots: new Set<number>(), dirty: false, key });
    // … 1 lines
    group.add(mesh);
  // … 1 lines
  }
