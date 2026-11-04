// Strict Dictionary -> BlastParams conversion for project/data/catalog/blast.json: a missing or mistyped key is
// reported and loading fails, so no default value can silently replace the data.
#include "api/catalog_loader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

float number(const Dictionary &d, const char *group, const char *key, PackedStringArray &errors) {
	if (!d.has(key)) {
		errors.push_back(String("blast.") + group + ": falta '" + key + "'");
		return 0.f;
	}
	return float(double(d[key]));
}

Dictionary group(const Dictionary &t, const char *key, PackedStringArray &errors) {
	if (!t.has(key)) {
		errors.push_back(String("blast: falta '") + key + "'");
		return Dictionary();
	}
	return t[key];
}

void curves(const Dictionary &t, const char *key, std::vector<BlastCurve> &out, PackedStringArray &errors) {
	Array rows = t.has(key) ? Array(t[key]) : Array();
	for (int i = 0; i < rows.size(); i++) {
		Dictionary row = rows[i];
		BlastCurve c;
		c.z0 = number(row, key, "z0", errors);
		c.z1 = number(row, key, "z1", errors);
		Array coeffs = row.has("c") ? Array(row["c"]) : Array();
		if (coeffs.size() != BLAST_COEFFS) errors.push_back(String("blast.") + key + ": esperado 7 coeficientes");
		for (int k = 0; k < BLAST_COEFFS && k < coeffs.size(); k++) c.c[k] = float(double(coeffs[k]));
		if (!out.empty() && c.z0 < out.back().z1) errors.push_back(String("blast.") + key + ": faixas fora de ordem");
		out.push_back(c);
	}
	if (out.empty()) errors.push_back(String("blast.") + key + ": nenhuma faixa");
}

} // namespace

bool load_blast_params(const Dictionary &t, BlastParams &p, PackedStringArray &errors) {
	const int before = int(errors.size());
	p.charge_kg_per_s2 = number(t, "root", "charge_kg_per_s2", errors);
	p.strength_min = int(number(t, "root", "strength_min", errors));
	p.strength_max = int(number(t, "root", "strength_max", errors));
	p.atmosphere_kpa = number(t, "root", "atmosphere_kpa", errors);
	curves(t, "pressure_curves", p.pressure, errors);
	curves(t, "impulse_curves", p.impulse, errors);
	curves(t, "duration_curves", p.duration, errors);
	p.response_ms = number(group(t, "structure", errors), "structure", "response_ms", errors);
	Dictionary lim = group(t, "limits_kpa", errors);
	p.light_kpa = number(lim, "limits_kpa", "light", errors);
	p.structural_kpa = number(lim, "limits_kpa", "structural", errors);
	p.collapse_kpa = number(lim, "limits_kpa", "collapse", errors);
	p.vehicle_kpa = number(lim, "limits_kpa", "vehicle", errors);
	p.pole_kpa = number(lim, "limits_kpa", "pole", errors);
	Dictionary lethal = group(t, "lethality", errors);
	p.lethal_threshold_kpa = number(lethal, "lethality", "threshold_kpa", errors);
	p.lethal_half_kpa = number(lethal, "lethality", "half_kpa", errors);
	p.lethal_threshold_prob = number(lethal, "lethality", "threshold_probability", errors);
	Dictionary ear = group(t, "eardrum", errors);
	p.ear_threshold_kpa = number(ear, "eardrum", "threshold_kpa", errors);
	p.ear_half_kpa = number(ear, "eardrum", "half_kpa", errors);
	p.ear_threshold_prob = number(ear, "eardrum", "threshold_probability", errors);
	Dictionary noise = group(t, "noise", errors);
	p.startle_pa = number(noise, "noise", "startle_pa", errors);
	p.reference_pa = number(noise, "noise", "reference_pa", errors);
	p.startle_cap_m = number(noise, "noise", "startle_cap_m", errors);
	Dictionary people = group(t, "people", errors);
	p.knock_min_s = number(people, "people", "knock_min_s", errors);
	p.knock_span_s = number(people, "people", "knock_span_s", errors);
	p.panic_s = number(people, "people", "panic_s", errors);
	p.panic_gain = number(people, "people", "panic_gain", errors);
	p.torn_kpa = number(people, "people", "torn_kpa", errors);
	p.sever_chance = number(people, "people", "sever_chance", errors);
	p.maim_chance = number(people, "people", "maim_chance", errors);
	Dictionary strike = group(t, "strike", errors);
	p.hold_growth_s = number(strike, "strike", "hold_growth_s", errors);
	p.fire_max = int(number(strike, "strike", "fire_max", errors));
	p.fire_min_s = number(strike, "strike", "fire_min_s", errors);
	p.fire_span_s = number(strike, "strike", "fire_span_s", errors);
	p.wear_gain = number(strike, "strike", "wear_gain", errors);
	Dictionary crater = group(t, "crater", errors);
	p.crater_k_earth = number(crater, "crater", "k_earth", errors);
	p.crater_k_hard = number(crater, "crater", "k_hard", errors);
	p.crater_ratio = number(crater, "crater", "diameter_to_depth", errors);
	p.pass_width_m = number(crater, "crater", "pass_width_m", errors);
	p.crater_min_m = number(crater, "crater", "min_radius_m", errors);
	return int(errors.size()) == before;
}

} // namespace city
