// Node shapes: a junction's paving from the node out to every arm's mouth and every corner's kerb (road_plate.cpp draws it), the
// corners between neighbouring arms (road_corner.cpp, as sim/roads/junction_corner.h lays them: kerb face, kerb stone and footway),
// dead ends closed by a face across the cut or a turning bulb behind a kerb; and the ground round each.
#include "render/roads/road_node.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <iterator>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

Vector3 flat_unit(Vector3 v) {
	v.y = 0;
	return v.length() > 0 ? v.normalized() : UP;
}

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

// The rim of the paving between two arms: from a's centre line across the left half of its mouth, round the corner's kerb line
// `line` (from a's mouth to b's), across the right half of b's mouth to its centre line. Each half mouth reads its arm's own kerb
// line carried on, so the grime by a lane's kerb is met where the lane ends.
void corner_rim(const Vector3 &node, const ArmEnd &a, const ArmEnd &b, const std::vector<Vector3> &line, std::vector<PlateSlice> &rim) {
	if (line.empty()) {
		rim.push_back(mouth_slice(node, a.p, b.p, mouth_point(a, a.arm.asph_left), a.d));
		return;
	}
	rim.push_back(mouth_slice(node, a.p, line.front(), line.front(), a.d));
	for (size_t k = 1; k < line.size(); k++) rim.push_back(kerb_slice(node, line[k - 1], line[k]));
	rim.push_back(mouth_slice(node, line.back(), b.p, line.back(), b.d));
}

void junction(const RoadEmit &e, int n, const std::vector<ArmEnd> &ends) {
	const RoadNode &nd = e.s.roads.nodes[n];
	const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	std::vector<SurfaceLayout> lays;
	for (const ArmEnd &en : ends) lays.push_back(surface_layout(e, en.sid));
	std::vector<PlateSlice> rim;
	const size_t k = ends.size();
	for (size_t i = 0; i < k; i++) {
		const ArmEnd &a = ends[i], &b = ends[(i + 1) % k];
		const CornerOutline o = corner_of(e, a, b);
		corner_rim(node, a, b, lifted(node, a, b, o.kerb, o.share, 0), rim);
		draw_corner(e, node, a, b, o, lays[i], lays[(i + 1) % k]);
	}
	draw_plate(e, node, ends, rim);
	if (!e.far) node_paint(e, n, ends); // paint is the near level's (road_markings.cpp)
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
		emit_quad(e.mb, p, uv, col, -f.d);
	};
	face(-f.r, -ar, walk);
	face(-ar, al, 0);
	face(al, f.l, walk);
}

// A circle of radius r round a turning bulb, open where the road comes in (node_outline.h bulb_ring), its height rising from the
// node's to the mouth's along the arm.
std::vector<Vector3> bulb_ring(const RoadEmit &e, const EndFrame &f, const ArmEnd &en, double r) {
	const double setback = bulb_setback(en.end);
	std::vector<Vector3> pts;
	for (const V2 &s : city::bulb_ring(e.s.roads, en.end, r)) {
		const double along = s.x;
		const float y = along <= 0 ? f.node.y : float(f.node.y + (en.p.y - f.node.y) * std::min(1.0, along / setback));
		pts.push_back(f.node + f.d * s.x + f.left * s.y + Vector3(0, y - f.node.y, 0));
	}
	return pts;
}

void bulb(const RoadEmit &e, int n, const ArmEnd &en) {
	const StreetLook &L = e.L;
	const EndFrame f = end_frame(e, n, en);
	const auto ring = [&](double r, float rise) {
		std::vector<Vector3> pts = bulb_ring(e, f, en, r);
		for (Vector3 &p : pts) p.y += rise;
		return pts;
	};
	const std::vector<Vector3> in = ring(BULB_R, 0);
	// the paving: round the ring its kerb, and across the opening the road comes in by, from the ring's end to the mouth's middle and
	// on to its start, each as far from the ring as it stands inside it
	std::vector<PlateSlice> rim;
	for (size_t k = 1; k < in.size(); k++) rim.push_back(kerb_slice(f.node, in[k - 1], in[k]));
	const float mouth_in = float(BULB_R) - Vector2(en.p.x - f.node.x, en.p.z - f.node.z).length();
	rim.push_back({ in.back(), en.p, float(BULB_R), 0, mouth_in });
	rim.push_back({ en.p, in.front(), float(BULB_R), mouth_in, 0 });
	draw_plate(e, f.node, { en }, rim);
	kerb_band(e, in, ring(BULB_R + f.kw, 0), f.kw > 0 ? L.h_curb : L.h_sidewalk, L.h_sidewalk, f.kw > 0);
	const std::vector<Vector3> inner = ring(BULB_R + f.kw, L.h_sidewalk), back = ring(BULB_R + BULB_WALK, L.h_sidewalk);
	std::vector<Vector3> patch = inner;
	patch.insert(patch.end(), back.rbegin(), back.rend());
	footway_patch(e, patch, inner, back);
	footway_skirt(e, back, inner, footway_colour(L, en));
}

// What a node is drawn as: nothing (no end, a rail's end, ends that all stand on it), a road's cut end, a turning bulb, a junction.
enum class NodeShape { NONE, CUT, BULB, JUNCTION };

NodeShape shape_of(const RoadNode &nd, const std::vector<ArmEnd> &ends) {
	if (ends.empty()) return NodeShape::NONE;
	if (ends.size() > 1) return plated(ends) ? NodeShape::JUNCTION : NodeShape::NONE;
	return ends[0].prof->rail ? NodeShape::NONE : nd.bulb ? NodeShape::BULB : NodeShape::CUT;
}

// The function that draws a node's shape.
template <typename Junction, typename Cut, typename Bulb>
void each_shape(const RoadEmit &e, int n, Junction &&plate, Cut &&cut, Bulb &&turn) {
	const std::vector<ArmEnd> ends = arm_ends(e, n);
	switch (shape_of(e.s.roads.nodes[size_t(n)], ends)) {
		case NodeShape::JUNCTION: plate(e, n, ends); break;
		case NodeShape::BULB: turn(e, n, ends[0]); break;
		case NodeShape::CUT: cut(e, n, ends[0]); break;
		default: break;
	}
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
		const CornerOutline o = corner_of(e, a, b);
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
	const std::vector<Vector3> ring = bulb_ring(e, f, en, BULB_R + BULB_WALK);
	std::vector<Vector3> away;
	for (const Vector3 &p : ring) away.push_back(flat_unit(p - f.node));
	verge_from(e, ring, away, f.kw > 0 ? e.L.h_sidewalk : 0.f);
}
} // namespace

bool node_paved(const RoadEmit &e, int n) {
	const NodeShape shape = shape_of(e.s.roads.nodes[size_t(n)], arm_ends(e, n));
	return shape == NodeShape::JUNCTION || shape == NodeShape::BULB;
}

void emit_verge_node(const RoadEmit &e, int n) { each_shape(e, n, junction_verge, cut_verge, bulb_verge); }

void emit_node(const RoadEmit &e, int n) { each_shape(e, n, junction, road_cut, bulb); }

} // namespace city
