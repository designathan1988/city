// The code of src/actionsWiring.ts that is the feature's (the rest of the file is shared).

// src/actionsWiring.ts:229-386 slice of src/actionsWiring.ts#createActions.explodeAt (shared): the feature's statements; "…" marks code left out
  function explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet = false): void {
    // … 12 lines
    deps.mutate(() => {
      // … 93 lines
      for (const item of doc.landscapeIn([world.x - radius, world.y - radius, world.x + radius, world.y + radius])) {
        if (!near(item.x, item.y, radius)) continue;
        hit.items.push({ kind: item.kind, x: item.x, y: item.y });
        if (item.kind === 'hydrant') scene.geyser(item.x, item.y, ground(item));
        doc.removeLandscape(item.id);
      // … 1 lines
      }
    // … 6 lines
    });
  // … 36 lines
  }
