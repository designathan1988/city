// The code of src/render/groundShear.ts that is the feature's (the rest of the file is shared).

// src/render/groundShear.ts:24-35 function shearMatrix: the feature's own
export function shearMatrix(matrix: Matrix4, g: Gradient, X0: number, Z0: number): void {
  if (g.gx === 0 && g.gy === 0) return;
  const e = matrix.elements;
  const a = g.gx;
  const b = -g.gy;
  // Row Y of S·M = row Y of M + a·row X + b·row Z - (a·X0 + b·Z0)·row W.
  const c = a * X0 + b * Z0;
  for (let col = 0; col < 4; col++) {
    const i = col * 4;
    e[i + 1] = e[i + 1]! + a * e[i]! + b * e[i + 2]! - c * e[i + 3]!;
  }
}
