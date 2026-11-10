// The code of src/render/poleLayer.ts that is the feature's (the rest of the file is shared).

// src/render/poleLayer.ts:561-565 slice of src/render/poleLayer.ts#PoleLayer.fill (shared): the feature's statements; "…" marks code left out
  fill(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource): void {
    for (const pole of source.poles.values()) this.place(net, groundAt, source, pole, undefined);
  // … 2 lines
  }

// src/render/poleLayer.ts:611-626 slice of src/render/poleLayer.ts#PoleLayer.place (shared): the feature's statements; "…" marks code left out
  private place(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource,
    pole: UtilityPole, kept: { x: number; y: number } | null | undefined): void {
    const { top, kerb, frame } = framePole(net, groundAt, source, pole, kept);
    // … 3 lines
    if (source.draws?.(pole) ?? true) {
      // … 2 lines
      placePole(pole, top, frame, kerb, groundAt, out);
    // … 1 lines
    }
    // … 1 lines
    this.poles.set(pole.id, { block: key, x: pole.x, y: pole.y, top, kerb, frame, slots });
  // … 1 lines
  }

// src/render/poleLayer.ts:711-721 slice of src/render/poleLayer.ts#buildUtilities (shared): the feature's statements; "…" marks code left out
export function buildUtilities(
  net: Network,
  // … 2 lines
  source: PoleSource = docPoles(net.doc),
// … 1 lines
): PoleLayer {
  // … 1 lines
  layer.fill(net, groundAt, source);
// … 1 lines
}

// src/render/poleLayer.ts:729-750 slice of src/render/poleLayer.ts#buildPolePreview (shared): the feature's statements; "…" marks code left out
export function buildPolePreview(
  net: Network,
// … 3 lines
): Utilities {
  // … 13 lines
  return buildUtilities(net, groundAt, kit,
    { poles, spans, spansAt: (id) => reaching.get(id) ?? [], draws: (pole) => !standing.has(pole.id) }, 'utility-preview');
}
