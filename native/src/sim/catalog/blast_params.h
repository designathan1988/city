// Parameters of explosions (data/catalog/blast.json): the airblast curves and the thresholds of what a given
// overpressure does. No values are written here; api/blast_loader.cpp fills them and rejects a missing key.
#pragma once

#include <vector>

namespace city {

constexpr int BLAST_COEFFS = 7; // A..G of F = exp(A + B ln Z + ... + G ln6 Z)

// One range of scaled distance Z (m per kg^(1/3)) and the coefficients of its fit.
struct BlastCurve {
	float z0 = 0, z1 = 0;
	float c[BLAST_COEFFS] = {};
};

struct BlastParams {
	float charge_kg_per_s2 = 0; // TNT mass of the player's force S: charge_kg_per_s2 * S^2
	int strength_min = 0, strength_max = 0;
	float atmosphere_kpa = 0;
	std::vector<BlastCurve> pressure, impulse, duration; // sorted by z0: incident overpressure (kPa), impulse (kPa-ms per kg^(1/3))
	float response_ms = 0; // the structure's own response time: loads shorter than this are felt in part
	float light_kpa = 0, structural_kpa = 0, collapse_kpa = 0, vehicle_kpa = 0, pole_kpa = 0;
	float lethal_threshold_kpa = 0, lethal_half_kpa = 0, lethal_threshold_prob = 0;
	float ear_threshold_kpa = 0, ear_half_kpa = 0, ear_threshold_prob = 0;
	float startle_pa = 0, reference_pa = 0, startle_cap_m = 0;
	float knock_min_s = 0, knock_span_s = 0, panic_s = 0, panic_gain = 0, torn_kpa = 0, sever_chance = 0, maim_chance = 0;
	float hold_growth_s = 0, fire_min_s = 0, fire_span_s = 0, wear_gain = 0;
	int fire_max = 0;
	float crater_k_earth = 0, crater_k_hard = 0, crater_ratio = 0, pass_width_m = 0, crater_min_m = 0;
};

} // namespace city
