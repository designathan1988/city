// See ground_seam.h. The paving near a chunk is read from what the road jobs draw (emit_segment, emit_node into a scratch builder),
// united in whole millimetres (Clipper2, as core/polygon_ops.cpp), its outline walked over the terrain's grid for the cells it
// crosses, and each cell to draw cut by it and triangulated (Clipper2 Triangulate, constrained Delaunay on the cut cell's own points).
#include "render/roads/ground_seam.h"

#include "render/owned_segments.h"

#include <iterator> // before Clipper2: its header needs it and does not say so

#include <clipper.h>
#include <clipper.triangulation.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <unordered_set>

using namespace godot;
using Clipper2Lib::FillRule;
using Clipper2Lib::Path64;
using Clipper2Lib::Paths64;
using Clipper2Lib::Point64;
using Clipper2Lib::Rect64;

namespace city {

namespace {
constexpr double MM = 1000; // Clipper2 works in whole millimetres (core/polygon_ops.cpp)
constexpr float UP_FACING = 0.5f; // a paving face's normal leans this far up at least; a kerb's face, a wall, is not paving
// A point of the cut polygon this near a paving triangle lies on it. Each point went through four roundings to the whole millimetre,
// each moving it up to half a millimetre square's diagonal off the line it was on: the triangle's vertex, the union's crossing of two
// faces, the cell's rectangle clip and the square's difference (cell_seam); so it stands up to four half diagonals off the paving.
constexpr double HALF_DIAGONAL_M = 0.7071067811865476 / MM; // sqrt(2) / 2 mm
constexpr int ROUNDINGS = 4;
constexpr double ON_PAVING_M = ROUNDINGS * HALF_DIAGONAL_M;

const Vector3 UP(0, 1, 0);

struct Tri {
	double x[3], z[3], h[3];
};

// The grid of the terrain in whole cells: cell (i, j) spans samples i..i+1, j..j+1.
struct Grid {
	int N = 0;
	double spacing = 0;
	int64_t step_mm = 0;
	double x_of(int i) const { return (double(i) - N * 0.5) * spacing; }
	int64_t mm_of(int i) const { return int64_t(i - N / 2) * step_mm; }
	int cell_of(double x) const { return int(std::floor(x / spacing + N * 0.5)); }
	int cell_of_mm(int64_t x_mm) const { // the cell whose span holds x_mm, its lower line included: exact in whole millimetres
		const int64_t q = x_mm + int64_t(N / 2) * step_mm;
		return int((q >= 0 ? q : q - step_mm + 1) / step_mm);
	}
	bool corner(int64_t x_mm) const { return x_mm % step_mm == 0; }
	int index_of_mm(int64_t x_mm) const { return int(x_mm / step_mm) + N / 2; }
};

int64_t cell_id(int i, int j) { return cell_key(i, j); }

// Everything the seam of a chunk reads: the paving at grade near it (its triangles, filed by cell, and their union) and its cut cells.
struct Paving {
	std::vector<Tri> tris;
	std::unordered_map<int64_t, std::vector<int>> by_cell;
	Paths64 shape; // the union, whole millimetres
	std::unordered_set<int64_t> cut;
};

// The height of triangle t at (x, z) when the point lies on it or within ON_PAVING_M of it in plan (the outline is rounded to whole
// millimetres, so a point of it may stand a hair outside a thin triangle): inside, the triangle's plane; outside, its nearest edge's
// height at the point's foot on it. NaN when the point is farther.
double tri_height(const Tri &t, double x, double z) {
	const double den = (t.z[1] - t.z[2]) * (t.x[0] - t.x[2]) + (t.x[2] - t.x[1]) * (t.z[0] - t.z[2]);
	if (std::fabs(den) <= 0) return std::numeric_limits<double>::quiet_NaN();
	const double wa = ((t.z[1] - t.z[2]) * (x - t.x[2]) + (t.x[2] - t.x[1]) * (z - t.z[2])) / den;
	const double wb = ((t.z[2] - t.z[0]) * (x - t.x[2]) + (t.x[0] - t.x[2]) * (z - t.z[2])) / den;
	const double wc = 1 - wa - wb;
	if (wa >= 0 && wb >= 0 && wc >= 0) return wa * t.h[0] + wb * t.h[1] + wc * t.h[2];
	double nearest = ON_PAVING_M, h = std::numeric_limits<double>::quiet_NaN();
	for (int e = 0; e < 3; e++) { // the point's distance to each edge, in plan (a distance, not a barycentric share of one)
		const int a = e, b = (e + 1) % 3;
		const double ex = t.x[b] - t.x[a], ez = t.z[b] - t.z[a], len2 = ex * ex + ez * ez;
		if (!(len2 > 0)) continue;
		const double s = std::clamp(((x - t.x[a]) * ex + (z - t.z[a]) * ez) / len2, 0.0, 1.0);
		const double d = std::hypot(x - (t.x[a] + ex * s), z - (t.z[a] + ez * s));
		if (d <= nearest) {
			nearest = d;
			h = t.h[a] + (t.h[b] - t.h[a]) * s;
		}
	}
	return h;
}

// The paving's height at (x, z): the highest triangle that holds the point (tri_height); NaN where none does.
double paving_at(const Paving &p, const Grid &g, double x, double z) {
	const auto it = p.by_cell.find(cell_id(g.cell_of(x), g.cell_of(z)));
	double best = std::numeric_limits<double>::quiet_NaN();
	if (it == p.by_cell.end()) return best;
	for (const int k : it->second) {
		const double h = tri_height(p.tris[size_t(k)], x, z);
		if (!std::isnan(h) && !(h <= best)) best = h;
	}
	return best;
}

// The road triangles at grade the segments and nodes near the box draw, up-facing: a deck in the air and a road in a bore are not on
// the terrain, which goes on under and over them.
void read_paving(const RoadEmit &e, const Grid &g, double x0, double z0, double x1, double z1, Paving &p) {
	const CityState &s = e.s;
	float widest = 0;
	for (const Profile &pr : s.catalog.profiles) widest = std::max(widest, pr.half());
	const double reach = double(widest) + g.spacing; // a segment's paving reaches its curve's box by its half at most
	MeshBuilder scratch;
	const RoadEmit scratch_emit{ s, e.L, scratch, e.shapes, false };
	std::unordered_set<int> nodes;
	s.roads.for_segments_near(x0 - reach, z0 - reach, x1 + reach, z1 + reach, [&](int sid) {
		emit_segment(scratch_emit, sid);
		const RoadSegment &sg = s.roads.segs[size_t(sid)];
		nodes.insert(sg.a); // a node's plate lies on its arms' first stretch: within their boxes
		nodes.insert(sg.b);
	});
	for (const int n : nodes)
		if (n >= 0 && s.roads.nodes[size_t(n)].alive) emit_node(scratch_emit, n);
	const float raised = s.catalog.tune.structure_raised_m;
	Paths64 faces;
	for (int64_t k = 0; k + 2 < scratch.idx.size(); k += 3) {
		const Vector3 a = scratch.v[scratch.idx[k]], b = scratch.v[scratch.idx[k + 1]], c = scratch.v[scratch.idx[k + 2]];
		const Vector3 nn = (b - a).cross(c - a);
		const float len = nn.length();
		if (!(len > 0) || std::fabs(nn.y) / len < UP_FACING) continue;
		const Vector3 mid = (a + b + c) / 3.f;
		const float over = mid.y - s.terrain.height_at(mid.x, mid.z);
		if (!(over > 0.f) || over > raised) continue; // in a bore the ground stands over it; on a deck, far under it
		Tri t{ { a.x, b.x, c.x }, { a.z, b.z, c.z }, { a.y, b.y, c.y } };
		const int id = int(p.tris.size());
		p.tris.push_back(t);
		const int i0 = g.cell_of(std::min({ t.x[0], t.x[1], t.x[2] }) - ON_PAVING_M), i1 = g.cell_of(std::max({ t.x[0], t.x[1], t.x[2] }) + ON_PAVING_M);
		const int j0 = g.cell_of(std::min({ t.z[0], t.z[1], t.z[2] }) - ON_PAVING_M), j1 = g.cell_of(std::max({ t.z[0], t.z[1], t.z[2] }) + ON_PAVING_M);
		for (int j = j0; j <= j1; j++)
			for (int i = i0; i <= i1; i++) p.by_cell[cell_id(i, j)].push_back(id);
		Path64 f;
		for (int v = 0; v < 3; v++) f.push_back(Point64(int64_t(std::llround(t.x[v] * MM)), int64_t(std::llround(t.z[v] * MM))));
		if (Clipper2Lib::Area(f) < 0) std::reverse(f.begin(), f.end());
		faces.push_back(std::move(f));
	}
	p.shape = Clipper2Lib::Union(faces, FillRule::NonZero);
}

// The cells an outline edge crosses (its own cells and every cell it touches, sides and corners included), in the grid's cells. The
// edge's ends and the grid's lines are whole millimetres, so the test is exact in integers (no tolerance to choose).
void cells_of_edge(const Grid &g, const Point64 &a, const Point64 &b, std::unordered_set<int64_t> &out) {
	const int64_t dx = b.x - a.x, dz = b.y - a.y;
	// the side of the edge's line a point is on, times its length (0 on the line)
	auto side = [&](int64_t x, int64_t z) { return dx * (z - a.y) - dz * (x - a.x); };
	const int i0 = g.cell_of_mm(std::min(a.x, b.x)), i1 = g.cell_of_mm(std::max(a.x, b.x));
	const int j0 = g.cell_of_mm(std::min(a.y, b.y)), j1 = g.cell_of_mm(std::max(a.y, b.y));
	// an end on a cell's lower line touches the cell below it too
	for (int j = j0 - 1; j <= j1; j++)
		for (int i = i0 - 1; i <= i1; i++) {
			const int64_t x0 = g.mm_of(i), x1 = g.mm_of(i + 1), z0 = g.mm_of(j), z1 = g.mm_of(j + 1);
			if (std::max(a.x, b.x) < x0 || std::min(a.x, b.x) > x1 || std::max(a.y, b.y) < z0 || std::min(a.y, b.y) > z1) continue;
			// the edge's line meets the cell's square (sides included): its corners are not all strictly on one side of it
			const int64_t s0 = side(x0, z0), s1 = side(x1, z0), s2 = side(x1, z1), s3 = side(x0, z1);
			if (std::min({ s0, s1, s2, s3 }) > 0 || std::max({ s0, s1, s2, s3 }) < 0) continue;
			out.insert(cell_id(i, j));
		}
}

// One cell's seam: its square less the paving, triangulated; each triangle's corners in metres with their heights, and whether each
// is a grid corner. False where the triangulation refused.
struct SeamTri {
	Vector3 p[3];
	bool corner[3];
};
bool cell_seam(const CityState &s, const Grid &g, const Paving &pv, int i, int j, std::vector<SeamTri> &out, int &unheighted, Vector3 &lost) {
	const int64_t x0 = g.mm_of(i), z0 = g.mm_of(j), x1 = g.mm_of(i + 1), z1 = g.mm_of(j + 1);
	const Paths64 square{ Path64{ Point64(x0, z0), Point64(x1, z0), Point64(x1, z1), Point64(x0, z1) } };
	const Paths64 near = Clipper2Lib::RectClip(Rect64(x0 - 1, z0 - 1, x1 + 1, z1 + 1), pv.shape);
	const Paths64 ground = near.empty() ? square : Clipper2Lib::Difference(square, near, FillRule::NonZero);
	if (ground.empty()) return true; // the cell lies under the paving
	// The height of every point of the cut polygon: a grid corner the terrain's, a point of the outline the paving's.
	auto own_height = [&](const Point64 &q) {
		if (g.corner(q.x) && g.corner(q.y)) return double(s.terrain.final_at(g.index_of_mm(q.x), g.index_of_mm(q.y)));
		return paving_at(pv, g, double(q.x) / MM, double(q.y) / MM);
	};
	std::vector<std::vector<double>> heights(ground.size());
	for (size_t r = 0; r < ground.size(); r++)
		for (const Point64 &q : ground[r]) heights[r].push_back(own_height(q));
	// A point of a triangle: one of the polygon's own, or one the triangulation put on a side of it (Clipper2's SplitEdge and
	// FixupEdgeIntersects, clipper.triangulation.cpp): the seam's edge is straight between the polygon's points, so such a point takes
	// the height of that side where it stands. NaN for a point on no side (counted, never covered by the terrain's height).
	auto height_of = [&](const Point64 &q) {
		for (size_t r = 0; r < ground.size(); r++) {
			const Path64 &ring = ground[r];
			for (size_t k = 0; k < ring.size(); k++) {
				const Point64 &a = ring[k], &b = ring[(k + 1) % ring.size()];
				if (q == a) return heights[r][k];
				const double ex = double(b.x - a.x), ez = double(b.y - a.y), len2 = ex * ex + ez * ez;
				if (!(len2 > 0)) continue;
				const double t = (double(q.x - a.x) * ex + double(q.y - a.y) * ez) / len2;
				if (t <= 0 || t >= 1) continue;
				const double off = std::hypot(double(q.x - a.x) - ex * t, double(q.y - a.y) - ez * t) / MM;
				// put there by one rounding of a crossing of two of its sides
				if (off <= HALF_DIAGONAL_M) return heights[r][k] + (heights[r][(k + 1) % ring.size()] - heights[r][k]) * t;
			}
		}
		return std::numeric_limits<double>::quiet_NaN();
	};
	Paths64 tris;
	if (Clipper2Lib::Triangulate(ground, tris, true) != Clipper2Lib::TriangulateResult::success) return false;
	for (const Path64 &t : tris) {
		if (t.size() != 3) continue;
		SeamTri st;
		bool whole = true;
		for (int v = 0; v < 3; v++) {
			const Point64 &q = t[size_t(v)];
			st.corner[v] = g.corner(q.x) && g.corner(q.y);
			const double h = height_of(q);
			if (std::isnan(h)) {
				whole = false;
				lost = Vector3(float(double(q.x) / MM), float(ground.size()), float(double(q.y) / MM));
			}
			st.p[v] = Vector3(float(double(q.x) / MM), float(h), float(double(q.y) / MM));
		}
		if (!whole) { // a point no side of the polygon holds: the probe reports it, and the triangle is not drawn half known
			unheighted++;
			continue;
		}
		out.push_back(st);
	}
	return true;
}

// Everything chunk `key` draws and holes, for the job and the probe alike.
struct ChunkSeam {
	std::vector<SeamTri> tris;
	std::vector<int64_t> cells, cut;
	int refused = 0, unheighted = 0;
	Vector3 lost; // the last point no side held
	Paving paving;
};

void build_chunk(const RoadEmit &e, int64_t key, ChunkSeam &cs, Grid &g) {
	const Heightfield &hf = e.s.terrain;
	g.N = hf.N;
	g.spacing = double(hf.spacing);
	g.step_mm = int64_t(std::llround(g.spacing * MM));
	if (g.N < 2 || hf.final_h.empty()) return;
	const double x0 = double(key_x(key)) * CHUNK, z0 = double(key_y(key)) * CHUNK, x1 = x0 + CHUNK, z1 = z0 + CHUNK;
	const double margin = g.spacing; // a cut cell the chunk owns can reach past its edge by a cell
	read_paving(e, g, x0 - margin, z0 - margin, x1 + margin, z1 + margin, cs.paving);
	for (const Path64 &ring : cs.paving.shape)
		for (size_t k = 0; k < ring.size(); k++) cells_of_edge(g, ring[k], ring[(k + 1) % ring.size()], cs.paving.cut);
	auto owned = [&](int i, int j) { return chunk_of(float(g.x_of(i) + g.spacing / 2), float(g.x_of(j) + g.spacing / 2)) == key; };
	auto inside = [&](int i, int j) { return i >= 0 && j >= 0 && i < g.N - 1 && j < g.N - 1; };
	// The cut cells the chunk owns: each drawn once, by the chunk of its middle (the terrain under it stays whole and lower).
	for (const int64_t c : cs.paving.cut) {
		const int i = key_x(c), j = key_y(c);
		if (inside(i, j) && owned(i, j)) cs.cut.push_back(c);
	}
	std::sort(cs.cut.begin(), cs.cut.end());
	cs.cells = cs.cut;
	for (const int64_t c : cs.cells)
		if (!cell_seam(e.s, g, cs.paving, key_x(c), key_y(c), cs.tris, cs.unheighted, cs.lost)) cs.refused++;
}
} // namespace

void emit_ground_seam(const RoadEmit &e, int64_t key, SeamResult &result) {
	ChunkSeam cs;
	Grid g;
	build_chunk(e, key, cs, g);
	result.refused = cs.refused;
	// One vertex a point, its normal the area-weighted sum of its triangles' (a corner's is the terrain's, read by the shader).
	std::unordered_map<int64_t, int> at;
	std::vector<Vector3> pos, nor;
	std::vector<bool> corner;
	std::vector<int> index;
	for (const SeamTri &t : cs.tris) {
		const Vector3 face = (t.p[1] - t.p[0]).cross(t.p[2] - t.p[0]);
		const Vector3 up = face.y < 0 ? -face : face;
		for (int v = 0; v < 3; v++) {
			const int64_t k = cell_key(int(std::llround(double(t.p[v].x) * MM)), int(std::llround(double(t.p[v].z) * MM)));
			auto it = at.find(k);
			if (it == at.end()) {
				it = at.emplace(k, int(pos.size())).first;
				pos.push_back(t.p[v]);
				nor.push_back(Vector3());
				corner.push_back(t.corner[v]);
			}
			nor[size_t(it->second)] += up;
			index.push_back(it->second);
		}
	}
	std::vector<int> ids(pos.size());
	for (size_t k = 0; k < pos.size(); k++) {
		const Vector3 n = nor[k].length_squared() > 0 ? nor[k].normalized() : UP;
		ids[k] = e.mb.add(pos[k], n, Color(1, 1, 1), Vector2(pos[k].x, pos[k].z), Vector2(corner[k] ? SEAM_CORNER : 0.f, 0.f));
	}
	for (size_t k = 0; k + 2 < index.size(); k += 3) e.mb.tri(ids[size_t(index[k])], ids[size_t(index[k + 1])], ids[size_t(index[k + 2])], UP);
	each_owned_segment(e.s, key, [&](int sid) { emit_wall_backfill(e, sid); });
}

SeamProbe probe_ground_seam(const CityState &s, const StreetLook &L, RoadShapes &shapes, int64_t key) {
	MeshBuilder unused;
	const RoadEmit e{ s, L, unused, shapes, false };
	ChunkSeam cs;
	Grid g;
	build_chunk(e, key, cs, g);
	SeamProbe p;
	p.cells = int(cs.cells.size());
	p.cut = int(cs.cut.size());
	p.triangles = int(cs.tris.size());
	p.refused = cs.refused;
	p.unheighted = cs.unheighted;
	p.lost = cs.lost;
	// Each drawn cell: the seam's area and the paving's inside it make the cell's, or the seam leaves a gap or covers the road.
	std::unordered_map<int64_t, double> seam_area;
	for (const SeamTri &t : cs.tris) {
		const Vector3 m = (t.p[0] + t.p[1] + t.p[2]) / 3.f;
		const double a = std::fabs(double((t.p[1].x - t.p[0].x) * (t.p[2].z - t.p[0].z) - (t.p[2].x - t.p[0].x) * (t.p[1].z - t.p[0].z))) / 2;
		seam_area[cell_id(g.cell_of(m.x), g.cell_of(m.z))] += a;
	}
	const double cell_area = g.spacing * g.spacing;
	for (const int64_t c : cs.cells) {
		const int i = key_x(c), j = key_y(c);
		const Paths64 square{ Path64{ Point64(g.mm_of(i), g.mm_of(j)), Point64(g.mm_of(i + 1), g.mm_of(j)), Point64(g.mm_of(i + 1), g.mm_of(j + 1)), Point64(g.mm_of(i), g.mm_of(j + 1)) } };
		double paved = 0;
		for (const Path64 &r : Clipper2Lib::Intersect(square, cs.paving.shape, FillRule::NonZero)) paved += Clipper2Lib::Area(r) / (MM * MM);
		const auto it = seam_area.find(c);
		const double seam = it == seam_area.end() ? 0 : it->second;
		p.worst_cover_m2 = std::max(p.worst_cover_m2, std::fabs(seam + std::fabs(paved) - cell_area));
	}
	return p;
}

} // namespace city
