// A junction's corner drawn from its outline (sim/roads/junction_corner.h): the kerb along the return, and the footway as one polygon
// from the kerb stone's back (and along an arm's run, where the corner runs the footway on past the carriageway's mouth) out to its
// back, its ends at the arms' footway mouths carrying the segments' own sections there (each piece's edge and height, kerb_drop.h),
// so the corner meets each segment's footway with no crack and no overlap. And where each segment's footway starts, as the corners
// of its two nodes say.
#include "render/roads/road_node.h"

#include "core/polygon_ops.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float SAME_M = 1e-3f; // points nearer than this in plan are one
constexpr int PAVINGS = 4; // StreetLook::paving's colours, one per Paving
constexpr double COVER_SHARE = 1e-4; // a footway's triangles leave out less than this share of its polygon: they cover it

Vector3 section_point(const RoadEmit &e, int sid, double v, float u, float h) {
	const SegFrame f = seg_frame(e.s, sid, v);
	return f.c + f.left * u + Vector3(0, h, 0);
}

// The section across end `en`'s footway on `side` at arc `v`: its stone's back first, its outer edge last; where the side has no
// footway, the carriageway's edge alone.
std::vector<Vector3> across(const RoadEmit &e, const ArmEnd &en, const SurfaceLayout &lay, int side, double v) {
	std::vector<Vector3> out;
	for (const SectionPoint &s : footway_section(e, en.sid, lay, side, v)) out.push_back(section_point(e, en.sid, v, s.u, s.h));
	if (out.empty()) {
		const SideSection sec = side_section(*en.prof, side == STREET_LEFT);
		out.push_back(section_point(e, en.sid, v, side == STREET_LEFT ? sec.back : -sec.back, 0));
	}
	return out;
}

double mouth_arc(const ArmEnd &en, const SurfaceLayout &lay) { return en.at_a ? lay.v0 : lay.v1; }
double walk_arc(const ArmEnd &en, const SurfaceLayout &lay, int side) { return en.at_a ? lay.walk_from[side] : lay.walk_to[side]; }

// The arcs the segment is drawn at strictly between its carriageway's mouth and its footway's on `side`, from the mouth out.
std::vector<double> run_arcs(const ArmEnd &en, const SurfaceLayout &lay, int side) {
	const double mouth = mouth_arc(en, lay), walk = walk_arc(en, lay, side);
	std::vector<double> out;
	for (double v : lay.arcs)
		if ((v - mouth) * (walk - mouth) > 0 && std::fabs(v - mouth) < std::fabs(walk - mouth)) out.push_back(v);
	if (!en.at_a) std::reverse(out.begin(), out.end());
	return out;
}

// Where a run's side has no kerb stone, the segment draws no face there (its footway is the corner's): the face from the
// carriageway up to the footway's edge along `tops`, over `bases`.
void run_faces(const RoadEmit &e, const std::vector<Vector3> &tops, const std::vector<Vector3> &bases, const Vector3 &towards_road) {
	for (size_t k = 1; k < tops.size(); k++) {
		const Vector3 p[4] = { bases[k - 1], bases[k], tops[k], tops[k - 1] };
		const Vector2 uv[4] = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
		const Vector2 none(POOL_NONE, 0), pools[4] = { none, none, none, none };
		emit_quad(e.mb, p, uv, pools, road_color(e.L.curb, RoadMat::SKIRT), towards_road);
	}
}

// One arm's run along its stone's back: the stations strictly between the carriageway's mouth and the footway's, from the mouth out.
std::vector<Vector3> run_of(const RoadEmit &e, const ArmEnd &en, const SurfaceLayout &lay, int side) {
	std::vector<Vector3> inside;
	for (double v : run_arcs(en, lay, side)) inside.push_back(across(e, en, lay, side, v).front());
	return inside;
}

// Where a run's side has no kerb stone the segment draws no face there (its footway is the corner's): the face from the carriageway
// up to the footway's edge, from the carriageway's mouth to the footway's.
void run_face(const RoadEmit &e, const ArmEnd &en, const SurfaceLayout &lay, int side) {
	const SideSection sec = side_section(*en.prof, side == STREET_LEFT);
	if (sec.stone || std::fabs(walk_arc(en, lay, side) - mouth_arc(en, lay)) < SAME_M) return;
	const float u = side == STREET_LEFT ? sec.back : -sec.back;
	std::vector<double> whole{ mouth_arc(en, lay) };
	const std::vector<double> arcs = run_arcs(en, lay, side);
	whole.insert(whole.end(), arcs.begin(), arcs.end());
	whole.push_back(walk_arc(en, lay, side));
	std::vector<Vector3> tops, bases;
	for (double v : whole) {
		tops.push_back(across(e, en, lay, side, v).front());
		bases.push_back(section_point(e, en.sid, v, u, 0));
	}
	const SegFrame f = seg_frame(e.s, en.sid, mouth_arc(en, lay));
	run_faces(e, tops, bases, side == STREET_LEFT ? -f.left : f.left);
}

// A corner's footway: its polygon, its inner side and its back (draw_corner, corner_census).
struct Patch {
	std::vector<Vector3> ring, inner, back;
};

void add_point(std::vector<Vector3> &line, const Vector3 &p) {
	if (line.empty() || Vector2(p.x - line.back().x, p.z - line.back().z).length() >= SAME_M) line.push_back(p);
}

// The footway's polygon: along its inner side from a's footway mouth (a's run in, the stone, b's run out) to b's, across b's
// mouth out, back along the back, across a's mouth in; the mouths carry the segments' sections, the rest the corner's walk height.
Patch patch_of(const RoadEmit &e, const Vector3 &node, const ArmEnd &a, const ArmEnd &b, const CornerOutline &o, const SurfaceLayout &la,
		const SurfaceLayout &lb) {
	const int side_a = left_side(a), side_b = STREET_LEFT + STREET_RIGHT - left_side(b); // a's left, b's right
	const bool raised = side_section(*a.prof, side_a == STREET_LEFT).raised || side_section(*b.prof, side_b == STREET_LEFT).raised;
	const float walk = raised ? e.L.h_sidewalk : 0.f;
	const auto at = [&](const V2 &p, float share) { return Vector3(node.x + p.x, a.p.y + (b.p.y - a.p.y) * share + walk, node.z + p.y); };
	const std::vector<Vector3> mouth_a = across(e, a, la, side_a, walk_arc(a, la, side_a)), mouth_b = across(e, b, lb, side_b, walk_arc(b, lb, side_b));
	std::vector<Vector3> run_a = run_of(e, a, la, side_a);
	std::reverse(run_a.begin(), run_a.end()); // in towards a's mouth
	Patch pt;
	add_point(pt.inner, mouth_a.front());
	for (const Vector3 &p : run_a) add_point(pt.inner, p);
	add_point(pt.inner, across(e, a, la, side_a, mouth_arc(a, la)).front());
	for (size_t k = 1; k + 1 < o.stone.size(); k++) add_point(pt.inner, at(o.stone[k], o.share[k]));
	add_point(pt.inner, across(e, b, lb, side_b, mouth_arc(b, lb)).front());
	for (const Vector3 &p : run_of(e, b, lb, side_b)) add_point(pt.inner, p);
	add_point(pt.inner, mouth_b.front());
	pt.back.push_back(mouth_a.back());
	for (size_t k = 0; k < o.back.size(); k++) {
		const Vector3 p = at(o.back[k], o.back_share[k]);
		const auto apart = [&](const Vector3 &q) { return Vector2(p.x - q.x, p.z - q.z).length() >= SAME_M; };
		if (apart(mouth_a.back()) && apart(mouth_b.back())) add_point(pt.back, p);
	}
	add_point(pt.back, mouth_b.back());
	for (const Vector3 &p : pt.inner) add_point(pt.ring, p);
	for (size_t k = 1; k < mouth_b.size(); k++) add_point(pt.ring, mouth_b[k]);
	for (size_t k = pt.back.size(); k-- > 0;) add_point(pt.ring, pt.back[k]);
	for (size_t k = mouth_a.size(); k-- > 1;) add_point(pt.ring, mouth_a[k]);
	if (pt.ring.size() > 1 && Vector2(pt.ring.front().x - pt.ring.back().x, pt.ring.front().z - pt.ring.back().z).length() < SAME_M) pt.ring.pop_back();
	return pt;
}

// Twice the area a ring's triangles cover against twice the ring's own: equal when the cut leaves nothing out.
bool covered(const std::vector<Vector3> &ring) {
	std::vector<double> x, z;
	for (const Vector3 &p : ring) {
		x.push_back(p.x);
		z.push_back(p.z);
	}
	double whole = 0, cut = 0;
	for (size_t i = 0; i < ring.size(); i++) whole += x[i] * z[(i + 1) % ring.size()] - x[(i + 1) % ring.size()] * z[i];
	const std::vector<int> t = ring_triangles(x, z);
	for (size_t k = 0; k + 2 < t.size(); k += 3) {
		const size_t i = size_t(t[k]), j = size_t(t[k + 1]), m = size_t(t[k + 2]);
		cut += std::fabs((x[j] - x[i]) * (z[m] - z[i]) - (z[j] - z[i]) * (x[m] - x[i]));
	}
	return std::fabs(std::fabs(whole) - cut) <= COVER_SHARE * std::fabs(whole);
}
} // namespace

namespace {

} // namespace

std::vector<ArmEnd> arm_ends(const RoadEmit &e, int n) {
	std::vector<ArmEnd> ends;
	for (const NodeEnd &ne : node_ends(e.s.roads, n)) { // anticlockwise already
		const RoadSegment &g = e.s.roads.segs[size_t(ne.seg)];
		ArmEnd en;
		en.end = ne;
		en.sid = ne.seg;
		en.at_a = ne.at_a;
		en.at = ne.at;
		en.p = Vector3(float(ne.x), float(e.s.roads.seg_y(ne.seg, ne.at_a ? ne.at : g.len - ne.at)), float(ne.z));
		en.d = Vector3(ne.dir.x, 0, ne.dir.y);
		en.prof = ne.prof;
		en.arm = ne.arm;
		ends.push_back(en);
	}
	return ends;
}

bool plated(const std::vector<ArmEnd> &ends) {
	std::vector<NodeEnd> plain;
	for (const ArmEnd &en : ends) plain.push_back(en.end);
	return node_plated(plain);
}

int left_side(const ArmEnd &en) { return en.at_a ? STREET_LEFT : STREET_RIGHT; }

Color footway_colour(const StreetLook &L, const ArmEnd &en) {
	const Profile &p = *en.prof;
	const Strip &s = (left_side(en) == STREET_LEFT ? p.edges.back() : p.edges.front()).strip;
	return s.kind == StripKind::SIDEWALK ? L.paving[int(s.pave) & (PAVINGS - 1)] : L.paving[0];
}

CornerOutline corner_of(const RoadEmit &e, const ArmEnd &a, const ArmEnd &b) {
	const RoadSegment &g = e.s.roads.segs[size_t(a.sid)];
	const RoadNode &nd = e.s.roads.nodes[size_t(a.at_a ? g.a : g.b)];
	return node_corner(e.s.roads, nd.x, nd.z, a.end, b.end);
}

void draw_corner(const RoadEmit &e, const Vector3 &node, const ArmEnd &a, const ArmEnd &b, const CornerOutline &o, const SurfaceLayout &la,
		const SurfaceLayout &lb) {
	const StreetLook &L = e.L;
	const int side_a = left_side(a), side_b = STREET_LEFT + STREET_RIGHT - left_side(b); // a's left, b's right
	const SideSection sa = side_section(*a.prof, side_a == STREET_LEFT), sb = side_section(*b.prof, side_b == STREET_LEFT);
	const float walk = sa.raised || sb.raised ? L.h_sidewalk : 0.f;
	const auto at = [&](const V2 &p, float share) { return Vector3(node.x + p.x, a.p.y + (b.p.y - a.p.y) * share, node.z + p.y); };
	std::vector<Vector3> in, bk;
	for (size_t k = 0; k < o.kerb.size(); k++) {
		in.push_back(at(o.kerb[k], o.share[k]));
		bk.push_back(at(o.stone[k], o.share[k]));
	}
	kerb_band(e, in, bk, sa.stone || sb.stone ? L.h_curb : walk, walk, sa.stone || sb.stone);
	run_face(e, a, la, side_a);
	run_face(e, b, lb, side_b);
	const Patch pt = patch_of(e, node, a, b, o, la, lb);
	footway_patch(e, pt.ring, pt.inner, pt.back);
	footway_skirt(e, pt.back, pt.inner, footway_colour(L, a));
}

void footway_mouths(const RoadEmit &e, int sid, double from[2], double to[2]) {
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	from[STREET_RIGHT] = from[STREET_LEFT] = g.retreat_a;
	to[STREET_RIGHT] = to[STREET_LEFT] = g.len - g.retreat_b;
	for (const bool at_a : { true, false }) {
		const int n = at_a ? g.a : g.b;
		const std::vector<ArmEnd> ends = arm_ends(e, n);
		if (!plated(ends)) continue;
		const size_t k = ends.size();
		for (size_t i = 0; i < k; i++) {
			if (ends[i].sid != sid || ends[i].at_a != at_a) continue;
			// the arm's left is its corner with the next end, where it is that corner's a; its right, with the one before, as b
			const double left_run = corner_of(e, ends[i], ends[(i + 1) % k]).run_a;
			const double right_run = corner_of(e, ends[(i + k - 1) % k], ends[i]).run_b;
			const int left = left_side(ends[i]), right = STREET_LEFT + STREET_RIGHT - left;
			(at_a ? from : to)[left] += at_a ? left_run : -left_run;
			(at_a ? from : to)[right] += at_a ? right_run : -right_run;
		}
	}
}

void corner_census(const RoadEmit &e, int node, CornerCensus &c) {
	const std::vector<ArmEnd> ends = arm_ends(e, node);
	if (!plated(ends)) return;
	const RoadNode &nd = e.s.roads.nodes[size_t(node)];
	const Vector3 at(float(nd.x), float(nd.y), float(nd.z));
	std::vector<SurfaceLayout> lays;
	for (const ArmEnd &en : ends) lays.push_back(surface_layout(e, en.sid));
	for (size_t i = 0; i < ends.size(); i++) {
		const size_t j = (i + 1) % ends.size();
		const CornerOutline o = corner_of(e, ends[i], ends[j]);
		c.corners++;
		c.kinds[int(o.kind)]++;
		if (!o.simple) c.crossed++;
		const Patch pt = patch_of(e, at, ends[i], ends[j], o, lays[i], lays[j]);
		if (covered(pt.ring)) continue;
		if (c.holed++ == 0) {
			c.first_node = node;
			c.first_ring = pt.ring;
		}
	}
}

} // namespace city
