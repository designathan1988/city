// The operations on a line of stations (grade_line.h). The sweeps edit the line in place; the means read running sums
// kept in double, so a mean costs the same whatever its reach and the order of the additions is one.
#include "sim/roads/grade_line.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LEAST_STEP_M = 1e-3f; // a step under this is a road with no length: nothing to sweep by
constexpr float HALF = 0.5f, QUARTER = 0.25f;
constexpr float EASE_A = 3.f, EASE_B = 2.f; // the cubic 3 t^2 - 2 t^3

// The line's running sums: sums[i] = v[0] + ... + v[i - 1].
std::vector<double> &sums_of(const std::vector<float> &v) {
	thread_local std::vector<double> sums;
	sums.resize(v.size() + 1);
	sums[0] = 0.0;
	for (size_t i = 0; i < v.size(); i++) sums[i + 1] = sums[i] + double(v[i]);
	return sums;
}

int stations_in(float reach, float step) { return std::max(1, int(std::lround(reach / std::max(LEAST_STEP_M, step)))); }
} // namespace

void grade_lowered(std::vector<float> &z, float step, float grade) {
	const float most = grade * std::max(LEAST_STEP_M, step);
	const int n = int(z.size());
	for (int i = 1; i < n; i++) z[size_t(i)] = std::min(z[size_t(i)], z[size_t(i - 1)] + most);
	for (int i = n - 2; i >= 0; i--) z[size_t(i)] = std::min(z[size_t(i)], z[size_t(i + 1)] + most);
}

void grade_raised(std::vector<float> &z, float step, float grade) {
	const float most = grade * std::max(LEAST_STEP_M, step);
	const int n = int(z.size());
	for (int i = 1; i < n; i++) z[size_t(i)] = std::max(z[size_t(i)], z[size_t(i - 1)] - most);
	for (int i = n - 2; i >= 0; i--) z[size_t(i)] = std::max(z[size_t(i)], z[size_t(i + 1)] - most);
}

void within_grade(std::vector<float> &z, float step, float grade) {
	thread_local std::vector<float> over;
	over = z;
	grade_raised(over, step, grade);
	grade_lowered(z, step, grade);
	for (size_t i = 0; i < z.size(); i++) z[i] = (z[i] + over[i]) * HALF;
}

void mean_over(std::vector<float> &v, float step, float reach) {
	const int n = int(v.size());
	if (n < 3) return;
	const int r = stations_in(reach, step);
	const std::vector<double> &sums = sums_of(v);
	const double first = double(v.front()), last = double(v.back());
	for (int i = 0; i < n; i++) {
		const int lo = i - r, hi = i + r; // the window; what of it lies past an end counts as that end
		const int from = std::max(0, lo), to = std::min(n - 1, hi);
		const double sum = sums[size_t(to + 1)] - sums[size_t(from)] + first * double(from - lo) + last * double(hi - to);
		v[size_t(i)] = float(sum / double(2 * r + 1));
	}
}

void rounded_between(std::vector<float> &v, float step, float reach, int first, int last, bool only_up) {
	if (last - first < 2) return;
	const int most = stations_in(reach, step);
	const std::vector<double> &sums = sums_of(v);
	for (int i = first + 1; i < last; i++) {
		const int r = std::min({ most, i - first, last - i });
		const float mean = float((sums[size_t(i + r + 1)] - sums[size_t(i - r)]) / double(2 * r + 1));
		v[size_t(i)] = only_up ? std::max(v[size_t(i)], mean) : mean;
	}
}

float line_at(const std::vector<float> &v, float at) {
	const float f = std::min(float(v.size() - 1), std::max(0.f, at));
	const size_t lo = size_t(f), hi = std::min(v.size() - 1, lo + 1);
	return v[lo] + (v[hi] - v[lo]) * (f - float(lo));
}

float eased(float t) {
	const float u = std::min(1.f, std::max(0.f, t));
	return u * u * (EASE_A - EASE_B * u);
}

// Where the two differ by `band` or more the lower stands; nearer, the vertical curve between them: their mean less a
// parabola of their difference, (d^2 + band^2) / (4 band), which meets the lower one level at d = +-band.
float lower_rounded(float a, float b, float band) {
	const float d = a - b;
	if (std::fabs(d) >= band) return std::min(a, b);
	return (a + b) * HALF - (d * d + band * band) * QUARTER / band;
}

} // namespace city
