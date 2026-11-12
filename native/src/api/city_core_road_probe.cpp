// The road surfaces of the whole city built on the spot as the road chunks build them (render/roads/road_mesh.h emit_segment,
// emit_node: each segment's section, each node's plate, corners and ends; the paint left out, it lies on the asphalt by design), and
// their top faces looked at two by two (CityProbes::road_overlaps): two tops at one height over the same ground stand in one another,
// and whichever is drawn last shows through the other (a corner's paving over an arm's planting strip, two arms' footways over one
// corner). How many pairs, how much ground, and where the first and a few more lie, for the photos to look at; and how many junction
// corners there are and how many of their footway outlines cross themselves; and the asphalt's wear as those meshes carry it (Wear:
// a junction's paving in each arm's own colour on its mouth and at nought from its kerb, a lane across itself and off a mouth).
#include "api/city_probes.h"

#include "render/mesh_builder.h"
#include "render/render_world.h"
#include "render/roads/road_mesh.h"
#include "render/roads/road_node.h"
#include "sim/state/city_state.h"
#include "sim/terrain/height_patch.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <algorithm>
#include <cmath>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float UP_DOT = 0.99f; // a face whose normal stands this near up is a top
constexpr float SAME_HEIGHT_M = 0.02f; // two tops nearer than this in height over one point stand at one height
constexpr float OVERLAP_M2 = 0.01f; // a shared area under this is two faces touching along an edge
constexpr float CELL_M = 4.f; // the tops are filed in a grid of this cell to find the pairs that may meet
constexpr int PLACES = 8; // the overlaps whose place the probe tells
constexpr float THIRD = 1.f / 3.f, HALF = 0.5f, FLAT = 1e-9f;

struct Top {
	V2 p[3];
	float y[3];
	float x0, z0, x1, z1;
	int from = 0; // what drew it: a segment's id, or -1 - a node's
};

// What drew a top, as the report names a pair: "segment" or "node".
const char *source(int from) { return from >= 0 ? "segment" : "node"; }

using Poly = std::vector<V2>;

// The part of convex polygon `a` inside triangle `t` (either winding): Sutherland-Hodgman against each of its edges.
Poly clip(Poly a, const V2 (&t)[3]) {
	const float wind = (t[1] - t[0]).cross(t[2] - t[0]) >= 0 ? 1.f : -1.f;
	for (int e = 0; e < 3 && !a.empty(); e++) {
		const V2 p = t[e], q = t[(e + 1) % 3];
		auto in = [&](const V2 &v) { return (q - p).cross(v - p) * wind >= 0; };
		Poly out;
		for (size_t i = 0; i < a.size(); i++) {
			const V2 &c = a[i], &n = a[(i + 1) % a.size()];
			const bool ci = in(c), ni = in(n);
			if (ci) out.push_back(c);
			if (ci != ni) {
				const float dc = (q - p).cross(c - p), dn = (q - p).cross(n - p);
				out.push_back(c + (n - c) * (dc / (dc - dn)));
			}
		}
		a = std::move(out);
	}
	return a;
}

float area_of(const Poly &a) {
	float s = 0;
	for (size_t i = 0; i < a.size(); i++) s += a[i].cross(a[(i + 1) % a.size()]);
	return std::fabs(s) * HALF;
}

V2 centre_of(const Poly &a) {
	V2 c(0, 0);
	for (const V2 &v : a) c = c + v;
	return c / float(a.size());
}

// The top's height over `at` (inside it, or on its plane past it), by its barycentric weights.
float height_at(const Top &t, const V2 &at) {
	const V2 e1 = t.p[1] - t.p[0], e2 = t.p[2] - t.p[0], d = at - t.p[0];
	const float den = e1.cross(e2);
	if (std::fabs(den) < FLAT) return (t.y[0] + t.y[1] + t.y[2]) * THIRD;
	const float b = d.cross(e2) / den, c = e1.cross(d) / den;
	return t.y[0] + (t.y[1] - t.y[0]) * b + (t.y[2] - t.y[0]) * c;
}

// Every top face among a mesh builder's triangles from index `from_idx` on, each marked with what drew it.
void tops_of(const MeshBuilder &mb, int64_t from_idx, int from, std::vector<Top> &out) {
	for (int64_t i = from_idx; i + 2 < mb.idx.size(); i += 3) {
		const Vector3 ps[3] = { mb.v[mb.idx[i]], mb.v[mb.idx[i + 1]], mb.v[mb.idx[i + 2]] };
		const Vector3 nn = (ps[1] - ps[0]).cross(ps[2] - ps[0]);
		if (nn.length() <= 0 || std::fabs(nn.normalized().y) < UP_DOT) continue;
		Top t;
		for (int k = 0; k < 3; k++) {
			t.p[k] = V2(ps[k].x, ps[k].z);
			t.y[k] = ps[k].y;
		}
		t.x0 = std::min({ ps[0].x, ps[1].x, ps[2].x });
		t.x1 = std::max({ ps[0].x, ps[1].x, ps[2].x });
		t.z0 = std::min({ ps[0].z, ps[1].z, ps[2].z });
		t.z1 = std::max({ ps[0].z, ps[1].z, ps[2].z });
		t.from = from;
		out.push_back(t);
	}
}

constexpr int CELL_BITS = 32; // a cell's x in the key's high half, its z in the low half
int64_t top_cell(int cx, int cz) { return (int64_t(cx) << CELL_BITS) ^ int64_t(uint32_t(cz)); }
int cell_of(float v) { return int(std::floor(v / CELL_M)); }

constexpr float GROUND_OVER_M = 0.01f; // the ground stands over a top by more than its heights' quantum: it shows through

// Whether the ground stands over a top: the final heights (the drawn ground; a top is no lot, so no lot's own level is asked) at its
// corners, bilinear, and at every height sample under it, against the top's own height there. `at` is where, `by` how far.
bool ground_over(const Top &t, const Heightfield &g, V2 &at, float &by) {
	const auto final_at = [&](int ix, int iz) { return g.final_at(ix, iz); };
	for (int k = 0; k < 3; k++) {
		const float ground = grid_sample(t.p[k].x, t.p[k].y, g.N, g.spacing, final_at);
		if (ground > t.y[k] + GROUND_OVER_M) {
			at = t.p[k];
			by = ground - t.y[k];
			return true;
		}
	}
	const float wind = (t.p[1] - t.p[0]).cross(t.p[2] - t.p[0]) >= 0 ? 1.f : -1.f;
	const int ix0 = std::max(0, int(std::ceil(t.x0 / g.spacing + g.N * HALF))), ix1 = std::min(g.N - 1, int(std::floor(t.x1 / g.spacing + g.N * HALF)));
	const int iz0 = std::max(0, int(std::ceil(t.z0 / g.spacing + g.N * HALF))), iz1 = std::min(g.N - 1, int(std::floor(t.z1 / g.spacing + g.N * HALF)));
	for (int iz = iz0; iz <= iz1; iz++)
		for (int ix = ix0; ix <= ix1; ix++) {
			const V2 p(g.x_of(ix), g.x_of(iz));
			bool inside = true;
			for (int e = 0; e < 3; e++) inside = inside && (t.p[(e + 1) % 3] - t.p[e]).cross(p - t.p[e]) * wind >= 0;
			if (!inside || g.final_at(ix, iz) <= height_at(t, p) + GROUND_OVER_M) continue;
			at = p;
			by = g.final_at(ix, iz) - height_at(t, p);
			return true;
		}
	return false;
}

// The tops the ground stands over: how many, the ground they cover, by what drew them, and the first.
struct Buried {
	int tops = 0;
	double ground = 0;
	Dictionary by;
	Array first;
	void count(const Top &t, const Heightfield &g) {
		V2 at;
		float over = 0;
		if (!ground_over(t, g, at, over)) return;
		tops++;
		ground += std::fabs((t.p[1] - t.p[0]).cross(t.p[2] - t.p[0])) * HALF;
		by[source(t.from)] = int(by.get(source(t.from), 0)) + 1;
		if (first.is_empty()) first = Array::make(at.x, at.y, over, t.from);
	}
};

constexpr float MM = 1000.f;
constexpr float ON_M = 2.f / MM, ON_Y_M = 0.01f; // a vertex this near a line in plan, and at its height, stands on it
constexpr float WEAR_AT_M = 0.06f; // a wear coordinate within this of a value is at it (a colour channel holds a tenth of a metre)
constexpr float OFF_MOUTH_M = 1.f; // a lane's vertex farther than this from a mouth stands off it
constexpr float TONE_STEP = 1.5f / 255.f; // two colours farther apart than this in a channel are two tones on the screen

// How far `p` stands from the stretch a-b, in plan.
float off_stretch(const Vector3 &p, const Vector3 &a, const Vector3 &b) { return dist_point_segment(V2(p.x, p.z), V2(a.x, a.z), V2(b.x, b.z)); }

// The asphalt's wear as the meshes carry it (road_look.h), counted on the vertices each segment and each junction just wrote.
struct Wear {
	int mouths = 0, mouth_points = 0, mouth_off = 0, split = 0, kerb_points = 0, kerb_off = 0;
	int lane_points = 0, lane_off = 0, lane_wide = 0, lane_past_mouth = 0, bay_points = 0, bay_inner = 0;
	Array first_off;

	static bool top(const MeshBuilder &mb, int64_t i, RoadMat m) { return mb.n[i].y > UP_DOT && int(std::floor(mb.c[i].a * MAT_SCALE)) == int(m); }

	// A segment's lanes (across themselves, off a mouth) and its other asphalt (off the kerb).
	void segment(const MeshBuilder &mb, int64_t from) {
		for (int64_t i = from; i < mb.v.size(); i++) {
			const Vector2 w = mb.uv2[i];
			if (top(mb, i, RoadMat::LANE)) {
				const bool wide = w.x >= mb.c[i].b * WEAR_LANE_M - WEAR_AT_M;
				lane_points++;
				lane_wide += wide;
				lane_off += w.x < -WEAR_AT_M;
				lane_past_mouth += w.y > OFF_MOUTH_M;
			} else if (top(mb, i, RoadMat::ASPHALT)) {
				bay_points++;
				bay_inner += w.x > WEAR_AT_M;
			}
		}
	}

	// A junction's paving: its vertices on an arm's mouth, on a corner's kerb line, and those that share a point.
	void node(const RoadEmit &e, int n, int64_t from) {
		const std::vector<ArmEnd> ends = arm_ends(e, n);
		if (ends.size() < 2 || !plated(ends)) return;
		const RoadNode &nd = e.s.roads.nodes[size_t(n)];
		mouths += int(ends.size());
		std::vector<Vector3> kerb;
		for (size_t k = 0; k < ends.size(); k++)
			for (const V2 &p : corner_of(e, ends[k], ends[(k + 1) % ends.size()]).kerb) kerb.push_back(Vector3(float(nd.x) + p.x, 0, float(nd.z) + p.y));
		std::unordered_map<int64_t, Color> seen; // a point of the paving, in millimetres, and the colour first found there
		for (int64_t i = from; i < e.mb.v.size(); i++) {
			if (!top(e.mb, i, RoadMat::PLATE)) continue;
			const Vector3 p = e.mb.v[i];
			const uint32_t colour = e.mb.c[i].to_rgba32();
			const auto found = seen.try_emplace(top_cell(int(std::lround(p.x * MM)), int(std::lround(p.z * MM))), e.mb.c[i]);
			const Color apart = found.first->second - e.mb.c[i];
			split += std::max({ std::fabs(apart.r), std::fabs(apart.g), std::fabs(apart.b) }) > TONE_STEP;
			bool on = false, own = false; // on a mouth; in the colour of an arm whose mouth it is on
			for (const ArmEnd &en : ends) {
				if (std::fabs(p.y - en.p.y) > ON_Y_M || off_stretch(p, mouth_point(en, -en.arm.asph_right), mouth_point(en, en.arm.asph_left)) > ON_M) continue;
				on = true;
				own = own || colour == road_color(e.L.carriage(*en.prof), RoadMat::PLATE).to_rgba32();
			}
			mouth_points += on;
			mouth_off += on && !own;
			if (on && !own && first_off.is_empty()) first_off = Array::make(p.x, p.y, p.z);
			for (const Vector3 &q : kerb) {
				if (off_stretch(p, q, q) > ON_M) continue;
				kerb_points++;
				kerb_off += std::fabs(e.mb.uv2[i].x) > ON_M;
				break;
			}
		}
	}

	Dictionary report() const {
		Dictionary d;
		d["mouths"] = mouths; // carriageway mouths on a junction's paving
		d["mouth_points"] = mouth_points; // the paving's vertices on one of them
		d["mouth_off"] = mouth_off; // of those, not in the colour of an arm whose mouth they are on
		d["first_off"] = first_off; // the first such: x, y, z
		d["split"] = split; // the paving's vertices that share a point with another of another colour
		d["kerb_points"] = kerb_points; // the paving's vertices on a corner's kerb line
		d["kerb_off"] = kerb_off; // of those, whose distance to the kerb is not nought
		d["lane_points"] = lane_points; // the lanes' vertices
		d["lane_off"] = lane_off; // of those, before the lane's own edge across it
		d["lane_wide"] = lane_wide; // of those, at the lane's width across it or past it (its far edge)
		d["lane_past_mouth"] = lane_past_mouth; // of those, off a mouth that opens on a node's paving
		d["bay_points"] = bay_points; // the vertices of the carriageway's asphalt that is no lane (bays, bike strips, shoulders)
		d["bay_inner"] = bay_inner; // of those, off the carriageway's edge
		return d;
	}
};
} // namespace

Dictionary CityProbes::road_overlaps() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	MeshBuilder mb;
	const RoadEmit e{ CityView(s), render_->street_look(), mb };
	std::vector<Top> tops;
	Wear wear;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		const int64_t at = mb.idx.size(), first = mb.v.size();
		emit_segment(e, sid);
		tops_of(mb, at, sid, tops);
		wear.segment(mb, first);
	}
	CornerCensus census;
	for (int n = 0; n < int(s.roads.nodes.size()); n++) {
		if (!s.roads.nodes[size_t(n)].alive) continue;
		const int64_t at = mb.idx.size(), first = mb.v.size();
		emit_node(e, n);
		tops_of(mb, at, -1 - n, tops);
		corner_census(e, n, census);
		wear.node(e, n, first);
	}
	Buried buried;
	for (const Top &t : tops) buried.count(t, s.terrain);
	std::unordered_map<int64_t, std::vector<int>> cells;
	for (int i = 0; i < int(tops.size()); i++)
		for (int cx = cell_of(tops[size_t(i)].x0); cx <= cell_of(tops[size_t(i)].x1); cx++)
			for (int cz = cell_of(tops[size_t(i)].z0); cz <= cell_of(tops[size_t(i)].z1); cz++) cells[top_cell(cx, cz)].push_back(i);
	int pairs = 0;
	double ground = 0;
	float largest = 0;
	Array first, places, worst;
	Dictionary by; // "kind/kind same|apart": pairs and the ground they share
	for (const auto &[key, members] : cells) {
		for (size_t i = 0; i < members.size(); i++) {
			for (size_t j = i + 1; j < members.size(); j++) {
				const Top &a = tops[size_t(members[i])], &b = tops[size_t(members[j])];
				const float x0 = std::max(a.x0, b.x0), z0 = std::max(a.z0, b.z0);
				if (x0 > std::min(a.x1, b.x1) || z0 > std::min(a.z1, b.z1)) continue;
				if (top_cell(cell_of(x0), cell_of(z0)) != key) continue; // the pair is weighed once, in the cell of its boxes' meeting corner
				const Poly shared = clip(Poly{ a.p[0], a.p[1], a.p[2] }, b.p);
				if (shared.size() < 3) continue;
				const float area = area_of(shared);
				if (area < OVERLAP_M2) continue;
				const V2 at = centre_of(shared);
				const float ya = height_at(a, at), yb = height_at(b, at);
				if (std::fabs(ya - yb) > SAME_HEIGHT_M) continue;
				pairs++;
				ground += area;
				const String kind = String(source(std::max(a.from, b.from))) + "/" + source(std::min(a.from, b.from)) + (a.from == b.from ? " same" : " apart");
				Array tally = by.get(kind, Array::make(0, 0.0));
				by[kind] = Array::make(int(tally[0]) + 1, double(tally[1]) + area);
				if (area > largest) {
					largest = area;
					worst = Array::make(at.x, at.y, area, a.from, b.from);
				}
				if (first.is_empty()) first = Array::make(at.x, at.y, area, ya, yb);
				if (places.size() < PLACES) places.push_back(Vector3(at.x, ya, at.y));
			}
		}
	}
	Dictionary d;
	d["tops"] = int64_t(tops.size()); // top faces of every road surface of the city
	d["pairs"] = pairs; // pairs of them at one height over the same ground
	d["ground_m2"] = ground; // the ground they share
	d["first"] = first; // x, z, area, the two heights
	d["places"] = places; // where a few of them lie
	d["by"] = by; // "kind/kind same|apart" (segment, node; one or two of them): [pairs, ground]
	d["worst"] = worst; // the largest: x, z, area, what drew each (a segment's id, -1 - a node's)
	d["corners"] = census.corners; // junction corners (sim/roads/junction_corner.h)
	d["corners_kinds"] = Array::make(census.kinds[0], census.kinds[1], census.kinds[2]); // by back: squared, moved out, run on
	d["corners_crossed"] = census.crossed; // of them, whose footway's outline is not a simple polygon
	d["corners_holed"] = census.holed; // whose drawn footway's triangles leave part of its polygon out
	Array ring;
	for (const Vector3 &p : census.first_ring) ring.push_back(Vector2(p.x, p.z));
	d["holed_first"] = Array::make(census.first_node, ring); // the first such: its node and its polygon in plan
	d["buried"] = buried.tops; // road tops the ground stands over (it shows through them)
	d["buried_m2"] = buried.ground;
	d["buried_by"] = buried.by; // by what drew them: segment, node
	d["buried_first"] = buried.first; // x, z, how far the ground stands over, what drew it
	d["wear"] = wear.report(); // the asphalt's wear as the meshes carry it
	return d;
}

} // namespace city
