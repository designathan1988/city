// Node shapes: an asphalt plate fanned from the node to every arm's mouth, the corners between neighbouring arms (road_corner.cpp, as
// sim/roads/junction_corner.h lays them: kerb face, kerb stone and footway), dead ends closed by a face across the cut or a turning
// bulb behind a kerb; and the ground round each.
#include "render/roads/road_node.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>
#include <iterator>

using namespace godot;

namespace city {

namespace {
constexpr double FILLET_SLACK = 0.05;
constexpr double TAU_D = 2 * PI_D;
const Vector3 UP(0, 1, 0);

Vector3 flat_unit(Vector3 v) {
	v.y = 0;
	return v.length() > 0 ? v.normalized() : UP;
}

Vector3 side(const ArmEnd &e, double u) { return e.p + Vector3(e.d.z, 0, -e.d.x) * float(u); }

// Width of the kerb stone on the arm's left (+1) or right (-1) side, 0 when that side has none (junction_corner.h side_section).
float kerb_w(const ArmEnd &e, int hand) {
	const SideSection s = side_section(*e.prof, (hand > 0) == e.at_a);
	return s.stone ? s.back - s.kerb : 0.f;
}

// A corner's line `pts` (node frame, shares between a's mouth and b's) at its road's height, `rise` over it.
std::vector<Vector3> lifted(const Vector3 &node, const ArmEnd &a, const ArmEnd &b, const std::vector<V2> &pts, const std::vector<float> &share, float rise) {
	std::vector<Vector3> out;
	for (size_t k = 0; k < pts.size(); k++) out.push_back(Vector3(node.x + pts[k].x, a.p.y + (b.p.y - a.p.y) * share[k] + rise, node.z + pts[k].y));
	return out;
}

// A plate from the node out to `ring`, each point in the tone of the road nearest it: the paving that is not asphalt and the turning
// bulb, which have no tones to share.
void fan(const RoadEmit &e, const Vector3 &centre, const std::vector<ArmEnd> &ends, const std::vector<Vector3> &ring) {
	const Vector2 pl(POOL_PLATE, 0);
	auto add = [&](const Vector3 &p, const Color &c) { return e.mb.add(p, UP, road_color(c, RoadMat::PLATE), Vector2(p.x, p.z), pl); };
	Color mean(0, 0, 0);
	for (const ArmEnd &en : ends) mean += e.L.carriage(*en.prof);
	const int c0 = add(centre, mean / float(ends.size()));
	std::vector<int> out;
	for (const Vector3 &p : ring) {
		const ArmEnd *nearest = &ends[0];
		for (const ArmEnd &en : ends) nearest = (p - en.p).length() < (p - nearest->p).length() ? &en : nearest;
		out.push_back(add(p, e.L.carriage(*nearest->prof)));
	}
	for (size_t i = 0; i < ring.size(); i++) e.mb.tri(c0, out[i], out[(i + 1) % ring.size()], UP);
}

// The asphalt between two arms, from a's centre line round the kerb to b's. Across a wedge the difference of the distances to the
// two lines is linear, so the mesh carries it and the shader lays the passage between the tones over a width of its own, the same
// from the kerb to the node (road.gdshader M_JOIN). Fans of arms of one tone are the same, and no more triangles.
void wedge(const RoadEmit &e, const Vector3 &node, const ArmEnd &a, const ArmEnd &b, const std::vector<Vector3> &line) {
	const Vector3 left_a(a.d.z, 0, -a.d.x), left_b(b.d.z, 0, -b.d.x);
	Color tones = wear_color(0, 0, 0, RoadMat::JOIN, int(a.prof->road_class));
	tones.r = (float(int(b.prof->road_class)) + MAT_HALF) / TONE_SLOTS;
	auto add = [&](const Vector3 &p) {
		const Vector3 v = p - node;
		return e.mb.add(p, UP, tones, Vector2(p.x, p.z), Vector2(POOL_PLATE, v.dot(left_a) + v.dot(left_b)));
	};
	const int c0 = add(node);
	int prev = add(a.p);
	for (const Vector3 &p : line) {
		const int next = add(p);
		e.mb.tri(c0, prev, next, UP);
		prev = next;
	}
	e.mb.tri(c0, prev, add(b.p), UP);
}

void junction(const RoadEmit &e, int n, const std::vector<ArmEnd> &ends) {
	const RoadNode &nd = e.s.roads.nodes[n];
	const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	std::vector<SurfaceLayout> lays;
	for (const ArmEnd &en : ends) lays.push_back(surface_layout(e, en.sid));
	std::vector<Vector3> ring;
	bool asphalt = true;
	for (const ArmEnd &en : ends) asphalt = asphalt && en.prof->surface == Surface::ASPHALT;
	const size_t k = ends.size();
	for (size_t i = 0; i < k; i++) {
		const ArmEnd &a = ends[i], &b = ends[(i + 1) % k];
		ring.push_back(side(a, -a.arm.asph_right));
		const CornerOutline o = corner_of(e, node, a, b);
		const std::vector<Vector3> in = lifted(node, a, b, o.kerb, o.share, 0);
		ring.insert(ring.end(), in.begin(), in.end());
		if (asphalt) wedge(e, node, a, b, in);
		draw_corner(e, node, a, b, o, lays[i], lays[(i + 1) % k]);
	}
	if (!asphalt) fan(e, node, ends, ring);
}

struct EndFrame {
	Vector3 node, d, left;
	float l, r, kw;
};

// The shared frame of an end cap: node, outward arm direction, its left, widths and the widest kerb.
EndFrame end_frame(const RoadEmit &e, int n, const ArmEnd &en) {
	const RoadNode &nd = e.s.roads.nodes[n];
	Vector3 node(float(nd.x), float(nd.y), float(nd.z)), d(float(en.arm.dx), 0, float(en.arm.dz));
	Vector3 left(d.z, 0, -d.x);
	return { node, d, left, float(en.arm.left), float(en.arm.right), std::max(kerb_w(en, 1), kerb_w(en, -1)) };
}

// A plain road end: the carriageway runs to the cut and the footways stop level with it, closed by one face down to the
// ground on each piece of the section (no kerb across the end).
void road_cut(const RoadEmit &e, int n, const ArmEnd &en) {
	const StreetLook &L = e.L;
	const EndFrame f = end_frame(e, n, en);
	const float ar = float(en.arm.asph_right), al = float(en.arm.asph_left), walk = f.kw > 0 ? L.h_sidewalk : 0.f;
	const Color col = road_color(L.curb, RoadMat::KERB);
	auto face = [&](float u0, float u1, float h) {
		if (u1 <= u0) return;
		const Vector3 a = f.node + f.left * u0, b = f.node + f.left * u1, up(0, h, 0), down(0, -L.skirt, 0);
		const Vector3 p[4] = { a + up, b + up, b + down, a + down };
		const Vector2 uv[4] = { Vector2(u0, h), Vector2(u1, h), Vector2(u1, -L.skirt), Vector2(u0, -L.skirt) };
		const Vector2 pool(POOL_NONE, 0), pools[4] = { pool, pool, pool, pool };
		emit_quad(e.mb, p, uv, pools, col, -f.d);
	};
	face(-f.r, -ar, walk);
	face(-ar, al, 0);
	face(al, f.l, walk);
}

// A circle of radius r round a turning bulb, open where the road comes in.
std::vector<Vector3> bulb_ring(const StreetLook &L, const EndFrame &f, const ArmEnd &en, double setback, double r) {
	std::vector<Vector3> pts;
	double a_in = std::acos(std::min(1.0, setback / r));
	for (int i = 0; i <= L.bulb_steps; i++) {
		double phi = a_in + double(i) / L.bulb_steps * (TAU_D - 2 * a_in), along = r * std::cos(phi), u = r * std::sin(phi);
		float y = along <= 0 ? f.node.y : float(f.node.y + (en.p.y - f.node.y) * std::min(1.0, along / setback));
		pts.push_back(f.node + f.d * float(along) + f.left * float(u) + Vector3(0, y - f.node.y, 0));
	}
	return pts;
}

void bulb(const RoadEmit &e, int n, const ArmEnd &en) {
	const StreetLook &L = e.L;
	const EndFrame f = end_frame(e, n, en);
	const double setback = std::max(en.at, FILLET_SLACK);
	const auto ring = [&](double r, float rise) {
		std::vector<Vector3> pts = bulb_ring(L, f, en, setback, r);
		for (Vector3 &p : pts) p.y += rise;
		return pts;
	};
	const std::vector<Vector3> in = ring(BULB_R, 0);
	fan(e, f.node, { en }, in);
	kerb_band(e, in, ring(BULB_R + f.kw, 0), f.kw > 0 ? L.h_curb : L.h_sidewalk, L.h_sidewalk, f.kw > 0);
	const std::vector<Vector3> inner = ring(BULB_R + f.kw, L.h_sidewalk), back = ring(BULB_R + BULB_WALK, L.h_sidewalk);
	std::vector<Vector3> patch = inner;
	patch.insert(patch.end(), back.rbegin(), back.rend());
	footway_patch(e, patch, inner, back);
	footway_skirt(e, back, inner, footway_colour(L, en));
}

// What a node is, and the function that draws that shape.
template <typename Junction, typename Cut, typename Bulb>
void each_shape(const RoadEmit &e, int n, Junction &&plate, Cut &&cut, Bulb &&turn) {
	const RoadNode &nd = e.s.roads.nodes[n];
	if (nd.segs.empty()) return;
	std::vector<ArmEnd> ends = arm_ends(e, n);
	if (ends.size() == 1) {
		if (ends[0].prof->rail) return;
		if (nd.bulb) turn(e, n, ends[0]);
		else cut(e, n, ends[0]);
		return;
	}
	if (plated(ends)) plate(e, n, ends);
}

// The verge along a line of the paving, each point taken `rise` up and sent out towards `from` the other way.
void verge_from(const RoadEmit &e, std::vector<Vector3> edge, const std::vector<Vector3> &away, float rise) {
	for (Vector3 &p : edge) p.y += rise;
	verge_ribbon(e, edge, away);
}

// The ground round each corner, from its footway's back: along a back moved out from the kerb, each point out the way it was moved;
// along a squared back, each side sending its ground straight out (past the point they overlap, the nearer one on top: the mitre).
void junction_verge(const RoadEmit &e, int n, const std::vector<ArmEnd> &ends) {
	const RoadNode &nd = e.s.roads.nodes[n];
	const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	for (size_t i = 0; i < ends.size(); i++) {
		const ArmEnd &a = ends[i], &b = ends[(i + 1) % ends.size()];
		const CornerOutline o = corner_of(e, node, a, b);
		const bool raised = side_section(*a.prof, a.at_a).raised || side_section(*b.prof, !b.at_a).raised;
		const float rise = raised ? e.L.h_sidewalk : 0.f;
		const std::vector<Vector3> back = lifted(node, a, b, o.back, o.back_share, 0);
		if (o.kind == CornerBack::OFFSET) {
			const std::vector<Vector3> kerb = lifted(node, a, b, o.kerb, o.share, 0);
			std::vector<Vector3> away;
			for (size_t k = 0; k < back.size(); k++) away.push_back(flat_unit(back[k] - kerb[k]));
			verge_from(e, back, away, rise);
			continue;
		}
		const Vector3 centre = lifted(node, a, b, { o.kerb[o.kerb.size() / 2] }, { o.share[o.kerb.size() / 2] }, 0).front();
		for (size_t k = 1; k < back.size(); k++) {
			Vector3 away = flat_unit(Vector3(back[k].z - back[k - 1].z, 0, back[k - 1].x - back[k].x));
			if (away.dot(back[k] + back[k - 1] - centre * 2) < 0) away = -away; // away from the kerb's return
			verge_from(e, { back[k - 1], back[k] }, { away, away }, rise);
		}
	}
}

void cut_verge(const RoadEmit &e, int n, const ArmEnd &en) {
	const EndFrame f = end_frame(e, n, en);
	const float ar = float(en.arm.asph_right), al = float(en.arm.asph_left), walk = f.kw > 0 ? e.L.h_sidewalk : 0.f;
	const float at[] = { -f.r, -ar, -ar, al, al, f.l }, up[] = { walk, walk, 0, 0, walk, walk };
	std::vector<Vector3> edge;
	for (size_t k = 0; k < std::size(at); k++) edge.push_back(f.node + f.left * at[k] + Vector3(0, up[k], 0));
	verge_ribbon(e, edge, std::vector<Vector3>(edge.size(), -f.d));
}

void bulb_verge(const RoadEmit &e, int n, const ArmEnd &en) {
	const EndFrame f = end_frame(e, n, en);
	const std::vector<Vector3> ring = bulb_ring(e.L, f, en, std::max(en.at, FILLET_SLACK), BULB_R + BULB_WALK);
	std::vector<Vector3> away;
	for (const Vector3 &p : ring) away.push_back(flat_unit(p - f.node));
	verge_from(e, ring, away, f.kw > 0 ? e.L.h_sidewalk : 0.f);
}
} // namespace

void emit_verge_node(const RoadEmit &e, int n) { each_shape(e, n, junction_verge, cut_verge, bulb_verge); }

void emit_node(const RoadEmit &e, int n) { each_shape(e, n, junction, road_cut, bulb); }

} // namespace city
