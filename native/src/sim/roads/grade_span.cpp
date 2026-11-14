// One span of a road solved between the heights its two nodes stand at (grade_parts.h), on the operations of
// grade_line.h. At grade: level over each plate, the design line's own grade taken up past it over a curve's length, what
// stands between road and line there eased out along a tie, held within the span's grade (exact), its changes of grade
// rounded. Raised: the least deck over its chord and over the ground's top with the room a deck needs, met from each
// land end by a ramp (a sag, a straight, a rounded crest), never under the ground. Sunken: an eased ramp down from each
// portal to the deepest floor the two ramps fit in the run, no deeper than the bore's depth.
#include "sim/roads/grade_parts.h"

#include "sim/roads/grade_line.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LEAST_RUN_M = 1e-3f;
constexpr float LEAST_TIE_M = 0.4f; // a tie is never shorter
constexpr float TIE_PER_GRADE = 3.f; // a tie is long enough to keep its own slope under half the grade (1.5 x amount / tie)
constexpr float CURVE_SHARE = 1.f / 3.f; // of a short span's free run a plate's curve takes at most
constexpr float EASED_STEEPEST = 1.5f; // the eased cubic's steepest slope against its mean
constexpr float HALF = 0.5f;

// The stations of a profile and its two plates, by index.
struct Stations {
	int count, last_a, first_b;
	explicit Stations(const GradeProfile &p) : count(int(p.h.size())), last_a(plate_last_a(p)), first_b(plate_first_b(p)) {}
};

// By station, what the design line's grade, not yet taken up past a plate, leaves between the road and the line: from the
// plate's edge `from` towards `to`, each station's rise counted by the share of the curve still ahead of it.
void grade_left(const std::vector<float> &line, int from, int to, float step, float curve, std::vector<float> &left) {
	left.assign(line.size(), 0.f);
	const int way = to > from ? 1 : -1;
	float sum = 0;
	for (int i = from; i != to; i += way) {
		const float mid = (float(std::abs(i - from)) + HALF) * step;
		sum -= (line[size_t(i + way)] - line[size_t(i)]) * (1.f - eased(mid / curve));
		left[size_t(i + way)] = sum;
	}
}

// Both plates level at the highest the span reached over each, never under its node.
void plates_at_highest(GradeProfile &p, const Stations &st, float ha, float hb) {
	float top_a = ha, top_b = hb;
	for (int i = 0; i <= st.last_a; i++) top_a = std::max(top_a, p.h[size_t(i)]);
	for (int i = st.first_b; i < st.count; i++) top_b = std::max(top_b, p.h[size_t(i)]);
	level_plates(p, top_a, top_b);
}

// How long a ramp is that gains `rise`.
float ramp_run(float rise, const GradeTune &t) { return rise <= 0 ? 0.f : rise / t.ramp + t.ramp_sag * HALF; }
} // namespace

int plate_last_a(const GradeProfile &p) {
	const int last = int(p.h.size()) - 1;
	int i = std::max(0, std::min(last, int(p.plate_a / p.step)));
	while (i < last && p.step * float(i + 1) <= p.plate_a) i++;
	while (i > 0 && p.step * float(i) > p.plate_a) i--;
	return i;
}

int plate_first_b(const GradeProfile &p) {
	const int last = int(p.h.size()) - 1;
	const float edge = p.len - p.plate_b;
	int i = std::max(0, std::min(last, int(std::ceil(edge / p.step))));
	while (i > 0 && p.step * float(i - 1) >= edge) i--;
	while (i < last && p.step * float(i) < edge) i++;
	return i;
}

int station_near(const GradeProfile &p, float s) {
	return std::max(0, std::min(int(p.h.size()) - 1, int(std::lround(s / p.step))));
}

float ramp_gain(float d, const GradeTune &t) {
	if (d <= 0) return 0.f;
	if (d < t.ramp_sag) return t.ramp * d * d / (2.f * t.ramp_sag);
	return t.ramp * (d - t.ramp_sag * HALF);
}

void level_plates(GradeProfile &p, float ha, float hb) {
	const Stations st(p);
	for (int i = 0; i <= st.last_a; i++) p.h[size_t(i)] = ha;
	for (int i = st.first_b; i < st.count; i++) p.h[size_t(i)] = hb;
}

void span_at_grade(GradeProfile &p, const GroundLines &g, float ha, float hb, float grade, const GradeTune &t) {
	const Stations st(p);
	const std::vector<float> &line = g.line;
	const float free = std::max(p.step, float(st.first_b - st.last_a) * p.step);
	const float curve = std::min(t.plate_curve, free * CURVE_SHARE);
	thread_local std::vector<float> left_a, left_b;
	left_a.assign(line.size(), 0.f);
	left_b.assign(line.size(), 0.f);
	if (p.plate_a > 0) grade_left(line, st.last_a, st.first_b, p.step, curve, left_a);
	if (p.plate_b > 0) grade_left(line, st.first_b, st.last_a, p.step, curve, left_b);
	// what stands between road and line at each plate: the node's height against the line, and the grade not taken up
	const float off_a = ha - line[size_t(st.last_a)], off_b = hb - line[size_t(st.first_b)];
	const auto tie = [&](float amount) {
		if (p.manual) return free; // an authored height is carried across the whole span
		return std::min(std::max(LEAST_TIE_M, p.len * t.tie_share), std::max(t.tie, TIE_PER_GRADE * std::fabs(amount) / t.ground));
	};
	const float tie_a = tie(off_a + left_a[size_t(st.first_b)]), tie_b = tie(off_b + left_b[size_t(st.last_a)]);
	const float most = std::max(grade, std::fabs(hb - ha) / free * t.ends_margin); // never under what its two ends need
	for (int i = st.last_a + 1; i < st.first_b; i++) {
		const float from_a = float(i - st.last_a) * p.step, from_b = float(st.first_b - i) * p.step;
		const float want = line[size_t(i)] + (off_a + left_a[size_t(i)]) * (1.f - eased(from_a / tie_a)) +
				(off_b + left_b[size_t(i)]) * (1.f - eased(from_b / tie_b));
		// within the cones of the span's grade from its two plates: what the exact envelopes then leave the plates at
		const float low = std::max(ha - most * from_a, hb - most * from_b), high = std::min(ha + most * from_a, hb + most * from_b);
		p.h[size_t(i)] = std::min(high, std::max(low, want));
	}
	level_plates(p, ha, hb);
	within_grade(p.h, p.step, most);
	rounded_between(p.h, p.step, t.curve, st.last_a, st.first_b, false);
	level_plates(p, ha, hb);
}

void span_raised(GradeProfile &p, const GroundLines &g, float ha, float hb, const RaisedEnds &e, const GradeTune &t) {
	const Stations st(p);
	const StructureSpec &spec = t.of(p.structure);
	const float edge_b = p.len - p.plate_b, run = std::max(LEAST_RUN_M, edge_b - p.plate_a);
	const bool landing = e.cap_a || e.cap_b; // a capped end: the deck takes the ramp's grade all along
	// the deck: the chord between what its two ends stand at, and the ground's top with the room a deck needs under it
	const float deck_a = e.land_a ? ha + spec.clearance : ha, deck_b = e.land_b ? hb + spec.clearance : hb;
	const float room = spec.deck + FOOTWAY_RISE_M + t.under_deck;
	for (int i = 0; i < st.count; i++) {
		const float along = std::min(1.f, std::max(0.f, (p.step * float(i) - p.plate_a) / run));
		p.h[size_t(i)] = std::max(deck_a + (deck_b - deck_a) * along, g.top[size_t(i)] + room);
	}
	grade_raised(p.h, p.step, landing ? t.ramp : t.deck);
	if (!e.land_a || !e.land_b) { // an end aloft meets the deck of its node: carried up to it
		for (int i = 0; !e.land_a && i <= st.last_a; i++) p.h[size_t(i)] = std::max(p.h[size_t(i)], ha);
		for (int i = st.first_b; !e.land_b && i < st.count; i++) p.h[size_t(i)] = std::max(p.h[size_t(i)], hb);
		grade_raised(p.h, p.step, landing ? t.ramp : t.deck_tie);
	}
	// from a land end the road is never over what a ramp has climbed; ramp and deck meet in a crest's curve
	const float band = t.crest * t.ramp * HALF;
	const float fade_a = std::max(t.floor_fade, (e.land_a ? ramp_run(deck_a - ha, t) : 0.f) * t.floor_ramp_share);
	const float fade_b = std::max(t.floor_fade, (e.land_b ? ramp_run(deck_b - hb, t) : 0.f) * t.floor_ramp_share);
	for (int i = 0; i < st.count; i++) {
		const float past_a = p.step * float(i) - p.plate_a, past_b = edge_b - p.step * float(i);
		float h = p.h[size_t(i)];
		if (e.land_a) h = lower_rounded(h, ha + ramp_gain(past_a, t), band);
		if (e.land_b) h = lower_rounded(h, hb + ramp_gain(past_b, t), band);
		// the ground is its floor, let go towards each junction (there the road comes down to the node's level)
		const float near_a = 1.f - eased(past_a / fade_a), near_b = 1.f - eased(past_b / fade_b);
		const float near = std::max(near_a, near_b), level = near_a >= near_b ? ha : hb, top = g.top[size_t(i)];
		p.h[size_t(i)] = std::max(h, top + (std::min(top, level) - top) * near);
	}
	grade_raised(p.h, p.step, t.ramp);
	rounded_between(p.h, p.step, t.curve, st.last_a, st.first_b, true);
	for (int i = 0; landing && i < st.count; i++) { // a capped end holds its ramp's line
		const float s = p.step * float(i);
		if (e.cap_a) p.h[size_t(i)] = std::min(p.h[size_t(i)], ha + ramp_gain(s - p.plate_a, t));
		if (e.cap_b) p.h[size_t(i)] = std::min(p.h[size_t(i)], hb + ramp_gain(edge_b - s, t));
	}
	plates_at_highest(p, st, ha, hb);
}

void span_sunken(GradeProfile &p, float ha, float hb, float depth, const GradeTune &t) {
	const Stations st(p);
	const float run = std::max(LEAST_RUN_M, p.len - p.plate_a - p.plate_b);
	// a ramp is an eased drop, level at both its ends, its steepest the tunnel's grade: a drop D takes D / mean of run
	const float mean = t.tunnel / EASED_STEEPEST;
	// the floor: the bore's depth, or the deepest the two ramps fit in the run (their two drops are mean x run)
	const float floor = std::min({ ha, hb, std::max(depth, (ha + hb - mean * run) * HALF) });
	const float ramp_a = (ha - floor) / mean, ramp_b = (hb - floor) / mean;
	for (int i = st.last_a + 1; i < st.first_b; i++) {
		const float past_a = p.step * float(i) - p.plate_a, past_b = run - past_a;
		const float from_a = ramp_a > 0 ? ha + (floor - ha) * eased(past_a / ramp_a) : floor;
		const float from_b = ramp_b > 0 ? hb + (floor - hb) * eased(past_b / ramp_b) : floor;
		p.h[size_t(i)] = std::max(from_a, from_b);
	}
	level_plates(p, ha, hb);
	grade_lowered(p.h, p.step, t.tunnel);
	plates_at_highest(p, st, ha, hb);
}

} // namespace city
