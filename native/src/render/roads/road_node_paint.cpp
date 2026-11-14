// What is painted on a node's paving (road_junction.cpp calls it at the near level), as thin lifted paint like a segment's
// lines (road_markings.cpp): a mini-roundabout's island, one white disc at the node (the UK's TD 54/07: "the white circle",
// "between one and four metres in diameter": data/render/streets.json markings.mini_island); and across a node where one
// two-way road carries on at another width, the lines that lead one road's paint into the other's: the two directions
// parting round a median's nose (or one line down the middle, each half in its own road's colour) and a dashed line
// between the lanes both roads have, every offset eased from one mouth's to the other's.
#include "render/roads/road_node.h"
#include "render/roads/road_section.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, TWICE = 2.f, FULL_TURN_RAD = 6.28318531f;
constexpr float EASE_A = 3.f, EASE_B = 2.f; // the cubic 3 u^2 - 2 u^3: level at both mouths
constexpr int LEAST_PIECES = 2;
constexpr int ISLAND_ARMS = 3; // a mini-roundabout is a junction of three roads or more
constexpr int TAPER_ARMS = 2; // a road that carries on at another width is a node of two

float eased(float u) { return u * u * (EASE_A - EASE_B * u); }

// The line between a road's two directions as its profile paints it, or none.
const MarkLine *axis_line(const Profile &p) {
	for (const MarkLine &m : p.lines)
		if (m.centre && m.kind != LineKind::NONE) return &m;
	return nullptr;
}

// Half the width of the reservation between a road's two directions (0: none).
float half_median(const Profile &p) {
	float u0 = 0, u1 = 0;
	return island_span(p, u0, u1) ? (u1 - u0) * HALF : 0.f;
}

// The lanes a direction and their width, of the direction with the more of them.
int lanes_a_way(const Profile &p) { return std::max(p.lanes_fwd(), p.lanes_back()); }
float width_a_lane(const Profile &p) { return !p.fwd.empty() ? p.fwd.front().w : !p.back.empty() ? p.back.front().w : 0.f; }

// The way across a node of two arms: from a's mouth through the node to b's, and each line's place across it.
struct Taper {
	const RoadEmit &e;
	Vector3 from, node, to;
	float len;

	// The point at share u of the way and the unit across it, to the left going from a to b.
	void frame(float u, Vector3 &at, Vector3 &left) const {
		const float r = 1 - u;
		at = from * (r * r) + node * (TWICE * r * u) + to * (u * u);
		Vector3 along = (node - from) * (TWICE * r) + (to - node) * (TWICE * u);
		along.y = 0;
		along = along.length() > 0 ? along.normalized() : Vector3(1, 0, 0);
		left = Vector3(along.z, 0, -along.x);
	}

	// A line `wide` across from share u0 to u1, at `offset(u)` to the left; `dash` > 0: painted `dash` of every `period` metres.
	template <typename Offset>
	void line(float u0, float u1, Offset &&offset, float wide, const Color &colour, float dash = 0, float period = 0) const {
		const float least = dash > 0 ? std::min(e.L.step, dash * HALF) : e.L.step;
		const int pieces = std::max(LEAST_PIECES, int(std::ceil(len * (u1 - u0) / std::max(least, e.L.face_eps))));
		const Color paint = road_color(colour, RoadMat::PAINT);
		const Vector3 lift(0, e.L.paint_lift, 0);
		for (int k = 0; k < pieces; k++) {
			const float ua = u0 + (u1 - u0) * float(k) / float(pieces), ub = u0 + (u1 - u0) * float(k + 1) / float(pieces);
			if (dash > 0 && std::fmod(len * (ua + ub) * HALF, period) >= dash) continue;
			Vector3 pa, la, pb, lb;
			frame(ua, pa, la);
			frame(ub, pb, lb);
			const float oa = offset(ua), ob = offset(ub), hw = wide * HALF;
			const Vector3 p[4] = { pa + la * (oa - hw) + lift, pa + la * (oa + hw) + lift, pb + lb * (ob + hw) + lift, pb + lb * (ob - hw) + lift };
			const Vector2 uv[4] = { Vector2(oa - hw, len * ua), Vector2(oa + hw, len * ua), Vector2(ob + hw, len * ub), Vector2(ob - hw, len * ub) };
			emit_quad(e.mb, p, uv, paint, Vector3(0, 1, 0));
		}
	}
};

Color centre_colour(const StreetLook &L, const StreetLook::PaintStyle &st, const MarkLine &m) {
	return m.color == LineColor::YELLOW && !st.all_white ? L.yellow : L.white;
}

// One white disc at the node: its diameter a share of the nearest mouth's setback, within the norm's least and most.
void island(const RoadEmit &e, const Vector3 &node, const std::vector<ArmEnd> &ends) {
	const StreetLook::MiniIsland &d = e.L.mini_island;
	double nearest = ends.front().at;
	for (const ArmEnd &en : ends) nearest = std::min(nearest, en.at);
	const float radius = std::clamp(float(nearest) * d.mouth_share, d.least_m, d.most_m) * HALF;
	const Color paint = road_color(e.L.white, RoadMat::PAINT);
	const Vector3 centre = node + Vector3(0, e.L.paint_lift, 0);
	const auto rim = [&](int k) {
		const float turn = FULL_TURN_RAD * float(k) / float(d.sides);
		return centre + Vector3(std::cos(turn), 0, std::sin(turn)) * radius;
	};
	for (int k = 0; k < d.sides; k++) {
		const Vector3 a = rim(k), b = rim(k + 1);
		const Vector3 p[4] = { centre, a, b, b };
		const Vector2 uv[4] = { Vector2(0, 0), Vector2(a.x - centre.x, a.z - centre.z), Vector2(b.x - centre.x, b.z - centre.z), Vector2(b.x - centre.x, b.z - centre.z) };
		emit_quad(e.mb, p, uv, paint, Vector3(0, 1, 0));
	}
}

// Across a node where a two-way road carries on at another width.
void taper(const RoadEmit &e, const Vector3 &node, const ArmEnd &a, const ArmEnd &b) {
	const Profile &pa = *a.prof, &pb = *b.prof;
	const MarkLine *ca = axis_line(pa), *cb = axis_line(pb);
	if (pa.one_way || pb.one_way || pa.rail || pb.rail || (pa.lines.empty() && pb.lines.empty())) return;
	const StreetLook &L = e.L;
	const StreetLook::PaintStyle &st = L.styles[size_t(e.s.marking_style.load())];
	const Taper way{ e, a.p, node, b.p, (node - a.p).length() + (b.p - node).length() };
	if (way.len <= L.face_eps) return;
	const float ma = half_median(pa), mb = half_median(pb);
	const auto median = [&](float u) { return ma + (mb - ma) * eased(u); };
	if (ma > 0 || mb > 0) { // the two directions part to either side of the reservation's nose
		const MarkLine *own = mb > 0 ? cb : ca, *other = mb > 0 ? ca : cb;
		const Color colour = own ? centre_colour(L, st, *own) : other ? centre_colour(L, st, *other) : L.white;
		way.line(0, 1, median, st.centre_w, colour);
		way.line(0, 1, [&](float u) { return -median(u); }, st.centre_w, colour);
	} else if (ca || cb) { // one line down the middle, each half in its own road's colour
		const auto middle = [](float) { return 0.f; };
		way.line(0, HALF, middle, st.centre_w, centre_colour(L, st, ca ? *ca : *cb));
		way.line(HALF, 1, middle, st.centre_w, centre_colour(L, st, cb ? *cb : *ca));
	}
	// the lanes both roads have run on, dashed as the faster road's lines are
	const float speed = std::max(pa.speed, pb.speed), wa = width_a_lane(pa), wb = width_a_lane(pb);
	size_t band = 0;
	while (band < st.dash_kmh.size() && speed >= st.dash_kmh[band]) band++;
	const float dash = st.dash[band], period = dash + st.gap[band];
	for (int k = 1; k < std::min(lanes_a_way(pa), lanes_a_way(pb)); k++)
		for (const float side : { -1.f, 1.f })
			way.line(0, 1, [&](float u) { return side * (median(u) + (wa + (wb - wa) * eased(u)) * float(k)); }, st.lane_w, L.white, dash, period);
}
} // namespace

void node_paint(const RoadEmit &e, int n, const std::vector<ArmEnd> &ends) {
	const RoadNode &nd = e.s.roads.nodes[size_t(n)];
	const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
	if (nd.ctl == CTL_MINI && int(ends.size()) >= ISLAND_ARMS) island(e, node, ends);
	if (int(ends.size()) == TAPER_ARMS) taper(e, node, ends[0], ends[1]);
}

} // namespace city
