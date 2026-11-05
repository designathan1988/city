// Side by side PNG of a replayed draw: WebGL (left), Godot (middle), difference ×8 (right); and the values at a few
// pixels. `node scripts/replay-view.mjs <replay out dir> <draw> [<png>]`
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';

const [dir, draw, pngOut] = process.argv.slice(2);
const cases = JSON.parse(fs.readFileSync(path.join(dir, 'godot', 'cases.json'), 'utf8'));
const c = cases.find((x) => String(x.index) === String(draw));
const [w, h] = c.size;
const W = fs.readFileSync(path.join(dir, 'webgl', `${draw}.rgba`));
const G = fs.readFileSync(path.join(dir, 'godot', 'out', `${draw}.rgba`));
const at = (img, x, y, flip) => { const i = ((flip ? h - 1 - y : y) * w + x) * 4; return [img[i], img[i + 1], img[i + 2], img[i + 3]]; };
const out = Buffer.alloc((w * 3 * 4 + 1) * h);
for (let y = 0; y < h; y++) {
  out[y * (w * 12 + 1)] = 0;
  for (let x = 0; x < w; x++) {
    // shown upright: WebGL's rows are bottom-up; Godot's too in GL orientation, else top-down
    const a = at(W, x, y, true), b = at(G, x, y, c.orientation === 'gl');
    const put = (k, p) => { const o = y * (w * 12 + 1) + 1 + (k * w + x) * 4; out[o] = p[0]; out[o + 1] = p[1]; out[o + 2] = p[2]; out[o + 3] = 255; };
    put(0, a); put(1, b); put(2, [0, 1, 2].map((k) => Math.min(255, Math.abs(a[k] - b[k]) * 8)));
  }
}
const crc = (buf) => { let c2 = ~0; for (const b of buf) { c2 ^= b; for (let k = 0; k < 8; k++) c2 = (c2 >>> 1) ^ (0xedb88320 & -(c2 & 1)); } return ~c2 >>> 0; };
const chunk = (type, data) => { const len = Buffer.alloc(4); len.writeUInt32BE(data.length); const td = Buffer.concat([Buffer.from(type), data]); const cr = Buffer.alloc(4); cr.writeUInt32BE(crc(td)); return Buffer.concat([len, td, cr]); };
const ihdr = Buffer.alloc(13); ihdr.writeUInt32BE(w * 3, 0); ihdr.writeUInt32BE(h, 4); ihdr[8] = 8; ihdr[9] = 6;
const png = Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr), chunk('IDAT', zlib.deflateSync(out)), chunk('IEND', Buffer.alloc(0))]);
const file = pngOut ?? path.join(dir, `view-${draw}.png`);
fs.writeFileSync(file, png);
for (const [x, y] of [[Math.floor(w / 2), Math.floor(h / 2)], [5, 5], [w - 6, h - 6], [Math.floor(w / 4), Math.floor(h * 3 / 4)]]) console.log(`(${x},${y}) webgl ${at(W, x, y, true).join(',')}  godot ${at(G, x, y, c.orientation === 'gl').join(',')}`);
console.log(file);
