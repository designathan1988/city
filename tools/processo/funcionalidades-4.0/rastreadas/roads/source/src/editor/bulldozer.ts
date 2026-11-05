// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:13-25 slice of src/editor/bulldozer.ts#BulldozerHost (shared): the feature's statements; "…" marks code left out
export interface BulldozerHost {
  // … 1 lines
  readonly net: Network;
// … 9 lines
}

// src/editor/bulldozer.ts:28-36 slice of src/editor/bulldozer.ts#removeRoads (shared): the feature's statements; "…" marks code left out
export function removeRoads(doc: RoadDoc, net: Network, ids: readonly SegmentId[]): boolean {
  return removeWithStreetKit(doc, net, ids, () => {
    const money = roadsBefore(doc);
    for (const id of ids) doc.removeSegment(id);
    doc.pruneOrphanNodes();
    settleRoadEdit(money, doc);
    return true;
  });
}

// src/editor/bulldozer.ts:44-44 slice of src/editor/bulldozer.ts#Bulldozer.drag (shared): the feature's statements; "…" marks code left out
  private drag: { pointer: number; a: Vec2; b: Vec2; world: Vec2; to: Vec2; anchor: Anchor } | null = null;

// src/editor/bulldozer.ts:61-63 slice of src/editor/bulldozer.ts#Bulldozer.down (shared): the feature's statements; "…" marks code left out
  down(pointer: number, screen: Vec2, world: Vec2, anchor: Anchor): void {
    this.drag = { pointer, a: { ...screen }, b: { ...screen }, world: { ...world }, to: { ...world }, anchor };
  }

// src/editor/bulldozer.ts:94-122 slice of src/editor/bulldozer.ts#Bulldozer.click (shared): the feature's statements; "…" marks code left out
  private click(screen: Vec2, world: Vec2, anchor: Anchor): void {
    // … 5 lines
    const onRoad = anchor.kind === 'segment' && onCarriageway(host.net, world);
    // … 1 lines
    const pole = onRoad ? null : doc.poleNear(world, host.poleReach());
    if (pole) {
      host.mutate(() => {
        doc.removePole(pole.id);
        return true;
      });
      host.hint('hint.pole.removed');
      return;
    }
    // … 1 lines
    const item = onRoad ? null : landscapeNearIn(doc, world, host.itemReach());
    if (item) {
      host.mutate(() => doc.removeLandscape(item.id));
      host.hint('hint.streetscape.removed');
      return;
    }
    if (anchor.kind === 'segment' && anchor.segment !== undefined) {
      const id = anchor.segment;
      host.mutate(() => removeRoads(doc, host.net, [id]));
    }
  }

// src/editor/bulldozer.ts:125-149 slice of src/editor/bulldozer.ts#Bulldozer.boxed (shared): the feature's statements; "…" marks code left out
  private boxed(a: Vec2, b: Vec2): void {
    // … 5 lines
    const segments = ribbonsNear(net, box).filter((ribbon) => inside(ribbon.full.sampleAt(ribbon.full.length / 2).p)).map((ribbon) => ribbon.id);
    // … 6 lines
    if (!segments.length && !builtIds.length && !lots.length && !poles.length && !items.length && !walls.length) return;
    this.host.mutate(() => {
      if (segments.length) removeRoads(doc, net, segments);
    // … 6 lines
    });
  // … 1 lines
  }
