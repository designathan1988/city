// The Diagnosis's rules (diagnosis.h): verdictOf, severityOf and the stages of the 4.0's doctor/findings.ts; the arithmetic of quickAb
// (doctor/measure.ts), with what the 4.0 leaves to the reader, whether the change moved more than the noise, said here.
#include "api/lab/diagnosis.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city::lab {

namespace {

// A GPU time this small is no time to share a stage of (findings.ts `Math.max(gpuMs, 1e-6)`).
constexpr double LEAST_GPU_MS = 1e-6;

double mean_of(const std::vector<double> &values) {
	double sum = 0;
	for (double v : values) sum += v;
	return values.empty() ? 0 : sum / static_cast<double>(values.size());
}

double tail_of(std::vector<double> values, double share) {
	if (values.empty()) return 0;
	std::sort(values.begin(), values.end());
	const std::size_t rank = static_cast<std::size_t>(std::ceil(static_cast<double>(values.size()) * share));
	return values[std::min(values.size(), std::max<std::size_t>(rank, 1)) - 1];
}

// measure.ts `mean`: one window with no timer leaves the other's reading.
std::optional<double> mean_of_two(std::optional<double> a, std::optional<double> b) {
	if (a && b) return (*a + *b) * 0.5;
	return a ? a : b;
}

} // namespace

double median(std::vector<double> values) {
	if (values.empty()) return std::numeric_limits<double>::quiet_NaN();
	std::sort(values.begin(), values.end());
	const std::size_t middle = values.size() / 2;
	return values.size() % 2 ? values[middle] : (values[middle - 1] + values[middle]) * 0.5;
}

Bound bound_of(const Times &t, const Rules &r) {
	if (!std::isfinite(t.cpu_ms) && !t.gpu_ms) return Bound::None;
	const double limit = r.budget_ms * r.bound_share;
	if (t.gpu_ms && *t.gpu_ms >= t.cpu_ms && *t.gpu_ms > limit) return Bound::Gpu;
	if (t.cpu_ms > t.gpu_ms.value_or(0) && t.cpu_ms > limit) return Bound::Cpu;
	return Bound::Headroom;
}

Severity severity_of(double ms, const Rules &r) {
	if (ms >= r.high_ms) return Severity::High;
	if (ms >= r.medium_ms) return Severity::Medium;
	return ms >= r.low_ms ? Severity::Low : Severity::Info;
}

std::vector<Finding> findings_of(const std::vector<Stage> &stages, std::optional<double> gpu_ms, const Rules &r) {
	std::vector<Finding> out;
	for (const Stage &s : stages) {
		const double ms = mean_of(s.samples);
		if (ms < r.stage_min_ms) continue;
		out.push_back({ s.id, ms, tail_of(s.samples, r.stage_tail), ms / std::max(gpu_ms.value_or(0), LEAST_GPU_MS), severity_of(ms, r) });
	}
	std::stable_sort(out.begin(), out.end(), [](const Finding &a, const Finding &b) { return a.ms > b.ms; });
	return out;
}

std::size_t hitches_in(const std::vector<double> &intervals, const Rules &r) {
	return static_cast<std::size_t>(std::count_if(intervals.begin(), intervals.end(), [&r](double ms) { return ms > r.stutter_ms; }));
}

Comparison compare(const Side &before, const Side &after, const Side &again) {
	Comparison c;
	c.gpu_before = mean_of_two(before.gpu_ms, again.gpu_ms);
	c.gpu_after = after.gpu_ms;
	c.cpu_before = (before.cpu_ms + again.cpu_ms) * 0.5;
	c.cpu_after = after.cpu_ms;
	c.draws_before = (before.draws + again.draws) * 0.5;
	c.draws_after = after.draws;
	c.primitives_before = (before.primitives + again.primitives) * 0.5;
	c.primitives_after = after.primitives;
	c.noise_gpu = before.gpu_ms && again.gpu_ms ? std::abs(*before.gpu_ms - *again.gpu_ms) : 0;
	c.noise_cpu = std::abs(before.cpu_ms - again.cpu_ms);
	const bool gpu_moved = c.gpu_before && c.gpu_after && std::abs(*c.gpu_after - *c.gpu_before) > c.noise_gpu;
	c.real = gpu_moved || std::abs(c.cpu_after - c.cpu_before) > c.noise_cpu;
	return c;
}

} // namespace city::lab
