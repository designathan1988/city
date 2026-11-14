// What is written on the lanes of a road, as thin lifted paint like its lines (road_markings.cpp), sized by
// data/render/streets.json markings.legends (CONTRAN's manual of horizontal marking, vol. IV): before a junction, an arrow
// on each lane where the lanes of the approach differ in the movements they take (a lane's painted turns where they bind,
// else its place among the lanes: the same rule the traffic's connections follow, sim/traffic/lane_graph.cpp), two rows of
// them; the stop word or the give-way triangle on every arriving lane of an approach that stops or gives way, wherever
// its plate stands (a sign of the player's or the junction's own priority); and along a bus lane its word. A legend is drawn for the driver who arrives: its top away from him.
#include "render/roads/road_mesh.h"

#include "sim/roads/junction.h"
#include "sim/state/city_cut.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;
constexpr float LETTER_W = 0.8f; // a letter's box is one high and this wide (streets.json letters)
constexpr float LETTER_ASPECT = 0.5f; // a letter is never wider than this share of its height
constexpr float FAR_END_M = 1.f; // a legend keeps this far from the other mouth's stop line
constexpr int ARROW_ROWS = 2; // the manual recommends two in sequence on a lane

// One lane as a place to paint on, for the traffic that arrives at one of the road's ends.
struct Bed {
	const RoadEmit &e;
	int sid;
	float u, w; // the lane's middle across the road (left of a to b positive) and its width
	float way; // +1: its traffic runs towards b; -1: towards a
	double line; // the arc of the stop line it arrives at
	float room; // how far before that line the lane runs free

	// A point of the lane: `right` metres to the driver's right of its middle, `back` metres before the stop line.
	Vector3 at(const Vector2 &p) const {
		const SegFrame f = seg_frame(e.s, sid, line - double(p.y) * way);
		return f.c + f.left * (u - p.x * way) + Vector3(0, e.L.paint_lift, 0);
	}
	// Four corners, each (right, back).
	void quad(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d) const {
		const Vector3 p[4] = { at(a), at(b), at(c), at(d) };
		const Vector2 uv[4] = { a, b, c, d };
		emit_quad(e.mb, p, uv, road_color(e.L.white, RoadMat::PAINT), Vector3(0, 1, 0));
	}
	// A bar from a to b, `wide` across, squared half its width past both ends so that strokes close their corners.
	void bar(const Vector2 &a, const Vector2 &b, float wide) const {
		const float len = (b - a).length();
		if (len <= 0) return;
		const Vector2 along = (b - a) / len, cap = along * (wide * HALF), side = Vector2(-along.y, along.x) * (wide * HALF);
		quad(a - cap - side, a - cap + side, b + cap + side, b + cap - side);
	}
	bool fits(float back) const { return back <= room - FAR_END_M; }
};

// The lane arrow of the movements `turns`, its tip `tip` metres before the stop line: a shaft narrowing upwards, a head
// ahead when it goes straight on, an arm with its own head to each side it turns to.
void arrow(const Bed &b, uint8_t turns, float tip, float length) {
	const StreetLook::Legends &g = b.e.L.legends;
	const float foot = tip + length;
	const auto at = [&](float right, float up) { return Vector2(right, foot - up); }; // `up`: from the arrow's foot
	const bool ahead = (turns & PAINTED_STRAIGHT) != 0;
	const float turn = length * g.turn_at, shaft = ahead ? length - g.head_m : turn;
	const float top = g.shaft_foot + (g.shaft_top - g.shaft_foot) * shaft / (length - g.head_m); // its width where it ends
	b.quad(at(-g.shaft_foot * HALF, 0), at(g.shaft_foot * HALF, 0), at(top * HALF, shaft), at(-top * HALF, shaft));
	if (ahead) b.quad(at(-g.head_w * HALF, shaft), at(g.head_w * HALF, shaft), at(0, length), at(0, length));
	for (const float side : { -1.f, 1.f }) {
		if (!(turns & (side < 0 ? PAINTED_LEFT : PAINTED_RIGHT))) continue;
		const float reach = b.w * g.arm_share, neck = reach - g.head_m * HALF, thick = g.shaft_foot * HALF;
		b.quad(at(0, turn - thick), at(side * neck, turn - thick), at(side * neck, turn + thick), at(0, turn + thick));
		b.quad(at(side * neck, turn - g.head_w * HALF), at(side * neck, turn + g.head_w * HALF), at(side * reach, turn), at(side * reach, turn));
	}
}

// A word across the lane, its letters `height` tall, their tops `clear` metres before the stop line.
void word(const Bed &b, const std::string &text, float height, float clear) {
	const StreetLook::Legends &g = b.e.L.legends;
	const float count = float(text.size());
	if (text.empty() || !b.fits(clear + height)) return;
	const float wide = std::min(b.w * g.lane_share / (count + (count - 1) * g.gap), height * LETTER_ASPECT), pitch = wide * (1 + g.gap);
	const float left = -(count * wide + (count - 1) * wide * g.gap) * HALF, stroke = wide * g.stroke;
	for (size_t i = 0; i < text.size(); i++) {
		const auto drawn = g.letters.find(text[i]);
		if (drawn == g.letters.end()) continue; // a letter the table does not draw
		const auto at = [&](const Vector2 &p) { return Vector2(left + float(i) * pitch + p.x * wide / LETTER_W, clear + height * (1 - p.y)); };
		for (const std::vector<Vector2> &line : drawn->second)
			for (size_t k = 0; k + 1 < line.size(); k++) b.bar(at(line[k]), at(line[k + 1]), stroke);
	}
}

// The give-way triangle: its base nearest the junction, pointing against the traffic.
void give_way(const Bed &b, float speed) {
	const StreetLook::Legends &g = b.e.L.legends;
	const bool fast = speed > g.yield_fast_kmh;
	const float len = fast ? g.yield_fast_m : g.yield_m, wide = std::min(fast ? g.yield_fast_w : g.yield_w, b.w * g.lane_share);
	const float band = fast ? g.yield_fast_band : g.yield_band;
	if (!b.fits(g.yield_clear + len)) return;
	const Vector2 one(-wide * HALF, g.yield_clear), two(wide * HALF, g.yield_clear), tip(0, g.yield_clear + len);
	b.bar(one, two, band);
	b.bar(two, tip, band);
	b.bar(tip, one, band);
}

// The turns the junction at `node` offers the traffic arriving on `sid`, as PAINTED_ bits: one for each road it may go
// on to, by the angle between the two (the traffic's own rule: lane_graph.cpp classify).
uint8_t offered_at(const CityView &s, int node, int sid) {
	const RoadNetwork &net = s.roads;
	const RoadSegment &g = net.segs[size_t(sid)];
	const Arm in = arm_of(g.q, net.profile_of(sid), g.a == node ? 0 : 1);
	const double hx = -in.dx, hz = -in.dz; // the heading it arrives with
	uint8_t out = 0;
	for (int other : net.nodes[size_t(node)].segs) {
		if (other == sid || net.movement_banned(node, sid, other)) continue;
		const RoadSegment &o = net.segs[size_t(other)];
		const Profile &po = net.profile_of(other);
		if (po.rail || (o.a == node ? po.fwd : po.back).empty()) continue; // no lane leaves the junction on it
		const Arm arm = arm_of(o.q, po, o.a == node ? 0 : 1);
		if (hx * arm.dx + hz * arm.dz > double(s.catalog.traffic.straight_dot)) out |= PAINTED_STRAIGHT;
		else out |= arm.dx * hz - arm.dz * hx > 0 ? PAINTED_LEFT : PAINTED_RIGHT;
	}
	return out;
}

// The movements lane k of n takes among those offered: its painted turns where they bind, else by its place (straight on
// from any, right from the kerb lane, left from the innermost).
uint8_t lane_turns(const LaneInfo &l, int k, int n, uint8_t offered) {
	if (l.turns & offered) return l.turns & offered;
	uint8_t out = offered & PAINTED_STRAIGHT;
	if (k == 0) out |= offered & PAINTED_RIGHT;
	if (k == n - 1) out |= offered & PAINTED_LEFT;
	return out;
}

// What an approach's lanes are told before the junction they arrive at: the rule's legend, then the rows of arrows.
void approach(const RoadEmit &e, int sid, const Profile &p, const std::vector<Bed> &beds, const std::vector<LaneInfo> &lanes, int node) {
	const StreetLook::Legends &g = e.L.legends;
	// the rule its plate says, the player's sign or the junction's own priority alike (city_cut.h leg_rule_of)
	const int rule = leg_rule_of(e.s, node, sid), n = int(beds.size());
	for (const Bed &b : beds) {
		if (rule == RULE_STOP) word(b, g.stop, p.speed > g.word_fast_kmh ? g.word_fast_m : g.word_m, g.word_clear);
		if (rule == RULE_YIELD) give_way(b, p.speed);
	}
	const uint8_t offered = offered_at(e.s, node, sid);
	std::vector<uint8_t> turns;
	for (int k = 0; k < n; k++) turns.push_back(lane_turns(lanes[size_t(k)], k, n, offered));
	// arrows only where lanes are for different movements
	if (n < 2 || std::all_of(turns.begin(), turns.end(), [&](uint8_t t) { return t == turns.front(); })) return;
	const bool fast = p.speed >= g.from_kmh, faster = p.speed > g.faster_kmh;
	const float length = faster ? g.arrow_fast_m : g.arrow_m, first = fast ? g.fast_first_m : g.first_m;
	const float next = faster ? g.faster_next_m : fast ? g.fast_next_m : g.next_m;
	// the rows are counted from where lanes can no longer be changed: before a signal, the start of its approach line
	// (road_markings.cpp; MBST vol. IV 9.1.1: 'the start of the single continuous approach line')
	const float held = leg_at_signal(e.s, node, sid) ? float(e.s.catalog.traffic.approach_line_m) : 0.f;
	for (int k = 0; k < n; k++)
		for (int row = 0; row < ARROW_ROWS && turns[size_t(k)]; row++) {
			const float tip = held + first + next * float(row);
			if (beds[size_t(k)].fits(tip + length)) arrow(beds[size_t(k)], turns[size_t(k)], tip, length);
		}
}
} // namespace

void emit_legends(const RoadEmit &e, int sid) {
	const RoadNetwork &net = e.s.roads;
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	const StreetLook::Legends &look = e.L.legends;
	// the two stop lines, as arcs from node a, and the free run between them
	const double at_a = g.retreat_a + net.stop_line_back(g.a, sid), at_b = g.len - g.retreat_b - net.stop_line_back(g.b, sid);
	const float room = float(at_b - at_a);
	if (room <= 0) return;
	for (int end = 0; end < 2; end++) { // the traffic that arrives at b runs on the forward lanes, at a on the backward ones
		const std::vector<LaneInfo> &lanes = end == 1 ? p.fwd : p.back;
		const int node = end == 1 ? g.b : g.a;
		std::vector<Bed> beds;
		for (const LaneInfo &l : lanes) beds.push_back({ e, sid, l.u, l.w, end == 1 ? 1.f : -1.f, end == 1 ? at_b : at_a, room });
		if (beds.empty()) continue;
		if (crossing_node(e.s, node)) approach(e, sid, p, beds, lanes, node);
		for (size_t k = 0; k < beds.size(); k++) { // a bus lane says so all along
			if (!lanes[k].bus || look.bus_every_m <= 0) continue;
			const float tall = p.speed > look.word_fast_kmh ? look.word_fast_m : look.word_m;
			for (float back = look.bus_first_m; beds[k].fits(back + tall + look.bus_first_m); back += look.bus_every_m) word(beds[k], look.bus, tall, back);
		}
	}
}

} // namespace city
