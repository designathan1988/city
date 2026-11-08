// One road's profile solved between the heights its two nodes settled on (Roadcraft 4.0 world/elevation.ts solveGround,
// solveVariable, solveRaised, solveSunken and the shaping of a grade line: moving means, grade limits, envelopes, vertical
// curves). The behaviour of the 4.0 on vectors of float edited in place; the copies a pass reads from are one scratch
// buffer kept between calls. The vertical curves' means are running sums (Means); smooth_profile sums each station's
// window afresh, O(n x radius) with radius the stations within SMOOTH_REACH_M: linear in the stations for a given step,
// and each sum keeps its order of addition (a running sum would move the last bits of the heights).
#include "sim/roads/road_grade_internal.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float RAMP_CURVE_M = 8.f; // RAMP_CURVE: the sag curve at the foot of a ramp
constexpr float VERTICAL_CURVE_M = 4.8f; // VERTICAL_CURVE_REACH: half the length of the parabola at a change of grade
constexpr float PLATE_TANGENT_M = 16.f; // PLATE_TANGENT_REACH: decay of the grade correction past a flat plate
constexpr float PLATE_TANGENT_SHARE = 0.55f; // PLATE_TANGENT_SHARE: of the incoming grade cancelled at the plate
constexpr float CREST_ROUND_RUN = 3.2f; // RAMP_CREST: the crest's rounding, as run at the ramp's grade
constexpr float CURVE_PEAK = 1.5f; // CURVE_PEAK: peak-to-mean slope of the smoothstep of a vertical curve
constexpr float TIE_REACH_M = 28.f; // TIE_REACH: how far past its plate a line is tied back to its node
constexpr float TIE_MIN_M = 0.4f; // solveGround tieFor: the shortest tie, and its share of the road
constexpr float TIE_ROAD_SHARE = 0.4f; // solveGround tieFor: a tie's share of the road
constexpr float PLATE_FADE_OUT = 16.f; // PLATE_BLEND: past a plate the ground stops being a ramp's floor
constexpr float RAMP_RELAX = 0.8f; // RAMP_RELAX: the share of a ramp over which the ground stops being a floor
constexpr float STEEP_SHARE = 0.75f; // roundPlateJoins: a join is rounded where the limiter held it this near the grade
constexpr float GRADE_SLACK = 1.1f; // the grade a span is given over what its two ends need
constexpr int SLOPE_PASSES = 6, ENVELOPE_PASSES = 4, PIN_ROUNDS = 3; // solveGround: the limiter's, the envelope's and the pins' passes
constexpr float TINY = 1e-6f, TINY_RUN = 1e-3f; // the least a share and a step divide by

std::vector<float> &scratch() {
	thread_local std::vector<float> buf;
	return buf;
}

float smoothstep(float a, float b, float v) {
	const float u = std::min(1.f, std::max(0.f, (v - a) / std::max(TINY, b - a)));
	return u * u * (3.f - 2.f * u);
}

float ease(float t) {
	const float u = std::min(1.f, std::max(0.f, t));
	return u * u * (3.f - 2.f * u);
}

// The smaller of two heights, the corner between them rounded over a band k high (a quadratic smooth minimum).
float smooth_min(float a, float b, float k) {
	const float h = std::max(k - std::fabs(a - b), 0.f) / k;
	return std::min(a, b) - h * h * k * 0.25f;
}

int radius_of(float reach, float step) { return std::max(1, int(std::lround(reach / std::max(TINY_RUN, step)))); }

// The mean of src over [i - r, i + r] (indices clamped when `clamp`), from running sums of src.
struct Means {
	std::vector<double> sum;
	explicit Means(const std::vector<float> &src) : sum(src.size() + 1, 0.0) {
		for (size_t i = 0; i < src.size(); i++) sum[i + 1] = sum[i] + src[i];
	}
	float inside(int i, int r) const { return float((sum[size_t(i + r + 1)] - sum[size_t(i - r)]) / double(2 * r + 1)); }
};

// Raises (up) or lowers samples until no step between neighbours is more than grade (gradeEnvelope, descentEnvelope).
void envelope(std::vector<float> &h, float step, float grade, bool up) {
	const float limit = grade * step;
	const int n = int(h.size());
	for (int pass = 0; pass < ENVELOPE_PASSES; pass++) {
		for (int i = 1; i < n; i++) h[size_t(i)] = up ? std::max(h[size_t(i)], h[size_t(i - 1)] - limit) : std::min(h[size_t(i)], h[size_t(i - 1)] + limit);
		for (int i = n - 2; i >= 0; i--) h[size_t(i)] = up ? std::max(h[size_t(i)], h[size_t(i + 1)] - limit) : std::min(h[size_t(i)], h[size_t(i + 1)] + limit);
	}
}

// The plates at their heights, and what `between` says for the stations between them (its index and its distance along).
template <typename F>
void between_plates(GradeProfile &p, float ha, float hb, F &&between) {
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		p.h[i] = s <= p.plate_a ? ha : s >= p.len - p.plate_b ? hb : between(i, s);
	}
}

// What a rounding of the grade starts from: its reach in samples, a copy of the heights and where the plates end.
struct Rounding {
	int radius, last;
	std::vector<float> &src;
	Means means;
	float edge_a, edge_b;
	explicit Rounding(const GradeProfile &p) : radius(radius_of(VERTICAL_CURVE_M, p.step)), last(int(p.h.size()) - 1), src(scratch() = p.h), means(src),
			edge_a(p.plate_a / p.step), edge_b((p.len - p.plate_b) / p.step) {}
};

// Vertical curves wherever the grade changes, off the plates (into_plates: the window may reach into them); `raise`: only up.
void round_grade_breaks(GradeProfile &p, bool raise, bool into_plates) {
	const Rounding g(p);
	const int radius = g.radius, last = g.last;
	const std::vector<float> &src = g.src;
	const float edge_a = g.edge_a, edge_b = g.edge_b;
	for (int i = 0; i <= last; i++) {
		if (float(i) <= edge_a || float(i) >= edge_b) continue;
		const int r = into_plates ? std::min({ radius, i, last - i })
								  : std::min({ radius, int(std::floor(float(i) - edge_a)), int(std::floor(edge_b - float(i))) });
		if (r < 1) continue;
		const float mean = g.means.inside(i, r);
		p.h[size_t(i)] = raise ? std::max(src[size_t(i)], mean) : mean;
	}
}

// The join of a level plate and the grade past it rounded, only where the limiter held the grade right at the edge.
void round_plate_joins(GradeProfile &p, const GradeTune &t) {
	const Rounding g(p);
	const int radius = g.radius, last = g.last;
	const std::vector<float> &src = g.src;
	const float edge_a = g.edge_a, edge_b = g.edge_b;
	auto steep = [&](float from, int dir) {
		const int i0 = int(std::lround(from)), i1 = i0 + dir * radius;
		if (i1 < 0 || i1 > last) return false;
		return std::fabs(src[size_t(i1)] - src[size_t(i0)]) / (float(radius) * p.step) >= STEEP_SHARE * t.ground;
	};
	const bool near_a = steep(std::ceil(edge_a), 1), near_b = steep(std::floor(edge_b), -1);
	for (int i = 0; i <= last; i++) {
		if (float(i) <= edge_a || float(i) >= edge_b) continue;
		if (!((near_a && float(i) - edge_a <= float(radius)) || (near_b && edge_b - float(i) <= float(radius)))) continue;
		const int r = std::min({ radius, i, last - i });
		if (r >= 1) p.h[size_t(i)] = g.means.inside(i, r);
	}
}

// Both plates flat at the highest the profile reached over each (flattenPlates).
void flatten_plates(GradeProfile &p, float ha, float hb) {
	float top_a = ha, top_b = hb;
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (s <= p.plate_a) top_a = std::max(top_a, p.h[i]);
		if (s >= p.len - p.plate_b) top_b = std::max(top_b, p.h[i]);
	}
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (s <= p.plate_a) p.h[i] = top_a;
		else if (s >= p.len - p.plate_b) p.h[i] = top_b;
	}
}

float ramp_length(float rise, const GradeTune &t) { return rise <= 0 ? 0.f : rise / t.ramp + RAMP_CURVE_M; }

// The base read between stations, linear (solveGround baseAt).
float base_at(const GradeProfile &p, float s) {
	const float at = std::min(float(p.base.size() - 1), std::max(0.f, s / p.step));
	const size_t lo = size_t(std::floor(at)), hi = std::min(p.base.size() - 1, lo + 1);
	return p.base[lo] + (p.base[hi] - p.base[lo]) * (at - float(lo));
}
} // namespace

int station_index(const GradeProfile &p, float s) {
	const int at = int(std::lround(s / p.step));
	return std::max(0, std::min(int(p.h.size()) - 1, at));
}

float ramp_rise(float d, const GradeTune &t) {
	if (d <= 0) return 0.f;
	if (d < RAMP_CURVE_M) return t.ramp * d * d / (2.f * RAMP_CURVE_M);
	return t.ramp * (d - RAMP_CURVE_M * 0.5f);
}

void smooth_profile(std::vector<float> &v, float step, float reach) {
	if (v.size() < 3) return;
	const int radius = radius_of(reach, step), last = int(v.size()) - 1;
	std::vector<float> &src = scratch();
	src = v;
	for (int i = 0; i <= last; i++) {
		double sum = 0;
		for (int k = -radius; k <= radius; k++) sum += src[size_t(std::max(0, std::min(last, i + k)))];
		v[size_t(i)] = float(sum / double(2 * radius + 1));
	}
}

void slope_limit(std::vector<float> &h, float step, float grade) {
	const float limit = grade * std::max(TINY_RUN, step);
	const int n = int(h.size());
	for (int pass = 0; pass < SLOPE_PASSES; pass++) {
		for (int i = 1; i < n; i++) h[size_t(i)] = std::min(h[size_t(i - 1)] + limit, std::max(h[size_t(i - 1)] - limit, h[size_t(i)]));
		for (int i = n - 2; i >= 0; i--) h[size_t(i)] = std::min(h[size_t(i + 1)] + limit, std::max(h[size_t(i + 1)] - limit, h[size_t(i)]));
	}
}

void dilate(std::vector<float> &v, int reach) {
	std::vector<float> &src = scratch();
	src = v;
	const int n = int(v.size());
	for (int i = 0; i < n; i++) {
		float high = src[size_t(i)];
		for (int k = std::max(0, i - reach); k <= std::min(n - 1, i + reach); k++) high = std::max(high, src[size_t(k)]);
		v[size_t(i)] = high;
	}
}

void pin_plates(GradeProfile &p, float ha, float hb) {
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (s <= p.plate_a) p.h[i] = ha;
		else if (s >= p.len - p.plate_b) p.h[i] = hb;
	}
}

void solve_ground(GradeProfile &p, float ha, float hb, const GradeTune &t) {
	const float shift_a = ha - p.base[size_t(station_index(p, p.plate_a))];
	const float shift_b = hb - p.base[size_t(station_index(p, p.len - p.plate_b))];
	// no plate at a cut of the road: the line runs through it, with nothing flat to take the slope off against
	const float slope_a = p.plate_a > 0.f ? (base_at(p, p.plate_a + p.step) - base_at(p, p.plate_a - p.step)) / (2.f * p.step) : 0.f;
	const float slope_b = p.plate_b > 0.f ? (base_at(p, p.len - p.plate_b + p.step) - base_at(p, p.len - p.plate_b - p.step)) / (2.f * p.step) : 0.f;
	// The transition is sized from the correction it carries.
	auto tie_for = [&](float shift) {
		return std::min(std::max(TIE_MIN_M, p.len * TIE_ROAD_SHARE), std::max(TIE_REACH_M, std::fabs(shift) * CURVE_PEAK / (t.ground * 0.5f)));
	};
	const float tie_a = tie_for(shift_a), tie_b = tie_for(shift_b);
	between_plates(p, ha, hb, [&](size_t i, float s) {
		const float wa = 1.f - smoothstep(p.plate_a, p.plate_a + tie_a, s), wb = 1.f - smoothstep(p.plate_b, p.plate_b + tie_b, p.len - s);
		return p.base[i] + shift_a * wa + shift_b * wb;
	});
	const float free = std::max(p.step, p.len - p.plate_a - p.plate_b);
	const float grade = std::max(t.ground, std::fabs(hb - ha) / free * GRADE_SLACK);
	for (int round = 0; round < PIN_ROUNDS; round++) {
		pin_plates(p, ha, hb);
		slope_limit(p.h, p.step, grade);
	}
	pin_plates(p, ha, hb);
	round_grade_breaks(p, false, false);
	// The tie has zero derivative at its end: the designed line's own slope survives against the flat plate, unless taken off.
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (s <= p.plate_a || s >= p.len - p.plate_b) continue;
		const float from_a = s - p.plate_a, from_b = p.len - p.plate_b - s;
		p.h[i] += -PLATE_TANGENT_SHARE * slope_a * from_a * std::exp(-from_a / PLATE_TANGENT_M) +
				PLATE_TANGENT_SHARE * slope_b * from_b * std::exp(-from_b / PLATE_TANGENT_M);
	}
	round_plate_joins(p, t);
}

void solve_variable(GradeProfile &p, float ha, float hb, const GradeTune &t) {
	const float edge_a = p.plate_a, edge_b = p.len - p.plate_b;
	const float lift_a = ha - p.base[size_t(station_index(p, edge_a))], lift_b = hb - p.base[size_t(station_index(p, edge_b))];
	const float run = std::max(TINY, edge_b - edge_a);
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (s <= edge_a) p.h[i] = ha;
		else if (s >= edge_b) p.h[i] = hb;
		else {
			const float e = ease((s - edge_a) / run);
			p.h[i] = p.base[i] + lift_a * (1.f - e) + lift_b * e;
		}
	}
	const float grade = std::max(t.ramp, std::fabs(hb - ha) / run * GRADE_SLACK);
	for (int round = 0; round < PIN_ROUNDS; round++) {
		slope_limit(p.h, p.step, grade);
		pin_plates(p, ha, hb);
	}
	round_grade_breaks(p, false, false);
	pin_plates(p, ha, hb);
}

void solve_raised(GradeProfile &p, float ha, float hb, const RaisedEnds &e, const GradeTune &t) {
	const float clearance = t.of(p.structure).clearance, crest = CREST_ROUND_RUN * t.ramp;
	const bool near_landing = e.cap_a || e.cap_b;
	const float edge_b = p.len - p.plate_b, run = std::max(TINY_RUN, edge_b - p.plate_a);
	// The straight deck from end to end, nothing under it closer than its structure.
	const float start_h = e.land_a ? ha + clearance + crest * 0.25f : ha, end_h = e.land_b ? hb + clearance + crest * 0.25f : hb;
	const float underside = t.of(p.structure).deck + FOOTWAY_RISE_M + UNDER_DECK_ROOM_M;
	for (size_t i = 0; i < p.h.size(); i++) {
		const float u = std::min(1.f, std::max(0.f, (p.step * float(i) - p.plate_a) / run));
		p.h[i] = std::max(start_h + (end_h - start_h) * u, p.ceil[i] + underside);
	}
	envelope(p.h, p.step, near_landing ? t.ramp : t.deck, true);
	if (!e.land_a || !e.land_b) {
		for (size_t i = 0; i < p.h.size(); i++) {
			const float s = p.step * float(i);
			if (!e.land_a && s <= p.plate_a) p.h[i] = std::max(p.h[i], ha);
			if (!e.land_b && s >= edge_b) p.h[i] = std::max(p.h[i], hb);
		}
		envelope(p.h, p.step, near_landing ? t.ramp : t.deck_tie, true);
	}
	for (size_t i = 0; i < p.h.size(); i++) {
		const float s = p.step * float(i);
		if (e.land_a) p.h[i] = smooth_min(p.h[i], ha + ramp_rise(s - p.plate_a, t), crest);
		if (e.land_b) p.h[i] = smooth_min(p.h[i], hb + ramp_rise(edge_b - s, t), crest);
	}
	const float ramp_a = e.land_a ? ramp_length(std::max(0.f, start_h - ha), t) : 0.f;
	const float ramp_b = e.land_b ? ramp_length(std::max(0.f, end_h - hb), t) : 0.f;
	const float reach_a = std::max(PLATE_FADE_OUT, ramp_a * RAMP_RELAX), reach_b = std::max(PLATE_FADE_OUT, ramp_b * RAMP_RELAX);
	for (size_t i = 0; i < p.h.size(); i++) {
		// The ground floor relaxed towards each junction.
		const float s = p.step * float(i);
		const float near_a = 1.f - smoothstep(p.plate_a, p.plate_a + reach_a, s), near_b = 1.f - smoothstep(p.plate_b, p.plate_b + reach_b, p.len - s);
		const float near = std::max(near_a, near_b), level = near_a >= near_b ? ha : hb;
		const float floor = near <= 0 ? p.ceil[i] : p.ceil[i] * (1.f - near) + std::min(p.ceil[i], level) * near;
		p.h[i] = std::max(p.h[i], floor);
	}
	envelope(p.h, p.step, t.ramp, true);
	round_grade_breaks(p, true, false);
	if (near_landing) {
		for (size_t i = 0; i < p.h.size(); i++) {
			const float s = p.step * float(i);
			if (e.cap_a) p.h[i] = std::min(p.h[i], ha + ramp_rise(s - p.plate_a, t));
			if (e.cap_b) p.h[i] = std::min(p.h[i], hb + ramp_rise(edge_b - s, t));
		}
	}
	flatten_plates(p, ha, hb);
}

void solve_sunken(GradeProfile &p, float ha, float hb, float wanted, const GradeTune &t) {
	const float run = std::max(TINY_RUN, p.len - p.plate_a - p.plate_b), target = std::min({ wanted, ha, hb });
	const float slope = t.tunnel / CURVE_PEAK;
	const float drop_a = std::max(0.f, ha - target), drop_b = std::max(0.f, hb - target);
	const float need = (drop_a + drop_b) / slope, scale = need > run ? run / need : 1.f;
	const float floor = std::min({ ha, hb, target + (1.f - scale) * std::max(drop_a, drop_b) });
	const float ramp_a = std::max(0.f, (ha - floor) / slope), ramp_b = std::max(0.f, (hb - floor) / slope);
	between_plates(p, ha, hb, [&](size_t, float s) {
		const float along = s - p.plate_a, back = run - along;
		const float down = ramp_a <= 0 ? floor : ha + (floor - ha) * ease(along / ramp_a);
		const float up = ramp_b <= 0 ? floor : hb + (floor - hb) * ease(back / ramp_b);
		return std::max(down, up); // the higher end wins, as the raised solver takes the lower
	});
	envelope(p.h, p.step, t.tunnel, false);
	flatten_plates(p, ha, hb);
}

} // namespace city
