// See road_spec.h. The limits are ROAD_SECTION_LIMITS of the 4.0, in metres.
#include "sim/roads/road_spec.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float SAME_WIDTH_M = 1e-3f;

void add(std::vector<SpecIssue> &out, const char *problem, const std::vector<int> &elements) {
	for (SpecIssue &i : out)
		if (i.problem == problem) {
			for (int e : elements)
				if (std::find(i.elements.begin(), i.elements.end(), e) == i.elements.end()) i.elements.push_back(e);
			return;
		}
	out.push_back({ problem, elements });
}

bool kerb_band(const SpecElement &e) { return e.kind == SpecKind::PARKING || e.kind == SpecKind::CYCLE; }

void check_lanes(const RoadSpec &s, const std::vector<int> &lanes, std::vector<SpecIssue> &out) {
	const std::vector<SpecElement> &e = s.elements;
	if (lanes.empty()) {
		add(out, "noLanes", {});
		return;
	}
	std::vector<int> off, forward, backward;
	for (int i : lanes) {
		if (std::fabs(e[size_t(i)].width - e[size_t(lanes[0])].width) > SAME_WIDTH_M) off.push_back(i);
		(e[size_t(i)].dir > 0 ? forward : backward).push_back(i);
	}
	if (!off.empty()) add(out, "laneWidths", off);
	if (!forward.empty() && !backward.empty() && forward.size() != backward.size()) add(out, "laneBalance", lanes);
	for (const std::vector<int> *own : { &forward, &backward })
		if (!own->empty() && std::all_of(own->begin(), own->end(), [&](int i) { return e[size_t(i)].bus; })) add(out, "busOnly", *own);
}

// The carriageway in order: backward lanes, then the median, then forward lanes.
void check_order(const RoadSpec &s, const std::vector<int> &inner, bool both_ways, std::vector<SpecIssue> &out) {
	bool seen_forward = false, seen_median = false;
	for (int i : inner) {
		const SpecElement &x = s.elements[size_t(i)];
		if (x.kind == SpecKind::MEDIAN) {
			if (seen_median || seen_forward || !both_ways) add(out, "median", { i });
			seen_median = true;
		} else if (x.kind == SpecKind::LANE) {
			if (x.dir > 0) seen_forward = true;
			else if (seen_forward || seen_median) add(out, "order", { i });
		}
	}
}

void check_widths(const RoadSpec &s, std::vector<SpecIssue> &out) {
	for (size_t i = 0; i < s.elements.size(); i++) {
		const SpecElement &x = s.elements[i];
		const bool lane = x.kind == SpecKind::LANE && (x.width < LANE_LEAST_M || x.width > LANE_MOST_M);
		const bool walk = x.kind == SpecKind::FOOTWAY && (x.width < WALK_LEAST_M || x.width > WALK_MOST_M);
		const bool median = x.kind == SpecKind::MEDIAN && (x.width <= 0 || x.width > MEDIAN_MOST_M);
		if (lane || walk || median) add(out, "width", { int(i) });
	}
}
} // namespace

std::vector<SpecIssue> spec_issues(const RoadSpec &s, RoadClass road_class) {
	std::vector<SpecIssue> out;
	const std::vector<SpecElement> &e = s.elements;
	const int n = int(e.size());
	// A highway or a ramp carries no footway at all (profiles.cpp foot_crossing; its shoulders are the class's); any other road one at each edge.
	const bool walks = road_class != RoadClass::HIGHWAY && road_class != RoadClass::RAMP;
	if (walks && (n == 0 || e[0].kind != SpecKind::FOOTWAY)) add(out, "footways", n > 0 ? std::vector<int>{ 0 } : std::vector<int>{});
	if (walks && (n < 3 || e[size_t(n - 1)].kind != SpecKind::FOOTWAY)) add(out, "footways", n > 1 ? std::vector<int>{ n - 1 } : std::vector<int>{});
	std::vector<int> inner, lanes;
	for (int i = walks ? 1 : 0; i < (walks ? n - 1 : n); i++) {
		inner.push_back(i);
		if (e[size_t(i)].kind == SpecKind::FOOTWAY) add(out, walks ? "footways" : "noFootways", { i });
		if (e[size_t(i)].kind == SpecKind::LANE) lanes.push_back(i);
	}
	check_lanes(s, lanes, out);
	std::vector<int> bands;
	for (size_t k = 0; k < inner.size(); k++) {
		if (!kerb_band(e[size_t(inner[k])])) continue;
		bands.push_back(inner[k]);
		if (k != 0 && k + 1 != inner.size()) add(out, "parkingSide", { inner[k] });
	}
	const bool forward = std::any_of(lanes.begin(), lanes.end(), [&](int i) { return e[size_t(i)].dir > 0; });
	const bool backward = std::any_of(lanes.begin(), lanes.end(), [&](int i) { return e[size_t(i)].dir < 0; });
	check_order(s, inner, forward && backward, out);
	check_widths(s, out);
	if ((road_class == RoadClass::HIGHWAY || road_class == RoadClass::RAMP) && !bands.empty()) add(out, "class", bands);
	return out;
}

RoadSpec spec_of(const Profile &p) {
	RoadSpec s;
	s.speed = p.speed;
	s.priority = p.priority;
	s.centre = p.centre;
	s.surface = int8_t(p.surface);
	const size_t n = p.strips.size();
	for (size_t i = 0; i < n; i++) { // the catalogue's right to left; a kerb counts in what it borders
		const Strip &x = p.strips[i];
		SpecElement e;
		e.width = x.width;
		e.dir = x.dir;
		e.bus = x.bus;
		e.solid = x.solid;
		e.flush = x.flush;
		e.pave = x.pave;
		switch (x.kind) {
			case StripKind::SIDEWALK: e.kind = SpecKind::FOOTWAY; break;
			case StripKind::PARKING: e.kind = SpecKind::PARKING; break;
			case StripKind::BIKE: e.kind = SpecKind::CYCLE; break;
			case StripKind::LANE: e.kind = SpecKind::LANE; break;
			case StripKind::MEDIAN: e.kind = SpecKind::MEDIAN; break;
			default: continue; // a kerb counts below; a shoulder or a barrier belongs to the class, not to what is composed
		}
		if (e.kind == SpecKind::FOOTWAY || e.kind == SpecKind::MEDIAN) {
			if (i > 0 && p.strips[i - 1].kind == StripKind::CURB) e.width += p.strips[i - 1].width;
			if (i + 1 < n && p.strips[i + 1].kind == StripKind::CURB) e.width += p.strips[i + 1].width;
		}
		s.elements.push_back(e);
	}
	std::reverse(s.elements.begin(), s.elements.end());
	return s;
}

std::vector<Strip> spec_strips(const RoadSpec &s, float kerb) {
	std::vector<Strip> out;
	Strip curb;
	curb.kind = StripKind::CURB;
	curb.width = kerb;
	for (size_t i = s.elements.size(); i-- > 0;) {
		const SpecElement &e = s.elements[i];
		Strip x;
		x.width = e.width;
		x.dir = e.dir;
		x.bus = e.bus;
		x.solid = e.solid;
		x.flush = e.flush;
		x.pave = e.pave;
		x.kind = e.kind == SpecKind::FOOTWAY ? StripKind::SIDEWALK : e.kind == SpecKind::PARKING ? StripKind::PARKING
				: e.kind == SpecKind::CYCLE ? StripKind::BIKE : e.kind == SpecKind::MEDIAN ? StripKind::MEDIAN : StripKind::LANE;
		const bool right_edge = i + 1 == s.elements.size();
		if (e.kind == SpecKind::FOOTWAY && !e.flush) {
			x.width -= kerb;
			if (right_edge) {
				out.push_back(x);
				out.push_back(curb);
			} else {
				out.push_back(curb);
				out.push_back(x);
			}
		} else if (e.kind == SpecKind::MEDIAN && !e.flush) {
			x.width -= 2 * kerb;
			out.insert(out.end(), { curb, x, curb });
		} else out.push_back(x);
	}
	return out;
}

std::string spec_centre(const RoadSpec &s) {
	int forward = 0, backward = 0;
	bool median = false;
	for (const SpecElement &e : s.elements) {
		if (e.kind == SpecKind::MEDIAN) median = true;
		if (e.kind == SpecKind::LANE) (e.dir > 0 ? forward : backward)++;
	}
	if (!s.centre.empty() && s.centre != "auto") return s.centre;
	if (median || forward == 0 || backward == 0) return "none";
	return forward + backward == 2 ? "dashed" : "solid";
}

} // namespace city
