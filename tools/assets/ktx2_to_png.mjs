// Converts the Roadcraft cooked KTX2 surfaces (rgba8, Zstandard supercompression) into PNG files Godot imports.
// A file with N layers becomes one PNG with the layers stacked top to bottom (Godot's 2D-array importer cuts
// slices row-major: slices/horizontal = 1, slices/vertical = N). Only mip level 0 is kept; Godot makes mipmaps.
// Layout per the Khronos KTX 2.0 spec (https://registry.khronos.org/KTX/specs/2.0/ktxspec.v2.html):
// header 80 bytes, level index of 24-byte entries at byte 80, supercompression 2 = Zstandard, inside a level the
// data is layer → face → z → rows, rows tightly packed.
// Usage: node ktx2_to_png.mjs <in.ktx2> <out.png>   → prints a JSON line {width, height, layers, vkFormat}.

import fs from 'node:fs';
import zlib from 'node:zlib';

const IDENTIFIER = Buffer.from([0xab, 0x4b, 0x54, 0x58, 0x20, 0x32, 0x30, 0xbb, 0x0d, 0x0a, 0x1a, 0x0a]);
const ZSTD = 2;
const RGBA8_FORMATS = new Set([37, 43]); // VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_SRGB

function readKtx2(buf) {
	if (!buf.subarray(0, 12).equals(IDENTIFIER)) throw new Error('not a KTX2 file');
	const u32 = (o) => buf.readUInt32LE(o);
	const head = {
		vkFormat: u32(12), typeSize: u32(16), width: u32(20), height: u32(24), depth: u32(28),
		layers: Math.max(1, u32(32)), faces: u32(36), levels: Math.max(1, u32(40)), scheme: u32(44),
	};
	if (!RGBA8_FORMATS.has(head.vkFormat)) throw new Error(`unsupported vkFormat ${head.vkFormat}`);
	const offset = Number(buf.readBigUInt64LE(80)), length = Number(buf.readBigUInt64LE(88));
	const raw = buf.subarray(offset, offset + length);
	const data = head.scheme === ZSTD ? zlib.zstdDecompressSync(raw) : head.scheme === 0 ? raw : null;
	if (!data) throw new Error(`unsupported supercompression ${head.scheme}`);
	const expected = head.width * head.height * 4 * head.layers * head.faces;
	if (data.length !== expected) throw new Error(`level 0 holds ${data.length} bytes, expected ${expected}`);
	return { head, data };
}

function chunk(type, body) {
	const len = Buffer.alloc(4); len.writeUInt32BE(body.length);
	const tb = Buffer.from(type, 'ascii');
	const crc = Buffer.alloc(4); crc.writeUInt32BE(zlib.crc32(Buffer.concat([tb, body])) >>> 0);
	return Buffer.concat([len, tb, body, crc]);
}

function encodePng(width, height, rgba) {
	const stride = width * 4, rows = Buffer.alloc((stride + 1) * height);
	for (let y = 0; y < height; y++) {
		rows[y * (stride + 1)] = 0; // filter: none
		rgba.copy(rows, y * (stride + 1) + 1, y * stride, (y + 1) * stride);
	}
	const ihdr = Buffer.alloc(13);
	ihdr.writeUInt32BE(width, 0); ihdr.writeUInt32BE(height, 4);
	ihdr[8] = 8; ihdr[9] = 6; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0; // 8-bit RGBA
	const sig = Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]);
	return Buffer.concat([sig, chunk('IHDR', ihdr), chunk('IDAT', zlib.deflateSync(rows, { level: 9 })), chunk('IEND', Buffer.alloc(0))]);
}

const [input, output] = process.argv.slice(2);
if (!input || !output) { console.error('usage: node ktx2_to_png.mjs <in.ktx2> <out.png>'); process.exit(2); }
const { head, data } = readKtx2(fs.readFileSync(input));
const stackedHeight = head.height * head.layers * head.faces; // layers stacked top to bottom
fs.writeFileSync(output, encodePng(head.width, stackedHeight, Buffer.from(data.buffer, data.byteOffset, data.length)));
console.log(JSON.stringify({ file: output, width: head.width, height: head.height, layers: head.layers, vkFormat: head.vkFormat }));
