// The profile of a raised road: its deck, ramps and vertical curves (road_grade.h), and what it stands above its chord.
#include "sim/roads/road_grade.h"

#include "sim/catalog/catalog.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float FLAT_LIFT_M = 0.001f; // a lift under this is no lift
}

RaisedRules raised_rules(const Tuning &tn, RoadStructure structure) {
	RaisedRules r;
	r.clearance = structure == RoadStructure::BRIDGE ? tn.road_clearance_bridge_m : tn.road_clearance_elevated_m;
	r.ramp = tn.road_ramp_grade;
	r.deck = tn.road_deck_grade;
	r.curve = tn.road_ramp_curve_m;
	r.step = tn.road_station_m;
	return r;
}

std::vector<float> raised_heights(double len, float ya, float yb, const RaisedRules &rules, const std::function<float(double)> &ground) {
	const int n = std::max(2, int(std::ceil(len / rules.step)) + 1);
	const double step = len / (n - 1);
	std::vector<float> want(static_cast<size_t>(n));
	for (int i = 0; i < n; i++) want[size_t(i)] = ground(i * step) + rules.clearance;
	// the deck clears every rise under it and takes no steeper grade than a deck may
	for (int i = 1; i < n; i++) want[size_t(i)] = std::max(want[size_t(i)], float(want[size_t(i - 1)] - rules.deck * step));
	for (int i = n - 2; i >= 0; i--) want[size_t(i)] = std::max(want[size_t(i)], float(want[size_t(i + 1)] - rules.deck * step));
	// no higher than a ramp from either end can climb
	for (int i = 0; i < n; i++) {
		const double at = i * step;
		want[size_t(i)] = std::min({ want[size_t(i)], float(ya + rules.ramp * at), float(yb + rules.ramp * (len - at)) });
	}
	// the vertical curves: a window that closes at the ends, so they stay where the nodes are
	std::vector<float> out(want.size());
	const int half = std::max(1, int(rules.curve / 2 / step));
	for (int i = 0; i < n; i++) {
		const int w = std::min({ half, i, n - 1 - i });
		float sum = 0;
		for (int k = i - w; k <= i + w; k++) sum += want[size_t(k)];
		out[size_t(i)] = sum / float(2 * w + 1);
	}
	return out;
}

float height_on(const std::vector<float> &heights, double share) {
	if (heights.empty()) return 0;
	const double at = std::clamp(share, 0.0, 1.0) * double(heights.size() - 1);
	const size_t i = std::min(size_t(at), heights.size() - 1), j = std::min(i + 1, heights.size() - 1);
	return heights[i] + (heights[j] - heights[i]) * float(at - double(i));
}

std::vector<float> lift_over(const std::function<float(double)> &height, double from, double to, double len, float step) {
	const int n = std::max(2, int(std::ceil(len / step)) + 1);
	const float ya = height(from), yb = height(to);
	std::vector<float> out(static_cast<size_t>(n));
	bool flat = true;
	for (int i = 0; i < n; i++) {
		const double share = double(i) / (n - 1);
		out[size_t(i)] = height(from + (to - from) * share) - (ya + (yb - ya) * float(share));
		flat = flat && std::fabs(out[size_t(i)]) < FLAT_LIFT_M;
	}
	return flat ? std::vector<float>() : out;
}

} // namespace city
