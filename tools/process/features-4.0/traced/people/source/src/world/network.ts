// The code of src/world/network.ts that is the feature's (the rest of the file is shared).

// src/world/network.ts:48-48 slice of src/world/network.ts#Network.crossingWalkable (shared): the feature's statements; "…" marks code left out
  private crossingWalkable: WalkableSurface | null = null;

// src/world/network.ts:109-111 method Network.junctionAt: the feature's own
  junctionAt(node: NodeId, level: SurfaceLevel): Junction | undefined {
    return this.junctions.get(node)?.get(level);
  }

// src/world/network.ts:320-322 slice of src/world/network.ts#Network.crossingSurface (shared): the feature's statements; "…" marks code left out
  crossingSurface(): WalkableSurface {
    return this.crossingWalkable ??= new WalkableSurface(this);
  }

// src/world/network.ts:363-368 slice of src/world/network.ts#Network.crosswalkDistanceAt.fits (shared): the feature's statements; "…" marks code left out
    const fits = (distance: number): boolean => {
      // … 2 lines
      return walkable.footway(frame.p.x + nx * lateralLeft, frame.p.y + ny * lateralLeft) &&
        walkable.footway(frame.p.x - nx * lateralRight, frame.p.y - ny * lateralRight);
    };
