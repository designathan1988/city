// Node shapes (Roadcraft jobs/roadMesh.ts buildJunction / buildDeadEnd / buildBulb): an asphalt plate fanned from the
// node to every arm's mouth, the corners between neighbouring arms as kerb returns (sim/roads/junction.h
// corner_fillet) or smooth curves, each corner carrying its kerb face, kerb stone and footway; dead ends get a footway
// pad behind a kerb or a turning bulb.
#include "render/roads/road_mesh.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr double FILLET_SLACK = 0.05, PLATE_MIN_RETREAT = 0.05, CTRL_MAX = 0.5, CTRL_MIN = -60;
constexpr double PARALLEL = 1e-3, SAME = 1e-3;
constexpr int MIN_ARC = 2;
constexpr double TAU_D = 2 * PI_D;
const Vector3 UP(0, 1, 0);

struct End {
	Vector3 p, d; // mouth centre, outward unit direction
	Arm arm;
	double at = 0; // setback
	const Profile *prof = nullptr;
	bool at_a = true;
};

Vector3 side(const End &e, double u) { return e.p + Vector3(e.d.z, 0, -e.d.x) * float(u); }

// Width of the kerb stone on the arm's left (+1) or right (-1) side, 0 when that side has no raised footway.
float kerb_w(const End &e, int hand) {
	const Profile &p = *e.prof;
	bool plus_u = (hand > 0) == e.at_a; // the arm's left is the profile's +u side at end a
	int n = int(p.edges.size());
	if (n < 2) return 0;
	const StripEdge &edge = plus_u ? p.edges[n - 1] : p.edges[0], &next = plus_u ? p.edges[n - 2] : p.edges[1];
	bool kerbed = edge.strip.kind == StripKind::SIDEWALK && !edge.strip.flush && next.strip.kind == StripKind::CURB;
	return kerbed ? next.u1 - next.u0 : 0.f;
}

Color walk_color(const StreetLook &L, const End &e, int hand) {
	const Profile &p = *e.prof;
	bool plus_u = (hand > 0) == e.at_a;
	const Strip &s = (plus_u ? p.edges.back() : p.edges.front()).strip;
	return s.kind == StripKind::SIDEWALK ? L.paving[int(s.pave) & 3] : L.paving[0];
}

std::vector<End> ends_of(const RoadEmit &e, int n) {
	std::vector<End> ends;
	for (int sid : e.s.roads.nodes[n].segs) {
		const RoadSegment &g = e.s.roads.segs[sid];
		End en;
		en.at_a = g.a == n;
		en.at = en.at_a ? g.retreat_a : g.retreat_b;
		SegFrame f = seg_frame(e.s, sid, en.at_a ? en.at : g.len - en.at);
		en.p = f.c;
		en.d = en.at_a ? f.fwd : -f.fwd;
		en.prof = &e.s.catalog.profile(g.profile);
		en.arm = arm_of(g.q, *en.prof, en.at_a ? 0 : 1);
		ends.push_back(en);
	}
	std::sort(ends.begin(), ends.end(), [](const End &a, const End &b) { return arm_angle(a.arm.dx, a.arm.dz) < arm_angle(b.arm.dx, b.arm.dz); });
	return ends;
}

// The kerb line from a's left side (u = ua) to b's right side (u = -ub).
std::vector<Vector3> corner(const StreetLook &L, const Vector3 &node, const End &a, double ua, const End &b, double ub) {
	std::vector<Vector3> out;
	Corner fc = corner_fillet(a.arm, b.arm);
	Vector3 pa = side(a, ua), pb = side(b, -ub);
	if (fc.mode == CornerMode::FILLET && a.at >= fc.trim_a - FILLET_SLACK && b.at >= fc.trim_b - FILLET_SLACK) {
		double ccx = node.x + fc.cx, ccz = node.z + fc.cz;
		double tax = node.x + a.arm.dx * fc.trim_a + a.arm.dz * ua, taz = node.z + a.arm.dz * fc.trim_a - a.arm.dx * ua;
		double tbx = node.x + b.arm.dx * fc.trim_b - b.arm.dz * ub, tbz = node.z + b.arm.dz * fc.trim_b + b.arm.dx * ub;
		double a0 = std::atan2(taz - ccz, tax - ccx), sweep = std::atan2(tbz - ccz, tbx - ccx) - a0;
		while (sweep > PI_D) sweep -= TAU_D;
		while (sweep < -PI_D) sweep += TAU_D;
		double rho = std::max(FILLET_SLACK, std::hypot(tax - ccx, taz - ccz));
		int steps = std::max(MIN_ARC, int(std::ceil(std::fabs(sweep) / L.arc_step)));
		out.push_back(pa);
		for (int i = 0; i <= steps; i++) {
			double t = double(i) / steps, ang = a0 + sweep * t;
			out.push_back(Vector3(float(ccx + std::cos(ang) * rho), float(a.p.y + (b.p.y - a.p.y) * t), float(ccz + std::sin(ang) * rho)));
		}
		out.push_back(pb);
		return out;
	}
	double den = a.d.x * b.d.z - a.d.z * b.d.x, kx = (pa.x + pb.x) / 2, kz = (pa.z + pb.z) / 2;
	if (std::fabs(den) > PARALLEL) {
		double wx = pb.x - pa.x, wz = pb.z - pa.z;
		double sa = (wx * b.d.z - wz * b.d.x) / den, sb = (wx * a.d.z - wz * a.d.x) / den;
		if (sa < CTRL_MAX && sb < CTRL_MAX && sa > CTRL_MIN && sb > CTRL_MIN) { kx = pa.x + a.d.x * sa; kz = pa.z + a.d.z * sa; }
	}
	for (int i = 0; i <= L.bevel_steps; i++) {
		double t = double(i) / L.bevel_steps, s = 1 - t;
		out.push_back(Vector3(float(s * s * pa.x + 2 * s * t * kx + t * t * pb.x), float(a.p.y + (b.p.y - a.p.y) * t),
				float(s * s * pa.z + 2 * s * t * kz + t * t * pb.z)));
	}
	return out;
}

// The line wa..wb metres outside the kerb line: square to the arms at the mouths, square to the line in between.
std::vector<Vector3> outside(const std::vector<Vector3> &line, const End &a, const End &b, float wa, float wb) {
	size_t n = line.size();
	float hand = 1;
	for (size_t k = 1; k < n; k++) {
		Vector3 t = line[k] - line[0];
		if (Vector2(t.x, t.z).length() > SAME) { hand = t.x * -a.d.x - t.z * a.d.z >= 0 ? 1.f : -1.f; break; }
	}
	std::vector<Vector3> out;
	for (size_t k = 0; k < n; k++) {
		Vector3 nn;
		if (k == 0) nn = Vector3(a.d.z, 0, -a.d.x);
		else if (k == n - 1) nn = Vector3(-b.d.z, 0, b.d.x);
		else {
			Vector3 t = line[k + 1] - line[k - 1];
			t.y = 0;
			t = t.length() > 0 ? t.normalized() : Vector3(1, 0, 0);
			nn = Vector3(-t.z, 0, t.x) * hand;
		}
		float w = wa + (wb - wa) * float(k) / float(std::max<size_t>(1, n - 1));
		out.push_back(line[k] + nn * w);
	}
	return out;
}

// One band of a kerbed edge between two sections (prev, cur): kerb face, stone top, 1 cm step, footway, skirt.
struct Section {
	Vector3 i, k, o, n;
};

void band(const RoadEmit &e, const Section &a, const Section &b, bool kerbed, const Color &walk) {
	const StreetLook &L = e.L;
	float top = kerbed ? L.h_curb : L.h_sidewalk, sh = L.h_sidewalk;
	Vector3 up_t(0, top, 0), up_s(0, sh, 0), down(0, -L.skirt, 0);
	Vector3 ak = kerbed ? a.k : a.i, bk = kerbed ? b.k : b.i;
	const Vector2 none[4] = { Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0), Vector2(POOL_NONE, 0) };
	auto xz = [](const Vector3 &p) { return Vector2(p.x, p.z); };
	auto quad = [&](const Vector3 p0, const Vector3 p1, const Vector3 p2, const Vector3 p3, const Color &c, RoadMat m, const Vector3 &out) {
		const Vector3 p[4] = { p0, p1, p2, p3 };
		const Vector2 uv[4] = { xz(p0) + Vector2(0, p0.y), xz(p1) + Vector2(0, p1.y), xz(p2) + Vector2(0, p2.y), xz(p3) + Vector2(0, p3.y) };
		emit_quad(e.mb, p, uv, none, road_color(c, m), out);
	};
	quad(a.i, b.i, b.i + up_t, a.i + up_t, L.curb, RoadMat::SKIRT, a.n);
	if (kerbed) {
		quad(a.i + up_t, b.i + up_t, b.k + up_t, a.k + up_t, L.curb, RoadMat::KERB, UP);
		quad(a.k + up_s, b.k + up_s, b.k + up_t, a.k + up_t, L.curb, RoadMat::SKIRT, -a.n);
	}
	quad(ak + up_s, bk + up_s, b.o + up_s, a.o + up_s, walk, RoadMat::FOOTWAY, UP);
	quad(a.o + down, b.o + down, b.o + up_s, a.o + up_s, walk, RoadMat::SKIRT, -a.n);
}

void kerbed_line(const RoadEmit &e, const std::vector<Vector3> &in, const std::vector<Vector3> &bk, const std::vector<Vector3> &out,
		bool kerbed, const Color &walk) {
	Section prev;
	for (size_t k = 0; k < in.size(); k++) {
		Vector3 nn = in[k] - out[k];
		nn.y = 0;
		nn = nn.length() > 0 ? nn.normalized() : UP;
		Section cur{ in[k], bk[k], out[k], nn };
		if (k > 0) band(e, prev, cur, kerbed, walk);
		prev = cur;
	}
}

void fan(const RoadEmit &e, const Vector3 &centre, const std::vector<Vector3> &ring, const Color &col) {
	const Vector2 pl(POOL_PLATE, 0);
	Color c = road_color(col, RoadMat::PLATE);
	int c0 = e.mb.add(centre, UP, c, Vector2(centre.x, centre.z), pl);
	std::vector<int> ids;
	for (const Vector3 &p : ring) ids.push_back(e.mb.add(p, UP, c, Vector2(p.x, p.z), pl));
	for (size_t i = 0; i < ids.size(); i++) e.mb.tri(c0, ids[i], ids[(i + 1) % ids.size()], UP);
}

void junction(const RoadEmit &e, int n, const std::vector<End> &ends) {
	const RoadNode &nd = e.s.roads.nodes[n];
	Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	std::vector<Vector3> ring;
	const Profile *best = ends[0].prof;
	size_t k = ends.size();
	for (size_t i = 0; i < k; i++) {
		const End &a = ends[i], &b = ends[(i + 1) % k];
		if (a.prof->priority > best->priority) best = a.prof;
		ring.push_back(side(a, -a.arm.asph_right));
		std::vector<Vector3> in = corner(e.L, node, a, a.arm.asph_left, b, b.arm.asph_right);
		ring.insert(ring.end(), in.begin(), in.end());
		float ka = kerb_w(a, 1), kb = kerb_w(b, -1);
		std::vector<Vector3> bk = outside(in, a, b, ka, kb);
		std::vector<Vector3> out = outside(in, a, b, float(a.arm.left - a.arm.asph_left), float(b.arm.right - b.arm.asph_right));
		kerbed_line(e, in, bk, out, ka > 0 || kb > 0, walk_color(e.L, a, 1));
	}
	fan(e, node, ring, e.L.surface[int(best->surface)]);
}

struct EndFrame {
	Vector3 node, d, left;
	float l, r, kw;
};

// The shared frame of an end cap: node, outward arm direction, its left, widths and the widest kerb.
EndFrame end_frame(const RoadEmit &e, int n, const End &en) {
	const RoadNode &nd = e.s.roads.nodes[n];
	Vector3 node(float(nd.x), float(nd.y), float(nd.z)), d(float(en.arm.dx), 0, float(en.arm.dz));
	Vector3 left(d.z, 0, -d.x);
	return { node, d, left, float(en.arm.left), float(en.arm.right), std::max(kerb_w(en, 1), kerb_w(en, -1)) };
}

void dead_end(const RoadEmit &e, int n, const End &en) {
	const StreetLook &L = e.L;
	EndFrame f = end_frame(e, n, en);
	const Vector3 &node = f.node, &d = f.d, &left = f.left;
	float l = f.l, r = f.r, kw = f.kw;
	float al = float(en.arm.asph_left), ar = float(en.arm.asph_right);
	// sections across the end, from the right side to the left, the kerb face toward the street (+d)
	auto pt = [&](float u, float back) { return node + left * u - d * back; };
	std::vector<Vector3> in = { pt(-r, 0), pt(-ar, 0), pt(al, 0), pt(l, 0) }, bk, out;
	for (const Vector3 &p : in) {
		bk.push_back(p - d * kw);
		out.push_back(p - d * L.dead_end_depth);
	}
	kerbed_line(e, in, bk, out, kw > 0, walk_color(L, en, 1));
}

void bulb(const RoadEmit &e, int n, const End &en) {
	const StreetLook &L = e.L;
	EndFrame f = end_frame(e, n, en);
	const Vector3 &node = f.node, &d = f.d, &left = f.left;
	double setback = std::max(en.at, FILLET_SLACK);
	float kw = f.kw;
	auto ring = [&](double r) {
		std::vector<Vector3> pts;
		double a_in = std::acos(std::min(1.0, setback / r));
		for (int i = 0; i <= L.bulb_steps; i++) {
			double phi = a_in + double(i) / L.bulb_steps * (TAU_D - 2 * a_in), along = r * std::cos(phi), u = r * std::sin(phi);
			float y = along <= 0 ? node.y : float(node.y + (en.p.y - node.y) * std::min(1.0, along / setback));
			pts.push_back(node + d * float(along) + left * float(u) + Vector3(0, y - node.y, 0));
		}
		return pts;
	};
	std::vector<Vector3> in = ring(BULB_R);
	fan(e, node, in, L.surface[int(en.prof->surface)]);
	kerbed_line(e, in, ring(BULB_R + kw), ring(BULB_R + BULB_WALK), kw > 0, walk_color(L, en, 1));
}
} // namespace

void emit_node(const RoadEmit &e, int n) {
	const RoadNode &nd = e.s.roads.nodes[n];
	if (nd.segs.empty()) return;
	std::vector<End> ends = ends_of(e, n);
	if (ends.size() == 1) {
		if (ends[0].prof->rail) return;
		if (nd.bulb) bulb(e, n, ends[0]);
		else dead_end(e, n, ends[0]);
		return;
	}
	bool any = false;
	for (const End &en : ends) any = any || en.at > PLATE_MIN_RETREAT;
	if (any) junction(e, n, ends);
}

} // namespace city
