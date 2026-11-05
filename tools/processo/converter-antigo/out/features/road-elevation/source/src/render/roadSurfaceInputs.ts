// The code of src/render/roadSurfaceInputs.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaceInputs.ts:451-531 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode (shared): the feature's statements; "…" marks code left out
  private placeNode(net: Network, node: NodeId, silent = false): void {
    // … 16 lines
    const plate = byLevel?.get(Level.Asphalt as SurfaceLevel);
    // … 9 lines
    if (net.transitions.has(node)) {
      const leg = plate?.legs[0];
      const pass = leg ? this.passOf(net, leg.seg) : null;
      if (plate && pass !== null) {
        add('ribbons', PHASE.taper, inputOf([plate.ring.flatten().map((point) => [point.x, point.y])]), pass);
        const sink: QuadSink = (color, quad) => { this.colour(color); add(paintSlot(color), PHASE.taperPaint, inputOf([quad]), pass); };
        for (const spec of transitionMarkings(net, node)) strokeQuads(sink, spec);
      }
    }
    // … 1 lines
    if (plate && record.control === 'mini' && record.incident.length >= 3) {
      const input = inputOf([plate.ring.flatten().map((point) => [point.x, point.y])]);
      const seen = new Set<string>();
      for (const id of record.incident) {
        const pass = this.passOf(net, id);
        if (pass === null || seen.has(pass)) continue;
        seen.add(pass);
        add('ribbons', PHASE.mini, input, pass);
      }
    }
  // … 33 lines
  }

// src/render/roadSurfaceInputs.ts:534-549 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.ownerOf (shared): the feature's statements; "…" marks code left out
  private ownerOf(net: Network, node: NodeId): string | null {
    // … 4 lines
    for (const id of record.incident) {
      // … 2 lines
      const clearance = roadStructure(structure).clearance;
      if (clearance < lowest) {
        lowest = clearance;
        owner = this.passOf(net, id);
      }
    }
    return owner;
  }

// src/render/roadSurfaceInputs.ts:651-657 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.passList (shared): the feature's statements; "…" marks code left out
  passList(): SurfacePass[] {
    const out: SurfacePass[] = ROAD_STRUCTURES.map((structure) => ({ id: structure.id, structure: structure.id }));
    for (const root of [...this.chains.keys()].sort((p, q) => p - q)) {
      out.push({ id: alignmentPass(root), structure: 'ground', segment: root, chain: this.chains.get(root)! });
    }
    return out;
  }
