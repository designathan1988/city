// streets.json "trees.placement" → TreeLook (see tree_place.h). The chances of the bands scale to `woods_per_ha`: the patches
// are stationary, so one sample of them, taken once, gives the land's mean chance.
#include "render/trees/tree_place.h"

#include "core/math.h"
#include "core/seeded.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HA_M2 = 10000.f;
constexpr float THRESHOLD_GAP = 1e-6f; // a land with no chance at all keeps the scale finite
// The sample of the patches: a square of CAL_SIDE² points CAL_STEP_M apart, past the largest patch's correlation.
constexpr int CAL_SIDE = 200;
constexpr float CAL_STEP_M = 97.f;
// The noise streams of the patches, one by scale (Roadcraft's numbers: the woods stand where they stand there).
constexpr uint32_t PATCH_SALTS[WOOD_PATCHES] = { 7, 9, 11 };
const char *const BAND_KEYS[WOOD_BANDS] = { "inner", "edge", "open" };

float at(const Variant &v, int i) {
	Array a = v;
	return a.size() > i ? float(a[i]) : 0.f;
}
} // namespace

float TreeLook::density_of(double x, double y) const {
	double patch = 0;
	for (int k = 0; k < WOOD_PATCHES; k++) patch += patch_weight[size_t(k)] * woods_noise(x, y, patch_m[size_t(k)], PATCH_SALTS[k]);
	return clampf((float(patch) - mass_from) / mass_span, 0.f, 1.f) * mass_gain - density_floor;
}

WoodBand TreeLook::band_at(float d) const {
	return d > band[size_t(WoodBand::INNER)].from ? WoodBand::INNER : d > band[size_t(WoodBand::EDGE)].from ? WoodBand::EDGE : WoodBand::OPEN;
}

float TreeLook::keep_at(float d, WoodBand b) const {
	const BandLook &B = band[size_t(b)];
	if (b == WoodBand::OPEN && d <= B.from) return B.lone * scale;
	float top = b == WoodBand::EDGE ? band[size_t(WoodBand::INNER)].from : B.from; // only the edge ramps up to a band above it
	float t = top > B.from ? clampf((d - B.from) / (top - B.from), 0.f, 1.f) : 0.f;
	return (B.keep0 + (B.keep1 - B.keep0) * t) * scale;
}

int SpeciesMix::pick(float r) const {
	for (size_t i = 0; i < weight.size(); i++)
		if (r < weight[i]) return species[i];
	return species.empty() ? 0 : species.back();
}

bool SpeciesMix::load(const Dictionary &mix, const TreeKit &kit, const String &where, PackedStringArray &errors) {
	species.clear();
	weight.clear();
	float total = 0;
	Array keys = mix.keys();
	for (int i = 0; i < keys.size(); i++) total += float(mix[keys[i]]);
	if (total <= 0) {
		errors.push_back("streets.json trees " + where + ": mistura de espécies vazia");
		return false;
	}
	float run = 0;
	for (int i = 0; i < keys.size(); i++) {
		int sp = kit.find(String(keys[i]));
		if (sp < 0) errors.push_back("streets.json trees " + where + ": espécie desconhecida " + String(keys[i]));
		run += float(mix[keys[i]]) / total;
		species.push_back(std::max(sp, 0));
		weight.push_back(run);
	}
	return errors.is_empty();
}

bool TreeLook::load(const Dictionary &t, const TreeKit &kit, PackedStringArray &errors) {
	if (!t.has("placement")) {
		errors.push_back("streets.json trees: falta placement");
		return false;
	}
	Dictionary p = t["placement"], bands = p.get("bands", Dictionary());
	near_m = p["near_m"]; fade_m = p["fade_m"]; far_m = p["far_m"];
	far_shadows = p["far_shadows"]; grid_m = p["grid_m"]; jitter = p["jitter"]; plate_m = p["lattice_span_m"];
	for (int k = 0; k < WOOD_PATCHES; k++) { patch_m[size_t(k)] = at(p["patch_m"], k); patch_weight[size_t(k)] = at(p["patch_weight"], k); }
	mass_from = p["mass_from"]; mass_span = p["mass_span"]; mass_gain = p["mass_gain"]; density_floor = p["density_floor"];
	slope_from = at(p["slope_grade"], 0); slope_to = at(p["slope_grade"], 1); slope_gain = p["slope_gain"]; slope_probe_m = p["slope_probe_m"];
	woods_per_ha = p["woods_per_ha"]; crowd_reach_m = p["crowd_reach_m"]; crowd_full = p["crowd_full"];
	crowd_tall = p["crowd_tall"]; crowd_narrow = p["crowd_narrow"]; edge_lean = p["edge_lean"]; lean_rise = p["lean_rise"]; width_vary = p["width_vary"];
	open_tall = p["open_tall"]; open_wide = p["open_wide"];
	tint_vary = p["tint_vary"]; min_y = p["min_y_m"]; max_grade = p["max_grade"]; road_margin = p["road_margin_m"]; lot_margin = p["lot_margin_m"];
	park_m2_per_tree = p["park_m2_per_tree"]; park_edge = p["park_edge_m"]; park_max = p["park_max"];
	park_h0 = at(p["park_height_m"], 0); park_h1 = at(p["park_height_m"], 1); street_height_m = p["street_height_m"];
	eco_gain = p["eco_gain"]; eco_mean = p["eco_mean"]; forest_gain = p["forest_gain"];
	for (const Variant &name : Array(p["planted_models"])) planted_species.push_back(std::max(kit.find(String(name)), 0));
	shrub_species = kit.find("shrub");
	hedge_species = kit.find("hedge");
	flowering_species = { kit.find("shrub_flowering"), kit.find("shrub_flowering_cream"), kit.find("shrub_flowering_red") };
	bed_species = { kit.find("flower_bed"), kit.find("flower_bed_cream"), kit.find("flower_bed_red") };
	if (shrub_species < 0 || hedge_species < 0 || *std::min_element(flowering_species.begin(), flowering_species.end()) < 0 || *std::min_element(bed_species.begin(), bed_species.end()) < 0)
		errors.push_back("streets.json trees: faltam espécies do jardim (shrub, shrub_flowering*, hedge, flower_bed*)");
	park.load(p["park_mix"], kit, "park_mix", errors);
	street.load(p["street_mix"], kit, "street_mix", errors);
	for (int b = 0; b < WOOD_BANDS; b++) {
		Dictionary d = bands.get(BAND_KEYS[b], Dictionary());
		BandLook &L = band[size_t(b)];
		L.from = d["from"];
		L.keep0 = at(d["keep"], 0);
		L.keep1 = at(d["keep"], 1);
		L.lone = d.get("lone", 0.f);
		L.h0 = at(d["size_m"], 0);
		L.h1 = at(d["size_m"], 1);
		L.mix.load(d.get("mix", Dictionary()), kit, String("bands.") + BAND_KEYS[b], errors);
		most = std::max(most, std::max(L.keep0, L.keep1));
	}
	double sum = 0;
	for (int j = 0; j < CAL_SIDE; j++)
		for (int i = 0; i < CAL_SIDE; i++) {
			float d = density_of(i * CAL_STEP_M, j * CAL_STEP_M) + eco_gain * eco_mean;
			sum += keep_at(d, band_at(d));
		}
	float cell = grid_m * grid_m, planned = float(sum) / float(CAL_SIDE * CAL_SIDE);
	scale = std::min(1.f, woods_per_ha * cell / HA_M2 / std::max(planned, THRESHOLD_GAP));
	most *= scale;
	return errors.is_empty() && grid_m > 0 && mass_span > 0 && slope_to > slope_from && crowd_full > 0;
}

} // namespace city
