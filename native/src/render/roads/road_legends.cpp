// What is painted on the road's floor beyond its lines (the lab's world/markings.ts, its look in world/roads/markingStyle.ts): the
// word of the map's style before the stop line of a leg that must stop and the give-way symbol before a leg that gives way
// (approachLegendsAt), the word along a bus lane (busLegends), the turn arrows of a lane with turns of its own (laneArrowMarkings)
// and a mini-roundabout's painted island (miniRoundaboutIsland). Every size is data/render/streets.json markings.floor's, the
// words and their length the style's; the rule of a leg is the traffic's own reading of the map (traffic::approach_rule).
#include "render/roads/road_paint.h"

#include "core/math.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr uint8_t TURN_LEFT = 1, TURN_STRAIGHT = 2, TURN_RIGHT = 4; // Strip::turns (sim/catalog/profile_types.h)

// A lane seen by its driver: points given to the driver's left (x) and along the way they drive (y), in metres from a place on
// the lane's middle `from` metres from node a, made the segment's (u, v).
struct LaneFrame {
	float u = 0;
	double from = 0;
	bool forward = true; // driving from node a to node b
	Vector2 at(float left, double along) const {
		return forward ? Vector2(u + left, float(from + along)) : Vector2(u - left, float(from - along));
	}
};

// A set of strokes drawn in a lane: each point (x, y) of a box `width` units wide becomes x * letter to the driver's right of
// the word's middle and y * length along the way.
void paint_strokes(const RoadPainter &pt, const LaneFrame &f, const StreetLook::Strokes &strokes, float x0, float width,
		float letter, float length, float line) {
	std::vector<Vector2> points;
	for (const std::vector<Vector2> &s : strokes) {
		points.clear();
		for (const Vector2 &q : s) points.push_back(f.at(-((q.x + x0) * letter - width * letter / 2), double(q.y) * length));
		pt.stroke(points, line, pt.e.L.white);
	}
}

// A word in a lane, its letters side by side letter_gap of a letter apart (wordStrokes), as wide as the lane allows.
void paint_word(const RoadPainter &pt, const LaneFrame &f, float lane_w, const std::u32string &text, float length) {
	const StreetLook::FloorPaint &F = pt.e.L.floor;
	const float width = std::max(0.f, float(text.size()) * (1 + F.letter_gap) - F.letter_gap);
	const float letter = std::min(lane_w * F.letter_share / std::max(1.f, width), length * F.letter_aspect);
	float x = 0;
	for (char32_t ch : text) {
		const auto g = F.glyphs.find(ch);
		if (g != F.glyphs.end()) paint_strokes(pt, f, g->second, x, width, letter, length, F.stroke);
		x += 1 + F.letter_gap;
	}
}

// The lanes driving towards node b (forward) or towards node a.
const std::vector<LaneInfo> &lanes_towards(const Profile &p, bool forward) { return forward ? p.fwd : p.back; }

// Each arriving leg's rule before its stop line: the style's word where it stops, the give-way symbol where it gives way.
void approach_legends(const RoadPainter &pt, const Profile &p, double v0, double v1, bool ca, bool cb) {
	const StreetLook &L = pt.e.L;
	const StreetLook::FloorPaint &F = L.floor;
	const StreetLook::PaintStyle &st = L.style(pt.e.s.marking_style.load());
	const RoadSegment &g = pt.e.s.roads.segs[size_t(pt.sid)];
	if (p.road_class == RoadClass::HIGHWAY || p.road_class == RoadClass::RAMP) return;
	const double stop = pt.e.s.catalog.furniture.crossing.stop_to_m;
	for (int end = 0; end < 2; end++) {
		if (!(end == 0 ? ca : cb)) continue;
		const int rule = traffic::approach_rule(pt.e.s, end == 0 ? g.a : g.b, pt.sid);
		if (rule == RULE_NONE) continue;
		const bool forward = end == 1; // traffic arriving at node b drives from a to b
		const bool word = rule == RULE_STOP;
		const float length = word ? st.legend_at(p.speed) : F.yield_length;
		const double before = stop + F.legend_gap + length; // from the mouth back to where the legend begins
		if (v1 - v0 - before < F.min_foot) continue;
		for (const LaneInfo &lane : lanes_towards(p, forward)) {
			const LaneFrame f{ lane.u, forward ? v1 - before : v0 + before, forward };
			if (word) paint_word(pt, f, lane.w, st.stop_text, length);
			else paint_strokes(pt, f, F.yield_symbol, 0, 1, std::min(lane.w * F.yield_share, F.yield_max), length, F.yield_stroke);
		}
	}
}

// The style's word along each bus lane, bus_first_m into the painted stretch l0..l1 and then every bus_every_m, read by its driver.
void bus_legends(const RoadPainter &pt, const Profile &p, double l0, double l1) {
	const StreetLook &L = pt.e.L;
	const StreetLook::FloorPaint &F = L.floor;
	const StreetLook::PaintStyle &st = L.style(pt.e.s.marking_style.load());
	const float length = st.legend_at(p.speed);
	const double run = l1 - l0;
	if (run < 2 * F.bus_first + length) return;
	for (int forward = 0; forward < 2; forward++)
		for (const LaneInfo &lane : lanes_towards(p, forward == 1)) {
			if (!lane.bus) continue;
			for (double k = F.bus_first; k + length < run - F.bus_first; k += F.bus_every)
				paint_word(pt, LaneFrame{ lane.u, forward == 1 ? l0 + k : l1 - k, forward == 1 }, lane.w, st.bus_text, length);
		}
}

// Where a lane may go as its rule says: the player's lane links at the node it drives to (traffic::lane_turns, the lane graph's
// own classification), else the turns its profile gives it; by_hand false: the junction decides and nothing is painted.
traffic::LaneTurns rule_of(const RoadPainter &pt, const LaneInfo &lane, int index, bool forward) {
	const RoadSegment &g = pt.e.s.roads.segs[size_t(pt.sid)];
	traffic::LaneTurns t = traffic::lane_turns(pt.e.s, forward ? g.b : g.a, pt.sid, index);
	if (t.by_hand || lane.turns == 0) return t;
	t.by_hand = true;
	t.left = lane.turns & TURN_LEFT;
	t.straight = lane.turns & TURN_STRAIGHT;
	t.right = lane.turns & TURN_RIGHT;
	return t;
}

// A lane's arrows, arrow_station_m before the mouth it drives to: a stem, then a head for each way it may go.
void lane_arrows(const RoadPainter &pt, const Profile &p, double v0, double v1) {
	const StreetLook::FloorPaint &F = pt.e.L.floor;
	if (v1 - v0 < F.arrow_min) return;
	std::vector<Vector2> points;
	for (int forward = 0; forward < 2; forward++) {
		const std::vector<LaneInfo> &lanes = lanes_towards(p, forward == 1);
		for (size_t k = 0; k < lanes.size(); k++) {
			const LaneInfo &lane = lanes[k];
			const traffic::LaneTurns rule = rule_of(pt, lane, int(k), forward == 1);
			if (!rule.by_hand || !(rule.left || rule.straight || rule.right)) continue;
			const LaneFrame f{ lane.u, forward == 1 ? v1 - F.arrow_station : v0 + F.arrow_station, forward == 1 };
			const float lateral = std::min(F.arrow_lateral, lane.w * F.arrow_lateral_share);
			auto draw = [&](const StreetLook::Strokes &strokes, float side) {
				for (const std::vector<Vector2> &s : strokes) {
					points.clear();
					for (const Vector2 &q : s) points.push_back(f.at(q.y * lateral * side, q.x));
					pt.stroke(points, F.arrow_stroke, pt.e.L.white);
				}
			};
			draw(F.arrow_stem, 1);
			if (rule.straight) draw(F.arrow_through, 1);
			if (rule.left) draw(F.arrow_left, 1);
			if (rule.right) draw(F.arrow_left, -1);
		}
	}
}

} // namespace

void floor_paint(const RoadPainter &pt, const Profile &p, double v0, double v1, bool ca, bool cb, double l0, double l1) {
	approach_legends(pt, p, v0, v1, ca, cb);
	bus_legends(pt, p, l0, l1);
	lane_arrows(pt, p, v0, v1);
}

void emit_node_paint(const RoadEmit &e, int node) {
	const RoadNode &nd = e.s.roads.nodes[size_t(node)];
	const StreetLook::FloorPaint &F = e.L.floor;
	if (e.far || nd.ctl != CTL_MINI || nd.segs.size() < size_t(CROSSWALK_ARMS)) return;
	double mouth = INFINITY;
	for (int seg : nd.segs) {
		const RoadSegment &g = e.s.roads.segs[size_t(seg)];
		mouth = std::min(mouth, g.a == node ? g.retreat_a : g.retreat_b);
	}
	const float r = std::clamp(float(mouth) * F.island_share, F.island_min, F.island_max);
	// one disc whose sides stand where a line island_line_m wide round the polygon of radius r had its outer edges
	const float half = PI_F / float(F.island_sides);
	const float radius = (r * std::cos(half) + F.island_line / 2) / std::cos(half);
	const Vector3 up(0, 1, 0), mid(float(nd.x), float(nd.y) + e.L.paint_lift, float(nd.z));
	const Color col = road_color(e.L.white, RoadMat::PAINT);
	const Vector2 lit(ROAD_POOL_PLATE, 0);
	const int centre = e.mb.add(mid, up, col, Vector2(mid.x, mid.z), lit);
	int first = -1, last = -1;
	for (int k = 0; k < F.island_sides; k++) {
		const float a = TAU_F * float(k) / float(F.island_sides);
		const Vector3 p = mid + Vector3(std::cos(a), 0, std::sin(a)) * radius;
		const int i = e.mb.add(p, up, col, Vector2(p.x, p.z), lit);
		if (last >= 0) e.mb.tri(centre, last, i, up);
		else first = i;
		last = i;
	}
	if (first >= 0) e.mb.tri(centre, last, first, up);
}

} // namespace city
