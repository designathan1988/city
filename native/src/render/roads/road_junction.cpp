// Node shapes: a junction's plate as one outline (the mouths of its arms joined by the corners between neighbouring arms: kerb
// returns, sim/roads/junction.h corner_fillet, or smooth curves) cut on its own points (core/ring_triangles.h), each point in the
// tone of the arm its stretch of outline belongs to, eased across a corner from one arm's to the next; each corner carrying its kerb
// face, kerb stone and footway; a dead end is a plain cut or a turning bulb.
#include "core/ring_triangles.h"
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
constexpr int BULB_RETURN_LEAST_STEPS = 2; // a kerb return in no fewer pieces than this

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

// A plate inside the outline `pts` (each point's sRGB tone in `tones`), cut on its own points (core/ring_triangles.h): the lab's
// junction plate is one asphalt surface (render/roads/junctionPiece.ts), meshed whole by earcut and its refine (pieceMesh.ts).
void cut_plate(const RoadEmit &e, const std::vector<Vector3> &pts, const std::vector<Color> &tones) {
	const Vector2 pl(ROAD_POOL_PLATE, 0);
	std::vector<double> plan;
	std::vector<int> ids;
	plan.reserve(pts.size() * 2);
	ids.reserve(pts.size());
	for (size_t i = 0; i < pts.size(); i++) {
		plan.insert(plan.end(), { double(pts[i].x), double(pts[i].z) });
		ids.push_back(e.mb.add(pts[i], UP, road_color(tones[i], RoadMat::PLATE), Vector2(pts[i].x, pts[i].z), pl));
	}
	const std::vector<int> tris = ring_triangles(plan);
	for (size_t t = 0; t + 2 < tris.size(); t += 3) e.mb.tri(ids[size_t(tris[t])], ids[size_t(tris[t + 1])], ids[size_t(tris[t + 2])], UP);
}

// The tone eased from a to b along a corner, u from 0 at a's mouth to 1 at b's: a cubic smoothstep, so the passage leaves each
// mouth with no crease (the tone of each mouth is its road's, where the road's own surface meets it).
Color eased_tone(const Color &a, const Color &b, float u) { return a.lerp(b, u * u * (3 - 2 * u)); }

void junction(const RoadEmit &e, int, const NodeShape &shape) {
	const std::vector<End> &ends = shape.ends;
	std::vector<Vector3> ring;
	std::vector<Color> tones;
	size_t k = ends.size();
	for (size_t i = 0; i < k; i++) {
		const End &a = ends[i], &b = ends[(i + 1) % k];
		const Color &ta = e.L.carriage(*a.prof), &tb = e.L.carriage(*b.prof);
		ring.push_back(side(a, -a.arm.asph_right));
		tones.push_back(ta);
		const std::vector<Vector3> &in = shape.corners[i].in;
		for (size_t j = 0; j < in.size(); j++) {
			ring.push_back(in[j]);
			tones.push_back(eased_tone(ta, tb, in.size() > 1 ? float(j) / float(in.size() - 1) : 0.f));
		}
		float ka = kerb_w(a, 1), kb = kerb_w(b, -1);
		std::vector<Vector3> bk = outside(in, a, b, ka, kb);
		kerbed_line(e, in, bk, shape.corners[i].out, float(a.arm.asph_left), ka > 0 || kb > 0);
	}
	cut_plate(e, ring, tones);
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

// A turning bulb (the lab's world/junction/bulb.ts): its circle round the node joined to the road's edges by kerb returns, at the
// carriageway, the kerb stone's back and the footway's outer edge, each line in the same pieces so the kerbed edge pairs them.
// Its mouth stands where the road's strip ends (its setback, reconciled against the road's length), the height eased from the
// node's to the mouth's.
void bulb(const RoadEmit &e, int n, const End &en) {
	const StreetLook &L = e.L;
	const Tuning &tn = e.s.catalog.tune;
	const EndFrame f = end_frame(e, n, en);
	const double setback = std::max(en.at, FILLET_SLACK);
	const float kw = f.kw;
	const double walk = std::max(en.arm.left - en.arm.asph_left, en.arm.right - en.arm.asph_right);
	const int returns = std::max(BULB_RETURN_LEAST_STEPS, int(std::ceil(PI_D / 2 / L.arc_step)));
	const auto line = [&](double grow, double hl, double hr) {
		BulbLine b;
		std::vector<Vector3> pts;
		if (!bulb_line(tn, hl, hr, grow, L.bulb_steps, returns, b)) return pts;
		b.pts.front().x = b.pts.back().x = setback;
		for (const XZ &q : b.pts) {
			const float y = q.x <= 0 ? f.node.y : float(f.node.y + (en.p.y - f.node.y) * std::min(1.0, q.x / setback));
			Vector3 p = f.node + f.d * float(q.x) + f.left * float(q.z);
			p.y = y;
			pts.push_back(p);
		}
		return pts;
	};
	const std::vector<Vector3> in = line(0, en.arm.asph_left, en.arm.asph_right);
	const std::vector<Vector3> bk = line(kw, en.arm.asph_left + kw, en.arm.asph_right + kw);
	const std::vector<Vector3> out = line(walk, en.arm.left, en.arm.right);
	if (in.empty() || bk.size() != in.size() || out.size() != in.size()) return;
	cut_plate(e, in, std::vector<Color>(in.size(), e.L.carriage(*en.prof)));
	kerbed_line(e, in, bk, out, float(en.arm.asph_left), kw > 0);
}
// What a node is, and the function that draws that shape. A plain road end draws nothing of its own: the road's surface runs to
// the cut and closes its raised strips there (road_segment.cpp end_faces), and the ground past it stands at the deck.
template <typename Junction, typename Bulb>
void each_shape(const RoadEmit &e, int n, Junction &&plate, Bulb &&turn) {
	const RoadNode &nd = e.s.roads.nodes[n];
	if (nd.segs.empty()) return;
	const std::shared_ptr<const NodeShape> shape = node_shape(e, n);
	const std::vector<End> &ends = shape->ends;
	if (ends.size() == 1) {
		if (ends[0].prof->rail) return;
		if (nd.bulb) turn(e, n, ends[0]);
		return;
	}
	if (is_plate(e.L.shapes.road_shapes, ends)) plate(e, n, *shape);
}

} // namespace

void emit_node(const RoadEmit &e, int n) { each_shape(e, n, junction, bulb); }

} // namespace city
