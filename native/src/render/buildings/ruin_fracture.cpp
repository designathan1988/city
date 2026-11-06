// The cut of a building into pieces (ruin_fracture.h), on a job's thread.
#include "render/buildings/ruin_fracture.h"

#include "render/buildings/ruin_cells.h"
#include "render/buildings/shell.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_map>

using namespace godot;

namespace city {

using namespace ruin;

namespace {
constexpr float LONGEST_SIDE_M = 2.8f; // a face is divided to this before the cells cut it
constexpr int CUTS_MOST = 8;
constexpr float WALL_DEPTH_M = 0.28f; // the thickness given to a wall
constexpr float PLATE_LEAST_M = 0.5f; // a face at least this long is wall: it gets a back and a side along its cuts (the small trim is closed boxes already)
constexpr float TRIM_AREA_M2 = 0.06f; // the bars and frames of a window: most of a near shell's triangles, and nothing to see in a building coming down
constexpr float WELD_PER_M = 200.f;
constexpr int WELD_HALF_BITS = 32;
constexpr float HALF_LEAST_M = 0.12f;
constexpr size_t MOST_TRIANGLES = 600000; // a cut past this stops dividing: the faces go to the cell of their middle
constexpr uint32_t PIECE_BYTE_MASK = 255u, BITS_PER_BYTE = 8u;
const Color BROKEN = Color(0.62f, 0.6f, 0.56f, 0.f); // the grey of broken masonry, on the cut faces; alpha is the surface kind, SK_SHELL
const Color SLAB = Color(0.8f, 0.78f, 0.74f, 0.f);
const Color PANE_DARK = Color(0.09f, 0.12f, 0.15f, 0.f);
const Vector2 CONCRETE = Vector2(float(b40::FIN_CONCRETE) + 1.f, 0.f); // shell.h: UV2.x is the finish + 1
const Vector2 PLAIN = Vector2();

Corner midway(const Corner &a, const Corner &b) {
	return Corner{ (a.p + b.p) * 0.5f, a.n, a.c.lerp(b.c, 0.5f), (a.t + b.t) * 0.5f, a.t2 };
}

float longest_edge(const Tri &t, int &which) {
	float best = 0;
	for (int i = 0; i < 3; i++) {
		const float d = t.v[i].p.distance_squared_to(t.v[(i + 1) % 3].p);
		if (d > best) {
			best = d;
			which = i;
		}
	}
	return std::sqrt(best);
}

// Split along the longest edge until every edge is under `LONGEST_SIDE_M`.
void divide(const Tri &whole, std::vector<Tri> &out, std::vector<Tri> &stack) {
	stack.push_back(whole);
	while (!stack.empty()) {
		const Tri t = stack.back();
		stack.pop_back();
		int i = 0;
		const float edge = longest_edge(t, i);
		if (edge <= LONGEST_SIDE_M || out.size() >= MOST_TRIANGLES) {
			out.push_back(t);
			continue;
		}
		const int j = (i + 1) % 3, k = (i + 2) % 3;
		const Corner m = midway(t.v[i], t.v[j]);
		stack.push_back(Tri{ { t.v[i], m, t.v[k] }, t.sheet });
		stack.push_back(Tri{ { m, t.v[j], t.v[k] }, t.sheet });
	}
}

void gather(const b40::Shell &s, bool panes, std::vector<Tri> &out, std::vector<Tri> &stack) {
	for (size_t i = 0; i + 2 < s.idx.size(); i += 3) {
		Tri t;
		for (int q = 0; q < 3; q++) {
			const size_t at = size_t(s.idx[i + size_t(q)]);
			t.v[q] = panes ? Corner{ s.pos[at], s.nrm[at], PANE_DARK, Vector2(), PLAIN } : Corner{ s.pos[at], s.nrm[at], s.col[at], s.uv[at], s.uv2[at] };
		}
		if ((t.v[1].p - t.v[0].p).cross(t.v[2].p - t.v[0].p).length() * 0.5f < TRIM_AREA_M2) continue;
		int edge = 0;
		t.sheet = !panes && longest_edge(t, edge) >= PLATE_LEAST_M;
		divide(t, out, stack);
	}
}

void gather_slab(const RuinSlab &ring, std::vector<Tri> &out, std::vector<Tri> &stack) {
	auto corner = [](const Vector3 &p) { return Corner{ p, Vector3(0, 1, 0), SLAB, Vector2(), CONCRETE }; };
	for (size_t i = 1; i + 1 < ring.size(); i++) divide(Tri{ { corner(ring[0]), corner(ring[i]), corner(ring[i + 1]) }, true }, out, stack);
}

uint64_t weld(const Vector3 &p) {
	const uint32_t x = uint32_t(int32_t(std::lround(p.x * WELD_PER_M))), y = uint32_t(int32_t(std::lround(p.y * WELD_PER_M))), z = uint32_t(int32_t(std::lround(p.z * WELD_PER_M)));
	return uint64_t(hash_combine(hash_combine(x, y), z)) << WELD_HALF_BITS | uint64_t(x * PRIME_X ^ y * PRIME_Y ^ z * PRIME_Z);
}
uint64_t edge_key(const Vector3 &a, const Vector3 &b) {
	const uint64_t x = weld(a), y = weld(b);
	return x < y ? x * 31u + y : y * 31u + x;
}

// The arrays of the one mesh, a piece at a time.
struct Writer {
	std::vector<Vector3> pos, nrm;
	std::vector<Color> col;
	std::vector<Vector2> uv, uv2;
	std::vector<uint32_t> piece;
	uint32_t id = 0;
	float turn = -1.f; // the sign of a front face's winding against its normal, read from the shell
	Vector3 lo, hi;

	void put(const Corner &c) {
		lo = lo.min(c.p);
		hi = hi.max(c.p);
		pos.push_back(c.p);
		nrm.push_back(c.n);
		col.push_back(c.c);
		uv.push_back(c.t);
		uv2.push_back(c.t2);
		piece.push_back(id);
	}
	void tri(const Corner &a, const Corner &b, const Corner &c) {
		const bool same = (b.p - a.p).cross(c.p - a.p).dot(a.n) * turn >= 0.f;
		put(a);
		put(same ? b : c);
		put(same ? c : b);
	}
	// The wall's thickness behind `t`, and a side along each edge cut (`once`: the edges no other face of the piece has).
	void thicken(const Tri &t, const std::unordered_map<uint64_t, int> &once) {
		Corner back[3];
		for (int q = 0; q < 3; q++) back[q] = Corner{ t.v[q].p - t.v[q].n * WALL_DEPTH_M, -t.v[q].n, BROKEN, t.v[q].t, PLAIN };
		tri(back[0], back[1], back[2]);
		const Vector3 middle = (t.v[0].p + t.v[1].p + t.v[2].p) / 3.f;
		for (int i = 0; i < 3; i++) {
			const int j = (i + 1) % 3;
			const auto seen = once.find(edge_key(t.v[i].p, t.v[j].p));
			if (seen == once.end() || seen->second != 1) continue;
			Vector3 out = (t.v[j].p - t.v[i].p).cross(t.v[i].n).normalized();
			if (out.dot(t.v[i].p - middle) < 0) out = -out;
			const Corner a{ t.v[i].p, out, BROKEN, Vector2(), PLAIN }, b{ t.v[j].p, out, BROKEN, Vector2(), PLAIN };
			const Corner c{ back[j].p, out, BROKEN, Vector2(), PLAIN }, d{ back[i].p, out, BROKEN, Vector2(), PLAIN };
			tri(a, b, c);
			tri(a, c, d);
		}
	}
};

// One cell's triangles written as a piece.
RuinPiece write_piece(Writer &w, const std::vector<Tri> &tris, const uint32_t *order, size_t count, std::unordered_map<uint64_t, int> &edges) {
	edges.clear();
	for (size_t i = 0; i < count; i++) {
		const Tri &t = tris[order[i]];
		if (!t.sheet) continue;
		for (int e = 0; e < 3; e++) edges[edge_key(t.v[e].p, t.v[(e + 1) % 3].p)]++;
	}
	w.lo = tris[order[0]].v[0].p;
	w.hi = w.lo;
	for (size_t i = 0; i < count; i++) {
		const Tri &t = tris[order[i]];
		for (const Corner &c : t.v) w.put(c);
		if (t.sheet) w.thicken(t, edges);
	}
	RuinPiece p;
	p.centre = (w.lo + w.hi) * 0.5f;
	p.half = ((w.hi - w.lo) * 0.5f).max(Vector3(HALF_LEAST_M, HALF_LEAST_M, HALF_LEAST_M));
	p.radius = p.half.length();
	p.low = w.lo.y;
	return p;
}

template <typename T, typename P>
void copy_to(P &dst, const std::vector<T> &src) {
	dst.resize(int64_t(src.size()));
	std::memcpy(dst.ptrw(), src.data(), src.size() * sizeof(T));
}

OwnedMesh bake(const Writer &w) {
	PackedVector3Array pv, pn;
	PackedColorArray pc;
	PackedVector2Array pu, pu2;
	PackedByteArray ids;
	copy_to(pv, w.pos);
	copy_to(pn, w.nrm);
	copy_to(pc, w.col);
	copy_to(pu, w.uv);
	copy_to(pu2, w.uv2);
	ids.resize(int64_t(w.piece.size() * sizeof(uint32_t)));
	uint8_t *out = ids.ptrw();
	for (size_t i = 0; i < w.piece.size(); i++)
		for (uint32_t b = 0; b < sizeof(uint32_t); b++) out[i * sizeof(uint32_t) + b] = uint8_t(w.piece[i] >> (b * BITS_PER_BYTE) & PIECE_BYTE_MASK);
	Array arr;
	arr.resize(RenderingServer::ARRAY_MAX);
	arr[RenderingServer::ARRAY_VERTEX] = pv;
	arr[RenderingServer::ARRAY_NORMAL] = pn;
	arr[RenderingServer::ARRAY_COLOR] = pc;
	arr[RenderingServer::ARRAY_TEX_UV] = pu;
	arr[RenderingServer::ARRAY_TEX_UV2] = pu2;
	arr[RenderingServer::ARRAY_CUSTOM0] = ids;
	const uint64_t format = RenderingServer::ARRAY_FLAG_COMPRESS_ATTRIBUTES | uint64_t(RenderingServer::ARRAY_CUSTOM_RGBA8_UNORM) << RenderingServer::ARRAY_FORMAT_CUSTOM0_SHIFT;
	RenderingServer *rs = RenderingServer::get_singleton();
	OwnedMesh m;
	m.rid = rs->mesh_create();
	rs->mesh_add_surface_from_arrays(m.rid, RenderingServer::PRIMITIVE_TRIANGLES, arr, Array(), Dictionary(), RenderingServer::ArrayFormat(format));
	return m;
}

// The bonds: pieces in cells that touch.
void bond(RuinMesh &out, const Cells &cells, const std::unordered_map<int, int> &piece_of) {
	std::vector<std::pair<int, int>> order(piece_of.begin(), piece_of.end());
	std::sort(order.begin(), order.end(), [](const auto &a, const auto &b) { return a.second < b.second; });
	for (const auto &[at, id] : order) {
		const int k = at % cells.nz, j = at / cells.nz % cells.ny, i = at / (cells.nz * cells.ny);
		RuinPiece &p = out.pieces[size_t(id)];
		p.first_link = int(out.links.size());
		for (int a = std::max(i - 1, 0); a <= std::min(i + 1, cells.nx - 1); a++)
			for (int b = std::max(j - 1, 0); b <= std::min(j + 1, cells.ny - 1); b++)
				for (int c = std::max(k - 1, 0); c <= std::min(k + 1, cells.nz - 1); c++) {
					const auto next = piece_of.find(cells.index(a, b, c));
					if (next != piece_of.end() && next->second != id) out.links.push_back(next->second);
				}
		p.links = int(out.links.size()) - p.first_link;
	}
}

float winding_of(const std::vector<Tri> &tris) {
	for (const Tri &t : tris) {
		const float area = (t.v[1].p - t.v[0].p).cross(t.v[2].p - t.v[0].p).dot(t.v[0].n);
		if (area != 0.f) return area > 0.f ? 1.f : -1.f;
	}
	return -1.f;
}
} // namespace

std::shared_ptr<RuinMesh> fracture_shell(const b40::Shell &walls, const b40::Shell &glass, const std::vector<RuinSlab> &slabs, uint32_t seed) {
	std::vector<Tri> tris, stack;
	gather(walls, false, tris, stack);
	gather(glass, true, tris, stack);
	const float turn = winding_of(tris);
	for (const RuinSlab &ring : slabs) gather_slab(ring, tris, stack);
	auto out = std::make_shared<RuinMesh>();
	if (tris.empty()) return out;
	Vector3 lo = tris[0].v[0].p, hi = lo;
	for (const Tri &t : tris)
		for (const Corner &c : t.v) {
			lo = lo.min(c.p);
			hi = hi.max(c.p);
		}
	const Cells cells(lo, hi, seed);
	std::vector<Tri> parts;
	parts.reserve(tris.size() * 2);
	for (const Tri &t : tris) cut(t, cells, parts, CUTS_MOST);
	std::vector<int> cell_of(parts.size());
	std::vector<uint32_t> order(parts.size());
	for (size_t i = 0; i < parts.size(); i++) {
		cell_of[i] = cells.of((parts[i].v[0].p + parts[i].v[1].p + parts[i].v[2].p) / 3.f);
		order[i] = uint32_t(i);
	}
	std::sort(order.begin(), order.end(), [&](uint32_t a, uint32_t b) { return cell_of[a] != cell_of[b] ? cell_of[a] < cell_of[b] : a < b; });
	Writer w;
	w.turn = turn;
	std::unordered_map<int, int> piece_of;
	std::unordered_map<uint64_t, int> edges;
	out->lo = lo;
	out->hi = hi;
	for (size_t from = 0; from < order.size();) {
		size_t to = from;
		while (to < order.size() && cell_of[order[to]] == cell_of[order[from]]) to++;
		w.id = uint32_t(out->pieces.size());
		piece_of[cell_of[order[from]]] = int(w.id);
		out->pieces.push_back(write_piece(w, parts, order.data() + from, to - from, edges));
		out->hi = out->hi.max(w.hi);
		from = to;
	}
	bond(*out, cells, piece_of);
	out->vertices = int64_t(w.pos.size());
	out->mesh = bake(w);
	return out;
}

} // namespace city
