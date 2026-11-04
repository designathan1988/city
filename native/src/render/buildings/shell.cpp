// Vertex writing for the building meshes: faces with their tone (a macro noise over the walls and the grime at their foot
// grime), clockwise winding toward `out` (Godot front faces), the vertex code of each surface kind, and the commit of
// one surface to a RenderingServer mesh.
#include "render/buildings/shell.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>
#include <cstring>
#include <iterator>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float PANE_RECESS_STEP_M = 0.01f; // COLOR.a above the kind: building_glass.gdshader reads the same
constexpr int PANE_KIND_BITS = 3, PANE_RECESS_MAX = 31;
constexpr float MACRO_M = 12.f, TONE_BASE = 0.97f, TONE_SPAN = 0.06f, GRIME_M = 1.2f, GRIME_FLOOR = 0.8f, GRIME_SPAN = 0.2f;
constexpr float SHELL_WALL_TOP = 0.5f, GAMMA_INV = 1.f / 2.2f, BYTE = 255.f, DEGENERATE = 1e-12f, BIG = 1e30f;
constexpr uint32_t H1 = 0x27d4eb2d, H2 = 0x165667b1, H3 = 0x3c6ef372, H4 = 0x85ebca6b;
constexpr int SH1 = 15, SH2 = 13;
constexpr double U32 = 4294967296.0;
constexpr uint32_t SEED_BYTE = 0xff;
constexpr float HALF_STEP = 0.5f;
constexpr int BYTE_BITS = 8, CORNERS = 8, FACES = 6, QUAD_INDICES = 6, BIT_A = 1, BIT_B = 2, BIT_C = 4;
// Box faces as corner quads (corner bit 1 = a1, 2 = b1, 4 = c1), in the order -e2, +e2, -e0, +e0, -e1, +e1.
constexpr int BOX_QUADS[FACES][4] = { { 0, 1, 3, 2 }, { 4, 5, 7, 6 }, { 0, 2, 6, 4 }, { 1, 3, 7, 5 }, { 0, 1, 5, 4 }, { 2, 3, 7, 6 } };
constexpr uint8_t BOX_SKIP[FACES] = { SKIP_E2_NEG, SKIP_E2_POS, SKIP_E0_NEG, SKIP_E0_POS, SKIP_E1_NEG, SKIP_E1_POS };

float lattice(int32_t i, int32_t j, int32_t k) {
	uint32_t h = (uint32_t(i) * H1) ^ (uint32_t(j) * H2) ^ (uint32_t(k) * H3);
	h = (h ^ (h >> SH1)) * H4;
	return float(double(h ^ (h >> SH2)) / U32);
}
float smooth(float t) { return t * t * (3 - 2 * t); }
float gamma(float v) { return std::pow(std::clamp(v, 0.f, 1.f), GAMMA_INV); }
// Four shares (0..1) as the RGBA8 of a vertex.
uint32_t pack(float r, float g, float b, float a) {
	auto byte = [](float v) { return uint32_t(std::clamp(v, 0.f, 1.f) * BYTE + HALF_STEP); };
	return byte(r) | byte(g) << BYTE_BITS | byte(b) << (2 * BYTE_BITS) | byte(a) << (3 * BYTE_BITS);
}
uint32_t trim_code(const Lin &trim, float frame) { return pack(gamma(trim.r), gamma(trim.g), gamma(trim.b), frame / FRAME_SPAN_M); }
// A box left open at both ends of an axis (a window frame's ring round its glass) is a tube: a unit-cube instance would
// close it and plug what it surrounds, so it stays faces. Any other skip only saves faces nobody sees.
bool tube(uint8_t skip) {
	constexpr uint8_t PAIRS[] = { SKIP_E0_NEG | SKIP_E0_POS, SKIP_E1_NEG | SKIP_E1_POS, SKIP_E2_NEG | SKIP_E2_POS };
	return std::any_of(std::begin(PAIRS), std::end(PAIRS), [skip](uint8_t pair) { return (skip & pair) == pair; });
}
} // namespace

// Trilinear value noise in map axes (x, north = -z, up), metres / 12.
float macro_noise(const Vector3 &p) {
	float x = p.x / MACRO_M, y = -p.z / MACRO_M, z = p.y / MACRO_M;
	int32_t i = int32_t(std::floor(x)), j = int32_t(std::floor(y)), k = int32_t(std::floor(z));
	float fx = smooth(x - float(i)), fy = smooth(y - float(j)), fz = smooth(z - float(k));
	auto l = [&](int a, int b, int c) { return lattice(i + a, j + b, k + c); };
	auto lx = [&](int b, int c) { return l(0, b, c) + (l(1, b, c) - l(0, b, c)) * fx; };
	float y0 = lx(0, 0) + (lx(1, 0) - lx(0, 0)) * fy, y1 = lx(0, 1) + (lx(1, 1) - lx(0, 1)) * fy;
	return y0 + (y1 - y0) * fz;
}

int Shell::vert(const Vector3 &p, const Vector3 &n, const Color &c, const Vector2 &t, const Vector2 &t2) {
	pos.push_back(p);
	nrm.push_back(n);
	col.push_back(c);
	uv.push_back(t);
	uv2.push_back(t2);
	if (rooms)
		for (std::vector<uint32_t> &c : custom) c.push_back(0);
	return int(pos.size()) - 1;
}

Color Shell::shaded(const Vector3 &p, const Lin &lin, bool wall, SurfaceKind kind) const {
	float k = shade;
	if (tone) {
		k *= TONE_BASE + TONE_SPAN * macro_noise(p);
		if (wall) k *= GRIME_FLOOR + GRIME_SPAN * smooth(std::clamp((p.y - grime_floor) / GRIME_M, 0.f, 1.f));
	}
	return Color(gamma(lin.r * k), gamma(lin.g * k), gamma(lin.b * k), float(kind) / BYTE);
}

Lin Shell::far_tint(const Lin &lin, int8_t fin) const {
	if (!far_tones || fin < 0) return lin;
	const Lin &k = (*far_tones)[size_t(fin)];
	return { lin.r * k.r, lin.g * k.g, lin.b * k.b };
}

void Shell::face(const Vector3 *p, int count, const Vector3 &out, const Lin &lin, int8_t fin, SurfaceKind kind) {
	Vector3 n = (p[1] - p[0]).cross(p[2] - p[0]);
	if (n.length_squared() < DEGENERATE && count == 4) n = (p[2] - p[1]).cross(p[3] - p[1]);
	if (n.length_squared() < DEGENERATE) return;
	n = n.normalized();
	if (n.dot(out) < 0) n = -n;
	bool wall = std::abs(n.y) < SHELL_WALL_TOP;
	Vector2 t2(float(fin + 1), 0);
	int base = int(pos.size());
	const Lin tint = far_tint(lin, fin);
	for (int i = 0; i < count; i++) vert(p[i], n, shaded(p[i], tint, wall, kind), Vector2(), t2);
	bool flip = (p[1] - p[0]).cross(p[2] - p[0]).dot(n) > 0; // clockwise seen from `out`
	for (int i = 1; i + 1 < count; i++) {
		idx.push_back(base);
		idx.push_back(base + (flip ? i + 1 : i));
		idx.push_back(base + (flip ? i : i + 1));
	}
}

void Shell::quad(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, const Lin &lin, int8_t fin,
		SurfaceKind kind) {
	const Vector3 p[4] = { a, b, c, d };
	face(p, 4, out, lin, fin, kind);
}

void Shell::tri(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &out, const Lin &lin, int8_t fin) {
	const Vector3 p[3] = { a, b, c };
	face(p, 3, out, lin, fin, SK_SHELL);
}

void Shell::rect(const Basis &f, float a0, float a1, float b0, float b1, float c, const Lin &lin, int8_t fin, bool back) {
	if (a1 - a0 <= 0 || b1 - b0 <= 0) return;
	quad(f.at(a0, b0, c), f.at(a1, b0, c), f.at(a1, b1, c), f.at(a0, b1, c), back ? -f.e2 : f.e2, lin, fin);
}

void Shell::box(const Basis &f, float a0, float a1, float b0, float b1, float c0, float c1, const Lin &lin, int8_t fin, uint8_t skip) {
	if (a1 <= a0 || b1 <= b0 || c1 <= c0) return;
	if (instanced && !tube(skip)) {
		float m0 = a1 - a0, m1 = b1 - b0, m2 = c1 - c0;
		float lo = std::min({ m0, m1, m2 }), hi = std::max({ m0, m1, m2 });
		Vector3 centre = f.at((a0 + a1) * 0.5f, (b0 + b1) * 0.5f, (c0 + c1) * 0.5f);
		Vector3 e0 = f.e0 * m0, e1 = f.e1 * m1, e2 = f.e2 * m2;
		Color c = shaded(centre, lin, false, SK_SHELL);
		auto byte = [](float v) { return uint32_t(std::clamp(v, 0.f, 1.f) * BYTE + 0.5f); };
		constexpr int RGBA_R_SHIFT = 24, RGBA_G_SHIFT = 16, RGBA_B_SHIFT = 8;
		BoxInstance inst;
		const float cols[3][3] = { { e0.x, e0.y, e0.z }, { e1.x, e1.y, e1.z }, { e2.x, e2.y, e2.z } };
		for (int i = 0; i < 3; i++)
			for (int k = 0; k < 3; k++) inst.basis[i * 3 + k] = cols[i][k];
		inst.origin[0] = centre.x; inst.origin[1] = centre.y; inst.origin[2] = centre.z;
		inst.color = byte(c.r) << RGBA_R_SHIFT | byte(c.g) << RGBA_G_SHIFT | byte(c.b) << RGBA_B_SHIFT | byte(c.a);
		inst.fin = fin;
		inst.thick = m0 + m1 + m2 - lo - hi; // the second largest side: the bar's width
		boxes.push_back(inst);
		return;
	}
	Vector3 p[CORNERS];
	for (int k = 0; k < CORNERS; k++) p[k] = f.at(k & BIT_A ? a1 : a0, k & BIT_B ? b1 : b0, k & BIT_C ? c1 : c0);
	const Vector3 out[FACES] = { -f.e2, f.e2, -f.e0, f.e0, -f.e1, f.e1 };
	for (int i = 0; i < FACES; i++) {
		const int *q = BOX_QUADS[i];
		if (!(skip & BOX_SKIP[i])) quad(p[q[0]], p[q[1]], p[q[2]], p[q[3]], out[i], lin, fin);
	}
}

void Shell::card(const Basis &f, float a0, float a1, float b0, float b1, float c, const Lin &lin, CardKind kind) {
	if (cards) {
		cards->card(f, a0, a1, b0, b1, c, lin, kind);
		return;
	}
	if (a1 - a0 <= 0 || b1 - b0 <= 0) return;
	sheet(f, a0, a1, b0, b1, c, shaded(f.at(a0, b0, c), lin, false, SK_SHELL), Vector2(float(kind), 0));
}

// A quad with UV 0..1 over it, the same colour and UV2 at its corners. Back: its first vertex.
int Shell::sheet(const Basis &f, float a0, float a1, float b0, float b1, float c, const Color &code, const Vector2 &t2) {
	const Vector3 n = f.e2;
	const int i0 = vert(f.at(a0, b0, c), n, code, Vector2(0, 0), t2), i1 = vert(f.at(a1, b0, c), n, code, Vector2(1, 0), t2);
	const int i2 = vert(f.at(a1, b1, c), n, code, Vector2(1, 1), t2), i3 = vert(f.at(a0, b1, c), n, code, Vector2(0, 1), t2);
	const bool flip = (pos[size_t(i1)] - pos[size_t(i0)]).cross(pos[size_t(i2)] - pos[size_t(i0)]).dot(n) > 0;
	const int t[QUAD_INDICES] = { i0, flip ? i2 : i1, flip ? i1 : i2, i0, flip ? i3 : i2, flip ? i2 : i3 };
	idx.insert(idx.end(), t, t + QUAD_INDICES);
	return i0;
}

void Shell::pane(const Basis &f, float a0, float a1, float b0, float b1, float c, const PaneLook &look) {
	if (glass) {
		glass->pane(f, a0, a1, b0, b1, c, look);
		return;
	}
	if (a1 - a0 <= 0 || b1 - b0 <= 0) return;
	const int recess = std::min(int(std::lround(look.recess / PANE_RECESS_STEP_M)), PANE_RECESS_MAX);
	Color code(float(look.slot & SEED_BYTE) / BYTE, float((look.slot >> BYTE_BITS) & SEED_BYTE) / BYTE, float(look.bay) / BYTE,
			float((look.plain ? SK_PANE_CLEAR : look.dark ? SK_PANE_DARK : SK_PANE) | (recess << PANE_KIND_BITS)) / BYTE);
	const int i0 = sheet(f, a0, a1, b0, b1, c, code, Vector2(a1 - a0, b1 - b0)), i1 = i0 + 1, i2 = i0 + 2, i3 = i0 + 3;
	if (!rooms) return;
	const PaneRoom &r = look.room;
	const float k = 1 / ROOM_SPAN_M;
	const uint32_t code0 = pack(r.back * k, r.ahead * k, r.depth * k, float(r.floor) / BYTE);
	const uint32_t code1 = pack(r.sill * k, r.head * k, float(r.piece + 1) / BYTE, r.piece_h * k);
	const uint32_t code2 = pack(r.piece_at * k * HALF_STEP + HALF_STEP, r.piece_half * k, r.piece_near * k, r.piece_far * k);
	const uint32_t code3 = trim_code(look.trim, look.frame);
	for (int i : { i0, i1, i2, i3 }) {
		custom[0][size_t(i)] = code0;
		custom[1][size_t(i)] = code1;
		custom[2][size_t(i)] = code2;
		custom[3][size_t(i)] = code3;
	}
}

void Shell::far_wall(const Vector3 p[4], const Vector3 &out, const Vector2 cell[4], const Lin &lin, int8_t fin, uint32_t seed, const FarWindows &w) {
	if (glass) {
		glass->grime_floor = grime_floor;
		glass->shade = shade;
		glass->tone = tone;
		glass->far_wall(p, out, cell, lin, fin, seed, w);
		return;
	}
	float fin_seed = float(fin + 1) + float(seed & SEED_BYTE) / (BYTE + 1);
	Vector3 n = (p[1] - p[0]).cross(p[3] - p[0]).normalized();
	if (n.dot(out) < 0) n = -n;
	int base = int(pos.size());
	const Lin tint = far_tint(lin, fin);
	for (int i = 0; i < 4; i++) vert(p[i], n, shaded(p[i], tint, false, SK_FAR_WALL), cell[i], Vector2(fin_seed, 0));
	bool flip = (p[1] - p[0]).cross(p[2] - p[0]).dot(n) > 0;
	const int i1 = base + 1, i2 = base + 2, i3 = base + 3;
	const int t[QUAD_INDICES] = { base, flip ? i2 : i1, flip ? i1 : i2, base, flip ? i3 : i2, flip ? i2 : i3 };
	idx.insert(idx.end(), t, t + QUAD_INDICES);
	if (!rooms) return;
	for (int i = base; i <= i3; i++) {
		custom[0][size_t(i)] = pack(w.width, w.sill, w.head, float(w.flags) / BYTE);
		custom[3][size_t(i)] = trim_code(w.trim, 0);
	}
}

void Shell::bounds(Vector3 &mn, Vector3 &mx) const {
	mn = Vector3(BIG, BIG, BIG);
	mx = -mn;
	for (const Vector3 &p : pos) {
		mn = Vector3(std::min(mn.x, p.x), std::min(mn.y, p.y), std::min(mn.z, p.z));
		mx = Vector3(std::max(mx.x, p.x), std::max(mx.y, p.y), std::max(mx.z, p.z));
	}
}

int64_t Shell::bytes(bool with_uv2) const {
	return int64_t((pos.size() + nrm.size()) * sizeof(Vector3) + col.size() * sizeof(Color) +
			(uv.size() + (with_uv2 ? uv2.size() : 0)) * sizeof(Vector2) + idx.size() * sizeof(int32_t) +
			(rooms ? pos.size() * CUSTOM_ARRAYS * sizeof(uint32_t) : 0));
}

void Shell::commit_to(const RID &mesh, bool with_uv2) const {
	if (idx.empty()) return;
	auto copy = [](auto &dst, const auto &src) {
		dst.resize(int64_t(src.size()));
		std::memcpy(dst.ptrw(), src.data(), src.size() * sizeof(src[0]));
	};
	PackedVector3Array pv, pn;
	PackedColorArray pc;
	PackedVector2Array pu, pu2;
	PackedInt32Array pi;
	copy(pv, pos); copy(pn, nrm); copy(pc, col); copy(pu, uv); copy(pi, idx);
	Array arr;
	arr.resize(RenderingServer::ARRAY_MAX);
	arr[RenderingServer::ARRAY_VERTEX] = pv;
	arr[RenderingServer::ARRAY_NORMAL] = pn;
	arr[RenderingServer::ARRAY_COLOR] = pc;
	arr[RenderingServer::ARRAY_TEX_UV] = pu;
	arr[RenderingServer::ARRAY_INDEX] = pi;
	if (with_uv2) {
		copy(pu2, uv2);
		arr[RenderingServer::ARRAY_TEX_UV2] = pu2;
	}
	uint64_t format = RenderingServer::ARRAY_FLAG_COMPRESS_ATTRIBUTES;
	if (rooms) {
		constexpr int ARRAYS[CUSTOM_ARRAYS] = { RenderingServer::ARRAY_CUSTOM0, RenderingServer::ARRAY_CUSTOM1, RenderingServer::ARRAY_CUSTOM2, RenderingServer::ARRAY_CUSTOM3 };
		constexpr uint64_t SHIFTS[CUSTOM_ARRAYS] = { RenderingServer::ARRAY_FORMAT_CUSTOM0_SHIFT, RenderingServer::ARRAY_FORMAT_CUSTOM1_SHIFT,
			RenderingServer::ARRAY_FORMAT_CUSTOM2_SHIFT, RenderingServer::ARRAY_FORMAT_CUSTOM3_SHIFT };
		for (int i = 0; i < CUSTOM_ARRAYS; i++) {
			PackedByteArray bytes;
			bytes.resize(int64_t(custom[i].size() * sizeof(uint32_t)));
			std::memcpy(bytes.ptrw(), custom[i].data(), custom[i].size() * sizeof(uint32_t));
			arr[ARRAYS[i]] = bytes;
			format |= uint64_t(RenderingServer::ARRAY_CUSTOM_RGBA8_UNORM) << SHIFTS[i];
		}
	}
	RenderingServer::get_singleton()->mesh_add_surface_from_arrays(mesh, RenderingServer::PRIMITIVE_TRIANGLES, arr, Array(), Dictionary(),
			RenderingServer::ArrayFormat(format));
}

} // namespace city::b40
