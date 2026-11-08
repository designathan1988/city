// Node shapes: an asphalt plate fanned from the
// node to every arm's mouth, the corners between neighbouring arms as kerb returns (sim/roads/junction.h
// corner_fillet) or smooth curves, each corner carrying its kerb face, kerb stone and footway; dead ends get a footway
// pad behind a kerb or a turning bulb.
#include "render/roads/road_mesh.h"
#include "render/roads/road_shapes.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <iterator>
#include <cmath>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

Vector3 flat_unit(Vector3 v) {
	v.y = 0;
	return v.length() > 0 ? v.normalized() : UP;
}

using End = RoadEnd;

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

// A plate from the node out to `ring`, each point in the tone of the road nearest it: the paving that is not asphalt and the turning
// bulb, which have no tones to share.
void fan(const RoadEmit &e, const Vector3 &centre, const std::vector<End> &ends, const std::vector<Vector3> &ring) {
	const Vector2 pl(ROAD_POOL_PLATE, 0);
	auto add = [&](const Vector3 &p, const Color &c) { return e.mb.add(p, UP, road_color(c, RoadMat::PLATE), Vector2(p.x, p.z), pl); };
	Color mean(0, 0, 0);
	for (const End &en : ends) mean += e.L.carriage(*en.prof);
	const int c0 = add(centre, mean / float(ends.size()));
	std::vector<int> out;
	for (const Vector3 &p : ring) {
		const End *nearest = &ends[0];
		for (const End &en : ends) nearest = (p - en.p).length() < (p - nearest->p).length() ? &en : nearest;
		out.push_back(add(p, e.L.carriage(*nearest->prof)));
	}
	for (size_t i = 0; i < ring.size(); i++) e.mb.tri(c0, out[i], out[(i + 1) % ring.size()], UP);
}

// The asphalt between two arms, from a's centre line round the kerb to b's. Across a wedge the difference of the distances to the
// two lines is linear, so the mesh carries it and the shader lays the passage between the tones over a width of its own, the same
// from the kerb to the node (road.gdshader M_JOIN). Fans of arms of one tone are the same, and no more triangles.
void wedge(const RoadEmit &e, const Vector3 &node, const End &a, const End &b, const std::vector<Vector3> &line) {
	const Vector3 left_a(a.d.z, 0, -a.d.x), left_b(b.d.z, 0, -b.d.x);
	Color tones = wear_color(0, 0, 0, RoadMat::JOIN, int(a.prof->road_class));
	tones.r = (float(int(b.prof->road_class)) + MAT_HALF) / TONE_SLOTS;
	auto add = [&](const Vector3 &p) {
		const Vector3 v = p - node;
		return e.mb.add(p, UP, tones, Vector2(p.x, p.z), Vector2(ROAD_POOL_PLATE, v.dot(left_a) + v.dot(left_b)));
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

void junction(const RoadEmit &e, int n, const NodeShape &shape) {
	const RoadNode &nd = e.s.roads.nodes[n];
	Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	const std::vector<End> &ends = shape.ends;
	std::vector<Vector3> ring;
	bool asphalt = true;
	for (const End &en : ends) asphalt = asphalt && en.prof->surface == Surface::ASPHALT;
	size_t k = ends.size();
	for (size_t i = 0; i < k; i++) {
		const End &a = ends[i], &b = ends[(i + 1) % k];
		ring.push_back(side(a, -a.arm.asph_right));
		const std::vector<Vector3> &in = shape.corners[i].in;
		ring.insert(ring.end(), in.begin(), in.end());
		if (asphalt) wedge(e, node, a, b, in);
		float ka = kerb_w(a, 1), kb = kerb_w(b, -1);
		std::vector<Vector3> bk = outside(in, a, b, ka, kb);
		kerbed_line(e, in, bk, shape.corners[i].out, float(a.arm.asph_left), ka > 0 || kb > 0, walk_color(e.L, a, 1));
	}
	if (!asphalt) fan(e, node, ends, ring);
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

// A plain road end: the carriageway runs to the cut and the footways stop level with it, closed by one face down to the
// ground on each piece of the section (Roadcraft 4.0 has no kerb across the end).
void road_cut(const RoadEmit &e, int n, const End &en) {
	const StreetLook &L = e.L;
	const EndFrame f = end_frame(e, n, en);
	const float ar = float(en.arm.asph_right), al = float(en.arm.asph_left), walk = f.kw > 0 ? L.h_sidewalk : 0.f;
	const Color col = road_color(L.curb, RoadMat::KERB);
	auto face = [&](float u0, float u1, float h) {
		if (u1 <= u0) return;
		const Vector3 a = f.node + f.left * u0, b = f.node + f.left * u1, up(0, h, 0), down(0, -L.skirt, 0);
		const Vector3 p[4] = { a + up, b + up, b + down, a + down };
		const Vector2 uv[4] = { Vector2(u0, h), Vector2(u1, h), Vector2(u1, -L.skirt), Vector2(u0, -L.skirt) };
		const Vector2 pool(ROAD_POOL_NONE, 0), pools[4] = { pool, pool, pool, pool };
		emit_quad(e.mb, p, uv, pools, col, -f.d);
	};
	face(-f.r, -ar, walk);
	face(-ar, al, 0);
	face(al, f.l, walk);
}

// A circle of radius r round a turning bulb, open where the road comes in.
std::vector<Vector3> bulb_ring(const StreetLook &L, const EndFrame &f, const End &en, double setback, double r) {
	std::vector<Vector3> pts;
	double a_in = std::acos(std::min(1.0, setback / r));
	for (int i = 0; i <= L.bulb_steps; i++) {
		double phi = a_in + double(i) / L.bulb_steps * (TAU_D - 2 * a_in), along = r * std::cos(phi), u = r * std::sin(phi);
		float y = along <= 0 ? f.node.y : float(f.node.y + (en.p.y - f.node.y) * std::min(1.0, along / setback));
		pts.push_back(f.node + f.d * float(along) + f.left * float(u) + Vector3(0, y - f.node.y, 0));
	}
	return pts;
}

void bulb(const RoadEmit &e, int n, const End &en) {
	const StreetLook &L = e.L;
	EndFrame f = end_frame(e, n, en);
	const Vector3 &node = f.node;
	double setback = std::max(en.at, FILLET_SLACK);
	float kw = f.kw;
	auto ring = [&](double r) { return bulb_ring(L, f, en, setback, r); };
	std::vector<Vector3> in = ring(BULB_R);
	fan(e, node, { en }, in);
	kerbed_line(e, in, ring(BULB_R + kw), ring(BULB_R + BULB_WALK), float(en.arm.asph_left), kw > 0, walk_color(L, en, 1));
}
// What a node is, and the function that draws that shape.
template <typename Junction, typename Cut, typename Bulb>
void each_shape(const RoadEmit &e, int n, Junction &&plate, Cut &&cut, Bulb &&turn) {
	const RoadNode &nd = e.s.roads.nodes[n];
	if (nd.segs.empty()) return;
	const std::shared_ptr<const NodeShape> shape = node_shape(e, n);
	const std::vector<End> &ends = shape->ends;
	if (ends.size() == 1) {
		if (ends[0].prof->rail) return;
		if (nd.bulb) turn(e, n, ends[0]);
		else cut(e, n, ends[0]);
		return;
	}
	if (is_plate(e.L.shapes.road_shapes, ends)) plate(e, n, *shape);
}

// The verge along a line of the paving, each point taken `rise` up and sent out towards `from` the other way.
void verge_from(const RoadEmit &e, std::vector<Vector3> edge, const std::vector<Vector3> &away, float rise) {
	for (Vector3 &p : edge) p.y += rise;
	verge_ribbon(e, edge, away);
}

void junction_verge(const RoadEmit &e, int, const NodeShape &shape) {
	const std::vector<End> &ends = shape.ends;
	for (size_t i = 0; i < ends.size(); i++) {
		const End &a = ends[i], &b = ends[(i + 1) % ends.size()];
		const std::vector<Vector3> &in = shape.corners[i].in, &out = shape.corners[i].out;
		const std::vector<Vector3> unit = outside(in, a, b, 1.f, 1.f);
		const size_t tip = shape.corners[i].tip;
		std::vector<Vector3> away;
		for (size_t k = 0; k < in.size(); k++) away.push_back(flat_unit(unit[k] - in[k]));
		const float rise = kerb_w(a, 1) > 0 || kerb_w(b, -1) > 0 ? e.L.h_sidewalk : 0.f;
		if (tip > 0) { // each edge sends its ground straight out: past the point they overlap, the nearer one on top, and that is the mitre
			verge_from(e, { out.front(), out[tip] }, { away.front(), away.front() }, rise);
			verge_from(e, { out[tip], out.back() }, { away.back(), away.back() }, rise);
			continue;
		}
		// Where the outer line runs back on itself the arms' own ribbons already meet: only where it runs with the kerb is the ground its own.
		size_t from = 0;
		for (size_t k = 1; k <= in.size(); k++) {
			if (k < in.size() && (out[k] - out[k - 1]).dot(in[k] - in[k - 1]) > 0) continue;
			if (k - from > 1) verge_from(e, { out.begin() + from, out.begin() + k }, { away.begin() + from, away.begin() + k }, rise);
			from = k;
		}
	}
}

void cut_verge(const RoadEmit &e, int n, const End &en) {
	const EndFrame f = end_frame(e, n, en);
	const float ar = float(en.arm.asph_right), al = float(en.arm.asph_left), walk = f.kw > 0 ? e.L.h_sidewalk : 0.f;
	const float at[] = { -f.r, -ar, -ar, al, al, f.l }, up[] = { walk, walk, 0, 0, walk, walk };
	std::vector<Vector3> edge;
	for (size_t k = 0; k < std::size(at); k++) edge.push_back(f.node + f.left * at[k] + Vector3(0, up[k], 0));
	verge_ribbon(e, edge, std::vector<Vector3>(edge.size(), -f.d));
}

void bulb_verge(const RoadEmit &e, int n, const End &en) {
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
