// The code of src/render/poleCells.ts that is the feature's (the rest of the file is shared).

// src/render/poleCells.ts:56-56 slice of src/render/poleCells.ts#PoleCells.from (shared): the feature's statements; "…" marks code left out
  private from: { net: Network; groundAt: (x: number, y: number) => number } | null = null;

// src/render/poleCells.ts:85-88 slice of src/render/poleCells.ts#PoleCells.source (shared): the feature's statements; "…" marks code left out
  source(net: Network, groundAt: (x: number, y: number) => number): void {
    if (this.from && this.from.net.doc !== net.doc) this.cells.clear();
    this.from = { net, groundAt };
  }

// src/render/poleCells.ts:176-213 slice of src/render/poleCells.ts#PoleCells.build (shared): the feature's statements; "…" marks code left out
  private build(rect: ChangeRect): Block {
    // … 1 lines
    const doc = net.doc;
    const source = docPoles(doc);
    // … 14 lines
    for (const pole of doc.polesIn(rect)) {
      if (Math.floor(pole.x / BLOCK) !== bx || Math.floor(pole.y / BLOCK) !== by) continue;
      own.push(pole);
      const framed = frameOf(pole);
      for (const list of out) list.length = 0;
      placePole(pole, framed.top, framed.frame, framed.kerb, groundAt, out);
      out.forEach((list, part) => { for (const placement of list) block.parts[part]!.add(pole.id, placement); });
    }
    for (const pole of own) {
      for (const span of doc.spansAt(pole.id)) {
        if (span.a !== pole.id) continue;
        const other = doc.poles.get(span.b);
        if (!other) continue;
        block.wires.add(span, wireEnd(span, frameOf(pole)), wireEnd(span, frameOf(other)));
      }
    }
  // … 3 lines
  }
