// The code of src/render/propGeometry.ts that is the feature's (the rest of the file is shared).

// src/render/propGeometry.ts:405-424 slice of src/render/propGeometry.ts#drainGeometry (shared): the feature's statements; "…" marks code left out
export function drainGeometry(): BufferGeometry {
  // … 3 lines
  const rise = FOOTWAY_RISE - m(0.03);
  const kerb = CURB_BAND;
  const parts = [
    // The mouth in the kerb face.
    part(new BoxGeometry(m(0.9), rise * 0.55, m(0.04)), slot, { at: [0, -rise * 0.62, -kerb - m(0.03)] }),
    // The lintel over it, a little proud of the kerb.
    part(new BoxGeometry(m(1.0), m(0.04), kerb + m(0.06)), concrete, { at: [0, m(0.01), -kerb / 2 - m(0.03)] }),
    // The concrete frame and the grate in the gutter.
    part(new BoxGeometry(m(1.0), m(0.02), m(0.5)), concrete, { at: [0, -rise + m(0.006), -kerb - m(0.3)] }),
  ];
  for (let i = 0; i < 7; i++) {
    parts.push(part(new BoxGeometry(m(0.035), m(0.022), m(0.4)), iron, { at: [-m(0.39) + i * m(0.13), -rise + m(0.012), -kerb - m(0.3)] }));
  }
  parts.push(part(new BoxGeometry(m(0.84), m(0.016), m(0.4)), slot, { at: [0, -rise + m(0.004), -kerb - m(0.3)] }));
  return merge(parts, false);
}
