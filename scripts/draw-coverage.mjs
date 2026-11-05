// An independent check of a captured draw's screen coverage: its vertices (position × instanceMatrix × modelView ×
// projection, the standard three path; displacements a shader adds are not modelled) projected in JS, with the
// draw's viewport. Says how many vertices land inside the clip volume: a draw both engines leave blank should have
// none (or only triangles that clip away), else the blank images are a shared fault, not an empty draw.
// `node scripts/draw-coverage.mjs <capture dir> <draw> [<draw>...]`
import fs from 'node:fs';
import path from 'node:path';

const [capDir, ...ids] = process.argv.slice(2);
const index = JSON.parse(fs.readFileSync(path.join(capDir, 'index.json'), 'utf8'));
const buf = new Map();
const B = (id) => buf.get(id) ?? buf.set(id, fs.readFileSync(path.join(capDir, 'blobs', `buffer_${id}.bin`))).get(id);
const read = (part, i) => { const b = B(part.buffer); const st = part.stride || part.size * 4; return Array.from({ length: part.size }, (_, c) => b.readFloatLE(part.offset + i * st + c * 4)); };
const mul = (m, v) => [0, 1, 2, 3].map((r) => m[r] * v[0] + m[4 + r] * v[1] + m[8 + r] * v[2] + m[12 + r] * v[3]);
for (const id of ids) {
  const d = index.draws[Number(id)];
  const U = (n) => d.uniforms[n]?.data;
  const P = U('projectionMatrix'), MV = U('modelViewMatrix');
  const pos = d.attributes.find((a) => a.name === 'position');
  const im = d.attributes.find((a) => a.name === 'instanceMatrix');
  if (!P || !MV || !pos || pos.parts[0].type !== 5126) { console.log(id, 'not modelled (no standard matrices or float positions)'); continue; }
  let vcount = 0;
  if (d.elements) { const e = B(d.elements); const sz = d.type === 5125 ? 4 : d.type === 5123 ? 2 : 1; for (let k = 0; k < d.count; k++) { const x = sz === 4 ? e.readUInt32LE(d.offset + k * 4) : sz === 2 ? e.readUInt16LE(d.offset + k * 2) : e.readUInt8(d.offset + k); if (x + 1 > vcount) vcount = x + 1; } } else vcount = d.offset + d.count;
  let inside = 0, total = 0;
  const minmax = [Infinity, -Infinity, Infinity, -Infinity];
  for (let inst = 0; inst < Math.max(1, d.instances); inst++) {
    const I = im ? im.parts.flatMap((p) => read(p, inst)) : [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1];
    for (let k = 0; k < vcount; k++) {
      const p = read(pos.parts[0], k);
      const c = mul(P, mul(MV, mul(I, [p[0], p[1], p[2], 1])));
      total++;
      const nx = c[0] / c[3], ny = c[1] / c[3];
      if (c[3] > 0) { minmax[0] = Math.min(minmax[0], nx); minmax[1] = Math.max(minmax[1], nx); minmax[2] = Math.min(minmax[2], ny); minmax[3] = Math.max(minmax[3], ny); }
      if (c[3] > 0 && Math.abs(c[0]) <= c[3] && Math.abs(c[1]) <= c[3] && c[2] >= -c[3] && c[2] <= c[3]) inside++;
    }
  }
  console.log(`draw ${id} (program ${d.program}): ${inside}/${total} vertices inside the clip volume; NDC x ${minmax[0].toFixed(2)}..${minmax[1].toFixed(2)} y ${minmax[2].toFixed(2)}..${minmax[3].toFixed(2)}`);
}
