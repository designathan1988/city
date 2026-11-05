// The verdict of a replayed draw against the WebGL reference, shared by scripts/frame-replay.mjs (the GDScript
// replay) and scripts/replay-compare.mjs (the C++ port). A draw's Godot passes (cases with the same index):
//  - '' the colour, and 'a' the alpha stored in the colour channels (three's blending off: Godot's opaque pipeline,
//    which overwrites like three, twice), or '' alone with its own alpha (blending on: the transparent pipeline);
//  - 'b0'..'b5' the 16 bytes of a float32 pixel, three a pass (a float32 target).
// Kinds: 'color8' (RGBA8 images: within 2/255 a channel), 'half' (half-float targets: float images, within 2 half
// ULP a channel), 'bits32' (the words rebuilt from the bytes, within 16 ULP). Depth-only draws: the 24-bit code.
// Rows: a case in GL orientation has readPixels' rows. Tiers: equal, near (≤ 1 % of the pixels), different,
// empty (both blank: proves nothing), void.
import fs from 'node:fs';
import path from 'node:path';

const f32 = (file) => { const b = fs.readFileSync(file); return new Float32Array(b.buffer.slice(b.byteOffset, b.byteOffset + b.length)); };
const ordered = (x) => { const u = new Int32Array(new Float32Array([x]).buffer)[0]; return u < 0 ? 0x80000000 - u : u; };
const ulp32 = (a, b) => (Number.isNaN(a) && Number.isNaN(b) ? 0 : a === b ? 0 : Math.abs(ordered(a) - ordered(b)));
// half ULP: the distance in units of the half float's last place at the larger magnitude (both values are halves)
const ulp16 = (a, b) => { if (Number.isNaN(a) && Number.isNaN(b)) return 0; if (a === b) return 0; const m = Math.max(Math.abs(a), Math.abs(b), 2 ** -14); return Math.abs(a - b) / 2 ** (Math.floor(Math.log2(m)) - 10); };
const tierOf = (over, lit, total) => (lit === 0 ? 'empty' : over === 0 ? 'equal' : over <= 0.01 * total ? 'near' : 'different');

export function judge(cases, webglDir, imgDir, { depthOnly = new Set(), programOf = () => null } = {}) {
  const report = [];
  const groups = new Map();
  for (const c of cases) (groups.get(c.index) ?? groups.set(c.index, []).get(c.index)).push(c);
  for (const [index, group] of groups) {
    const c = group[0]; const [w, h] = c.size; const n = w * h; const kind = c.kind ?? 'color8';
    const program = programOf(index);
    const base = { index, program, size: c.size, kind };
    const part = (p) => group.find((x) => (x.part ?? '') === p);
    const file = (cs) => path.join(imgDir, `${cs.id ?? index}.${cs.bits ? 'f32' : 'rgba'}`);
    const wf = path.join(webglDir, `${index}.${kind === 'color8' ? 'rgba' : 'f32'}`);
    const need = kind === 'bits32' ? [0, 1, 2, 3, 4, 5].map((j) => part(`b${j}`)) : [part(''), ...(part('a') ? [part('a')] : [])];
    if (!fs.existsSync(wf) || need.some((x) => !x || !fs.existsSync(file(x)))) { report.push({ ...base, pass: false, tier: 'void', why: !fs.existsSync(wf) ? 'no valid WebGL reference' : 'a Godot pass is missing' }); continue; }
    const gl = c.orientation === 'gl';
    const row = (y) => (gl ? y : h - 1 - y); // Godot's row for WebGL's row y
    let over = 0, lit = 0, max = 0;
    if (kind === 'bits32') {
      const W = f32(wf); const B = need.map((x) => f32(file(x)));
      const dv = new DataView(new ArrayBuffer(16));
      for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
        const wi = (y * w + x) * 4, gi = (row(y) * w + x) * 4;
        for (let b = 0; b < 16; b++) dv.setUint8(b, Math.round(B[Math.floor(b / 3)][gi + (b % 3)] * 255));
        let bad = false, any = false;
        for (let k = 0; k < 4; k++) { const g = dv.getFloat32(k * 4); const r = W[wi + k]; if (g !== 0 || r !== 0) any = true; const d = ulp32(g, r); max = Math.max(max, d); if (d > 16) bad = true; }
        if (any) lit++; if (bad) over++;
      }
    } else if (kind === 'half') {
      const W = f32(wf); const C = f32(file(need[0])); const A = need[1] ? f32(file(need[1])) : null;
      for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
        const wi = (y * w + x) * 4, gi = (row(y) * w + x) * 4;
        const g = [C[gi], C[gi + 1], C[gi + 2], A ? A[gi] : C[gi + 3]];
        let bad = false, any = false;
        for (let k = 0; k < 4; k++) { if (g[k] !== 0 || W[wi + k] !== 0) any = true; const d = ulp16(g[k], W[wi + k]); max = Math.max(max, d); if (d > 2) bad = true; }
        if (any) lit++; if (bad) over++;
      }
    } else {
      const W = fs.readFileSync(wf); const C = fs.readFileSync(file(need[0])); const A = need[1] ? fs.readFileSync(file(need[1])) : null;
      for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
        const wi = (y * w + x) * 4, gi = (row(y) * w + x) * 4;
        const g = [C[gi], C[gi + 1], C[gi + 2], A ? A[gi] : C[gi + 3]];
        let dd = 0;
        if (depthOnly.has(program)) { const code = (v) => v[0] * 65536 + v[1] * 256 + v[2]; dd = Math.abs(code([W[wi], W[wi + 1], W[wi + 2]]) - code(g)) <= 64 ? 0 : 255; }
        else for (let k = 0; k < 4; k++) dd = Math.max(dd, Math.abs(W[wi + k] - g[k]));
        if (W[wi] + W[wi + 1] + W[wi + 2] + W[wi + 3] + g[0] + g[1] + g[2] + g[3] > 0) lit++;
        max = Math.max(max, dd); if (dd > 2) over++;
      }
    }
    const tier = tierOf(over, lit, n);
    report.push({ ...base, pass: tier === 'equal', tier, over, lit, max });
  }
  return report.sort((a, b) => a.index - b.index);
}
