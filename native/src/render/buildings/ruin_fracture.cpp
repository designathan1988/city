// The cut of a building into pieces (ruin_fracture.h), on a job's thread.
#include "render/buildings/ruin_fracture.h"

#include "render/buildings/ruin_cells.h"
#include "render/buildings/shell.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <unordered_map>

using namespace godot;

namespace city {

using namespace ruin;

namespace {
constexpr float LONGEST_SIDE_M = 2.8f; // a face is divided to this before the cells cut it
constexpr int CUTS_MOST = 8; // the most cells a piece is cut into
constexpr float PLATE_LEAST_M = 0.5f; // a face at least this long is wall: it gets a back and a side along its cuts (the small trim is closed boxes already)
constexpr float TRIM_AREA_M2 = 0.06f; // the bars and frames of a window: most of a near shell's triangles, and nothing to see in a building coming down
// Vertices weld at half centimetres, a welded point packed in two 32-bit halves; a cut piece is at least HALF_LEAST_M a side.
constexpr float WELD_PER_M = 200.f;
constexpr int WELD_HALF_BITS = 32;
constexpr float HALF_LEAST_M = 0.12f;
constexpr size_t MOST_TRIANGLES = 600000; // a cut past this stops dividing: the faces go to the cell of their middle
constexpr uint32_t PIECE_BYTE_MASK = 255u, BITS_PER_BYTE = 8u; // a piece's id in a byte
constexpr uint64_t EDGE_MIX = 31u; // an edge's key from its two welded ends (Java's String.hashCode multiplier)
const Color BROKEN = Color(0.62f, 0.6f, 0.56f, 0.f); // the grey of broken masonry, on the cut faces; alpha is the surface kind, SK_SHELL
const Color SLAB = Color(0.8f, 0.78f, 0.74f, 0.f); // a slab's cut face
const Color PANE_DARK = Color(0.09f, 0.12f, 0.15f, 0.f); // a pane's broken face
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

// Split along the longest edge until every edge is under `VORONOI_EDGE_M`, each piece handed to `emit` as it comes; `made` counts
// the pieces of the whole cut: past MOST_TRIANGLES a face goes on undivided.
template <typename Emit>
void divide(const Tri &whole, std::vector<Tri> &stack, size_t &made, Emit &&emit) {
	stack.push_back(whole);
	while (!stack.empty()) {
		const Tri t = stack.back();
		stack.pop_back();
		int i = 0;
		const float edge = longest_edge(t, i);
		if (edge <= VORONOI_EDGE_M || made >= MOST_TRIANGLES) {
			made++;
			emit(t);
			continue;
		}
		const int j = (i + 1) % 3, k = (i + 2) % 3;
		const Corner m = midway(t.v[i], t.v[j]);
		stack.push_back(Tri{ { t.v[i], m, t.v[k] }, t.sheet });
		stack.push_back(Tri{ { m, t.v[j], t.v[k] }, t.sheet });
	}
}

// fn(face) for every face of the shell that is kept (the window trim is not), as it is drawn; the panes dark.
template <typename F>
void each_face(const b40::Shell &s, bool panes, F &&fn) {
	for (size_t i = 0; i + 2 < s.idx.size(); i += 3) {
		Tri t;
		for (int q = 0; q < 3; q++) {
			const size_t at = size_t(s.idx[i + size_t(q)]);
			t.v[q] = panes ? Corner{ s.pos[at], s.nrm[at], PANE_DARK, Vector2(), PLAIN } : Corner{ s.pos[at], s.nrm[at], s.col[at], s.uv[at], s.uv2[at] };
		}
		if ((t.v[1].p - t.v[0].p).cross(t.v[2].p - t.v[0].p).length() * 0.5f < TRIM_AREA_M2) continue;
		int edge = 0;
		t.sheet = !panes && longest_edge(t, edge) >= PLATE_LEAST_M;
		fn(t);
	}
}

template <typename F>
void each_slab_face(const RuinSlab &ring, F &&fn) {
	auto corner = [](const Vector3 &p) { return Corner{ p, Vector3(0, 1, 0), SLAB, Vector2(), CONCRETE }; };
	for (size_t i = 1; i + 1 < ring.size(); i++) fn(Tri{ { corner(ring[0]), corner(ring[i]), corner(ring[i + 1]) }, true });
}

uint64_t weld(const Vector3 &p) {
	const uint32_t x = uint32_t(int32_t(std::lround(p.x * WELD_PER_M))), y = uint32_t(int32_t(std::lround(p.y * WELD_PER_M))), z = uint32_t(int32_t(std::lround(p.z * WELD_PER_M)));
	return uint64_t(hash_combine(hash_combine(x, y), z)) << WELD_HALF_BITS | uint64_t(x * PRIME_X ^ y * PRIME_Y ^ z * PRIME_Z);
}
uint64_t edge_key(const Vector3 &a, const Vector3 &b) {
	const uint64_t x = weld(a), y = weld(b);
	return x < y ? x * EDGE_MIX + y : y * EDGE_MIX + x;
}

// The arrays of the one mesh, sized once (a pass counts every piece's corners first), written a piece at a time.
struct Writer {
	Vector3 *pos = nullptr, *nrm = nullptr;
	Color *col = nullptr;
	Vector2 *uv = nullptr, *uv2 = nullptr;
	uint8_t *piece = nullptr; // each corner's piece, four bytes little endian (CUSTOM0)
	size_t at = 0;
	uint32_t id = 0;
	float turn = -1.f; // the sign of a front face's winding against its normal, read from the shell
	float depth = RUIN_WALL_DEPTH_M; // how far behind a wall face its back lies; 0: in its plane, with no sides
	Vector3 lo, hi;

	void put(const Corner &c) {
		lo = lo.min(c.p);
		hi = hi.max(c.p);
		pos[at] = c.p;
		nrm[at] = c.n;
		col[at] = c.c;
		uv[at] = c.t;
		uv2[at] = c.t2;
		for (uint32_t b = 0; b < sizeof(uint32_t); b++) piece[at * sizeof(uint32_t) + b] = uint8_t(id >> (b * BITS_PER_BYTE) & PIECE_BYTE_MASK);
		at++;
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
		for (int q = 0; q < 3; q++) back[q] = Corner{ t.v[q].p - t.v[q].n * depth, -t.v[q].n, BROKEN, t.v[q].t, PLAIN };
		tri(back[0], back[1], back[2]);
		if (depth <= 0.f) return;
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

constexpr size_t FACE_CORNERS = 3, SIDE_CORNERS = 6; // a face, and a side along a cut edge (two faces)

// How many times each edge of a piece's wall faces is shared: an edge on one face only was cut, and gets a side.
void count_edges(const std::vector<Tri> &tris, std::unordered_map<uint64_t, int> &edges) {
	edges.clear();
	for (const Tri &t : tris) {
		if (!t.sheet) continue;
		for (int e = 0; e < 3; e++) edges[edge_key(t.v[e].p, t.v[(e + 1) % 3].p)]++;
	}
}

// The corners a piece writes: each face, and for a wall face its back and (`sides`) a side along each edge cut.
size_t corners_of(const std::vector<Tri> &tris, std::unordered_map<uint64_t, int> &edges, bool sides) {
	count_edges(tris, edges);
	size_t n = 0;
	for (const Tri &t : tris) {
		n += FACE_CORNERS;
		if (!t.sheet) continue;
		n += FACE_CORNERS;
		for (int e = 0; sides && e < 3; e++) {
			const auto seen = edges.find(edge_key(t.v[e].p, t.v[(e + 1) % 3].p));
			if (seen != edges.end() && seen->second == 1) n += SIDE_CORNERS;
		}
	}
	return n;
}

// One cell's triangles written as a piece.
RuinPiece write_piece(Writer &w, const std::vector<Tri> &tris, std::unordered_map<uint64_t, int> &edges) {
	count_edges(tris, edges);
	w.lo = tris.front().v[0].p;
	w.hi = w.lo;
	for (const Tri &t : tris) {
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

OwnedMesh bake(const PackedVector3Array &pv, const PackedVector3Array &pn, const PackedColorArray &pc, const PackedVector2Array &pu,
		const PackedVector2Array &pu2, const PackedByteArray &ids) {
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

// The sign of a face's winding against its normal, 0 when the face has no area.
float winding(const Tri &t) {
	const float area = (t.v[1].p - t.v[0].p).cross(t.v[2].p - t.v[0].p).dot(t.v[0].n);
	return area > 0.f ? 1.f : area < 0.f ? -1.f : 0.f;
}
} // namespace

// Streamed: the faces' box first (a face divided stays in the box of its corners), then each face divided, each piece of it cut
// along the cells and filed by cell in the order made; the cells written in ascending order straight into the mesh's arrays,
// each freed once written. The same pieces in the same order as dividing every face, cutting every piece and sorting by cell.
std::shared_ptr<RuinMesh> fracture_shell(const b40::Shell &walls, const b40::Shell &glass, const std::vector<RuinSlab> &slabs, uint32_t seed,
		const b40::Shell *sided) {
	auto out = std::make_shared<RuinMesh>();
	const bool own_depth = sided != nullptr;
	const auto no_back = [](auto &&fn) { return [fn](Tri t) { t.sheet = false; fn(t); }; }; // a face drawn with its other side
	bool any = false;
	Vector3 lo, hi;
	const auto bound = [&](const Tri &t) {
		for (const Corner &c : t.v) {
			lo = any ? lo.min(c.p) : c.p;
			hi = any ? hi.max(c.p) : c.p;
			any = true;
		}
	};
	each_face(walls, false, bound);
	each_face(glass, true, bound);
	if (sided) each_face(*sided, false, bound);
	for (const RuinSlab &ring : slabs) each_slab_face(ring, bound);
	if (!any) return out;
	const Cells cells(lo, hi, seed);
	std::unordered_map<int, std::vector<Tri>> by_cell;
	std::vector<Tri> stack, cuts;
	size_t made = 0;
	float turn = 0; // the winding of the first wall or pane piece with an area (-1 when none has)
	const auto file = [&](const Tri &face, bool shell) {
		divide(face, stack, made, [&](const Tri &t) {
			if (shell && turn == 0) turn = winding(t);
			cuts.clear();
			cut(t, cells, cuts, CUTS_MOST);
			for (const Tri &part : cuts) by_cell[cells.of((part.v[0].p + part.v[1].p + part.v[2].p) / 3.f)].push_back(part);
		});
	};
	each_face(walls, false, [&](const Tri &t) { file(t, true); });
	each_face(glass, true, [&](const Tri &t) { file(t, true); });
	if (sided) each_face(*sided, false, no_back([&](const Tri &t) { file(t, true); }));
	for (const RuinSlab &ring : slabs) each_slab_face(ring, [&](const Tri &t) { file(t, false); });
	std::vector<int> order;
	order.reserve(by_cell.size());
	for (const auto &kv : by_cell) order.push_back(kv.first);
	std::sort(order.begin(), order.end());
	std::unordered_map<uint64_t, int> edges;
	size_t corners = 0;
	for (int c : order) corners += corners_of(by_cell[c], edges, !own_depth);
	PackedVector3Array pv, pn;
	PackedColorArray pc;
	PackedVector2Array pu, pu2;
	PackedByteArray ids;
	pv.resize(int64_t(corners));
	pn.resize(int64_t(corners));
	pc.resize(int64_t(corners));
	pu.resize(int64_t(corners));
	pu2.resize(int64_t(corners));
	ids.resize(int64_t(corners * sizeof(uint32_t)));
	Writer w;
	w.pos = pv.ptrw();
	w.nrm = pn.ptrw();
	w.col = pc.ptrw();
	w.uv = pu.ptrw();
	w.uv2 = pu2.ptrw();
	w.piece = ids.ptrw();
	w.turn = turn == 0 ? -1.f : turn;
	w.depth = own_depth ? 0.f : RUIN_WALL_DEPTH_M;
	std::unordered_map<int, int> piece_of;
	out->lo = lo;
	out->hi = hi;
	for (int c : order) {
		std::vector<Tri> &tris = by_cell[c];
		w.id = uint32_t(out->pieces.size());
		piece_of[c] = int(w.id);
		out->pieces.push_back(write_piece(w, tris, edges));
		out->hi = out->hi.max(w.hi);
		std::vector<Tri>().swap(tris); // written: freed
	}
	bond(*out, cells, piece_of);
	out->vertices = int64_t(corners);
	out->mesh = bake(pv, pn, pc, pu, pu2, ids);
	return out;
}

} // namespace city
