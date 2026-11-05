// Bays and paint of a street's parking bands (parking_layout.h).
#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

constexpr float EPS_BAY = 1e-4f; // a run exactly a whole number of bays long holds them all
constexpr int RIGHT = 0, LEFT = 1;

// `from` less every span of `cut`.
void subtract(std::vector<ArcSpan> &from, const std::vector<ArcSpan> &cut) {
	std::vector<ArcSpan> next;
	for (const ArcSpan &c : cut) {
		next.clear();
		for (const ArcSpan &s : from) {
			if (c.second <= s.first || c.first >= s.second) { next.push_back(s); continue; }
			if (c.first > s.first) next.emplace_back(s.first, c.first);
			if (c.second < s.second) next.emplace_back(c.second, s.second);
		}
		from.swap(next);
	}
}

// How far from `node` along a segment that ends there no bay may start: past the corner and its crosswalk, or just the mouth on a joint.
double end_clear(const RoadNetwork &net, int node, double mouth) {
	const TrafficParams &tp = net.cat->traffic;
	const int arms = int(net.nodes[size_t(node)].segs.size());
	if (arms <= 1) return tp.kerb_dead_end_m;
	if (!net.has_crosswalks(node) && arms == 2) return mouth;
	const CrossingDef &x = net.cat->furniture.crossing;
	const double crossing = net.has_crosswalks(node) ? std::max(x.zebra_to_m, x.stop_to_m) + tp.kerb_crossing_clear_m : 0.0;
	return mouth + std::max(double(tp.kerb_corner_clear_m), crossing);
}

} // namespace

double arc_of_t(const RoadSegment &g, double t) {
	const double at = std::max(0.0, std::min(1.0, t)) * (g.table.size() - 1);
	const size_t i = std::min(size_t(at), g.table.size() - 2);
	return g.table[i] + (g.table[i + 1] - g.table[i]) * (at - double(i));
}

bool parks_along(const RoadNetwork &net, int sid) {
	for (const StripEdge &e : net.profile_of(sid).edges)
		if (e.strip.kind == StripKind::PARKING) return true;
	return false;
}

void kerb_layout(const RoadNetwork &net, int sid, const std::vector<ArcSpan> gates[2], KerbLayout &out) {
	out = KerbLayout();
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	const float pitch = net.cat->traffic.kerb_bay_pitch_m;
	const double from = end_clear(net, g.a, g.retreat_a), to = g.len - end_clear(net, g.b, g.retreat_b);
	for (const StripEdge &e : p.edges) {
		if (e.strip.kind != StripKind::PARKING) continue;
		const int side = e.u0 + e.u1 > 0 ? LEFT : RIGHT;
		const float kerb = side == LEFT ? e.u1 : e.u0, inner = side == LEFT ? e.u0 : e.u1;
		std::vector<ArcSpan> free{ { from, to } }, parked;
		subtract(free, gates[side]);
		for (const ArcSpan &s : free) {
			const int count = int(std::floor((s.second - s.first) / pitch + EPS_BAY));
			if (count < 1) continue;
			const double first = s.first + (s.second - s.first - double(count) * pitch) / 2;
			for (int k = 0; k < count; k++) out.bays.push_back({ side, first + double(k) * pitch, first + double(k + 1) * pitch, kerb, inner });
			out.runs.push_back({ side, first, first + double(count) * pitch, kerb, inner });
			parked.emplace_back(first, first + double(count) * pitch);
		}
		std::vector<ArcSpan> rest{ { g.retreat_a, g.len - g.retreat_b } };
		subtract(rest, parked);
		for (const ArcSpan &s : rest) out.bans.push_back({ side, s.first, s.second, kerb, inner });
	}
}

} // namespace city
