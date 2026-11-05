// The code of src/render/poleBake.ts that is the feature's (the rest of the file is shared).

// src/render/poleBake.ts:75-83 slice of src/render/poleBake.ts#framePole (shared): the feature's statements; "…" marks code left out
export function framePole(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource,
  pole: UtilityPole, kept: { x: number; y: number } | null | undefined): { readonly top: number; readonly kerb: { x: number; y: number } | null; readonly frame: PoleArms } {
  // … 2 lines
  const kerb = kept !== undefined ? kept : kerbward(net, { x: pole.x, y: pole.y }, m(6));
  // … 1 lines
  const frame = poleArmsAt(pole, source.spansAt(pole.id), source.poles, () => kerb ?? { x: 0, y: 1 });
  return { top, kerb, frame };
}
