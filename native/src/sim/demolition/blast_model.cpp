// Airblast curves and the effects thresholds (see blast_model.h and the sources in data/catalog/blast.json).
#include "sim/demolition/blast_model.h"

#include <algorithm>
#include <cmath>

namespace city::blast {

namespace {
constexpr float CDF_SCALE = 0.70710678f; // 1 / sqrt(2): erfc argument of the normal cdf
constexpr int RANGE_BISECTIONS = 40; // 2^-40 of the search window: far below a millimetre
constexpr float RANGE_LIMIT_M = 20000.f; // no scaled range is searched beyond this
constexpr float NEAREST_RANGE_M = 0.05f; // the least range a blast is read at: the curves start there
constexpr float MIN_KG = 1e-6f; // a charge of nothing still has a scaled distance
constexpr float PA_PER_KPA = 1000.f; // the kilopascal
constexpr float QUANTILE_LIMIT = 8.f; // the standard normal's quantile is searched within +-8
constexpr int QUANTILE_BISECTIONS = 48; // halvings of that window: far below the curves' own precision

// F = exp(A + B ln Z + C ln2 Z + ...) of the row holding Z. Nearer than the first row: its value at its start (DDESB
// TP 20 2.4.3.2). Farther than the last row: a row that is a plain power law in the data (A + B ln Z, the pressure's and
// the impulse's) goes on; a row with higher terms is a polynomial fitted only to its own range (the duration's, to
// Z = 40) and diverges outside it, so it is held at its far end, as the near end is.
bool power_law(const BlastCurve &r) {
	for (int k = 2; k < BLAST_COEFFS; k++)
		if (r.c[k] != 0.f) return false;
	return true;
}

float evaluate(const std::vector<BlastCurve> &rows, float z) {
	if (rows.empty()) return 0.f;
	const BlastCurve *row = &rows.back();
	for (const BlastCurve &r : rows)
		if (z < r.z1) { row = &r; break; }
	z = std::max(z, rows.front().z0);
	if (row == &rows.back() && !power_law(*row)) z = std::min(z, row->z1);
	const float lz = std::log(z);
	float sum = 0, power = 1;
	for (int k = 0; k < BLAST_COEFFS; k++) {
		sum += row->c[k] * power;
		power *= lz;
	}
	return std::exp(sum);
}

// z with normal_cdf(z) = p, by bisection.
float normal_quantile(float p) {
	float lo = -QUANTILE_LIMIT, hi = QUANTILE_LIMIT;
	for (int i = 0; i < QUANTILE_BISECTIONS; i++) {
		const float mid = 0.5f * (lo + hi);
		(normal_cdf(mid) < p ? lo : hi) = mid;
	}
	return 0.5f * (lo + hi);
}

// Log-normal probit through (low, p_low) and (half, 0.5), its spread `sigma` (probit_sigma): probability at `pressure`.
float lognormal(float pressure, float low, float half, float sigma) {
	if (pressure <= 0 || half <= low || low <= 0) return 0.f;
	return normal_cdf(std::log(pressure / half) / sigma);
}
} // namespace

float probit_sigma(float low, float half, float p_low) {
	if (half <= low || low <= 0) return 0.f; // no curve: lognormal answers 0 before it reads the spread
	return std::log(half / low) / -normal_quantile(p_low);
}

float normal_cdf(float z) { return 0.5f * std::erfc(-z * CDF_SCALE); }

float charge_kg(const BlastParams &p, float strength) {
	const float s = std::min(std::max(strength, float(p.strength_min)), float(p.strength_max));
	return p.charge_kg_per_s2 * s * s;
}

float scaled_distance(float kg, float r) { return r / std::cbrt(std::max(kg, MIN_KG)); }

Airblast airblast(const BlastParams &p, float kg, float r) {
	const float z = scaled_distance(kg, std::max(r, 0.f));
	Airblast a;
	a.pressure_kpa = evaluate(p.pressure, z);
	a.impulse_kpa_ms = evaluate(p.impulse, z) * std::cbrt(kg);
	a.duration_ms = evaluate(p.duration, z) * std::cbrt(kg);
	return a;
}

float felt_pressure(const BlastParams &p, const Airblast &a) { return a.pressure_kpa * std::min(1.f, a.duration_ms / p.response_ms); }

float range_at_pressure(const BlastParams &p, float kg, float kpa) {
	if (airblast(p, kg, NEAREST_RANGE_M).pressure_kpa < kpa) return 0.f;
	float lo = NEAREST_RANGE_M, hi = RANGE_LIMIT_M;
	for (int i = 0; i < RANGE_BISECTIONS; i++) {
		const float mid = 0.5f * (lo + hi);
		(airblast(p, kg, mid).pressure_kpa >= kpa ? lo : hi) = mid;
	}
	return lo;
}

float lethality(const BlastParams &p, float pressure_kpa) {
	return lognormal(pressure_kpa, p.lethal_threshold_kpa, p.lethal_half_kpa, p.lethal_sigma);
}

float eardrum(const BlastParams &p, float pressure_kpa) {
	return lognormal(pressure_kpa, p.ear_threshold_kpa, p.ear_half_kpa, p.ear_sigma);
}

float startle_range(const BlastParams &p, float kg) {
	const float kpa = p.startle_pa / PA_PER_KPA;
	float r = range_at_pressure(p, kg, kpa);
	if (r <= 0.f) r = p.startle_cap_m;
	return std::min(r, p.startle_cap_m);
}

float crater_radius(const BlastParams &p, float fireball_m) { return std::max(p.crater_min_m, p.crater_share * fireball_m); }

float crater_depth(const BlastParams &p, float strength) {
	const float s = std::min(std::max(strength, float(p.strength_min)), float(p.strength_max));
	return p.crater_depth_m + p.crater_depth_per_force_m * s;
}

} // namespace city::blast
