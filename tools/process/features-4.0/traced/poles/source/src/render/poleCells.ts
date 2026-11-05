// The code of src/render/poleCells.ts that is the feature's (the rest of the file is shared).

// src/render/poleCells.ts:28-31 slice of src/render/poleCells.ts#HeldBlock (shared): the feature's statements; "…" marks code left out
interface HeldBlock {
  readonly block: Block;
// … 1 lines
}

// src/render/poleCells.ts:50-50 slice of src/render/poleCells.ts#PoleCells.group (shared): the feature's statements; "…" marks code left out
  readonly group: ViewedRoot;

// src/render/poleCells.ts:53-53 slice of src/render/poleCells.ts#PoleCells.live (shared): the feature's statements; "…" marks code left out
  private readonly live = new Set<Block>();

// src/render/poleCells.ts:54-54 slice of src/render/poleCells.ts#PoleCells.scale (shared): the feature's statements; "…" marks code left out
  private readonly scale = new ScreenScale();

// src/render/poleCells.ts:66-76 slice of src/render/poleCells.ts#PoleCells.constructor (shared): the feature's statements; "…" marks code left out
  constructor(kit: SceneryKit, name = 'utilities') {
    this.group = new ViewedRoot((camera) => this.view(camera));
    this.group.name = name;
    // … 1 lines
    this.cells = new ViewCells<HeldBlock>({
      name, version: 1, side: BLOCK, reach: 0, cpuBudget: 16 * 1048576, gpuBudget: 128 * 1048576,
      kinds: POLE_CHANGES, pad: POLE_REACH + SPAN_REACH,
    // … 2 lines
    });
  }

// src/render/poleCells.ts:96-134 slice of src/render/poleCells.ts#PoleCells.look (shared): the feature's statements; "…" marks code left out
  look(view: PoleView, deadline: number): void {
    // … 4 lines
    const bx0 = Math.floor((view.x - reach) / BLOCK), bx1 = Math.floor((view.x + reach) / BLOCK);
    const by0 = Math.floor((view.y - reach) / BLOCK), by1 = Math.floor((view.y + reach) / BLOCK);
    for (let bx = bx0; bx <= bx1; bx++) {
      for (let by = by0; by <= by1; by++) {
        const x0 = bx * BLOCK, y0 = by * BLOCK, x1 = x0 + BLOCK, y1 = y0 + BLOCK;
        const d = Math.hypot(Math.max(x0 - view.x, 0, view.x - x1), Math.max(y0 - view.y, 0, view.y - y1));
        if (d > reach) continue;
        // Three's axes: x east, y up, z south.
        this.box.min.set(x0 - margin, view.ground + POLE_LOW, -(y1 + margin));
        this.box.max.set(x1 + margin, view.ground + POLE_HIGH, -(y0 - margin));
        // Off the frame but under the near cascade (`KeptFrustum.keep`): held, its poles casting there. DO NOT drop a block for leaving the
        // frame: its shadow left the near cascade with it, and came back with it, at every frame of a flight (`utility-poles`, 2026-10-22).
        if (!view.frustum.intersectsBox(this.box) && !(view.frustum as KeptFrustum).keep?.intersectsBox(this.box)) continue;
        const one = this.wantedPool[wanted.length] ?? (this.wantedPool[wanted.length] = { key: 0 as CellKey, d: 0 });
        one.key = this.cells.cellAt(x0 + BLOCK / 2, y0 + BLOCK / 2);
        one.d = d;
        wanted.push(one);
      }
    }
  // … 14 lines
  }

// src/render/poleCells.ts:137-173 slice of src/render/poleCells.ts#PoleCells.derive (shared): the feature's statements; "…" marks code left out
  private derive(key: CellKey, rect: ChangeRect): { value: HeldBlock; cpuBytes: number } {
    // … 4 lines
    for (const part of block.parts) {
      const matrices = part.seal();
      if (matrices) buffers.push(matrices);
    }
    const wires = block.wires.seal();
    if (wires) {
      buffers.push(wires);
      gpuOnlyAttributes(block.wires.drawn!, { wires }, () => {
        const again = this.build(rect);
        const array = again.wires.instances;
        this.release(again);
        return { wires: array };
      });
    }
  // … 17 lines
  }

// src/render/poleCells.ts:176-213 slice of src/render/poleCells.ts#PoleCells.build (shared): the feature's statements; "…" marks code left out
  private build(rect: ChangeRect): Block {
    // … 2 lines
    const source = docPoles(doc);
    const block: Block = { parts: this.specs.map((spec) => new PartStore(spec, this.group)), wires: new WireStore(this.group), poles: new Set() };
    // … 10 lines
    const bx = Math.round(rect[0] / BLOCK), by = Math.round(rect[1] / BLOCK);
    // … 2 lines
    for (const pole of doc.polesIn(rect)) {
      if (Math.floor(pole.x / BLOCK) !== bx || Math.floor(pole.y / BLOCK) !== by) continue;
      // … 4 lines
      out.forEach((list, part) => { for (const placement of list) block.parts[part]!.add(pole.id, placement); });
    }
    for (const pole of own) {
      for (const span of doc.spansAt(pole.id)) {
        // … 3 lines
        block.wires.add(span, wireEnd(span, frameOf(pole)), wireEnd(span, frameOf(other)));
      }
    }
    for (const part of block.parts) part.flush();
    block.wires.flush();
    return block;
  }

// src/render/poleCells.ts:215-219 slice of src/render/poleCells.ts#PoleCells.release (shared): the feature's statements; "…" marks code left out
  private release(block: Block): void {
    for (const part of block.parts) part.dispose();
    block.wires.dispose();
    this.live.delete(block);
  }

// src/render/poleCells.ts:222-232 slice of src/render/poleCells.ts#PoleCells.view (shared): the feature's statements; "…" marks code left out
  private view(camera: Camera): void {
    this.scale.set(camera);
    wireScreen.uWirePixels.value = screen.halfHeight;
    for (const block of this.live) {
      // … 1 lines
      for (const part of block.parts) if (part.count > 0) nearest = Math.max(nearest, this.scale.atBox(part.box));
      if (block.wires.count > 0) nearest = Math.max(nearest, this.scale.atBox(block.wires.box));
      for (const part of block.parts) part.view(this.scale, nearest);
      block.wires.view(this.scale, nearest);
    }
  }

// src/render/poleCells.ts:234-237 slice of src/render/poleCells.ts#PoleCells.dispose (shared): the feature's statements; "…" marks code left out
  dispose(): void {
    // … 1 lines
    this.group.clear();
  }
